#include "CAFExample/HWWJVTWeight.h"
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

ClassImp(HWWJVTWeight)

//______________________________________________________________________________________________

HWWJVTWeight::HWWJVTWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWJVTWeight::HWWJVTWeight(const TString& expression, const std::string& jvtSFName):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s', %s",expression.Data(), jvtSFName.c_str());
  this->setExpression(expression);
  this->m_jvtSFName = jvtSFName;
}

//______________________________________________________________________________________________

HWWJVTWeight::~HWWJVTWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWJVTWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

bool HWWJVTWeight::initializeSF(){
  DEBUGclass("Initializing HWWJVTWeight for sample with path '%s'",fSample->getPath().Data());

  // check if the tags in the sample folder indicate that a variation is requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));

  bool registerVariation = false;
  TString decorationName;
  if (variationRequested){
    DEBUGclass("Variation requested for JVT weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_jvtSFName+"___"+m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("No variation requested for JVT weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_jvtSFName;
  }

  m_jvtSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWJVTWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWJVTWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function for %s", this->getExpression().Data());

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double jvtWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(jvtWeight, cand, evt, evtInfo))
    return jvtWeight;


  if (!(*m_jvtSFDecor).isAvailable(*evtInfo)) {
    ERRORclass("The decoration %s is not available. Are you using an old PxAOD version (<16)?");
  }
  jvtWeight = (*m_jvtSFDecor)(*evtInfo);
  DEBUGclass("%s: %f", this->getExpression().Data(), jvtWeight);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = jvtWeight;
  return jvtWeight;
}
