#include "CAFExample/HWWTriggerWeight.h"
// #define _DEBUG_
#include "QFramework/TQLibrary.h"
#include <algorithm>
#include <cassert>
// EDM includes:
#include "xAODEventInfo/EventInfo.h"

#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicSet.h"

ClassImp(HWWTriggerWeight)

//______________________________________________________________________________________________

HWWTriggerWeight::TriggerSetup::TriggerSetup(){
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWTriggerWeight::TriggerSetup::TriggerSetup(TString triggerPeriod, TString campaign){
  DEBUGclass("constructor called");
  m_triggerPeriod = triggerPeriod;
  m_campaign = campaign;
}

//______________________________________________________________________________________________

HWWTriggerWeight::TriggerSetup::~TriggerSetup(){
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWTriggerWeight::TriggerSetup::addTrigger(TString trigger){
  if (std::find(m_triggers.begin(), m_triggers.end(), trigger) != m_triggers.end()){
    WARNclass("The trigger %s has already been added to the period %s before. Please make sure you're using this method correctly. Not adding the trigger again.", trigger.Data(), m_triggerPeriod.Data());
    return;
  }

  m_triggers.push_back(trigger);
}

//______________________________________________________________________________________________

void HWWTriggerWeight::TriggerSetup::addElectronToolSetup(TString triggerID, TString electronToolSetupString){
  if (m_electronToolSetup.find(triggerID) != m_electronToolSetup.end()){
    ERRORclass("The trigger ID %s has already been added to the period %s. Please add them all in one go (separated by commas)! Exiting.", triggerID.Data(), m_triggerPeriod.Data());
    throw std::runtime_error("");
  }

  m_electronToolSetup[triggerID] = electronToolSetupString;
}

//______________________________________________________________________________________________

std::vector<HWWTriggerWeight::TriggerSetup> HWWTriggerWeight::triggerSetups = std::vector<HWWTriggerWeight::TriggerSetup>();

//______________________________________________________________________________________________

HWWTriggerWeight::HWWTriggerWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWTriggerWeight::HWWTriggerWeight(int nLeptonWeights){
  DEBUGclass("constructor called with nLeptonWeights=%d", nLeptonWeights);
  m_nLeptonWeights = nLeptonWeights;
}

//______________________________________________________________________________________________

HWWTriggerWeight::HWWTriggerWeight(const TString& expression, int nLeptonWeights):
  HWWSFBase(expression)
{
  DEBUGclass("constructor called with '%s' and nLeptonWeights=%d",expression.Data(), nLeptonWeights);
  this->SetName(TQObservable::makeObservableName(expression));
  this->setExpression(expression);
  m_nLeptonWeights = nLeptonWeights;
}

//______________________________________________________________________________________________

HWWTriggerWeight::~HWWTriggerWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

HWWTriggerWeight::TriggerSetup& HWWTriggerWeight::createNewTriggerSetup(TString triggerPeriod, TString campaign){
  DEBUGclass("createNewTriggerSetup with triggerPeriod %s and campaign %s", triggerPeriod.Data(), campaign.Data());
  HWWTriggerWeight::triggerSetups.emplace_back(triggerPeriod, campaign);
  return HWWTriggerWeight::triggerSetups.back();
}

//______________________________________________________________________________________________

void HWWTriggerWeight::defineVariationTriggerElectrons(const TString& variationTypeMatchElectrons, const TString& variationNameMatchElectrons){
  DEBUGclass("Defining variation trigger for %s", this->getExpression().Data());
  this->m_variationTypeMatchElectrons = variationTypeMatchElectrons;
  if (variationNameMatchElectrons.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatchElectrons.");
    return;
  }
  this->m_variationNameMatchElectrons = variationNameMatchElectrons;
}

//______________________________________________________________________________________________

void HWWTriggerWeight::defineVariationTriggerMuons(const TString& variationTypeMatchMuons, const TString& variationNameMatchMuons){
  DEBUGclass("Defining variation trigger for %s", this->getExpression().Data());
  this->m_variationTypeMatchMuons = variationTypeMatchMuons;
  if (variationNameMatchMuons.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatchMuons.");
    return;
  }
  this->m_variationNameMatchMuons = variationNameMatchMuons;
}

//______________________________________________________________________________________________

template<class T> bool HWWTriggerWeight::varyTools(ToolHandleArray<T>& tools){
  //CP::ISystematicsTool
  bool atLeastOneToolWasVaried = false;
  const CP::SystematicSet& systematicSet = CP::SystematicSet(m_variationName.Data());
  for (ToolHandle<T>& t: tools){
    if (t->applySystematicVariation(systematicSet) == CP::SystematicCode::Ok)
      atLeastOneToolWasVaried = true;
    else
      WARNclass("The tool %s does not like the systematic variation %s. Continue processing without this variation.", t.name(), m_variationName.Data());
  }
  return atLeastOneToolWasVaried;
}

//______________________________________________________________________________________________

void HWWTriggerWeight::varyElectronTools(){
  DEBUGclass("Variation requested in %s with variationName %s and variationNameMatchElectrons %s",this->getExpression().Data(), m_variationName.Data(), m_variationNameMatchElectrons.Data());
  DEBUGclass("Applying systematic variation %s to electron tools", m_variationName.Data());

  if (varyTools(electronEffToolsHandles) || varyTools(electronSFToolsHandles))
    registerVariation();
  return;
}

//______________________________________________________________________________________________

void HWWTriggerWeight::varyMuonTools(){
  DEBUGclass("Variation requested in %s with variationName %s and variationNameMatchMuons %s",this->getExpression().Data(), m_variationName.Data(), m_variationNameMatchMuons.Data());
  DEBUGclass("Applying systematic variation %s to muon tool", m_variationName.Data());

  if (varyTools(muonToolsHandles))
    registerVariation();
  return;
}

//______________________________________________________________________________________________

void HWWTriggerWeight::registerVariations(int nVariations){
  DEBUGclass("registering %d variations", nVariations);
  for (int i = 0; i < nVariations; i++)
    registerVariation();
  return;
}

//______________________________________________________________________________________________

bool HWWTriggerWeight::initializeSF(){
  // This class initializes the TrigGlobalEfficiencyCorrectionTool and the
  // necessary electron and muon tools. Since the initialization of tools
  // is quite expensive, we check in the beginning if previously initialized
  // tools can be reused.
  // All tool handles are shared. This means that if a tool handle is recreated
  // with the same name as a previous tool, this tool will be shared. We use
  // this to our benefit.

  TString CandName = "";
  if(!this->fSample->getTagString("~cand",CandName)) return false;
  this->mCandName = "Event"+CandName;

  TString campaign = this->fSample->getTagStringDefault("~campaign", "");
  bool campaignTagDefined = true;
  if (campaign.IsNull())
    campaignTagDefined = false;

  // check if m_variationType indicates that the trigger SF should be varied
  bool variationRequestedElectrons = ((m_variationType == m_variationTypeMatchElectrons) && (m_variationName.BeginsWith(m_variationNameMatchElectrons)));
  bool variationRequestedMuons = ((m_variationType == m_variationTypeMatchMuons) && (m_variationName.BeginsWith(m_variationNameMatchMuons)));

  TString triggerToolName = "TrigGlobalEfficiencyCorrectionTool/GlobalTriggerTool_";
  triggerToolName.Append(campaign);
  triggerToolName.Append("_");
  if ((variationRequestedElectrons) || (variationRequestedMuons))
    triggerToolName.Append(m_variationName);
  else{
    triggerToolName.Append("nominal");
  }

  // If previous sample was from the same campaign, then we can just reuse the
  // trigger tool.
  if (!m_initializedForCampaign.IsNull() && m_initializedForCampaign.EqualTo(campaign)){
    if (!m_triggerTool.isInitialized()){
      // Seen this case when restricting channels. Not sure why this happens, but things are probably initialized for one channel and not for the other.
      DEBUGclass("Trigger tool is not initialized. This is unexpected, but can possibly be recovered by setting up again. Otherwise this can cause a crash in getValue(...).");
      m_initializedForCampaign = "";
    }
    else{
      // this check ensures that varyElectronTools and varyMuonTools has been called properly.
      if (!triggerToolName.EndsWith(m_triggerTool.name().c_str())){
        ERRORclass("Trigger tool has unexpected name (%s, whereas %s was expected). A wrong systematic variation could be applied or the tool could be set up for the wrong mc campaign.", m_triggerTool.name().c_str(), triggerToolName.Data());
      }
      DEBUGclass("Not re-initializing tools for trigger weight. They are already set up for campaign %s.", campaign.Data());

      // Make sure that the variations are registered with HWWSFBase
      // This can lead to slightly different warnings for samples which reuse
      // tools (i.e. here we assume that the variation works fine, even though
      // it is possible that varyTools() returned false. However, the correct
      // warning message will be printed when processing the first sample.
      registerVariations(variationRequestedElectrons + variationRequestedMuons);
      return true;
    }
  }

  // If a tool with the exact same name is already user-configured, then
  // use it (allows to share same tool between different channels (eg. em/me)).
  m_triggerTool = asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool>((const std::string) triggerToolName);
  if (m_triggerTool.isUserConfigured()){
    DEBUGclass("The tool %s is shared and has been configured before. Don't reconfigure it.", triggerToolName.Data());
    if (m_triggerTool.initialize() == StatusCode::SUCCESS){
      m_initializedForCampaign = campaign;
      // Make sure that the variations are registered with HWWSFBase
      // This can lead to slightly different behaviour for samples which reuse
      // tools.
      registerVariations(variationRequestedElectrons + variationRequestedMuons);
      return true;
    }
    else{
      WARNclass("The trigger tool %s was supposed to be re-used for this input sample. But its initialize method is not successful. We are going to re-create the trigger tool and all corresponding electron/muon tools. This shouldn't cause a problem to your analysis, but might make it quite a bit slower. Try to understand this by running this class in debug mode.", m_triggerTool.name().c_str());
    }
  }

  if (m_nLeptonWeights < 1)
    ERRORclass("The trigger weight is set up to calculate trigger scale factors for %d leptons. Please use a constructor to set the number of leptons to consider for the trigger weight. For now, this observable will eat up your resources and just return 1.", m_nLeptonWeights);

  // Everything up to here tried to re-use tools that were initialized
  // before. At this point, we delete all of them and create them from
  // scratch.
  electronEffToolsHandles.clear();
  electronSFToolsHandles.clear();
  muonToolsHandles.clear();
  electronToolsFactory.clear();
  muonToolsFactory.clear();
  legsPerTool.clear();
  m_initializedForCampaign = "";

  // triggerCombination will be passed to the trigger tool
  // key: trigger period
  // entry: all triggers of the period combined with OR (" || ")
  std::map<std::string, std::string> triggerCombination;

  if (HWWTriggerWeight::triggerSetups.size() == 0){
    ERRORclass("No trigger setups defined.");
    return false;
  }

  // Loop over all triggerSetups and find out if different campaigns are
  // defined. Only consider campaigns that have non-empty trigger keys.
  bool noCampaignsDefined = true;
  bool differentCampaignsDefined = false;
  TString tmp_campaign = "";
  bool setupsWithoutTriggerKeysPresent = false;
  for (auto& triggerSetup: HWWTriggerWeight::triggerSetups){
    if (triggerSetup.m_triggers.size() == 0){
      setupsWithoutTriggerKeysPresent = true;
      WARNclass("There are no trigger keys defined for trigger period %s. Ignoring this period.", triggerSetup.m_triggerPeriod.Data());
      continue;
    }
    if (noCampaignsDefined){
      tmp_campaign = triggerSetup.m_campaign;
      noCampaignsDefined = false;
    }
    else if (!tmp_campaign.EqualTo(triggerSetup.m_campaign)){
      differentCampaignsDefined = true;
      break;
    }
  }

  bool initialized = false;
  int nElectronTools = -1;
  // for each triggerSetup, we need to combine the triggers and set up electron tools
  for (auto& triggerSetup: HWWTriggerWeight::triggerSetups){
    if (triggerSetup.m_campaign != campaign){
      if (differentCampaignsDefined){
        DEBUGclass("Skipping trigger setup for %s, because it does not match the current campaign name %s.", triggerSetup.m_campaign.Data(), campaign.Data());
	continue;
      }
      else{
	// If there is only one trigger setup and no campaign defined on the
	// sample folder, then the user probably analyzes only one cmpaign.
	// Otherwise, print a warning message.
	if (!campaignTagDefined || triggerSetup.m_campaign.EqualTo("")){
	  WARNclass("No campaign tag is defined for the current sample. Since we only have one trigger setup available '%s' (trigger period '%s'), we'll use it and hope for the best.", triggerSetup.m_campaign.Data(), triggerSetup.m_triggerPeriod.Data());
	  INFOclass("If the trigger setup matches your samples, you can suppress this warning. To do so, please set a tag in your sample folder like <campaign = %s>.", triggerSetup.m_campaign.Data());
          INFOclass("If this trigger setup does not match your samples, make sure that you have the correct triggers defined in HWWTriggerWeight.py.");
	  INFOclass("This warning is printed once per trigger period and systematic variation, but won't be reprinted for other samples of this campaign.");
	}
      }
    }

    DEBUGclass("Initializing campaign: %s", triggerSetup.m_campaign.Data());
    DEBUGclass("and trigger period: %s", triggerSetup.m_triggerPeriod.Data());

    if (triggerSetup.m_triggers.size() == 0){
      DEBUGclass("Skipping trigger setup %s", triggerSetup.m_campaign.Data());
      continue;
    }

    // construct here the trigger expression for each trigger period
    TString triggerExpression = "";
    for (auto& trigger: triggerSetup.m_triggers){
      triggerExpression.Append(trigger);
      triggerExpression.Append(" || ");
    }
    TQStringUtils::removeTrailingText(triggerExpression, " || ");
    DEBUGclass("Using trigger expression %s.", triggerExpression.Data());
    triggerCombination[triggerSetup.m_triggerPeriod.Data()] = triggerExpression.Data();

    // set up electron tools
    for (auto& eSetup: triggerSetup.m_electronToolSetup){
      nElectronTools++;
      for (int j = 0; j < 2; ++j){ // we need two tools, one for efficiencies, one for scale factors
	TString name = "";
	if (j == 0)
	  name = "AsgElectronEfficiencyCorrectionTool/ElTrigSF_";
	else if (j == 1)
	  name = "AsgElectronEfficiencyCorrectionTool/ElTrigEff_";
	name.Append(TQStringUtils::makeValidIdentifier(m_variationName, TQStringUtils::alphanum+"_"));
	name.Append("_");
	name.Append(campaign);
	name.Append("_");
	name.Append(std::to_string(nElectronTools));
	electronToolsFactory.emplace_back(name.Data());
	asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool>& t = electronToolsFactory.back();
        // The electron tool picks the most recent map file automatically. The following line can overwrite the default
        // t.setProperty("MapFilePath", "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map1.txt").ignore();

	DEBUGclass("Setting TriggerKey of %s to %s", name.Data(), eSetup.second.Data());
	t.setProperty("TriggerKey", (TString(j==1?"Eff_":"")+eSetup.second).Data()).ignore();

	// todo: read IdKey and IsoKey from lepton id helper
	t.setProperty("IdKey","Tight").ignore();
	t.setProperty("IsoKey","FCTight").ignore();
	t.setProperty("CorrelationModel","TOTAL").ignore();
	// t.setProperty("OutputLevel", MSG::DEBUG).ignore();

	/*
	  If you see an error message at this point like
	  "Key does not exist in the map file, Please configure it properly.."
	  it means that the key
	  <TriggerKey>_<IdKey>_<IsoKey>
	  cannot be found before the "=" in the file
	  /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ElectronEfficiencyCorrection/2015_2017/rel21.2/Consolidation_September2018_v1/map1.txt.
	  The property IsoKey is not currently used, because this file uses
	  inconsistent naming of isolation WP. Sometimes, they use the pattern
	  FCTight, sometimes they use FixedCutTight.
	*/

	if(t.retrieve() != StatusCode::SUCCESS){
	  ERRORclass("Unable to initialize electron tool %s!", name.Data());
	  return false;
	}

	// the ToolHandleArrays are needed for the global trigger tool
	auto toolHandle = t.getHandle();
	if (j == 0)
	  electronSFToolsHandles.push_back(toolHandle);
	else
	  electronEffToolsHandles.push_back(toolHandle);

	DEBUGclass("Mapping tool %s TriggerKey to the string: %s", toolHandle.name().c_str(), eSetup.first.Data());
	legsPerTool.emplace(toolHandle.name(), eSetup.first);
      }
    }

    initialized = true;
  }

  if (!initialized){
    // Something is wrong (no electron tool was initialized).
    WARNclass("No trigger setup matched the current campaign %s. Available are:", campaign.EqualTo("") ? "<empty string>" : campaign.Data());
    for (auto& triggerSetup:HWWTriggerWeight::triggerSetups){
      INFOclass("  %s (trigger period %s)", triggerSetup.m_campaign.Data(), triggerSetup.m_triggerPeriod.Data());
    }
    if (setupsWithoutTriggerKeysPresent){
      INFOclass("This warning could also be caused by a trigger setup which has no trigger keys defined (see warnings earlier).");
    }

    // Pretend that the tool was varied to suppress a warning message from HWWSFBase.
    // (the warning message does not help to trace back the problem and is caused
    // by this more general one).
    registerVariations(variationRequestedElectrons + variationRequestedMuons);
    WARNclass("Trigger weight will be set to 1 for this sample and campaign %s", campaign.Data());
    m_returnOneForThisSample = true;
    return true;
  }

  // set up muon tool
  muonToolsFactory.emplace_back();
  asg::AnaToolHandle<CP::IMuonTriggerScaleFactors>& t = muonToolsFactory.back();
  ASG_SET_ANA_TOOL_TYPE(t, CP::MuonTriggerScaleFactors);
  TString name = "MuonTrigEff_";
  name.Append(TQStringUtils::makeValidIdentifier(m_variationName, TQStringUtils::alphanum+"_"));
  t.setName(name.Data());
  // todo: read MuonQuality from lepton id helper
  t.setProperty("MuonQuality", "Tight").ignore();
  t.setProperty("OutputLevel", MSG::WARNING).ignore();
  // t.setProperty("OutputLevel", MSG::DEBUG).ignore();

  if(t.retrieve() != StatusCode::SUCCESS){
    ERRORclass("Unable to initialize muon tool %s!", name.Data());
    return false;
  }

  muonToolsHandles.push_back(t.getHandle());

  if (variationRequestedElectrons)
    varyElectronTools();
  if (variationRequestedMuons)
    varyMuonTools();

  m_triggerTool.setProperty("ElectronEfficiencyTools",electronEffToolsHandles).ignore();
  m_triggerTool.setProperty("ElectronScaleFactorTools",electronSFToolsHandles).ignore();
  m_triggerTool.setProperty("MuonTools",muonToolsHandles).ignore();
  m_triggerTool.setProperty("TriggerCombination", triggerCombination).ignore();
  m_triggerTool.setProperty("ListOfLegsPerTool",legsPerTool).ignore();
  m_triggerTool.setProperty("OutputLevel", MSG::WARNING).ignore();
  // m_triggerTool.setProperty("OutputLevel", MSG::DEBUG).ignore();

  if(m_triggerTool.initialize() != StatusCode::SUCCESS){
    ERRORclass("Unable to initialize trigger tool!");
    return false;
  }
  m_initializedForCampaign = campaign;
  DEBUGclass("All necessary electron, muon and trigger tools have successfully been reconfigured. Leaving this method.");
  return true;
}



//______________________________________________________________________________________________

double HWWTriggerWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  if (m_returnOneForThisSample)
    return 1.0;

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double triggerSF = 1.0;

  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(triggerSF, cand, evt, evtInfo))
    return triggerSF;

  int nParts = evt->nParts();
  int nOtherParts = evt->nOtherParts();
  int nLeptonsAdded = 0;

  std::vector<const xAOD::Electron*> myTriggeringElectrons;
  std::vector<const xAOD::Muon*> myTriggeringMuons;

  for(int iPart=0; iPart < nParts ; iPart++){
    if (nLeptonsAdded >= m_nLeptonWeights)
      break;
    const xAOD::IParticle* ipart = evt->part(iPart);
    if (ipart->type() == xAOD::Type::Electron){
      const xAOD::Electron* electrons = dynamic_cast<const xAOD::Electron*>(ipart);
      myTriggeringElectrons.push_back(electrons);
      nLeptonsAdded++;
      DEBUGclass("Elec with pt = %10.2f, eta = %5.2f", electrons->pt(), electrons->eta());
    }
    if (ipart->type() == xAOD::Type::Muon){
      const xAOD::Muon* muons = dynamic_cast<const xAOD::Muon*>(ipart);
      myTriggeringMuons.push_back(muons);
      nLeptonsAdded++;
      DEBUGclass("Muon with pt = %10.2f, eta = %5.2f", muons->pt(), muons->eta());
    }
  }

  for(int iOtherPart=0; iOtherPart < nOtherParts ; iOtherPart++){
    if (nLeptonsAdded >= m_nLeptonWeights)
      break;
    const xAOD::IParticle* ipart = evt->otherPart(iOtherPart);
    if (ipart->type() == xAOD::Type::Electron){
      const xAOD::Electron* electrons = dynamic_cast<const xAOD::Electron*>(ipart);
      myTriggeringElectrons.push_back(electrons);
      nLeptonsAdded++;
      DEBUGclass("Elec with pt = %10.2f, eta = %5.2f", electrons->pt(), electrons->eta());
    }
    if (ipart->type() == xAOD::Type::Muon){
      const xAOD::Muon* muons = dynamic_cast<const xAOD::Muon*>(ipart);
      myTriggeringMuons.push_back(muons);
      nLeptonsAdded++;
      DEBUGclass("Muon with pt = %10.2f, eta = %5.2f", muons->pt(), muons->eta());
    }
  }

  if (myTriggeringElectrons.size() + myTriggeringMuons.size() == 0){
    DEBUGclass("No leptons found for trigger scale factor. Returning 1.");
    return 1.;
  }

  // auto cc = m_triggerTool->getEfficiencyScaleFactor(evtInfo->auxdata<unsigned int>("RandomRunNumber"), p, triggerSF);
  auto cc = m_triggerTool->getEfficiencyScaleFactor(myTriggeringElectrons, myTriggeringMuons, triggerSF);
  if (cc != CP::CorrectionCode::Ok){
    WARNclass("Scale factor evaluation failed. Returning 1. Event number: %d", this->getCurrentEntry());
    if (cc == CP::CorrectionCode::OutOfValidityRange){
      INFOclass("The warning of the trigger tool might have been caused by a low trigger threshold. The kinematics of the leptons might be of interest:");
      for (auto& p: myTriggeringElectrons)
        INFOclass("Elec with pt = %10.2f, eta = %5.2f, phi = %5.2f, E = %10.2f", p->pt(), p->eta(), p->phi(), p->e());
      for (auto& p: myTriggeringMuons)
        INFOclass("Muon with pt = %10.2f, eta = %5.2f, phi = %5.2f, E = %10.2f", p->pt(), p->eta(), p->phi(), p->e());
      if (++m_outOfValidityRangeCount >= 1000){
        BREAKclass("This is the 1000th warning message of this type. Exiting to prevent your logfile from exploding.");
      }
    }
    if (cc == CP::CorrectionCode::Error){
      INFOclass("The warning of the trigger tool might have been caused by a mistake in the trigger setup (the trigger period for which the tool is set up does not match the run number of the event).");
      if (++m_errorCount >= 10){
        BREAKclass("This is the 10th error message of this type. It will likely not get any better. Exiting.");
      }
    }
  }

  DEBUGclass("Trigger scale factor: %f", triggerSF);
  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = triggerSF;
  return triggerSF;
}

//______________________________________________________________________________________________

bool HWWTriggerWeight::finalizeSF(){
  if (m_returnOneForThisSample){
    WARNclass("The trigger scale factor for this sample was forced to 1.");
    m_returnOneForThisSample = false;
  }
  return true;
}

//______________________________________________________________________________________________

const TString& HWWTriggerWeight::getExpression() const {
  return this->fExpression;
}

//______________________________________________________________________________________________

bool HWWTriggerWeight::hasExpression() const {
  return true;
}

//______________________________________________________________________________________________

void HWWTriggerWeight::setExpression(const TString& expr){
  this->fExpression = expr;
}
