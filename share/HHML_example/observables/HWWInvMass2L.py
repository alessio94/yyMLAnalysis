from QFramework import TQObservable,INFO,ERROR,BREAK,WARN


from CAFExample import HWWInvMass2L

def addObservables(config):

  INFO("adding invariant mass observable")

  invmass_l0l1 = HWWInvMass2L("invMassl0l1", 0, 1)
  invmass_l0otherPart0= HWWInvMass2L("invMassl0otherPart0", 0, 2)
  invmass_l1otherPart0= HWWInvMass2L("invMassl1otherPart0", 1, 2)

  if not TQObservable.addObservable(invmass_l0l1):
    INFO("failed to add invariant mass Observable")
    return False
  if not TQObservable.addObservable(invmass_l0otherPart0):
    INFO("failed to add invariant mass Observable")
    return False
  if not TQObservable.addObservable(invmass_l1otherPart0):
    INFO("failed to add invariant mass Observable")
    return False

  return True

if __name__ == "__main__":
  tags = TQTaggable()
  if addObservables(tags):
    print("Successfully added invariant mass observables")
  else:
    ERROR("Failed to add invariant mass observables")
