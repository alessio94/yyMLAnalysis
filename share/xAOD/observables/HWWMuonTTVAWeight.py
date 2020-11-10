from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWMuonTTVAWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    useMuonTTVASF = config.getTagBoolDefault("UseMuonTTVASF", True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    muonTTVAWeight = HWWMuonTTVAWeight("muonTTVAWeight", "effiSFTTVA")
    variationType = "xAODsfSystematics"
    variationNameMatch = "MUON_EFF_TTVA"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        muonTTVAWeight.defineVariationTrigger(variationType, variationNameMatch)

    if not useMuonTTVASF or mcWeightsOnly:
        muonTTVAWeight.setUseWeight(False)

    if not TQObservable.addObservable(muonTTVAWeight, "muonTTVAWeight"):
        WARN("failed to add observable '{:s}'".format(muonTTVAWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWMuonTTVAWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
