#include "CAFExample/HWWMuonWeight.h"
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

ClassImp(HWWMuonWeight)

//______________________________________________________________________________________________

HWWMuonWeight::HWWMuonWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWMuonWeight::HWWMuonWeight(const TString& expression, const std::string& muonSFName):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
  this->m_muonSFName = muonSFName;
  this->m_lowPtMuonsSeparately = false;
}

//______________________________________________________________________________________________

HWWMuonWeight::HWWMuonWeight(const TString& expression, const std::string& muonSFName, const std::string& muonSFNameLowPt, const double ptCut):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
  this->m_muonSFName = muonSFName;
  this->m_muonSFNameLowPt = muonSFNameLowPt;
  this->m_lowPtMuonsSeparately = true;
  if (ptCut <= 1000)
    WARNclass("You want to treat muons with less than %f MeV differently? Did you mix up the units?", ptCut);
  this->m_ptCut = ptCut;
}

//______________________________________________________________________________________________

HWWMuonWeight::~HWWMuonWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWMuonWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger for %s", this->getExpression().Data());
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

void HWWMuonWeight::defineVariationTriggerLowPt(const TString& variationTypeLowPt, const TString& variationNameMatchLowPt){
  DEBUGclass("Defining variation trigger for %s low pt", this->getExpression().Data());
  if (!m_lowPtMuonsSeparately){
    WARNclass("You are defining a variation trigger for low-pt muons, but you don't treat the low-pt %s separately. Thus, this method is useless. If you want to treat the low-pt region separately, use the corresponding constructor to define a decoration name and a pt cut.", this->getExpression().Data());
  }
  this->m_variationTypeMatchLowPt = variationTypeLowPt;
  if (variationNameMatchLowPt.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatchLowPt = variationNameMatchLowPt;
}

//______________________________________________________________________________________________

bool HWWMuonWeight::initializeSF(){
  DEBUGclass("Initializing %s for sample with path '%s'", this->getExpression().Data(),fSample->getPath().Data());

  DEBUGclass("VariationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());

  bool registerVariation = false;
  TString decorationName;
  TString decorationNameLowPt;

  // check if the tags in the sample folder indicate that a variation is requested
  // treat low-pt muons separately if requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));
  bool variationRequestedLowPt = ((m_variationType == m_variationTypeMatchLowPt) && (m_variationName.BeginsWith(m_variationNameMatchLowPt)));

  if (variationRequested){
    DEBUGclass("Default variation requested for %s with variationName %s and variationNameMatch %s",this->getExpression().Data(), m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_muonSFName+"___"+m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("No default variation requested for %s with variationName %s and variationNameMatch %s",this->getExpression().Data(), m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_muonSFName;
  }
  m_muonSFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));

  if (m_lowPtMuonsSeparately){
    if ((!m_variationTypeMatch.IsNull()) && (!m_variationNameMatch.IsNull()) &&
	(m_variationTypeMatchLowPt.IsNull()) && (m_variationNameMatchLowPt.IsNull())){
      WARNclass("You have defined a variation for the default %s, but not for the low-pt region (pt < %f MeV).", this->getExpression().Data(), m_ptCut);
    }
    if (variationRequestedLowPt){
      DEBUGclass("Low-pT variation requested for %s with variationName %s and variationNameMatchLowPt %s", this->getExpression().Data(), m_variationName.Data(), m_variationNameMatchLowPt.Data());
      decorationNameLowPt = m_muonSFNameLowPt+"___"+m_variationName.Data();
      registerVariation = true;
    }
    else{
      DEBUGclass("No low-pT variation requested for %s with variationName %s and variationNameMatchLowPt %s", this->getExpression().Data(), m_variationName.Data(), m_variationNameMatchLowPt.Data());
      decorationNameLowPt = m_muonSFNameLowPt;
    }
    m_muonSFDecorLowPt = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationNameLowPt.Data()));
  }
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWMuonWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWMuonWeight::getMuonWeight(const xAOD::IParticle* p) const {
  DEBUGclass("retrieving %s for particle %p", this->getExpression().Data(),p);
  double retval = 1.;
  if( p->type() != xAOD::Type::Muon ){
    DEBUGclass("Not an muon, will return %f", retval);
    return retval;
  }

  // access the decoration
  if ((m_lowPtMuonsSeparately) && (p->pt() <= m_ptCut))
    retval = (*m_muonSFDecorLowPt)(*p);
  else
    retval = (*m_muonSFDecor)(*p);
  DEBUGclass("%s for particle %p = %f", this->getExpression().Data(),p , retval);
  return retval;
}

//______________________________________________________________________________________________

double HWWMuonWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function for %s", this->getExpression().Data());

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double muonWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(muonWeight, cand, evt, evtInfo))
    return muonWeight;

  unsigned int nLeptons = evt->nLeptons();

  for( unsigned int i=0; i<nLeptons; ++i ) {
    DEBUGclass("muon  weights: index %d",(int)i);
    muonWeight *= this->getMuonWeight(evt->part(i));
  }
  DEBUGclass("Total muon weight = %f", muonWeight);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = muonWeight;
  return muonWeight;
}
