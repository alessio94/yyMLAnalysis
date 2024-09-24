#include "CAFExample/HWWElectronWeight.h"
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

ClassImp(HWWElectronWeight)

//______________________________________________________________________________________________

HWWElectronWeight::HWWElectronWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWElectronWeight::HWWElectronWeight(const TString& expression, const std::string& electronSFName):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
  this->m_electronSFName = electronSFName;
  this->m_lowPtElectronsSeparately = false;
}

//______________________________________________________________________________________________

HWWElectronWeight::HWWElectronWeight(const TString& expression, const std::string& electronSFName, const std::string& electronSFNameLowPt, const double ptCut):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
  this->m_electronSFName = electronSFName;
  this->m_electronSFNameLowPt = electronSFNameLowPt;
  this->m_lowPtElectronsSeparately = true;
  if (ptCut <= 1000)
    WARNclass("You want to treat electrons with less than %f MeV differently? Did you mix up the units?", ptCut);
  this->m_ptCut = ptCut;
}

//______________________________________________________________________________________________

HWWElectronWeight::~HWWElectronWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWElectronWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger for %s", this->getExpression().Data());
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

void HWWElectronWeight::defineVariationTriggerLowPt(const TString& variationTypeLowPt, const TString& variationNameMatchLowPt){
  DEBUGclass("Defining variation trigger for %s low pt", this->getExpression().Data());
  if (!m_lowPtElectronsSeparately){
    WARNclass("You are defining a variation trigger for low-pt electrons, but you don't treat the low-pt %s separately. Thus, this method is useless. If you want to treat the low-pt region separately, use the corresponding constructor to define a decoration name and a pt cut.", this->getExpression().Data());
  }
  this->m_variationTypeMatchLowPt = variationTypeLowPt;
  if (variationNameMatchLowPt.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatchLowPt = variationNameMatchLowPt;
}

//______________________________________________________________________________________________

bool HWWElectronWeight::initializeSF(){
  DEBUGclass("Initializing %s for sample with path '%s'", this->getExpression().Data(),fSample->getPath().Data());

  DEBUGclass("VariationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());

  bool registerVariation = false;
  TString decorationName;
  TString decorationNameLowPt;

  // check if the tags in the sample folder indicate that a variation is requested
  // treat low-pt electrons separately if requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));
  bool variationRequestedLowPt = ((m_variationType == m_variationTypeMatchLowPt) && (m_variationName.BeginsWith(m_variationNameMatchLowPt)));

  if (variationRequested){
    DEBUGclass("Default variation requested for %s with variationName %s and variationNameMatch %s",this->getExpression().Data(), m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_electronSFName+"___"+m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("No default variation requested for %s with variationName %s and variationNameMatch %s",this->getExpression().Data(), m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_electronSFName;
  }
  m_electronSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));

  if (m_lowPtElectronsSeparately){
    if ((!m_variationTypeMatch.IsNull()) && (!m_variationNameMatch.IsNull()) &&
	(m_variationTypeMatchLowPt.IsNull()) && (m_variationNameMatchLowPt.IsNull())){
      WARNclass("You have defined a variation for the default %s, but not for the low-pt region (pt < %f MeV).", this->getExpression().Data(), m_ptCut);
    }
    if (variationRequestedLowPt){
      DEBUGclass("Low-pT variation requested for %s with variationName %s and variationNameMatchLowPt %s", this->getExpression().Data(), m_variationName.Data(), m_variationNameMatchLowPt.Data());
      decorationNameLowPt = m_electronSFNameLowPt+"___"+m_variationName.Data();
      registerVariation = true;
    }
    else{
      DEBUGclass("No low-pT variation requested for %s with variationName %s and variationNameMatchLowPt %s", this->getExpression().Data(), m_variationName.Data(), m_variationNameMatchLowPt.Data());
      decorationNameLowPt = m_electronSFNameLowPt;
    }
    m_electronSFDecorLowPt = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationNameLowPt.Data()));
  }
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWElectronWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWElectronWeight::getElectronWeight(const xAOD::IParticle* p) const {
  DEBUGclass("retrieving %s for particle %p", this->getExpression().Data(),p);
  double retval = 1.;
  if( p->type() != xAOD::Type::Electron ){
    DEBUGclass("Not an electron, will return %f", retval);
    return retval;
  }

  // access the decoration
  if ((m_lowPtElectronsSeparately) && (p->pt() <= m_ptCut))
    retval = (*m_electronSFDecorLowPt)(*p);
  else
    retval = (*m_electronSFDecor)(*p);
  DEBUGclass("%s for particle %p = %f", this->getExpression().Data(),p , retval);
  return retval;
}

//______________________________________________________________________________________________

double HWWElectronWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function for %s", this->getExpression().Data());

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double electronWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(electronWeight, cand, evt, evtInfo))
    return electronWeight;

  unsigned int nLeptons = evt->nLeptons();

  for( unsigned int i=0; i<nLeptons; ++i ) {
    DEBUGclass("electron  weights: index %d",(int)i);
    electronWeight *= this->getElectronWeight(evt->part(i));
  }
  DEBUGclass("Total electron weight = %f", electronWeight);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = electronWeight;
  return electronWeight;
}
