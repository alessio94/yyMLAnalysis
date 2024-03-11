#include "CAFExample/HWWBTagCounterObs.h"
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
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWBTagCounterObs::~HWWBTagCounterObs(){
  // default destructor
  DEBUGclass("destructor called");
} 

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
/* example block:
//#include "xAODParticleEvent/CompositeParticleContainer.h"
*/
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"

//______________________________________________________________________________________________

std::string HWWBTagCounterObs::getBTagName() {
  return m_bTagVarName;
}

std::string HWWBTagCounterObs::getBTagWP() {
  return m_OperatingPoint;
}

float HWWBTagCounterObs::getPtCut() {
  return m_ptCut;
}

bool HWWBTagCounterObs::getUseOtherJets() {
  return m_useOtherJets;
}

bool HWWBTagCounterObs::getUseHardCuts() {
  return m_useHardCuts;
}

float HWWBTagCounterObs::getBTagCut() {
  return m_bTagCut;
}

std::string HWWBTagCounterObs::getJetAuthor() {
  return m_jetAuthor;
}

//______________________________________________________________________________________________

void HWWBTagCounterObs::setBTagName(std::string BTagName, std::string BTagWP) {
  m_bTagVarName = BTagName;
  m_OperatingPoint = BTagWP;
}


void HWWBTagCounterObs::setPtCut(float PtCut) {
  m_ptCut = PtCut;
}

/*void HWWBTagCounterObs::setUseOtherJets(bool UseOtherJets) {
  m_useOtherJets = UseOtherJets;
}*/

void HWWBTagCounterObs::setUseHardCuts(bool UseHardCuts) {
  m_useHardCuts = UseHardCuts;
}

void HWWBTagCounterObs::setBTagCut(float BTagCut) {
  m_bTagCut = BTagCut;
  m_useHardCuts = true;
}

void HWWBTagCounterObs::setJetAuthor(std::string JetAuthor) {
  m_jetAuthor = JetAuthor;
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
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return false;
  }

  // Get the event candidate from the container
  const xAOD::CompositeParticle *Evt = cand->at(0);




  // Get the number of b-tags from the nominally-selected jets
  int nPassBTag=0;
  for ( size_t i=0; i<Evt->nParts(); ++i ){
    const xAOD::IParticle* part = Evt->part(i);
    if ( part->type() != xAOD::Type::Jet ) continue;
    const xAOD::Jet* jet = static_cast<const xAOD::Jet*>(part);
    if ( std::abs( jet->eta() ) > 2.5 ) continue;
    if ( jet->pt() < m_ptCut ) continue;
    if ( not jet->btagging() ) {
      DEBUGclass("No btagging information available for this jet (large-R jet?). Skipping");
      continue;
    }

    if (m_useHardCuts) { 
      const float tagWeight = jet->btagging()->auxdata<double>(m_bTagVarName);
      DEBUGclass("Got other jet tagWeight of %f", tagWeight );
      if ( tagWeight > m_bTagCut ) nPassBTag += 1;
      
	  } else{
      bool isBtagged = m_btagtool->accept(*jet);
      if ( isBtagged ) {
        nPassBTag += 1;
      }
    }
  
  }
  // Also use the sub-threshold other jets to also account for jets below the threshold (25GeV) for jets in the PAOD selection
  //if (m_useOtherJets) {
    for ( size_t i=0; i<Evt->nOtherParts(); ++i ){
      const xAOD::IParticle* part = Evt->otherPart(i);
      if ( part->type() != xAOD::Type::Jet ) continue;
      const xAOD::Jet* jet = static_cast<const xAOD::Jet*>(part);
      if ( std::abs( jet->eta() ) > 2.5 ) continue;
      if ( jet->pt() < m_ptCut ) continue;
      if ( not jet->btagging() ) {
        DEBUGclass("No btagging information available for this jet (large-R jet?). Skipping");
        continue;
      }


      if (m_useHardCuts){
        const float tagWeight = jet->btagging()->auxdata<double>(m_bTagVarName);
        DEBUGclass("Got other jet tagWeight of %f", tagWeight );
        if ( tagWeight > m_bTagCut ) nPassBTag += 1;
	    
	    } else {
        bool isOtherBtagged = m_btagtool->accept(*jet);
        if ( isOtherBtagged ) {
          nPassBTag += 1;
        }
      }
    }
  
  //}
  DEBUGclass("Got %d b-tags",nPassBTag);
  return static_cast<double>(nPassBTag);
  #endif
}
//______________________________________________________________________________________________

bool HWWBTagCounterObs::initializeSelf(){
  // initialize this observable
  // called once per sample (input file) so that the observable knows the name of the event candidate
  // will be EventEM or EventME in the case of DF analysis (depending on the current channel)
  TString ContName = "";
  if(!this->fSample->getTagString("~cand",ContName)) return false;
  this->mContName = "Event"+ContName;

  using namespace asg::msgUserCode;

  if( m_wasInitialized || m_useHardCuts ) return true;

  // Initialize BTaggingSelectionTool
  if( m_btagtool.empty() ) {
    TString toolName = TString("HWWBTagTool_") + this->GetName(); // use a unique name for each observable instance
    if( asg::ToolStore::contains<BTaggingSelectionTool>(toolName.Data()) ) {
      m_btagtool = dynamic_cast<IBTaggingSelectionTool*>(asg::ToolStore::get(toolName.Data()));
    }
    else {
      IBTaggingSelectionTool* tool = new BTaggingSelectionTool(toolName.Data());
      DEBUGclass("created new tool '%s'",tool->name().c_str());
      m_btagtool = tool;
      ANA_CHECK_THROW(asg::setProperty( tool, "MaxEta", 2.5 ));
      ANA_CHECK_THROW(asg::setProperty( tool, "MinPt", m_ptCut ));
      ANA_CHECK_THROW(asg::setProperty( tool, "JetAuthor", m_jetAuthor ));
      ANA_CHECK_THROW(asg::setProperty( tool, "TaggerName", m_bTagVarName ));
      ANA_CHECK_THROW(asg::setProperty( tool, "FlvTagCutDefinitionsFileName", "xAODBTaggingEfficiency/13TeV/2020-21-13TeV-MC16-CDI-2020-03-11_v1.root" ));
      ANA_CHECK_THROW(asg::setProperty( tool, "OperatingPoint", m_OperatingPoint ));

      //if( !m_btagtool->initialize() ) throw std::runtime_error("Failed to initialise BTaggingSelectionTool");
      ANA_CHECK_THROW(m_btagtool->initialize());
    }
  }
  else {
    throw std::runtime_error("Encountered non-empty BTag tool handle. Most likely this was already set, otherwise something went horribly wrong...");
  }
  m_wasInitialized = true;  

  DEBUGclass("initializing");
  return true;
}

//______________________________________________________________________________________________

bool HWWBTagCounterObs::finalizeSelf(){
  // finalize this observable
  // remember to undo anything you did in initializeSelf() !
  
  DEBUGclass("finalizing");
  return true;
}
//______________________________________________________________________________________________

HWWBTagCounterObs::HWWBTagCounterObs(const TString& name,
				     std::string BTagName, std::string BTagWP, float PtCut, /*bool UseOtherJets,*/ bool UseHardCuts, float BTagCut, std::string jetAuthor):
TQEventObservable(name),
m_bTagVarName(BTagName),
m_OperatingPoint(BTagWP),
m_ptCut(PtCut),
//m_useOtherJets(UseOtherJets),
m_useHardCuts(UseHardCuts),
m_bTagCut(BTagCut),
m_jetAuthor(jetAuthor)
{

  // nominal constructor
  DEBUGclass("constructor called with '%s'",name.Data());
}
