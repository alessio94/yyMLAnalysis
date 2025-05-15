#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingUtilities.h"
#include "yyMLAnalysisCode/HWWBTagCounterObs.h"
#include "QFramework/TQSample.h"
#include <limits>
// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(HWWBTagCounterObs)

//______________________________________________________________________________________________

HWWBTagCounterObs::HWWBTagCounterObs(){
  DEBUGclass("Default constructor called");
}

HWWBTagCounterObs::HWWBTagCounterObs(const TString& name,
                                     std::string bTagName,
                                     std::string bTagWP,
                                     std::string jetAuthor,
                                     bool useOtherJets,
                                     bool useHardCuts,
                                     float ptCut,
                                     float bTagCut ) : TQEventObservable(name),
                                                        m_bTagVarName(bTagName),
                                                        m_operatingPoint(bTagWP),
                                                        m_jetAuthor(jetAuthor),
                                                        m_useOtherJets(useOtherJets),
                                                        m_useHardCuts(useHardCuts),
                                                        m_ptCut(ptCut),
                                                        m_bTagCut(bTagCut) {
  DEBUGclass("Constructor called with '%s'", name.Data());
}

HWWBTagCounterObs::~HWWBTagCounterObs(){
  DEBUGclass("Destructor called");
} 

//______________________________________________________________________________________________

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
/* example block:
//#include "xAODParticleEvent/CompositeParticleContainer.h"
*/
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

//______________________________________________________________________________________________

void HWWBTagCounterObs::setBTagName(std::string BTagName, std::string BTagWP) {
  m_bTagVarName = BTagName;
  m_operatingPoint = BTagWP;
}

void HWWBTagCounterObs::setBTagCut(float BTagCut) {
  m_bTagCut = BTagCut;
  m_useHardCuts = true;
}

//______________________________________________________________________________________________


double HWWBTagCounterObs::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the TQEventObservable only works in an ASG RELEASE, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add an ASG Analysis Release in order to use this feature!"
  return std::numeric_limits<double>::quiet_NaN();
  #else
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!
  
  // first, you can retrieve the data members you needwith the 'retrieve' method
  // as arguments to this function, pass the member pointer to the container as well as the container name
    /* example block:
  // Retrieve CompositeParticle container
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
  DEBUGclass("failed to retrieve candidates!");
  return false;
  }
   */

  // after you have retrieved your data members, you can proceed to calculate the return value
  // probably, you first have to retrieve an element from the container
  /* example block:
  const xAOD::CompositeParticle *Evt = cand->at(0);
  */

  // in the end, you should calculate your return value 
  // of course, you can use other data members of your observable at any time
  /* example block:
     const double retval = Evt->part(0)->pt();
  */

  // Retrieve CompositeParticle container
  const xAOD::CompositeParticleContainer* cand = 0;
  if (!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()) {
    DEBUGclass("failed to retrieve candidates!");
    return false;
  }

  // Get the event candidate from the container
  const xAOD::CompositeParticle* Evt = cand->at(0);

  // Get the number of b-tags from the nominally-selected jets
  int nPassBTag = 0;
  for (size_t i = 0; i < Evt->nParts(); i ++) {
    const xAOD::IParticle* particle = Evt->part(i);
    if (particle->type() != xAOD::Type::Jet) { continue; }
    const xAOD::Jet* jet = static_cast<const xAOD::Jet*>(particle);
    if (!PassBTagCuts(jet)) { continue; }
    nPassBTag++;
  }

  // Use the sub-threshold container OtherParts to account for jets below the pT threshold
  if (m_useOtherJets) {
    for (size_t i = 0; i < Evt->nOtherParts(); i ++) {
      const xAOD::IParticle* particle = Evt->otherPart(i);
      if (particle->type() != xAOD::Type::Jet) { continue; }
      const xAOD::Jet* jet = static_cast<const xAOD::Jet*>(particle);
      if (!PassBTagCuts(jet)) { continue; }
      nPassBTag++;
    }
  }

  //}
  DEBUGclass("Got %d b-tags this event", nPassBTag);
  return static_cast<double>(nPassBTag);
#endif
}

bool HWWBTagCounterObs::PassBTagCuts(const xAOD::Jet* jet) const {
  // Simple Kinematic Cuts
  if (std::abs(jet->eta()) > 2.5) { return false; }
  if (jet->pt() < m_ptCut) { return false; }

  // Obtain btagging information for this jet
  const xAOD::BTagging* bTagging = xAOD::BTaggingUtilities::getBTagging(*jet);
  if (!bTagging) {
    DEBUGclass("No btagging information available for this jet. Skipping");
    return false;
  }

  if (m_useHardCuts) {
    const float tagWeight = bTagging->auxdata<double>(m_bTagVarName);
    DEBUGclass("Got other jet tagWeight of %f", tagWeight);
    if (tagWeight <= m_bTagCut) { return false; }
  } else {
    bool isBtagged = static_cast<bool>(m_btagtool->accept(*jet));
    DEBUGclass("Jet is b-tagged: %d", isBtagged);
    if (!isBtagged) { return false; }
  }

  return true;
}

//______________________________________________________________________________________________

bool HWWBTagCounterObs::initializeSelf(){
  // initialize this observable
  // called once per sample (input file) so that the observable knows the name of the event candidate
  // will be EventEM or EventME in the case of DF analysis (depending on the current channel)
 
  if (m_wasInitialized) { 
    DEBUGclass("Was already initialized. Skipping initializaiton.");
    return true;
  }
  DEBUGclass("Initializing");

  // Obtain the container name for the CompositeParticleContainer
  TString ContName = "";
  if (!this->fSample->getTagString("~cand", ContName)) {
    ERRORclass("Could not get tag string ~cand.");
    return false;
  }
  this->mContName = "Event" + ContName;

  // Namespace is required to prevent errors within ANA_CHECK_THROW
  using namespace asg::msgUserCode;

  // Initialize BTaggingSelectionTool
  if (m_btagtool.empty()) {
    TString toolName = TString("HWWBTagTool_") + this->GetName();  // use a unique name for each observable instance
    if (asg::ToolStore::contains<BTaggingSelectionTool>(toolName.Data())) {
      DEBUGclass("Found tool %s", toolName.Data());
      m_btagtool = dynamic_cast<IBTaggingSelectionTool*>(asg::ToolStore::get(toolName.Data()));
    } else {
      DEBUGclass("Creating new tool '%s'", toolName.Data());
      IBTaggingSelectionTool* tool = new BTaggingSelectionTool(toolName.Data());
      m_btagtool = ToolHandle<IBTaggingSelectionTool>(tool);
      ANA_CHECK_THROW(asg::setProperty(tool, "MaxEta", 2.5));
      ANA_CHECK_THROW(asg::setProperty(tool, "MinPt", m_ptCut));
      ANA_CHECK_THROW(asg::setProperty(tool, "JetAuthor", m_jetAuthor));
      ANA_CHECK_THROW(asg::setProperty(tool, "TaggerName", m_bTagVarName));
      ANA_CHECK_THROW(asg::setProperty(tool, "FlvTagCutDefinitionsFileName", "xAODBTaggingEfficiency/13p6TeV/2023-22-13p6TeV-MC21-CDI_Test_2023-08-1_v1.root"));
      ANA_CHECK_THROW(asg::setProperty(tool, "OperatingPoint", m_operatingPoint));
      ANA_CHECK_THROW(m_btagtool->initialize());
    }
  } else {
    throw std::runtime_error("Encountered non-empty BTag tool handle. Most likely this was already set, otherwise something went horribly wrong...");
  }

  m_wasInitialized = true;
  DEBUGclass("Initialization Complete");
  return true;
}


//______________________________________________________________________________________________

bool HWWBTagCounterObs::finalizeSelf(){
  DEBUGclass("Finalizing");
  return true;
}
//______________________________________________________________________________________________


