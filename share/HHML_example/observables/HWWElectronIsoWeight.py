from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWElectronWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    electronID = config.getTagStringDefault("ElectronID", "MediumLH").Data()
    electronIDLowPt = config.getTagStringDefault("ElectronIDLowPt", "TightLH").Data()
    electronIsoWP = config.getTagStringDefault("electronIsolationWP", "FCTight").Data()

    if electronIsoWP == "" or electronID == ""  or electronIDLowPt == "":
        return False

    # list of electron WPs for which no SF is available in V20 PAODs (update for future versions)
    noElectronIsoSF = electronIsoWP in ['Scan', 'NoIso', 'PLVTight', 'FixedCutPflowLoose', 'FixedCutPflowTight', 'FCTightTrackOnly_FixedRad', 'FCLoose_FixedRad', 'FCTight_FixedRad', 'PLVTight_ttH']

    electronIsoWPDecorationName = "effiSFIso" + electronIsoWP + "_wrt"
    electronIsoWPDecorationNameLowPt = electronIsoWPDecorationName + electronIDLowPt
    electronIsoWPDecorationName      = electronIsoWPDecorationName + electronID

    electronIsoWeight = HWWElectronWeight("electronIsoWeight", electronIsoWPDecorationName, electronIsoWPDecorationNameLowPt, 25e3)
    variationType = "xAODsfSystematics"
    variationNameMatch = "EL_EFF_Iso"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        electronIsoWeight.defineVariationTrigger(variationType, variationNameMatch)
        electronIsoWeight.defineVariationTriggerLowPt(variationType, variationNameMatch)

    if electronIsoWP == "Rel20.7" or noElectronIsoSF:
        WARN("Using electronIsoWeight 1, because scale factor is allegedly not available (if you think it should be available, fix the hard-coded list in HWWElectronIsoWeight.py).")
        electronIsoWeight.setUseWeight(False)

    if mcWeightsOnly:
        electronIsoWeight.setUseWeight(False)

    if not TQObservable.addObservable(electronIsoWeight, "electronIsoWeight"):
        WARN("failed to add observable '{:s}'".format(electronIsoWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWElectronIsoWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
