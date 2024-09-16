from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWMuonWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    useMuonSF = config.getTagBoolDefault("UseMuonSF", True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    # We distinguish between regular muons and low-pT muons. The cut
    # is hard-coded in HWWMuonWeight::getMuonWeight(). The two muon
    # types have different quality requirements.
    # In the systematic variations, regular and LOWPT muons are
    # treated differently. Here, however, the LOWPT cut is set by the
    # muon CP group and is different. We treat these two definitions
    # independently.

    muonQuality = config.getTagStringDefault("MuonQuality", "Tight").Data()
    muonQualityLowPt = config.getTagStringDefault("MuonQualityLowPt", "Tight").Data()

    if muonQuality == ""  or muonQualityLowPt == "":
        return False

    muonDecorationName = "effiSF" + muonQuality
    muonDecorationNameLowPt = "effiSF" + muonQualityLowPt

    muonRecoWeight = HWWMuonWeight("muonRecoWeight", muonDecorationName, muonDecorationNameLowPt, 25e3)
    # The trailing S matches both "SYS" and "STAT" and is needed to
    # distinguish reconstruction variations from trigger variations,
    # which are named MUON_EFF_Trig.
    variationType = "xAODsfSystematics"
    variationNameMatch = "MUON_EFF_RECO_S"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        muonRecoWeight.defineVariationTrigger(variationType, variationNameMatch)
        muonRecoWeight.defineVariationTriggerLowPt(variationType, variationNameMatch)

    if not useMuonSF or mcWeightsOnly:
        muonRecoWeight.setUseWeight(False)

    if not TQObservable.addObservable(muonRecoWeight, "muonRecoWeight"):
        WARN("failed to add observable '{:s}'".format(muonRecoWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWMuonRecoWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
