from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWElectronWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    electronID = config.getTagStringDefault("ElectronID", "MediumLH").Data()
    electronIDLowPt = config.getTagStringDefault("ElectronIDLowPt", "TightLH").Data()

    if electronID == ""  or electronIDLowPt == "":
        return False

    electronID = "effiSF"+electronID
    electronIDLowPt = "effiSF"+electronIDLowPt

    electronIDWeight = HWWElectronWeight("electronIDWeight", electronID, electronIDLowPt, 25e3)
    variationType = "xAODsfSystematics"
    variationNameMatch = "EL_EFF_ID"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        electronIDWeight.defineVariationTrigger(variationType, variationNameMatch)
        electronIDWeight.defineVariationTriggerLowPt(variationType, variationNameMatch)

    if mcWeightsOnly:
        electronIDWeight.setUseWeight(False)

    if not TQObservable.addObservable(electronIDWeight, "electronIDWeight"):
        WARN("failed to add observable '{:s}'".format(electronIDWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWElectronIDWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
