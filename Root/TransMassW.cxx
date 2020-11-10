#include "CAFExample/TransMassW.h"
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

ClassImp(TransMassW)

//______________________________________________________________________________________________

TransMassW::TransMassW(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

TransMassW::~TransMassW(){
  // default destructor
  DEBUGclass("destructor called");
}

bool TransMassW::initializeSelf() {

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
double TransMassW::getValue() const {

  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the TQEventObservable only works in an ASG RELEASE, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add an ASG Analysis Release in order to use this feature!"
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
      throw std::runtime_error("[TransMassW] inside getValue() ::  unknown lep identifier!");
  }

  // std::cout << "Running with mode " << m_lepId << ", nOtherMuons = " << evtCand->nOtherMuons() << ", otherLep(0)->type() = " << evtCand->otherPart(0)->type() << ", the_lepton->type() = " << the_lepton->type() << std::endl; 

double transMass2 = 0;

  if (the_lepton->type() == xAOD::Type::Electron) { // lepton is electron
	  const xAOD::Electron * elec1 = static_cast<const xAOD::Electron*>(the_lepton);
	  double deltaPhi = fabs(elec1->phi()-metObj->phi());
	  if (deltaPhi >= TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;
	  transMass2 = 2*elec1->pt()*metObj->met()*(1-cos(deltaPhi));
  } else { // muon
    const xAOD::Muon * muon1 = static_cast<const xAOD::Muon*>(the_lepton);
    double deltaPhi = fabs(muon1->phi()-metObj->phi());
    if (deltaPhi >= TMath::Pi()) deltaPhi = 2*TMath::Pi()-deltaPhi;
    transMass2 = 2*muon1->pt()*metObj->met()*(1-cos(deltaPhi));
  }

  DEBUGclass("returning transMass=%d", transMass2);
  // return transverse mass value
  return sqrt(transMass2);
  #endif
}
//______________________________________________________________________________________________

TransMassW::TransMassW(const TString& name, const int lepton_id ):
  TQEventObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
  // check that int is 1, 2 or 3
  if (!(lepton_id == 1 || lepton_id == 2 || lepton_id == 3)) {
    throw std::runtime_error("[TransMassW] unknown lepton idx passed! Must be 1 (leadLep), 2 (subleadLep) or 3 (third lep)");
  }
  // set lepton idx enum
  m_lepId = static_cast<which_lepton>(lepton_id);


}
