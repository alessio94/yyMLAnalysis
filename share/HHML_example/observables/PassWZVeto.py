from QFramework import TQTreeObservable, INFO, ERROR
import ROOT

from CAFExample import PassWZVeto

def addObservables():
    # add WZ veto for lead, sublead and third lepton
    # idx 1, 2, 3 respectively
    # if you want to try out different mT cuts, they can be passed
    # through to the constructor (in MeV) like so:
    # myWZVetoObs = PassWZVeto("PassWZVetoObs", 1, 50000 (electron cut), 30000 (muon cut))

    mTcut_electron = 50000.
    mTcut_muon     = 50000. # sticking with 50 GeV for now, but this we should change to something lower, like 30 GeV

    myWZVetoLeadLep = PassWZVeto("PassWZVetoLeadLep", 1, mTcut_electron, mTcut_muon)
    if not TQTreeObservable.addObservable(myWZVetoLeadLep):
        INFO("failed to add observable")
        return False

    myWZVetoSubleadLep = PassWZVeto("PassWZVetoSubleadLep", 2, mTcut_electron, mTcut_muon)
    if not TQTreeObservable.addObservable(myWZVetoSubleadLep):
        INFO("failed to add observable")
        return False

    myWZVetoThirdLep = PassWZVeto("PassWZVetoThirdLep", 3, mTcut_electron, mTcut_muon)
    if not TQTreeObservable.addObservable(myWZVetoThirdLep):
        INFO("failed to add observable")
        return False

    return True
