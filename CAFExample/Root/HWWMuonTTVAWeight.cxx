#include "CAFExample/HWWMuonTTVAWeight.h"
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
#include "xAODMuon/MuonContainer.h"

ClassImp(HWWMuonTTVAWeight)

//______________________________________________________________________________________________

HWWMuonTTVAWeight::HWWMuonTTVAWeight(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWMuonTTVAWeight::HWWMuonTTVAWeight(const TString& expression, const std::string& muonTTVASFName):
  HWWSFBase(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->m_muonTTVASFName = muonTTVASFName;
}

//______________________________________________________________________________________________

HWWMuonTTVAWeight::~HWWMuonTTVAWeight(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

void HWWMuonTTVAWeight::defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch){
  DEBUGclass("Defining variation trigger");
  this->m_variationTypeMatch = variationTypeMatch;
  if (variationNameMatch.BeginsWith("nominal")){
    ERRORclass("No variation should start with \'nominal\'. Not setting m_variationNameMatch.");
    return;
  }
  this->m_variationNameMatch = variationNameMatch;
}

//______________________________________________________________________________________________

bool HWWMuonTTVAWeight::initializeSF(){
  DEBUGclass("Initializing HWWMuonTTVAWeight for sample with path '%s'",fSample->getPath().Data());

  // check if the tags in the sample folder indicate that a variation is requested
  bool variationRequested = ((m_variationType == m_variationTypeMatch) && (m_variationName.BeginsWith(m_variationNameMatch)));

  bool registerVariation = false;
  TString decorationName;
  if (variationRequested){
    DEBUGclass("Default requested for muon TTVA weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_muonTTVASFName + "___" + m_variationName.Data();
    registerVariation = true;
  }
  else{
    DEBUGclass("No variation requested for muon TTVA weight with variationName %s and variationNameMatch %s", m_variationName.Data(), m_variationNameMatch.Data());
    decorationName = m_muonTTVASFName;
  }

  m_muonTTVASFDecor = std::unique_ptr<SG::AuxElement::Accessor<float>> (new SG::AuxElement::Accessor<float>(decorationName.Data()));
  if (registerVariation)
    HWWSFBase::registerVariation();

  return true;
}

//______________________________________________________________________________________________

bool HWWMuonTTVAWeight::finalizeSF(){
  return true;
}

//______________________________________________________________________________________________

double HWWMuonTTVAWeight::getMuonTTVAWeight(const xAOD::IParticle* p) const {
  DEBUGclass("retrieving muon TTVA weight for particle %p",p);
  double retval = 1.;
  if( p->type() != xAOD::Type::Muon ){
    DEBUGclass("(Muon TTVA) Not a muon, will return %f", retval);
    return retval;
  }

  // access the decoration
  retval = (*m_muonTTVASFDecor)(*p);

  DEBUGclass("Muon TTVA weight = %f", retval);
  return retval;
}

//______________________________________________________________________________________________

double HWWMuonTTVAWeight::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  const xAOD::CompositeParticleContainer *cand = 0;
  const xAOD::CompositeParticle *evt = 0;
  const xAOD::EventInfo *evtInfo = 0;

  double muonTTVAWeight = 1.0;
  // If a value has been calculated before or retrieval of the necessary
  // variables fails, return the corresponding value (see HWWSFBase).
  if (this->getDefaultValue(muonTTVAWeight, cand, evt, evtInfo))
    return muonTTVAWeight;

  unsigned int nLeptons = evt->nLeptons();
  if(!m_muonTTVASFDecor) {
    WARNclass("No decoration accessor available. Returning 1.");
    return 1.0;
  }

  for( unsigned int i=0; i<nLeptons; ++i ) {
    DEBUGclass("muon TTVA weights: index %d",(int)i);
    muonTTVAWeight *= this->getMuonTTVAWeight(evt->part(i));
  }

  DEBUGclass("Total muon TTVA weight = %f", muonTTVAWeight);

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = muonTTVAWeight;
  return muonTTVAWeight;
}
