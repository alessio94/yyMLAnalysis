#include "yyMLAnalysisCode/HWWJetCounter.h"
#include "QFramework/TQUtils.h"

// uncomment the following line to enable debug printouts
//#define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"
#include "QFramework/TQSample.h"

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/Jet.h"
#include "FourMomUtils/xAODP4Helpers.h"

ClassImp(HWWJetCounter)

//______________________________________________________________________________________________

HWWJetCounter::HWWJetCounter():
  m_jetType(JetType::nominal),
  m_cutVal(25000.),
  m_fwdcutVal(30000.),
  m_etamin(0.),
  m_etamax(2.5)
{
  // default constructor
  DEBUGclass("default constructor called");

  DEBUGclass("Jet pT cut val: %f", m_cutVal);
  DEBUGclass("Forward jet pT cut val: %f", m_fwdcutVal);
  DEBUGclass("Jet Eta region: %s", m_etaRegionString);
  DEBUGclass("Jet min eta: %f", m_etamin);
  DEBUGclass("Jet max eta: %f", m_etamax);
}

//______________________________________________________________________________________________

HWWJetCounter::~HWWJetCounter(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

bool HWWJetCounter::initializeSelf(){
  DEBUGclass("Initializing HWWJetCounter for sample with path '%s'",fSample->getPath().Data());
  TString candName = "";
  if(!this->fSample->getTagString(this->fDoTruth ? "~cand.original" : "~cand" ,candName)) return false;
  this->m_candName = TString::Format("Event%s%s", this->fDoTruth ? "Truth" : "", candName.Data());
  return true;
}

//______________________________________________________________________________________________

void HWWJetCounter::setCutValue(float val, float fwdval){
    m_cutVal = val;
    m_fwdcutVal = fwdval;

}

//______________________________________________________________________________________________

void HWWJetCounter::setEtaRange(float min, float max){
    m_etamin = min;
    m_etamax = max;
}

//______________________________________________________________________________________________
bool HWWJetCounter::muonOverlapsWithJet(const xAOD::IParticle* jet, const xAOD::IParticle* muon) const {
  // check if we have a muon
  if (muon) { // particle exists...
    if (muon->type() == xAOD::Type::Muon) { // ... is a muon ...
      if (xAOD::P4Helpers::deltaR(jet, muon, false) < 0.2) { // useRapidity = false
        return true; // ... and overlaps with jet
      }
    }
  } else {
    ERRORclass("[HWWJetCounter] You passed a null ptr to overlapsWithJet fcn!");
  }
  return false;
}

double HWWJetCounter::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");
  // the following protective if-block should NEVER evaluate true
  // for performance reasons, we disable it per preprocessor directive unless in debugging mode
  #ifdef _DEBUG_
  if(!this->fEvent){
    ERRORclass("failed to access xAOD::TEvent member!");
    return 0;
  }
  #endif

  DEBUGclass("retrieving container");
  // Retrieve the event candidates
  const xAOD::CompositeParticleContainer *cand(0);
  if( !this->fEvent->retrieve(cand, this->m_candName.Data()).isSuccess() ){
    ERRORclass("failed to retrieve candidates!");
    return 0;
  }

  DEBUGclass("retrieving event candidate from container");
  if(cand->size() == 0){
    ERRORclass("in %s: event '%d' in '%s' has no candidate of type '%s'!",this->GetName(),(int)(this->getCurrentEntry()),this->fSample->getPath().Data(),this->m_candName.Data());
    return 0.;
  }
  const xAOD::CompositeParticle *evt = cand->at(0);

  // Check eta range for jet studies
  std::vector<const xAOD::IParticle*> allJets;
  const xAOD::IParticle* part;
  for (size_t i=0; i<evt->nParts(); ++i) {
    part = evt->part(i);
    if (part->type() == xAOD::Type::Jet) allJets.push_back(part);
  }
  for (size_t i=0; i<evt->nOtherParts(); ++i) {
    part = evt->otherPart(i);
    if (part->type() == xAOD::Type::Jet) allJets.push_back(part);
  }
  

  int jetCounter = 0;
  for (const xAOD::IParticle*& jet : allJets) {
  //for( unsigned int i=0; i<evt->nParts(); ++i ) {
  //  if( evt->part(i)->type() == xAOD::Type::Jet ) {
      if ((fabs(jet->eta()) < m_etamax)  && (fabs(jet->eta()) > m_etamin)) {
        if( jet->pt() > m_cutVal ) {
          jetCounter++;
          // if mu-jet OR flagged for first other muon, decrement counter again in case other muon overlaps with jet
          if (m_rejectJetsOverlappingOtherMuon && muonOverlapsWithJet(jet, evt->otherPart(0))) { //this is very strange, this assumes the first otherPart could be a muon but ommits cases where, e.g., otherPart(0) is not a muon but otherPart(1) is... not sure what the intention was here...
            jetCounter--;
          }
          // if mu-jet OR flagged for signal muons, decrement counter again for each overlap
          if (m_rejectJetsOverlappingSignalMuons) {
            for( unsigned int j=0; j<evt->nParts(); ++j ) {
              if ( evt->part(j)->type() == xAOD::Type::Muon) {
                if ( muonOverlapsWithJet(jet, evt->part(j))) {
                  jetCounter--;
                }
              }
            }
          }
        }
      } else {
        if( jet->pt() > m_fwdcutVal ) {
          switch (m_jetType) {
            case JetType::nominal:

              jetCounter++;
              break;
            case JetType::FJVT:
              {
                bool passFJVT = jet->auxdata<char>("passFJVT");
                DEBUGclass("pass FJVT: %i ", passFJVT);
                if (passFJVT) jetCounter++;
                break;
              }
            case JetType::FJVTTight:
              {
                bool passFJVTTight = jet->auxdata<char>("passFJVTTight");
                DEBUGclass("pass FJVTTight: %i ", passFJVTTight);
                if (passFJVTTight) jetCounter++;
                break;
              }
          }
        }
      }
  //  }
  //}
  }
  return jetCounter;
}

//______________________________________________________________________________________________

HWWJetCounter::HWWJetCounter(const TString& expression, const TString& jetTypeString, float cutVal, float fwdcutVal 
                              , const bool removeOROtherMuon, const bool removeORSignalMuons):
                              // , const HWW::HWWLeptonIDHelper* lepIDHelper):
  TQEventObservable(expression),
  m_cutVal(cutVal),
  m_fwdcutVal(fwdcutVal)
  , m_rejectJetsOverlappingOtherMuon(removeOROtherMuon)
  , m_rejectJetsOverlappingSignalMuons(removeORSignalMuons)
  // , m_leptonIDHelper(lepIDHelper)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());
  // the predefined string member "expression" allows your observable to store an expression of your choice
  // this string will be the verbatim argument you passed to the constructor of your observable
  // you can use it to choose between different modes or pass configuration options to your observable
  this->setExpression(expression);
  //this->SetName(TQObservable::makeObservableName(expression)); //this is pointless, since the same line is in the ctor of TQObservable.
  setJetType(jetTypeString);
  DEBUGclass("Jet pT cut val: %f", m_cutVal);
  DEBUGclass("Forward Jet pT cut val: %f", m_fwdcutVal);
}

//______________________________________________________________________________________________

const TString& HWWJetCounter::getExpression() const {
  // retrieve the expression associated with this observable
  return this->fExpression;
}

//______________________________________________________________________________________________

void HWWJetCounter::setExpression(const TString& expression) {
  this->fExpression = expression;
}

//______________________________________________________________________________________________

bool HWWJetCounter::hasExpression() const {
  // check if this observable type knows expressions
  return true;
}

void HWWJetCounter::setJetType(const TString& jetTypeString) {
  // Only 3 types, so use if-else rather than a map
  if (jetTypeString == "nominal") {
    m_jetType = JetType::nominal;
  }
  else if (jetTypeString == "FJVT") {
    m_jetType = JetType::FJVT;
  }
  else if (jetTypeString == "FJVTTight") {
    m_jetType = JetType::FJVTTight;
  }
  else {
    WARNclass("Unknown jet type '%s' in HWWJetCounter observable. Please use one of 'nominal', 'FJVT', or 'FJVTTight'. Setting to 'nominal'.", jetTypeString.Data());
    m_jetType = JetType::nominal;
  }
}
