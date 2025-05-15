#########################################################################################################
# Package: yyMLAnalysisCode## ############################################################################
cmake_minimum_required( VERSION 3.5.0 )
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)

# Declare the package name:
project( yyMLAnalysisCode VERSION 1.0.0 )

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

# set some variables for easier handling
file(GLOB yyMLAnalysisCodePython yyMLAnalysisCode/python/*.py)
file(GLOB yyMLAnalysisCodeTests test/*.py)

# add CAFlwtnn to setup scripts (?)
set(CAF_PRELOAD_LIBRARIES libCAFlwtnn)

IF(${AnalysisBase_FOUND})
  # this is ASG compilation, we don't need to do anything
  message( "compiling using ASG enviroment, assuming we have xAOD tools" )
  add_definitions(-DHAS_XAOD)
  file(GLOB yyMLAnalysisCodeSources yyMLAnalysisCode/Root/*.cxx)
  file(GLOB yyMLAnalysisCodeHeaders yyMLAnalysisCode/yyMLAnalysisCode/*.h)

  set(yyMLAnalysisCodePythonDir ${CMAKE_CURRENT_BINARY_DIR}/yyMLAnalysisCode)
  set(yyMLAnalysisCodeLinkDef ${yyMLAnalysisCodePythonDir}/LinkDef.h)
  execute_process(COMMAND mkdir -p ${yyMLAnalysisCodePythonDir})
  list(APPEND yyMLAnalysisCodePython ${yyMLAnalysisCodePythonDir}/__init__.py)

  set(BINARY_TAG ${ATLAS_PLATFORM})

  # generate the LinkDef.h
  execute_process(
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/CAFCore/QFramework/share/generateBindings.py --set-working-directory ${CMAKE_CURRENT_SOURCE_DIR} --pkgname yyMLAnalysisCode --loadpackage --verbose --python ${yyMLAnalysisCodePythonDir}/__init__.py --linkdef ${yyMLAnalysisCodeLinkDef} --headers ${yyMLAnalysisCodeHeaders}
    )

  # write a setup script
  execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/setup/writeSetupScript.py ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/setup ${CMAKE_CURRENT_BINARY_DIR}/CAFCore ${CMAKE_CURRENT_BINARY_DIR})

  # Set up the usage of CTest:
  IF(NOT ${HAS_PARENT})
    atlas_ctest_setup() # Set up the project:
    atlas_project( yyMLAnalysisCode 1.0.0
      USE AnalysisBase ${AnalysisBase_VERSION} )

    # Generate an environment setup script:
    lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
    install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh DESTINATION . )

    # Set up the usage of CPack:
    set(CMAKE_INSTALL_PREFIX /yyMLAnalysisCode/1.0.0/InstallArea/${BINARY_TAG})
    atlas_cpack_setup()
  ELSE()
    # Set up the usage of CPack:
    set(CMAKE_INSTALL_PREFIX /yyMLAnalysisCode/1.0.0/InstallArea/${BINARY_TAG})
    atlas_cpack_setup()

  ENDIF()

  # Add preprocesser defines for library compilation
  target_compile_definitions(yyMLAnalysisCode PUBLIC HAS_XAOD)

else()
  # this is standalone compilation, we need to stitch together the project
  message( "compiling yyMLAnalysisCode in standalone mode" )
  set(WITHOUT_ASG ON )

  set(yyMLAnalysisCodePythonDir ${CMAKE_CURRENT_BINARY_DIR}/yyMLAnalysisCode)
  execute_process(COMMAND mkdir -p ${yyMLAnalysisCodePythonDir})
  foreach(module ${yyMLAnalysisCodePython})
    execute_process(COMMAND ln -sf ${module} ${yyMLAnalysisCodePythonDir})
  endforeach()

  set(yyMLAnalysisCodeLinkDef ${CMAKE_CURRENT_BINARY_DIR}/yyMLAnalysisCode/yyMLAnalysisCode/LinkDef.h)
  file(GLOB yyMLAnalysisCodeSourcesAll yyMLAnalysisCode/Root/*.cxx )
  file(GLOB yyMLAnalysisCodeHeadersAll yyMLAnalysisCode/yyMLAnalysisCode/*.h )
  execute_process(COMMAND grep -L xAOD ${yyMLAnalysisCodeSourcesAll} OUTPUT_VARIABLE yyMLAnalysisCodeSourcesTMP )
  execute_process(COMMAND grep -L xAOD ${yyMLAnalysisCodeHeadersAll} OUTPUT_VARIABLE yyMLAnalysisCodeHeadersTMP )
  string(REGEX REPLACE "[ \r\n]+" ";" yyMLAnalysisCodeSources ${yyMLAnalysisCodeSourcesTMP})
  string(REGEX REPLACE "[ \r\n]+" ";" yyMLAnalysisCodeHeaders ${yyMLAnalysisCodeHeadersTMP})

  # generate the LinkDef.h
  execute_process(
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/CAFCore/QFramework/share/generateBindings.py --set-working-directory ${CMAKE_CURRENT_SOURCE_DIR} --pkgname yyMLAnalysisCode --loadpackage --verbose --python ${yyMLAnalysisCodePythonDir}/__init__.py --linkdef ${yyMLAnalysisCodeLinkDef} --headers ${yyMLAnalysisCodeHeaders}
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
  include_directories ("${PROJECT_SOURCE_DIR}/yyMLAnalysisCode")
  include_directories ("${PROJECT_SOURCE_DIR}/CAFCore/QFramework")
  include_directories ("${PROJECT_SOURCE_DIR}/CAFCore/SFramework")
  include_directories ("${ROOT_INCLUDE_DIRS}")

  # generate the dictionary source code
  include(${ROOT_USE_FILE})
  ROOT_GENERATE_DICTIONARY(G__yyMLAnalysisCode ${yyMLAnalysisCodeHeaders} LINKDEF ${yyMLAnalysisCodeLinkDef})

  # register the shared object to include both sources and dictionaries
  add_library( yyMLAnalysisCode SHARED ${yyMLAnalysisCodeSources} G__yyMLAnalysisCode.cxx)

  # require ROOT
  find_package( ROOT REQUIRED )
  include(${ROOT_USE_FILE})

  # enabling python test cases
  find_package(Python REQUIRED)
  include(CTest)
  enable_testing()

  # link everything together at the end
  target_link_libraries( yyMLAnalysisCode QFramework SFramework ${ROOT_LIBRARIES} )

  if(EXISTS ${PROJECT_SOURCE_DIR}/VIPUnfolding)
    message("adding VIPUnfolding")
    add_subdirectory(BootstrapGenerator)
    add_subdirectory(VIPUnfolding)
  endif()

  message("adding CAFCore")
  add_subdirectory(CAFCore)

  message("adding SubmissionHelpers")
  add_subdirectory(SubmissionHelpers)  

endif()
