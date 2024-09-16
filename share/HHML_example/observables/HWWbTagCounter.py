from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

def addObservables(config):
    from CAFExample import HWWBTagCounterObs

    jetAuthor = config.getTagStandardStringDefault("jetCollection", "AntiKt4EMPFlowJets")

    # Append the extra BTagging information for jetauthor
    Suffix = ""
    if jetAuthor == "AntiKt4EMPFlowJets":
        Suffix = "_BTagging201903"
    elif jetAuthor == "AntiKt4EMTopoJets":
        Suffix = "_BTagging201810"
    else:
        Suffix = ""
       
    jetAuthor = jetAuthor + Suffix

    nBTag = HWWBTagCounterObs("nBTag")
    nBTag.setJetAuthor(jetAuthor)
    
    nBJetsMV2c10 = HWWBTagCounterObs("nBJetsMV2c10")
    nBJetsMV2c10.setPtCut(30000.0)    
    nBJetsMV2c10.setBTagName("MV2c10", "FixedCutBEff_85")
    nBJetsMV2c10.setJetAuthor(jetAuthor)

    nBJetsMV2c10_WP77 = HWWBTagCounterObs("nBJetsMV2c10_WP77")
    nBJetsMV2c10_WP77.setPtCut(30000.)
    nBJetsMV2c10_WP77.setBTagName("MV2c10", "FixedCutBEff_77")
    nBJetsMV2c10_WP77.setJetAuthor(jetAuthor)

    nBJetsMV2c10_WP70 = HWWBTagCounterObs("nBJetsMV2c10_WP70")
    nBJetsMV2c10_WP70.setPtCut(30000.)
    nBJetsMV2c10_WP70.setBTagName("MV2c10", "FixedCutBEff_70")
    nBJetsMV2c10_WP70.setJetAuthor(jetAuthor)

    nBJetsMV2c10_WP60 = HWWBTagCounterObs("nBJetsMV2c10_WP60")
    nBJetsMV2c10_WP60.setPtCut(30000.)
    nBJetsMV2c10_WP60.setBTagName("MV2c10", "FixedCutBEff_60")
    nBJetsMV2c10_WP60.setJetAuthor(jetAuthor)

    nBJetsSubMV2c10 = HWWBTagCounterObs("nBJetsSubMV2c10")
    nBJetsSubMV2c10.setPtCut(20000.)
    nBJetsSubMV2c10.setBTagName("MV2c10", "FixedCutBEff_85")
    #nBJetsSubMV2c10.setUseOtherJets(True)
    nBJetsSubMV2c10.setJetAuthor(jetAuthor)

    nBJetsSubMV2c10_WP77 = HWWBTagCounterObs("nBJetsSubMV2c10_WP77")
    nBJetsSubMV2c10_WP77.setPtCut(20000.)
    nBJetsSubMV2c10_WP77.setBTagName("MV2c10", "FixedCutBEff_77")
    #nBJetsSubMV2c10_WP77.setUseOtherJets(True)
    nBJetsSubMV2c10_WP77.setJetAuthor(jetAuthor)

    nBJetsSubMV2c10_WP70 = HWWBTagCounterObs("nBJetsSubMV2c10_WP70")
    nBJetsSubMV2c10_WP70.setPtCut(20000.)
    nBJetsSubMV2c10_WP70.setBTagName("MV2c10", "FixedCutBEff_70")
    #nBJetsSubMV2c10_WP70.setUseOtherJets(True)
    nBJetsSubMV2c10_WP70.setJetAuthor(jetAuthor)

    nBJetsSubMV2c10_WP60 = HWWBTagCounterObs("nBJetsSubMV2c10_WP60")
    nBJetsSubMV2c10_WP60.setPtCut(20000.)
    nBJetsSubMV2c10_WP60.setBTagName("MV2c10", "FixedCutBEff_60")
    #nBJetsSubMV2c10_WP60.setUseOtherJets(True)
    nBJetsSubMV2c10_WP60.setJetAuthor(jetAuthor)

    nBJetsDL1r = HWWBTagCounterObs("nBJetsDL1r")
    nBJetsDL1r.setPtCut(30000.0)
    nBJetsDL1r.setBTagName("DL1r", "FixedCutBEff_85")
    nBJetsDL1r.setJetAuthor(jetAuthor)

    nBJetsDL1r_WP77 = HWWBTagCounterObs("nBJetsDL1r_WP77")
    nBJetsDL1r_WP77.setPtCut(30000.0)
    nBJetsDL1r_WP77.setBTagName("DL1r", "FixedCutBEff_77")
    nBJetsDL1r_WP77.setJetAuthor(jetAuthor)

    nBJetsDL1r_WP70 = HWWBTagCounterObs("nBJetsDL1r_WP70")
    nBJetsDL1r_WP70.setPtCut(30000.0)
    nBJetsDL1r_WP70.setBTagName("DL1r", "FixedCutBEff_70")
    nBJetsDL1r_WP70.setJetAuthor(jetAuthor)

    nBJetsDL1r_WP60 = HWWBTagCounterObs("nBJetsDL1r_WP60")
    nBJetsDL1r_WP60.setPtCut(30000.0)
    nBJetsDL1r_WP60.setBTagName("DL1r", "FixedCutBEff_60")
    nBJetsDL1r_WP60.setJetAuthor(jetAuthor)

    nBJetsSubDL1r = HWWBTagCounterObs("nBJetsSubDL1r")
    nBJetsSubDL1r.setPtCut(20000.0)
    nBJetsSubDL1r.setBTagName("DL1r", "FixedCutBEff_85")
    #nBJetsSubDL1r.setUseOtherJets(True)
    nBJetsSubDL1r.setJetAuthor(jetAuthor)

    nBJetsSubDL1r_WP77 = HWWBTagCounterObs("nBJetsSubDL1r_WP77")
    nBJetsSubDL1r_WP77.setPtCut(20000.0)
    nBJetsSubDL1r_WP77.setBTagName("DL1r", "FixedCutBEff_77")
    #nBJetsSubDL1r_WP77.setUseOtherJets(True)
    nBJetsSubDL1r_WP77.setJetAuthor(jetAuthor)

    nBJetsSubDL1r_WP70 = HWWBTagCounterObs("nBJetsSubDL1r_WP70")
    nBJetsSubDL1r_WP70.setPtCut(20000.0)
    nBJetsSubDL1r_WP70.setBTagName("DL1r", "FixedCutBEff_70")
    #nBJetsSubDL1r_WP70.setUseOtherJets(True)
    nBJetsSubDL1r_WP70.setJetAuthor(jetAuthor)

    nBJetsSubDL1r_WP60 = HWWBTagCounterObs("nBJetsSubDL1r_WP60")
    nBJetsSubDL1r_WP60.setPtCut(20000.0)
    nBJetsSubDL1r_WP60.setBTagName("DL1r", "FixedCutBEff_60")
    #nBJetsSubDL1r_WP60.setUseOtherJets(True)
    nBJetsSubDL1r_WP60.setJetAuthor(jetAuthor)

   # Set a custom working point by manually selecting the discriminant value directly (only supports MV2 right now.)
    nBJetsMV2c10_WPCustom = HWWBTagCounterObs("nBJetsMV2c10_WPCustom")
    nBJetsMV2c10_WPCustom.setPtCut(30000.0)
    nBJetsMV2c10_WPCustom.setBTagName("MV2c10", "FixedCutBEff_85")
    nBJetsMV2c10_WPCustom.setUseHardCuts(True)
    nBJetsMV2c10_WPCustom.setBTagCut(0.05)
    nBJetsMV2c10_WPCustom.setJetAuthor(jetAuthor)

    if not TQObservable.addObservable(nBTag):
        INFO("failed to add nBTag observable")
        return False

    if not TQObservable.addObservable(nBJetsMV2c10):
        INFO("failed to add nBJetsMV2c10 observable")
        return False

    if not TQObservable.addObservable(nBJetsMV2c10_WP77):
        INFO("failed to add nBJetsMV2c10_WP77 observable")
        return False

    if not TQObservable.addObservable(nBJetsMV2c10_WP70):
        INFO("failed to add nBJetsMV2c10_WP70 observable")
        return False

    if not TQObservable.addObservable(nBJetsMV2c10_WP60):
        INFO("failed to add nBJetsMV2c10_WP60 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubMV2c10):
        INFO("failed to add nBJetsSubMV2c10 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubMV2c10_WP77):
        INFO("failed to add nBJetsSubMV2c10_WP77 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubMV2c10_WP70):
        INFO("failed to add nBJetsSubMV2c10_WP70 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubMV2c10_WP60):
        INFO("failed to add nBJetsSubMV2c10_WP60 observable")
        return False

    if not TQObservable.addObservable(nBJetsDL1r):
        INFO("failed to add nBJetsDL1r observable")
        return False

    if not TQObservable.addObservable(nBJetsDL1r_WP77):
        INFO("failed to add nBJetsDL1r_WP77 observable")
        return False

    if not TQObservable.addObservable(nBJetsDL1r_WP70):
        INFO("failed to add nBJetsDL1r_WP70 observable")
        return False

    if not TQObservable.addObservable(nBJetsDL1r_WP60):
        INFO("failed to add nBJetsDL1r_WP60 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubDL1r):
        INFO("failed to add nBJetsSubDL1r observable")
        return False

    if not TQObservable.addObservable(nBJetsSubDL1r_WP77):
        INFO("failed to add nBJetsSubDL1r_WP77 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubDL1r_WP70):
        INFO("failed to add nBJetsSubDL1r_WP70 observable")
        return False

    if not TQObservable.addObservable(nBJetsSubDL1r_WP60):
        INFO("failed to add nBJetsSubDL1r_WP60 observable")
        return False

    if not TQObservable.addObservable(nBJetsMV2c10_WPCustom):
        INFO("failed to add nBJetsMV2c10_WPCustom observable")
        return False

    return True
