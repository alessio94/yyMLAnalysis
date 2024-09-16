from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWElectronWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    useElectronSF = config.getTagBoolDefault("UseElectronSF", True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    electronRecoWeight = HWWElectronWeight("electronRecoWeight", "effiSFRecoTrk")
    variationType = "xAODsfSystematics"
    variationNameMatch = "EL_EFF_Reco"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        electronRecoWeight.defineVariationTrigger(variationType, variationNameMatch)

    if not useElectronSF or mcWeightsOnly:
        electronRecoWeight.setUseWeight(False)

    if not TQObservable.addObservable(electronRecoWeight, "electronRecoWeight"):
        WARN("failed to add observable '{:s}'".format(electronRecoWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWElectronRecoWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
