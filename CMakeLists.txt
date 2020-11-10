#########################################################################################################
# Package: CAFExample## ############################################################################
cmake_minimum_required( VERSION 2.8.12 )
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)

# Declare the package name:
project( CAFExample )

# check if this is the top-level CMakeLists.txt
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(HAS_PARENT 0)
else()
  set(HAS_PARENT 1)
endif()

# we try to discover if we are compiling inside an ASG environment or not
find_package( AnalysisBase QUIET)

# define dependencies on other packages
find_package( ROOT COMPONENTS Core Matrix RooFitCore Minuit2)

# In previous versions, we auto-generated the file below. It is not needed
# anymore and breaks the setup. So we delete it if it looks like our file.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py")
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py contents)
  STRING(REGEX REPLACE ";" "\\\\;" contents ${contents})
  STRING(REGEX REPLACE "\n" ";" contents ${contents})
  list(GET contents 0 firstLine)
  if(${firstLine} MATCHES "\# this is an automatically generated*")
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py)
    message("Removed obsolete file " ${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py)
  endif()
endif()

# set some variables for easier handling
file(GLOB CAFExamplePython python/*.py)
file(GLOB CAFExampleTests test/*.py)

# add CAFlwtnn to setup scripts (?)
set(CAF_PRELOAD_LIBRARIES libCAFlwtnn)

IF(${AnalysisBase_FOUND})
  # this is ASG compilation, we don't need to do anything
  file(GLOB CAFExampleSources Root/*.cxx)
  file(GLOB CAFExampleHeaders CAFExample/*.h)

  set(CAFExamplePythonDir ${CMAKE_CURRENT_BINARY_DIR}/CAFExample)
  set(CAFExampleLinkDef ${CAFExamplePythonDir}/LinkDef.h)
  execute_process(COMMAND mkdir -p ${CAFExamplePythonDir})
  list(APPEND CAFExamplePython ${CAFExamplePythonDir}/__init__.py)

  set(BINARY_TAG ${ATLAS_PLATFORM})

  # generate the LinkDef.h
  execute_process(
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/CAFCore/QFramework/share/generateBindings.py --set-working-directory ${CMAKE_CURRENT_SOURCE_DIR} --pkgname CAFExample --loadpackage --verbose --python ${CAFExamplePythonDir}/__init__.py --linkdef ${CAFExampleLinkDef} --headers ${CAFExampleHeaders}
    )

  # write a setup script
  execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/setup/writeSetupScript.py ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/setup ${CMAKE_CURRENT_BINARY_DIR}/CAFCore ${CMAKE_CURRENT_BINARY_DIR})

  # Set up the usage of CTest:
  IF(NOT ${HAS_PARENT})
    atlas_ctest_setup() # Set up the project:
    atlas_project( CAFExample 1.0.0
      USE AnalysisBase ${AnalysisBase_VERSION} )

    # Generate an environment setup script:
    lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
    install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh DESTINATION . )

    # Set up the usage of CPack:
    set(CMAKE_INSTALL_PREFIX /InstallArea/${BINARY_TAG})
    atlas_cpack_setup()
  ENDIF()

  # register this as a package to ASG
  atlas_subdir( CAFExample )

  # use the standard ROOT method of dictionary generation
  atlas_add_root_dictionary( CAFExample CAFExampleCintDict
    ROOT_HEADERS ${CAFExampleHeaders} ${CAFExampleLinkDef}
    EXTERNAL_PACKAGES ROOT
    )

  # put everything together to make CMake compile your library
  atlas_add_library( CAFExample
    ${CAFExampleHeaders} ${CAFExampleSources} ${CAFExampleCintDict}
    PUBLIC_HEADERS CAFExample
    PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES QFramework CAFxAODUtils
    PileupReweightingLib MCTruthClassifierLib GoodRunsListsLib ElectronEfficiencyCorrectionLib MuonEfficiencyCorrectionsLib
    TriggerAnalysisInterfacesLib FourMomUtils xAODBTaggingEfficiencyLib AsgAnalysisInterfaces FTagAnalysisInterfacesLib TrigGlobalEfficiencyCorrectionLib
    # here go all the libraries corresonding to the packages that you depend on
    PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
    )

  # also have all of your python files installed
  atlas_install_python_modules( ${CAFExamplePython} )

  # check if we have any test cases
  if(${CAFExampleTests})
    # register your test cases from the "test" folder
    find_package(PythonInterp REQUIRED)
    foreach(TestScript test)
      get_filename_component(TestName ${TestScript} NAME_WE)
      atlas_add_test( ${TestName} SCRIPT ${TestScript} )
    endforeach()
  endif()

  # Set up the usage of CPack:
  set(CMAKE_INSTALL_PREFIX /InstallArea/${BINARY_TAG})
  atlas_cpack_setup()
else()
  # this is standalone compilation, we need to stitch together the project
  message( "compiling CAFExample in standalone mode" )
  set(WITHOUT_ASG ON )

  set(CAFExamplePythonDir ${CMAKE_CURRENT_BINARY_DIR}/CAFExample)
  execute_process(COMMAND mkdir -p ${CAFExamplePythonDir})
  foreach(module ${CAFExamplePython})
    execute_process(COMMAND ln -sf ${module} ${CAFExamplePythonDir})
  endforeach()

  set(CAFExampleLinkDef ${CMAKE_CURRENT_BINARY_DIR}/CAFExample/LinkDef.h)
  file(GLOB CAFExampleSourcesAll Root/*.cxx )
  file(GLOB CAFExampleHeadersAll CAFExample/*.h )
  execute_process(COMMAND grep -L xAOD ${CAFExampleSourcesAll} OUTPUT_VARIABLE CAFExampleSourcesTMP )
  execute_process(COMMAND grep -L xAOD ${CAFExampleHeadersAll} OUTPUT_VARIABLE CAFExampleHeadersTMP )
  string(REGEX REPLACE "[ \r\n]+" ";" CAFExampleSources ${CAFExampleSourcesTMP})
  string(REGEX REPLACE "[ \r\n]+" ";" CAFExampleHeaders ${CAFExampleHeadersTMP})

  # generate the LinkDef.h
  execute_process(
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/CAFCore/QFramework/share/generateBindings.py --set-working-directory ${CMAKE_CURRENT_SOURCE_DIR} --pkgname CAFExample --loadpackage --verbose --python ${CAFExamplePythonDir}/__init__.py --linkdef ${CAFExampleLinkDef} --headers ${CAFExampleHeaders}
    )

  # set general CMake settings
  set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
  set(CMAKE_DISABLE_SOURCE_CHANGES  OFF)
  set(CMAKE_VERBOSE_MAKEFILE OFF)
  set(CMAKE_COLOR_MAKEFILE   ON)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

  # write a setup script
  execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/setup/writeSetupScript.py ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/setup ${CMAKE_CURRENT_BINARY_DIR}/CAFCore ${CMAKE_CURRENT_BINARY_DIR})

  # register all the files and directories
  include_directories ("${PROJECT_SOURCE_DIR}")
  include_directories ("${PROJECT_SOURCE_DIR}/CAFCore/QFramework")
  include_directories ("${PROJECT_SOURCE_DIR}/CAFCore/SFramework")
  include_directories ("${ROOT_INCLUDE_DIRS}")

  # generate the dictionary source code
  include(${ROOT_USE_FILE})
  ROOT_GENERATE_DICTIONARY(G__CAFExample ${CAFExampleHeaders} LINKDEF ${CAFExampleLinkDef})

  # register the shared object to include both sources and dictionaries
  add_library( CAFExample SHARED ${CAFExampleSources} G__CAFExample.cxx)

  # require ROOT
  find_package( ROOT REQUIRED )
  include(${ROOT_USE_FILE})

  # enabling python test cases
  find_package(PythonInterp REQUIRED)
  include(CTest)
  enable_testing()

  # link everything together at the end
  target_link_libraries( CAFExample QFramework SFramework ${ROOT_LIBRARIES} )

  if(EXISTS ${PROJECT_SOURCE_DIR}/VIPUnfolding)
    message("adding VIPUnfolding")
    add_subdirectory(BootstrapGenerator)
    add_subdirectory(VIPUnfolding)
  endif()

  message("adding CAFCore")
  add_subdirectory(CAFCore)

endif()
