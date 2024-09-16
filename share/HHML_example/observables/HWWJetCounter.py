from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

def addObservables(config):
    from CAFExample import HWWJetCounter

    nNominalJets = HWWJetCounter("nNominalJets", "nominal", 25000., 30000.)
    nNominalJets.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nNominalJets):
        INFO("failed to add nNominalJets variable")
        return False
    
    nJetsFJVT = HWWJetCounter("nJetsFJVT", "FJVT", 25000., 30000.)
    nJetsFJVT.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nJetsFJVT):
        INFO("failed to add nJetsFJVT variable")
        return False

    nJetsFJVTTight = HWWJetCounter("nJetsFJVTTight", "FJVTTight", 25000., 30000.)
    nJetsFJVTTight.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nJetsFJVTTight):
        INFO("failed to add nJetsFJVT variable")
        return False

    nJetsFJVT3030 = HWWJetCounter("nJetsFJVT3030", "FJVT", 30000., 30000.)
    nJetsFJVT3030.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nJetsFJVT3030):
        INFO("failed to add nJetsFJVT3030 variable")
        return False
    
    nJets3030 = HWWJetCounter("nJets3030", "nominal", 30000., 30000.)
    nJets3030.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nJets3030):
        INFO("failed to add nJets3030 variable")
        return False

    nJets2525 = HWWJetCounter("nJets2525", "nominal", 25000., 25000.)
    nJets2525.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nJets2525):
        INFO("failed to add nJets2525 variable")
        return False

    nJets2020 = HWWJetCounter("nJets2020", "nominal", 20000., 20000.)
    nJets2020.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nJets2020):
        INFO("failed to add nJets2020 variable")
        return False

    nECJetsFJVTTight = HWWJetCounter("nECJetsFJVTTight", "FJVTTight", 25000., 30000.)
    nECJetsFJVTTight.setEtaRange(2.75, 3.25)
    if not TQObservable.addObservable(nECJetsFJVTTight):
        INFO("failed to add nECJetsFJVT variable")
        return False

    nECJetsFJVT3030 = HWWJetCounter("nECJetsFJVT3030", "FJVT", 30000., 30000.)
    nECJetsFJVT3030.setEtaRange(2.75, 3.25)
    if not TQObservable.addObservable(nECJetsFJVT3030):
        INFO("failed to add nECJetsFJVT3030 variable")
        return False

    nFWJetsFJVTTight = HWWJetCounter("nFWJetsFJVTTight", "FJVTTight", 25000., 30000.)
    nFWJetsFJVTTight.setEtaRange(3.25, 4.5)
    if not TQObservable.addObservable(nFWJetsFJVTTight):
        INFO("failed to add nJetsFJVT variable")
        return False

    nFWJetsFJVT3030 = HWWJetCounter("nFWJetsFJVT3030", "FJVT", 30000., 30000.)
    nFWJetsFJVT3030.setEtaRange(3.25, 4.5)    
    if not TQObservable.addObservable(nFWJetsFJVT3030):
        INFO("failed to add nFWJetsFJVT3030 variable")
        return False

    nTruthJets = HWWJetCounter("nTruthJets", "nominal", 25000., 30000.)
    nTruthJets.setDoTruth(True)
    nTruthJets.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nTruthJets):
        INFO("failed to add nTruthJets variable")
        return False

    nTruthJets3030 = HWWJetCounter("nTruthJets3030", "nominal", 30000., 30000.)
    nTruthJets3030.setDoTruth(True)
    nTruthJets3030.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nTruthJets3030):
        INFO("failed to add nTruthJets3030 variable")
        return False

    nTruthJets2020 = HWWJetCounter("nTruthJets2020", "nominal", 20000., 20000.)
    nTruthJets2020.setDoTruth(True)
    nTruthJets2020.setEtaRange(0, 2.5)
    if not TQObservable.addObservable(nTruthJets2020):
        INFO("failed to add nTruthJets2020 variable")
        return False

    #rejectJetsOverlappingWithOtherMuon = True
    #nJets3030NoMuOR = HWWJetCounter("nJets3030NoMuOR", "nominal", 30000., 30000., rejectJetsOverlappingWithOtherMuon)
    #if not TQObservable.addObservable(nJets3030NoMuOR):
    #    INFO("failed to add nJets3030NoMuOR variable")
    #    return False

    tightJetPtCut = config.getTagDoubleDefault("tightJetPtCut", 30000.)
    INFO("Tight jet pT cut: "+str(tightJetPtCut))
    nTightJets = HWWJetCounter("nTightJets", "nominal", tightJetPtCut, tightJetPtCut)
    if not TQObservable.addObservable(nTightJets):
        INFO("failed to add nTightJets variable")
        return False

    nTightTruthJets = HWWJetCounter("nTightTruthJets", "nominal", tightJetPtCut, tightJetPtCut)
    nTightTruthJets.setDoTruth(True)
    if not TQObservable.addObservable(nTightTruthJets):
        INFO("failed to add nTightTruthJets variable")
        return False

    return True
