from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

from CAFExample import HWWTriggerWeight
from CommonAnalysisHelpers import common

def addObservables(config):

    useTriggerSF = config.getTagBoolDefault("UseTriggerSF", True)
    mcWeightsOnly = config.getTagBoolDefault("UseMCWeightsOnly", False)

    # For all regular analyses, we want to consider the trigger scale factors of
    # the leading 2 leptons. For the ZjetsFF estimate, 3 leptons are needed.
    nLeptonTriggerSFWeights = config.getTagIntegerDefault("nLeptonTriggerSFWeights", 3)
    triggerCombination = config.getTagStringDefault("triggerCombination", "singleLepton").Data()

    if triggerCombination not in ["singleAndDilepton", "singleLepton"]:
        WARN("Trigger combination " + triggerCombination + " not known. This could mess up the trigger weights.")

    # The following sets of four lines are the 2015, 2016, 2017, 2018 triggers
    # and their respective setup in the electron tool.
    # (some of the lines are expected to be the same)
    # References:
    # unprescaled triggers: https://twiki.cern.ch/twiki/bin/view/Atlas/LowestUnprescaled
    # trigger combination:  https://twiki.cern.ch/twiki/bin/view/Atlas/TrigGlobalEfficiencyCorrectionTool
    # electron triggers:    https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/XAODElectronEfficiencyCorrectionTool
    # muon triggers:        https://twiki.cern.ch/twiki/bin/view/Atlas/MuonTriggerPhysicsRecommendationsRel212017

    # The electron tools need to be initialized for each trigger leg individually.
    # In the following, we define a list of trigger keys and the corresponding
    # identifier for the electron tool.

    electronToolSetup = {}
    electronToolSetup["2015"] = {}
    electronToolSetup["2016"] = {}
    electronToolSetup["2017"] = {}
    electronToolSetup["2018"] = {}

    # /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map1.txt
    # e triggers
    electronToolSetup["2015"]["e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose"] \
    = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
    electronToolSetup["2016"]["e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"] \
    = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
    electronToolSetup["2017"]["e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"] \
    = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"
    electronToolSetup["2018"]["e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"] \
    = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0"

    # e mu triggers
    electronToolSetup["2015"]["e17_lhloose"]      = "MULTI_L_2015_e17_lhloose_2016_2018_e17_lhloose_nod0"
    electronToolSetup["2016"]["e17_lhloose_nod0"] = "MULTI_L_2015_e17_lhloose_2016_2018_e17_lhloose_nod0"
    electronToolSetup["2017"]["e17_lhloose_nod0"] = "MULTI_L_2015_e17_lhloose_2016_2018_e17_lhloose_nod0"
    electronToolSetup["2018"]["e17_lhloose_nod0"] = "MULTI_L_2015_e17_lhloose_2016_2018_e17_lhloose_nod0"

    # more e mu triggers
    electronToolSetup["2015"]["e7_lhmedium"]      = "MULTI_L_2015_e7_lhmedium_2016_2018_e7_lhmedium_nod0"
    electronToolSetup["2016"]["e7_lhmedium_nod0"] = "MULTI_L_2015_e7_lhmedium_2016_2018_e7_lhmedium_nod0"
    electronToolSetup["2017"]["e7_lhmedium_nod0"] = "MULTI_L_2015_e7_lhmedium_2016_2018_e7_lhmedium_nod0"
    electronToolSetup["2018"]["e7_lhmedium_nod0"] = "MULTI_L_2015_e7_lhmedium_2016_2018_e7_lhmedium_nod0"

    # Electron tool doesn't need mu triggers. So we're done :-)


    # Now come the trigger keys
    triggers = {}
    triggers["2015"] = []
    triggers["2016"] = []
    triggers["2017"] = []
    triggers["2018"] = []


    if triggerCombination == "singleAndDilepton" or triggerCombination == "singleLepton":
        # e triggers
        triggers["2015"].append("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose")
        triggers["2016"].append("e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0")
        triggers["2017"].append("e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0")
        triggers["2018"].append("e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0")

        # mu triggers
        triggers["2015"].append("mu20_iloose_L1MU15_OR_mu50")
        triggers["2016"].append("mu26_ivarmedium_OR_mu50")
        triggers["2017"].append("mu26_ivarmedium_OR_mu50")
        triggers["2018"].append("mu26_ivarmedium_OR_mu50")

    if triggerCombination == "singleAndDilepton":
        # e mu triggers
        triggers["2015"].append("e17_lhloose_mu14")
        triggers["2016"].append("e17_lhloose_nod0_mu14")
        triggers["2017"].append("e17_lhloose_nod0_mu14")
        triggers["2018"].append("e17_lhloose_nod0_mu14")

        # more e mu triggers
        #triggers["2015"].append("e7_lhmedium_mu24")
        #triggers["2016"].append("e7_lhmedium_nod0_mu24")
        #triggers["2017"].append("e7_lhmedium_nod0_mu24")
        #triggers["2018"].append("e7_lhmedium_nod0_mu24")



    mapping_triggerPeriodToMCCampaign = {}
    mapping_triggerPeriodToMCCampaign["2015"] = "c16a"
    mapping_triggerPeriodToMCCampaign["2016"] = "c16a"
    mapping_triggerPeriodToMCCampaign["2017"] = "c16d"
    mapping_triggerPeriodToMCCampaign["2018"] = "c16e"

    triggerWeight = HWWTriggerWeight("triggerWeight", nLeptonTriggerSFWeights)

    # The variable triggers holds trigger keys for all periods. The outer loop
    # iterates over these trigger periods (i.e. years).
    for triggerPeriod in triggers:
        if not triggerPeriod in electronToolSetup:
            ERROR("For the trigger period " + triggerPeriod + " no electronToolSetup has been defined. This could cause a crash.")
        # Create a trigger setup for this triggerPeriod. This instance will hold
        # all necessary information to calculate scale factors for this period.
        # triggerSetup is automatically registered in HWWTriggerWeight.
        triggerSetup = HWWTriggerWeight.createNewTriggerSetup(triggerPeriod, mapping_triggerPeriodToMCCampaign[triggerPeriod])

        # add triggers of this trigger period to the trigger setup
        for trigger in triggers[triggerPeriod]:
            triggerSetup.addTrigger(trigger)

        # For the electron tool, merge all entries that have the same value.
        # This is done so that the same electron tool can be shared between
        # different triggers (using the same trigger leg).
        electronToolSetupInPeriod = electronToolSetup[triggerPeriod]
        mergedElectronToolSetupInPeriod = {}
        for triggerKeyID in electronToolSetupInPeriod:
            electronToolSetupString = electronToolSetupInPeriod[triggerKeyID]
            # (effectively swap key, value pair)
            if electronToolSetupString in mergedElectronToolSetupInPeriod:
                mergedElectronToolSetupInPeriod[electronToolSetupString] = mergedElectronToolSetupInPeriod[electronToolSetupString] + "," + triggerKeyID
            else:
                mergedElectronToolSetupInPeriod[electronToolSetupString] = triggerKeyID
        for electronToolSetupString in mergedElectronToolSetupInPeriod:
            triggerKeyIDs = mergedElectronToolSetupInPeriod[electronToolSetupString]
            triggerSetup.addElectronToolSetup(triggerKeyIDs, electronToolSetupString)

    variationType = "xAODsfSystematics"
    variationNameMatch = "EL_EFF_Trigger"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        triggerWeight.defineVariationTriggerElectrons(variationType, variationNameMatch)
    variationNameMatch = "MUON_EFF_Trig"
    if common.isPresentVariationNameHead(config, variationType, variationNameMatch):
        triggerWeight.defineVariationTriggerMuons(variationType, variationNameMatch)

    if not useTriggerSF or mcWeightsOnly:
        triggerWeight.setUseWeight(False)

    if not TQObservable.addObservable(triggerWeight, "triggerWeight"):
        WARN("failed to add observable '{:s}'".format(triggerWeight.getExpression().Data()))
        return False

    return True

if __name__ == "__main__":
  print("Inside main of HWWTriggerWeight snippet, will now call addObservables")
  from QFramework import TQTaggable
  tags = TQTaggable()
  addObservables(tags)
