from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWPRWWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    usePRWTool = config.getTagBoolDefault("UsePRWTool", True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    prwWeight = HWWPRWWeight("prwWeight", "PileupWeight")
    variationType = "xAODsfSystematics"
    variationNameMatch = "PRW_DATASF"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        prwWeight.defineVariationTrigger(variationType, variationNameMatch)

    if not usePRWTool or mcWeightsOnly:
        prwWeight.setUseWeight(False)

    if not TQObservable.addObservable(prwWeight, "prwWeight"):
        WARN("failed to add observable '{:s}'".format(prwWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWPRWWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
