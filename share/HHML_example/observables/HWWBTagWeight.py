from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWBTagWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    useOtherJets = config.getTagBoolDefault("useSubThresholdBTagWeights",True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    bTagWeight = HWWBTagWeight("bTagWeight", "effiSF", useOtherJets)

    variationType = "xAODsfSystematics"
    variationNameMatch = "FT_EFF_"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        bTagWeight.defineVariationTrigger(variationType, variationNameMatch)

    if mcWeightsOnly:
        bTagWeight.setUseWeight(False)

    if not TQObservable.addObservable(bTagWeight, "bTagWeight"):
        WARN("failed to add observable '{:s}'".format(bTagWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWBTagWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
