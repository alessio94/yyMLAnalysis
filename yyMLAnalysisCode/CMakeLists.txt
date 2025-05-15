#########################################################################################################
# Package: yyMLAnalysisCode## ############################################################################
cmake_minimum_required( VERSION 3.5.0 )

IF(${AnalysisBase_FOUND})
  # register this as a package to ASG
  atlas_subdir( yyMLAnalysisCode )

  # use the standard ROOT method of dictionary generation
  atlas_add_root_dictionary( yyMLAnalysisCode yyMLAnalysisCodeCintDict
    ROOT_HEADERS ${yyMLAnalysisCodeHeaders} ${yyMLAnalysisCodeLinkDef}
    EXTERNAL_PACKAGES ROOT
    )

  # put everything together to make CMake compile your library
  atlas_add_library( yyMLAnalysisCode
    ${yyMLAnalysisCodeHeaders} ${yyMLAnalysisCodeSources} ${yyMLAnalysisCodeCintDict}
    PUBLIC_HEADERS yyMLAnalysisCode
    PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
    LINK_LIBRARIES QFramework CAFxAODUtils
    PileupReweightingLib MCTruthClassifierLib GoodRunsListsLib MuonEfficiencyCorrectionsLib
    FourMomUtils xAODBTaggingEfficiencyLib AsgAnalysisInterfaces FTagAnalysisInterfacesLib TrigGlobalEfficiencyCorrectionLib
    # here go all the libraries corresonding to the packages that you depend on
    PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
    )

  # also have all of your python files installed
  atlas_install_python_modules( ${yyMLAnalysisCodePython} )

  # check if we have any test cases
  if(${yyMLAnalysisCodeTests})
    # register your test cases from the "test" folder
    find_package(PythonInterp REQUIRED)
    foreach(TestScript test)
      get_filename_component(TestName ${TestScript} NAME_WE)
      atlas_add_test( ${TestName} SCRIPT ${TestScript} )
    endforeach()
  endif()

endif()
