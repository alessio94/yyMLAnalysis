#include "CAFExample/HWWBTagWeight.h"
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

ClassImp(HWWBTagWeight)

//______________________________________________________________________________________________

HWWBTagWeight::HWWBTagWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWBTagWeight::HWWBTagWeight(const TString& expression, const std::string& bTagSFName, bool useOtherJets):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
  this->m_bTagSFName = bTagSFName;
  this->m_useOtherJets = useOtherJets;
}

//______________________________________________________________________________________________

HWWBTagWeight::~HWWBTagWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWBTagWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

bool HWWBTagWeight::initializeSF(){
  DEBUGclass("Initializing HWWBTagWeight for sample with path '%s'",fSample->getPath().Data());

  // check if the tags in the sample folder indicate that a variation is requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));

  bool registerVariation = false;
  TString decorationName;
  if (variationRequested){
    DEBUGclass("Variation requested for bTag weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_bTagSFName+"___"+m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("No variation requested for bTag weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_bTagSFName;
  }

  m_bTagSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWBTagWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWBTagWeight::getBTagWeight(const xAOD::IParticle* p) const {
  DEBUGclass("retrieving bTag weight for particle %p",p);
  double retval = 1.;
  if( p->type() != xAOD::Type::Jet ){
    DEBUGclass("Not a jet, will return %f", retval);
    return retval;
  }
  if (fabs(p->eta())>=2.5){
    DEBUGclass("Jet has eta >= 2.5, will return %f", retval);
    return retval;
  }

  // access the decoration
  retval = (*m_bTagSFDecor)(*p);

  DEBUGclass("BTag weight = %f", retval);
  return retval;
}

//______________________________________________________________________________________________

double HWWBTagWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double bTagWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(bTagWeight, cand, evt, evtInfo))
    return bTagWeight;

  bTagWeight = 1.;
  if (!m_bTagSFDecor) {
    ERRORclass("no btag SF decoration available");
    return bTagWeight;
  }

  DEBUGclass("Evaluating bTagWeight for event %i, channel %s, expression %s", entry, m_candName.Data(), getExpression().Data());

  for( unsigned int i=0; i<evt->nParts(); ++i ) {
    bTagWeight *= this->getBTagWeight(evt->part(i));
  }
  if (m_useOtherJets) {
    for( unsigned int i=0; i<evt->nOtherParts(); ++i ) {
      bTagWeight *= this->getBTagWeight(evt->otherPart(i));
    }
  }
  DEBUGclass("Total btag weight = %f", bTagWeight);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = bTagWeight;
  return bTagWeight;
}
