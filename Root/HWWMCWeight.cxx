#include "CAFExample/HWWMCWeight.h"
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

ClassImp(HWWMCWeight)

//______________________________________________________________________________________________

HWWMCWeight::HWWMCWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWMCWeight::HWWMCWeight(const TString& expression):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
}

//______________________________________________________________________________________________

HWWMCWeight::~HWWMCWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWMCWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

bool HWWMCWeight::initializeSF(){
  DEBUGclass("Initializing HWWMCWeight for sample with path '%s'",fSample->getPath().Data());

  //name of PDF weight
  TString pdfweightname = "";
  if(this->fSample->getTagString("~pdfWeightName",pdfweightname)) {
    this->m_pdfWeightName = pdfweightname;
  }

  bool registerVariation = false;
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));
  DEBUGclass("VariationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());

  if (variationRequested){
    DEBUGclass("Default variation requested for MC weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    m_mcSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(m_variationName.Data()));
    registerVariation = true;
  }
  if (m_pdfWeightName != ""){
    m_pdfWeightSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(m_pdfWeightName));
  }
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWMCWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWMCWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  const xAOD::EventInfo *evtInfo = 0;

  double mcWeight = 1.0;

  // Don't call HWWSFBase::getDefaultValue(...) here, because it tries to
  // retrieve the event container, which might not exist if running over
  // truth+reco PAODs. This is unfortunately an inconsistency between this
  // variable and all other observables inheriting from HWWSFBase.

  if (this->isData){
    DEBUGclass("This is data. Returning unity.");
    return 1.0;
  }

  if (!this->getUseWeight()){
    DEBUGclass("Weight switched off. Returning unity.");
    return 1.0;
  }

  // Do caching manually (it's otherwise done in HWWSFBase)
  if(this->getCurrentEntry() == this->fCachedEntry) {
    DEBUGclass("skipping reevalution for event %d, returning %f", this->getCurrentEntry(), this->fCachedValue);
    return this->fCachedValue;
  }

  // Retrieve the EventInfo container
  if( !this->fEvent->retrieve(evtInfo, "EventInfo").isSuccess() ){
    ERRORclass("failed to retrieve event info!");
    return 0;
  }

  // todo: can this go into initialize? What does weightIndex do anyway?
  //weight index
  int weightindex = 0;
  if(this->fSample->getTagInteger("~weightIndex",weightindex)) {
    DEBUGclass("Check sample path : '%s'",fSample->getPath().Data());
  }

  DEBUGclass("retrieving MC event weight");

  // variations are read with the decor
  if ( m_mcSFDecor ) {
    if(evtInfo->mcChannelNumber() == 345324) {
      DEBUGclass("Using uncertainty from variation");
      mcWeight = (*m_mcSFDecor)(*evtInfo);
    }
  }
  else{
    // default values are read here
    mcWeight = evtInfo->mcEventWeight(0);
    if(weightindex != 0) {
      DEBUGclass("Using event weight index = %d",weightindex);
      mcWeight = evtInfo->mcEventWeight(weightindex);
      DEBUGclass("Original weight = %f",evtInfo->mcEventWeight(0));
      //some special case for NNLOPS
      if(evtInfo->mcChannelNumber() == 345324) {
	//check if weight is NAN, only for uncertainty weights
	if(!std::isfinite(mcWeight) && weightindex != 151) mcWeight = evtInfo->mcEventWeight(150);
	if(mcWeight > 100.0) mcWeight=100.0;
	if(mcWeight < -100.0) mcWeight=-100.0;
      }
    }
  }

  // this only executes if the decor is set (only if pdfWeightName is not equal to "")
  if (m_pdfWeightSFDecor){
    DEBUGclass("Using event pdf weight = %s",m_pdfWeightName.c_str());
    mcWeight = (*m_pdfWeightSFDecor)(*evtInfo);
    DEBUGclass("Original weight = %f",evtInfo->mcEventWeight(0));
  }

  DEBUGclass("MC event weight = %f", mcWeight);
  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = mcWeight;
  return mcWeight;
}
