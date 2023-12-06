#########################################################################################################
# Package: CAFExample## ############################################################################
cmake_minimum_required( VERSION 3.5.0 )

IF(${AnalysisBase_FOUND})
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
    PileupReweightingLib MCTruthClassifierLib GoodRunsListsLib MuonEfficiencyCorrectionsLib
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

endif()
