from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

#todo: remove HWWForwardJVTWeight once PxAOD naming is changed
from CAFExample import HWWJVTWeight, HWWForwardJVTWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    jvtWeight = HWWJVTWeight("jvtWeight", "effiSFJVT")
    variationType = "xAODsfSystematics"
    variationNameMatch = "JET_JvtEfficiency"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        jvtWeight.defineVariationTrigger(variationType, variationNameMatch)
    
    forwardJVTWeight = HWWForwardJVTWeight("forwardJVTWeight", "effiSFForwardJVT")
    variationType = "xAODsfSystematics"
    variationNameMatch = "JET_fJvtEfficiency"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        forwardJVTWeight.defineVariationTrigger(variationType, variationNameMatch)

    if mcWeightsOnly:
        jvtWeight.setUseWeight(False)
        forwardJVTWeight.setUseWeight(False)

    if not TQObservable.addObservable(jvtWeight, "jvtWeight"):
        WARN("failed to add observable '{:s}'".format(jvtWeight.getExpression().Data()))
        return False

    if not TQObservable.addObservable(forwardJVTWeight, "forwardJVTWeight"):
        WARN("failed to add observable '{:s}'".format(forwardJVTWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWJVTWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
