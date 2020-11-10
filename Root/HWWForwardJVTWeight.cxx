#include "CAFExample/HWWForwardJVTWeight.h"
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

ClassImp(HWWForwardJVTWeight)

//______________________________________________________________________________________________

HWWForwardJVTWeight::HWWForwardJVTWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWForwardJVTWeight::HWWForwardJVTWeight(const TString& expression, const std::string& forwardJVTSFName):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s', %s",expression.Data(), forwardJVTSFName.c_str());
  this->setExpression(expression);
  this->m_forwardJVTSFName = forwardJVTSFName;
}

//______________________________________________________________________________________________

HWWForwardJVTWeight::~HWWForwardJVTWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWForwardJVTWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

bool HWWForwardJVTWeight::initializeSF(){
  DEBUGclass("Initializing HWWForwardJVTWeight for sample with path '%s'",fSample->getPath().Data());

  // check if the tags in the sample folder indicate that a variation is requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));

  // This is uggly! If the PxAOD naming for this decoration were different, we wouldn't need this condition. In fact, this class would be identical to the HWWJVTWeight class making it obsolete.
  // Update: the PxAODs are renamed as of V19. Delete this class once support for V18 and below can be dropped.
  // todo
  if (m_variationName.EqualTo("JET_fJvtEfficiency__1down"))
    m_variationName = "JET_JvtEfficiency__1down";
  if (m_variationName.EqualTo("JET_fJvtEfficiency__1up"))
    m_variationName = "JET_JvtEfficiency__1up";

  bool registerVariation = false;
  TString decorationName;
  if (variationRequested){
    DEBUGclass("Variation requested for JVT weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
     decorationName = m_forwardJVTSFName+"___"+m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("No variation requested for JVT weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_forwardJVTSFName;
  }

  m_forwardJVTSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWForwardJVTWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWForwardJVTWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function for %s", this->getExpression().Data());
  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double forwardJVTWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(forwardJVTWeight, cand, evt, evtInfo))
    return forwardJVTWeight;

  if (!(*m_forwardJVTSFDecor).isAvailable(*evtInfo)) {
    ERRORclass("The decoration %s is not available. Are you using an old PxAOD version (<16)?");
  }
  forwardJVTWeight = (*m_forwardJVTSFDecor)(*evtInfo);
  DEBUGclass("%s: %f", this->getExpression().Data(), forwardJVTWeight);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = forwardJVTWeight;
  return forwardJVTWeight;
}
