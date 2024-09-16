from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWMuonWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    useMuonIsoSF = config.getTagBoolDefault("UseMuonIsoSF", True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    muonIsoWP = config.getTagStringDefault("muonIsolationWP", "FCTight").Data()
    if muonIsoWP == "":
        return False

    noMuonIsoSF = muonIsoWP in ['Scan', 'NoIso', 'PLVTight', 'PLVTight_ttH']

    if muonIsoWP == "Rel20.7" or noMuonIsoSF:
        from CAFExample import HWWMuonIsoWeight
        muonIsoWeight = HWWMuonIsoWeight("muonIsoWeight", "effiSFIsoGradient")
        from ROOT import TFile
        muonisosfpath_abs = common.findConfigPath("auxData/calibration/Iso_HWWIsoIso_Z.root")
        muonisosffile = TFile.Open(muonisosfpath_abs,"READ")
        muonIsoWeight.setMuonIsoScaleFactors(muonisosffile.Get("SF_All"),muonisosffile.Get("SF_sys_All"))
        if noMuonIsoSF:
            WARN("Using muonIsoWeight 1, because scale factor is allegedly not available (if you think it should be available, fix the hard-coded list in HWWMuonIsoWeight.py).")
            muonIsoWeight.setUseWeight(False)
    else:
        muonIsoWeight = HWWMuonWeight("muonIsoWeight", "effiSFIso" + muonIsoWP)


    variationType = "xAODsfSystematics"
    variationNameMatch = "MUON_EFF_ISO"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        muonIsoWeight.defineVariationTrigger(variationType, variationNameMatch)

    if not useMuonIsoSF or mcWeightsOnly:
        muonIsoWeight.setUseWeight(False)

    if not TQObservable.addObservable(muonIsoWeight, "muonIsoWeight"):
        WARN("failed to add observable '{:s}'".format(muonIsoWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWMuonIsoWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
