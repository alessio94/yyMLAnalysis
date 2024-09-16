from QFramework import TQObservable,INFO,ERROR,BREAK,WARN


from CAFExample import TransMassW

def addObservables():
    # add Trans Mass for lead, sublead and third lepton
    # idx 1, 2, 3 respectively
    # through to the constructor (in MeV) like so:
    # myTransMassWObs = TransMassW("TransMassWObs", 1)


    myTransMassWLeadLep = TransMassW("TransMassWLeadLep", 1)
    if not TQObservable.addObservable(myTransMassWLeadLep):
        INFO("failed to add observable")
        return False

    myTransMassWSubleadLep = TransMassW("TransMassWSubleadLep", 2)
    if not TQObservable.addObservable(myTransMassWSubleadLep):
        INFO("failed to add observable")
        return False

    myTransMassWThirdLep = TransMassW("TransMassWThirdLep", 3)
    if not TQObservable.addObservable(myTransMassWThirdLep):
        INFO("failed to add observable")
        return False

    return True
