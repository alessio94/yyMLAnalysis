from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWMCWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    mcWeight = HWWMCWeight("mcWeight")

    # Here, we will have several different names for variations.
    # ggF signal uncertainties: weight_qcd_wg1_*
    # top theory variations: ...
    # WW theory variations: ...
    # Therefore, we will define the variationNameMatch to something that will never be matched. This
    # also means that there will be no error checking at this stage. Don't worry, though! The code
    # will be able to cope with that and throw proper errors if non existing variations are parsed!
    variationType = "xAODsfSystematics"
    variationNameMatch = "noOfficialVariationWillEverHaveThisWeirdName"
    mcWeight.defineVariationTrigger(variationType, variationNameMatch)

    if not TQObservable.addObservable(mcWeight, "mcWeight"):
        WARN("failed to add observable '{:s}'".format(mcWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWMCWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
