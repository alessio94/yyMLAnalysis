#include "yyMLAnalysisCode/HWWBTagWeightNoDeco.h"
#include "QFramework/TQUtils.h"

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingUtilities.h"

// Tool includes:
#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

// Systematics includes:
//#include "PATInterfaces/SystematicCode.h"
#include <AsgMessaging/StatusCode.h>
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/CorrectionCode.h"

// STL includes:
#include <cmath>

ClassImp(HWWBTagWeightNoDeco)

//______________________________________________________________________________________________

HWWBTagWeightNoDeco::HWWBTagWeightNoDeco() {
  // Default constructor
  DEBUGclass("Default constructor called.");
}

//______________________________________________________________________________________________

HWWBTagWeightNoDeco::HWWBTagWeightNoDeco(const TString& expression, const std::string& BTagName, const std::string& BTagWP, const float ptCut, const float etaCut, const std::string& jetAuthor) :
  HWWSFBase(expression)
{
  // Constructor with expression argument
  DEBUGclass("Constructor called with '%s'.", expression.Data());
  this->setExpression(expression);
  this->m_bTagVarName    = BTagName;
  this->m_OperatingPoint = BTagWP;
  this->m_ptCut          = ptCut;
  this->m_etaCut         = etaCut;
  this->m_jetAuthor      = jetAuthor;
}

//______________________________________________________________________________________________

HWWBTagWeightNoDeco::~HWWBTagWeightNoDeco() {
  // Default destructor
  DEBUGclass("Destructor called.");
}

//______________________________________________________________________________________________

void HWWBTagWeightNoDeco::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch) {
  DEBUGclass("Defining variation trigger.");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

std::string HWWBTagWeightNoDeco::getTtbarDSID(const uint32_t mcChannelNumber) const {
  std::string ttbarDSID = "410470";
  std::vector<uint32_t> ttbarDSID_default = {410000, 410001, 410002, 410009, 410011, 410012, 410013, 410014, 410015, 410016, 410017, 410018, 410019, 410025, 410026, 410049, 410050, 410064, 410065, 410103, 410104, 410105, 410106, 410107, 410108, 410109, 410151, 410152, 410153, 410154, 429007};
  for (size_t i = 0; i < ttbarDSID_default.size(); i++) {
    if (mcChannelNumber == ttbarDSID_default[i]) {
      ttbarDSID = "default";
      return ttbarDSID;
    }
  }
  std::vector<uint32_t> ttbarDSID_410558 = {410557, 410558, 410559};
  for (size_t i = 0; i < ttbarDSID_410558.size(); i++) {
    if (mcChannelNumber == ttbarDSID_410558[i]) {
      ttbarDSID = "410558";
      return ttbarDSID;
    }
  }
  std::vector<uint32_t> ttbarDSID_410464 = {410464, 410465, 410466};
  for (size_t i = 0; i < ttbarDSID_410464.size(); i++) {
    if (mcChannelNumber == ttbarDSID_410464[i]) {
      ttbarDSID = "410464";
      return ttbarDSID;
    }
  }
  std::vector<uint32_t> ttbarDSID_410250 = {410189, 410252};
  for (size_t i = 0; i < ttbarDSID_410250.size(); i++) {
    if (mcChannelNumber == ttbarDSID_410250[i]) {
      ttbarDSID = "410250";
      return ttbarDSID;
    }
  }
  return ttbarDSID;
}

//______________________________________________________________________________________________


bool HWWBTagWeightNoDeco::initializeSF() {

  using namespace asg::msgUserCode;

  DEBUGclass("Initializing HWWBTagWeightNoDeco for sample with path '%s'.", fSample->getPath().Data());

  // Get the appropriate ttbar DSID
  const xAOD::EventInfo* evtInfo = nullptr;
  if (!this->fEvent->retrieve(evtInfo, "EventInfo").isSuccess()) {
    WARNclass("Failed to retrieve event info!");
    return false;
  }
  uint32_t mcChannelNumber = evtInfo->mcChannelNumber();
  std::string ttbarDSID = HWWBTagWeightNoDeco::getTtbarDSID(mcChannelNumber);

  // Check if the tags in the sample folder indicate that a variation is requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));

  // Initialize BTaggingEfficiencyTool
  if (m_btagtool.empty()) {
    TString toolName;
    if (variationRequested) {
      toolName = "HWWBTagEfficiencyTool_" + m_variationName + "_ttbarDSID" + TString(ttbarDSID) + "_" + this->GetName();
    }
    else {
      toolName = "HWWBTagEfficiencyTool_Nominal_ttbarDSID" + TString(ttbarDSID) + "_" + this->GetName();
    }
    // First check if we can get the tool from the tool store
    if (asg::ToolStore::contains<BTaggingEfficiencyTool>(toolName.Data())) {
      m_btagtool = dynamic_cast<IBTaggingEfficiencyTool*>(asg::ToolStore::get(toolName.Data()));
    }
    // Else we need to initialize our tool
    else {
      IBTaggingEfficiencyTool* tool = new BTaggingEfficiencyTool(toolName.Data());
      DEBUGclass("Created new tool '%s'.", tool->name().c_str());
      m_btagtool = tool;
      ANA_CHECK_THROW(asg::setProperty(tool, "MinPt",                       m_ptCut         ));
      ANA_CHECK_THROW(asg::setProperty(tool, "JetAuthor",                   m_jetAuthor     ));
      ANA_CHECK_THROW(asg::setProperty(tool, "TaggerName",                  m_bTagVarName   ));
      ANA_CHECK_THROW(asg::setProperty(tool, "ScaleFactorFileName",         m_CDI           ));
      ANA_CHECK_THROW(asg::setProperty(tool, "OperatingPoint",              m_OperatingPoint));
      ANA_CHECK_THROW(asg::setProperty(tool, "SystematicsStrategy",         m_systStrat     ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EfficiencyBCalibrations",     ttbarDSID       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EfficiencyCCalibrations",     ttbarDSID       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EfficiencyTCalibrations",     ttbarDSID       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EfficiencyLightCalibrations", ttbarDSID       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EigenvectorReductionB",       m_systRed       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EigenvectorReductionC",       m_systRed       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "EigenvectorReductionLight",   m_systRed       ));
      ANA_CHECK_THROW(asg::setProperty(tool, "OutputLevel",                 m_outputLvl     ));
      // Initialize the tool (do this BEFORE adding systematics)
      ANA_CHECK_THROW(m_btagtool->initialize());
      // Apply the variation (if requested)
      if (variationRequested) {
        DEBUGclass("Variation requested for bTag weight with variationName %s and variationNameMatch %s.", m_variationName.Data(), m_variationNameMatch.Data());
        CP::SystematicVariation sysVar = CP::SystematicVariation(m_variationName.Data());
        CP::SystematicSet sysSet{sysVar};
        if (!m_btagtool->isAffectedBySystematic(sysVar)) {
          ERRORclass("BTaggingEfficiencyTool is NOT affected by systematic variation %s.", m_variationName.Data());
          return StatusCode::FAILURE;
        }
        if (m_btagtool->applySystematicVariation(sysSet) != StatusCode::SUCCESS) {
          ERRORclass("Cannot configure BTaggingEfficiencyTool for systematic variation %s.", m_variationName.Data());
          return StatusCode::FAILURE;
        }
      }
      else {
        DEBUGclass("No variation requested for bTag weight with variationName %s and variationNameMatch %s.", m_variationName.Data(), m_variationNameMatch.Data());
      }
    }
  }

  // Initialize BTaggingSelectionTool
  // --> Similar procedure to above, except we don't need to worry about the variations
  if (m_btagselecttool.empty()) {
    TString SelectionToolName = TString("HWWBTagSelectTool_") + this->GetName();
    if (asg::ToolStore::contains<BTaggingSelectionTool>(SelectionToolName.Data())) {
      m_btagselecttool = dynamic_cast<IBTaggingSelectionTool*>(asg::ToolStore::get(SelectionToolName.Data()));
    }
    else {
      IBTaggingSelectionTool* selecttool = new BTaggingSelectionTool(SelectionToolName.Data());
      DEBUGclass("Created new tool '%s'.", selecttool->name().c_str());
      m_btagselecttool = selecttool;
      // The following eta cut is applied manually in getBTagWeight()
      // --> Even though including it will not change the returned btag weight (as failing the eta would return 1. before reaching this tool), let's disable it to avoid confusion
      // ANA_CHECK_THROW(asg::setProperty(selecttool, "MaxEta",                       m_etaCut        ));
      //////////////////////////////////////////
      // Do we want to do the same with pt??? //
      //////////////////////////////////////////
      ANA_CHECK_THROW(asg::setProperty(selecttool, "MinPt",                        m_ptCut         ));
      ANA_CHECK_THROW(asg::setProperty(selecttool, "JetAuthor",                    m_jetAuthor     ));
      ANA_CHECK_THROW(asg::setProperty(selecttool, "TaggerName",                   m_bTagVarName   ));
      ANA_CHECK_THROW(asg::setProperty(selecttool, "FlvTagCutDefinitionsFileName", m_CDI           ));
      ANA_CHECK_THROW(asg::setProperty(selecttool, "OperatingPoint",               m_OperatingPoint));
      ANA_CHECK_THROW(asg::setProperty(selecttool, "OutputLevel",                  m_outputLvl     ));
      ANA_CHECK_THROW(m_btagselecttool->initialize());
    }
  }

  // Register our variation with HWWSFBase
  if (variationRequested) {
    HWWSFBase::registerVariation();
  }

  return true;

}

//______________________________________________________________________________________________

bool HWWBTagWeightNoDeco::finalizeSF() {
  return true;
}

//______________________________________________________________________________________________

float HWWBTagWeightNoDeco::getBTagWeight(const xAOD::Jet* jet) const{
  DEBUGclass("Retrieving bTag weight for jet %p.", jet);
  float retval = 1.0;
  // The selection tool will apply our minimum pt and maximum eta cuts on the jets, so don't have to (in theory...?)
  // But we keep this hard eta cut anyways, as it make the btag weight match with the old observable
  if (std::fabs(jet->eta()) >= m_etaCut){
    DEBUGclass("Jet has eta >= %f, will return %f.", m_etaCut, retval);
    return retval;
  }
  if (m_btagselecttool->accept(*jet)) {
    // Get the scale factor
    if (m_btagtool->getScaleFactor(*jet, retval) == CP::CorrectionCode::Error) {
      ERRORclass("BTaggingEfficiencyTool reported a CP::CorrectionCode::Error.");
      return StatusCode::FAILURE;
    }
  }
  else {
    // Get the inefficiency scale factor
    if (m_btagtool->getInefficiencyScaleFactor(*jet, retval) == CP::CorrectionCode::Error) {
      ERRORclass("BTaggingEfficiencyTool reported a CP::CorrectionCode::Error.");
      return StatusCode::FAILURE;
    }
  }
  DEBUGclass("BTag weight = %f.", retval);
  return retval;
}

//______________________________________________________________________________________________

double HWWBTagWeightNoDeco::getValue() const {

  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("Entering function.");

  const xAOD::CompositeParticleContainer* cand = nullptr;
  const xAOD::CompositeParticle* evt           = nullptr;
  const xAOD::EventInfo* evtInfo               = nullptr;

  double retval = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(retval, cand, evt, evtInfo)) {
    return retval;
  }

  DEBUGclass("Evaluating bTag weight for event %i, channel %s, expression %s.", this->getCurrentEntry(), m_candName.Data(), getExpression().Data());

  for (std::size_t i = 0; i < evt->nParts(); i++) {
    const xAOD::IParticle* part = evt->part(i);
    if (part->type() != xAOD::Type::Jet) {
      DEBUGclass("Particle %p is not a jet -- skipping!", part);
      continue;
    }
    const xAOD::Jet* jet = static_cast<const xAOD::Jet*>(part);
    if (!xAOD::BTaggingUtilities::getBTagging(*jet)) {
        DEBUGclass("No bTagging information available for this jet (large-R jet?). Skipping.");
        continue;
    }
    retval *= this->getBTagWeight(jet);
  }
  // Always loop over otherParts to avoid inconsistencies due to non-trivial correlation between CAF-level configured pT cut and pT cut used at PAOD production (which are generally inconsistent!)
  for (std::size_t i = 0; i < evt->nOtherParts(); i++) {
    const xAOD::IParticle* otherpart = evt->otherPart(i);
    if (otherpart->type() != xAOD::Type::Jet) {
      DEBUGclass("Particle %p is not a jet -- skipping!", otherpart);
      continue;
    }
    const xAOD::Jet* otherjet = static_cast<const xAOD::Jet*>(otherpart);
    if (!xAOD::BTaggingUtilities::getBTagging(*otherjet)) {
      DEBUGclass("No bTagging information available for this jet (large-R jet?). Skipping.");
      continue;
    }
    retval *= this->getBTagWeight(otherjet);
  }

  DEBUGclass("Total bTag weight = %f.", retval);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = retval;
  return retval;
}
