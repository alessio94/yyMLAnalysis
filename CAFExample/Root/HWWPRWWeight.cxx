#include "CAFExample/HWWPRWWeight.h"
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

ClassImp(HWWPRWWeight)

//______________________________________________________________________________________________

HWWPRWWeight::HWWPRWWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWPRWWeight::HWWPRWWeight(const TString& expression, const std::string& prwSFName):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
  m_prwSFName = prwSFName;
}

//______________________________________________________________________________________________

HWWPRWWeight::~HWWPRWWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWPRWWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

bool HWWPRWWeight::initializeSF(){
  DEBUGclass("Initializing HWWPRWWeight for sample with path '%s'",fSample->getPath().Data());

  // check if the tags in the sample folder indicate that a variation is requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));

  bool registerVariation = false;
  TString decorationName;
  if (variationRequested){
    DEBUGclass("Variation requested for PRW weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_prwSFName+"___"+m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("Variation requested for PRW weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_prwSFName;
  }

  m_prwSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWPRWWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWPRWWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double prwWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(prwWeight, cand, evt, evtInfo))
    return prwWeight;

  if( m_prwSFDecor ) {
    DEBUGclass("m_prwSFDecor True");
    prwWeight = (*m_prwSFDecor)(*evtInfo);
    DEBUGclass("PRW weight = %f", prwWeight);
  }

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = prwWeight;
  return prwWeight;
}
