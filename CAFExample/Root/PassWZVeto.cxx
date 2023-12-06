#include "CAFExample/PassWZVeto.h"
#include <limits>
#include <iostream>
using namespace std;

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(PassWZVeto)

//______________________________________________________________________________________________

PassWZVeto::PassWZVeto(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

PassWZVeto::~PassWZVeto(){
  // default destructor
  DEBUGclass("destructor called");
}

bool PassWZVeto::initializeSelf() {

  TString s;
  if(!this->fSample->getTagString("~cand",s)){
    return false;
  }
  this->fCandName = "Event"+s;
  return  true;
}

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
//#include "xAODParticleEvent/CompositeParticleContainer.h"

//______________________________________________________________________________________________
double PassWZVeto::vetoTruth(const xAOD::MissingET* metObj, const xAOD::TruthParticle* truthPart) const {
  
  if (!(truthPart->isElectron() || truthPart->isMuon()))
  {
    //DEBUGclass("running on truth: particle isn't muon or electron. should not end up here.");
    throw std::runtime_error("[PassWZVeto] inside vetoTruth. fake truth particle is not electron or muon! Should not end up here");
  }

  bool doVeto(false);

	double deltaPhi = fabs(truthPart->phi() - metObj->phi());
	if (deltaPhi >= TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;
	double transMass2 = 2*truthPart->pt()*metObj->met()*(1-cos(deltaPhi));
	if (sqrt(transMass2) > (truthPart->isElectron() ? m_mTCut_fakeElectron : m_mTCut_fakeMuon)) doVeto = true;

  DEBUGclass("running on truth: returning passVeto=%d", !doVeto);
  // return true if passed
  return static_cast<double>(!doVeto);

}



//______________________________________________________________________________________________
double PassWZVeto::getValue() const {

  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the TQEventObservable only works in an ASG environment which HAS_XAOD, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add '-DASG_RELEASE' to your packages 'Makefile.RootCore'"
  return std::numeric_limits<double>::quiet_NaN();
  #else
  // the following protective if-block should NEVER evaluate true
  // for performance reasons, we disable it per preprocessor directive unless in debugging mode
  #ifdef _DEBUG_
  if(!this->fEvent){
    DEBUGclass("failed to access xAOD::TEvent member!");
    return 0;
  }
  #endif

  if (!this->fEvent->retrieve(this->mCand,this->fCandName.Data()).isSuccess()){
    DEBUGclass("Failed to retrieve event candidate!");//" %s", this->fCandName.Data()).Data());
    return 0.;
  }
  // get the event candidate
  const xAOD::CompositeParticle *evtCand = this->mCand->at(0);

  const xAOD::MissingET * metObj = evtCand->missingET();

  const xAOD::IParticle* the_lepton = nullptr; // to be filled below

  switch (m_lepId) {
    case leadlep:
      the_lepton = evtCand->part(0);
      break;
    case subleadlep:
      the_lepton = evtCand->part(1);
      break;
    case thirdlep:
      the_lepton = evtCand->otherPart(0);
      break;
    default:
      throw std::runtime_error("[PassWZVeto] inside getValue() ::  unknown lep identifier!");
  }
  if (!the_lepton || !(the_lepton->type() == xAOD::Type::Electron || the_lepton->type() == xAOD::Type::Muon)) {
    // if we're running on truth the obj will be a truth particle
    const xAOD::TruthParticle* truthPart = static_cast<const xAOD::TruthParticle*>(the_lepton);
    if (truthPart )  {
      return vetoTruth(metObj, truthPart);
    }
      
    throw std::runtime_error("[PassWZVeto] lepton is null or is not electron or muon!");
  }

  // std::cout << "Running with mode " << m_lepId << ", nOtherMuons = " << evtCand->nOtherMuons() << ", otherLep(0)->type() = " << evtCand->otherPart(0)->type() << ", the_lepton->type() = " << the_lepton->type() << std::endl; 

  bool doVeto(false);

  if (the_lepton->type() == xAOD::Type::Electron) { // lepton is electron
	  const xAOD::Electron * elec1 = static_cast<const xAOD::Electron*>(the_lepton);
	  double deltaPhi = fabs(elec1->phi()-metObj->phi());
	  if (deltaPhi >= TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;
	  double transMass2 = 2*elec1->pt()*metObj->met()*(1-cos(deltaPhi));
	  if (sqrt(transMass2) > m_mTCut_fakeElectron) doVeto = true;
  } else { // muon
    const xAOD::Muon * muon1 = static_cast<const xAOD::Muon*>(the_lepton);
    double deltaPhi = fabs(muon1->phi()-metObj->phi());
    if (deltaPhi >= TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;
    double transMass2 = 2*muon1->pt()*metObj->met()*(1-cos(deltaPhi));
    if (sqrt(transMass2) > m_mTCut_fakeMuon) doVeto = true;
  }

  DEBUGclass("returning passVeto=%d", !doVeto);
  // return true if passed
  return static_cast<double>(!doVeto);
  #endif
}
//______________________________________________________________________________________________

PassWZVeto::PassWZVeto(const TString& name, const int lepton_id, const double mTCut_fakeElectron, const double mTCut_fakeMuon ):
  TQEventObservable(name)
, m_mTCut_fakeElectron(mTCut_fakeElectron)
, m_mTCut_fakeMuon(mTCut_fakeMuon)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
  // check that int is 1, 2 or 3
  if (!(lepton_id == 1 || lepton_id == 2 || lepton_id == 3)) {
    throw std::runtime_error("[PassWZVeto] unknown lepton idx passed! Must be 1 (leadLep), 2 (subleadLep) or 3 (third lep)");
  }
  // set lepton idx enum
  m_lepId = static_cast<which_lepton>(lepton_id);


}
