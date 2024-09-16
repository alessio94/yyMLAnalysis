from QFramework import TQTreeObservable, INFO
import ROOT

from CAFExample import MjjMaxObservable

def addObservables():
	
    obs = MjjMaxObservable("MjjMax")
    if not TQObservable.addObservable(obs, "MjjMax"):
        INFO("failed to add MjjMax Observable")
        return False
    return True;
