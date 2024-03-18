from QFramework import TQObservable,INFO,ERROR,BREAK,WARN


def addObservables():
    from CAFExample import HWWNLeptonObs

    nLep = HWWNLeptonObs("nLep")

    nVetoMuon = HWWNLeptonObs("nVetoMuon")
    nVetoMuon.setMode(1)
    nVetoMuon.setEle_LH(4)
    nVetoMuon.setIsolation(1)

    nVetoEle = HWWNLeptonObs("nVetoEle")
    nVetoEle.setMode(2)
    nVetoEle.setEle_LH(4)
    nVetoEle.setIsolation(1)

    # number of leptons in otherParts with the ID criteria of the ZH analysis
    nVetoLepton = HWWNLeptonObs("nVetoLepton")
    nVetoLepton.setMode(3) # muons + electrons
    nVetoLepton.setEle_LH(2) # LH medium (slightly looser than ZH definition)
    nVetoLepton.setIsolation(1) # isoFCLoose (e and m)
    nVetoLepton.m_useMuonQuality = True
    nVetoLepton.m_muonQuality = 1 # 1 = Medium

    nVetoLeptonTight = HWWNLeptonObs("nVetoLeptonTight")
    nVetoLeptonTight.setMode(3) # muons + electrons
    nVetoLeptonTight.setEle_LH(3) # LH Tight 
    nVetoLeptonTight.setIsolation(3) # FCTight (e and m)
    nVetoLeptonTight.m_useMuonQuality = True
    nVetoLeptonTight.m_muonQuality = 1 # 1 = Medium

    TriLep = HWWNLeptonObs("TriLep")
    TriLep.setMode(4)

    VetoLepPt = HWWNLeptonObs("VetoLepPt")
    VetoLepPt.setMode(7)
    VetoLepPt.setEle_LH(4)
    VetoLepPt.setIsolation(1)

    VetoLepEta = HWWNLeptonObs("VetoLepEta")
    VetoLepEta.setMode(8)
    VetoLepEta.setEle_LH(4)
    VetoLepEta.setIsolation(1)

    VetoLepPhi = HWWNLeptonObs("VetoLepPhi")
    VetoLepPhi.setMode(9)
    VetoLepPhi.setEle_LH(4)
    VetoLepPhi.setIsolation(1)

    nOtherJets = HWWNLeptonObs("nOtherJets")
    nOtherJets.setMode(10)

    if not TQObservable.addObservable(nLep):
        INFO("failed to add nLep observable")
        return False
    if not TQObservable.addObservable(nVetoMuon):
        INFO("failed to add nVetoMuon observable")
        return False
    if not TQObservable.addObservable(nVetoEle):
        INFO("failed to add nVetoEle observable")
        return False
    if not TQObservable.addObservable(nVetoLepton):
        INFO("failed to add nVetoLepton observable")
        return False
    if not TQObservable.addObservable(nVetoLeptonTight):
        INFO("failed to add nVetoLeptonTight observable")
        return False
    if not TQObservable.addObservable(TriLep):
        INFO("failed to add TriLep observable")
        return False
    if not TQObservable.addObservable(VetoLepPt):
        INFO("failed to add VetoLepPt observable")
        return False
    if not TQObservable.addObservable(VetoLepEta):
        INFO("failed to add VetoLepEta observable")
        return False
    if not TQObservable.addObservable(VetoLepPhi):
        INFO("failed to add VetoLepPhi observable")
        return False
    if not TQObservable.addObservable(nOtherJets):
        INFO("failed to add nOtherJets observable")
        return False

    return True
