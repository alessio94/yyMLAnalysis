from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

def addObservables():
    from CAFExample import HWWMtautauObs
    
    mtt = HWWMtautauObs("mtt")
    if not TQObservable.addObservable(mtt):
        INFO("failed to add mtt observable")
        return False
    
    mtt_truth = HWWMtautauObs("mttTruth")
    mtt_truth.setDoTruth(True)
    if not TQObservable.addObservable(mtt_truth):
        INFO("failed to add mttTruth variable")
        return False

    return True
