import QFramework

from CAFExample import PtLep1SquaredObs

def addObservables():

  myObs = PtLep1SquaredObs("PtLep1Squared")
  if not QFramework.TQTreeObservable.addObservable(myObs):
    return False

  return True

if __name__ == "__main__":
  print("Inside main of PtLep1SquaredObs snippet, will now call addObservables")
  addObservables()
