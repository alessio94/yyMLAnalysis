from QFramework import TQObservable,WARN

from CAFExample import HWWBTagWeightNoDeco
from CommonAnalysisHelpers import common

def addObservables(config):

    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)
    BTagName = "DL1r"
    BTagWP = "FixedCutBEff_85"
    JetAuthor = config.getTagStandardStringDefault("jetCollection", "AntiKt4EMPFlowJets")  
    # Append the extra BTagging information for jetauthor
    Suffix = ""
    if JetAuthor == "AntiKt4EMPFlowJets":
        Suffix = "_BTagging201903"
    elif JetAuthor == "AntiKt4EMTopoJets":
        Suffix = "_BTagging201810"
    else:
        Suffix = ""

    JetAuthor = JetAuthor + Suffix

    bTagWeight20 = HWWBTagWeightNoDeco("bTagWeightOTF20", BTagName, BTagWP, 20000., 2.5, JetAuthor)
    bTagWeight30 = HWWBTagWeightNoDeco("bTagWeightOTF30", BTagName, BTagWP, 30000., 2.5, JetAuthor)

    variationType = "xAODsfSystematics"
    variationNameMatch = "FT_EFF_"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        bTagWeight20.defineVariationTrigger(variationType, variationNameMatch)
        bTagWeight30.defineVariationTrigger(variationType, variationNameMatch)

    if mcWeightsOnly:
        bTagWeight20.setUseWeight(False)
        bTagWeight30.setUseWeight(False)

    if not TQObservable.addObservable(bTagWeight20):
        WARN("failed to add observable '{:s}'".format(bTagWeight20.getExpression().Data()))
        return False
    if not TQObservable.addObservable(bTagWeight30):
        WARN("failed to add observable '{:s}'".format(bTagWeight30.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWBTagWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
