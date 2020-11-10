#include "CAFExample/HWWInvMass2L.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(HWWInvMass2L)

//______________________________________________________________________________________________

HWWInvMass2L::HWWInvMass2L(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________
bool HWWInvMass2L::initializeSelf(){
  TString CandName = "";
  if(!this->fSample->getTagString("~cand",CandName)) return false;
  this->mCandName = "Event"+CandName;
  return true;
}

//______________________________________________________________________________________________

HWWInvMass2L::~HWWInvMass2L(){
  // default destructor
  DEBUGclass("destructor called");
}

#define XAOD_STANDALONE 1

double HWWInvMass2L::getInvMass(const xAOD::IParticle* part_one, const xAOD::IParticle* part_two) const {
  // make sure both particles are OK
  if (! (part_two && part_two))
  { throw std::runtime_error("[HWWInvMass2L] inside getMll() :: at least one of your particles are null. You should make your invariant-mass members exist before calling this observable."); }

  TLorentzVector part1; part1.SetPtEtaPhiE(part_one->pt(), part_one->eta(), part_one->phi(), part_one->e());
  TLorentzVector part2; part2.SetPtEtaPhiE(part_two->pt(), part_two->eta(), part_two->phi(), part_two->e());
  return (part1+part2).M();

}
//______________________________________________________________________________________________

double HWWInvMass2L::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the TQEventObservable only works in an ASG RELEASE, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add an ASG Analysis Release in order to use this feature!"
  return std::numeric_limits<double>::quiet_NaN();
  #else
  // get event candidate
  if(!this->fEvent->retrieve(this->mCand, this->mCandName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return 0;
  }
  // DEBUGclass("retrieve candidates! %s",this->mCandName.Data());
  const xAOD::CompositeParticle *Evt = this->mCand->at(0);
  // DEBUGclass(" this->mCand->size() %i",this->mCand->size());
  if(this->mCand->size()==0) return -99.;

  double invmass(-99.);
  // which objects to use for inv mass
  switch (m_invMassPair) {
    case objectPair::LEAD_AND_SUBLEAD:
      invmass = getInvMass(Evt->part(0), Evt->part(1));
      break;
    case objectPair::LEAD_AND_THIRD:
      invmass = getInvMass(Evt->part(0), Evt->otherPart(0));
      break;
    case objectPair::SUBLEAD_AND_THIRD:
      invmass = getInvMass(Evt->part(1), Evt->otherPart(0));
      break;
    default:
      throw std::runtime_error("[HWWInvMass2L] unrecognised objectPair enum! This shouldn't happen. Check your initialisation of this observable.");
  }

  DEBUGclass("returning %f", invmass);
  return invmass;
  #endif
}
//______________________________________________________________________________________________

HWWInvMass2L::HWWInvMass2L(const TString& name, const int idx_firstlep, const int idx_secondlep):
TQEventObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
  if (idx_firstlep == 0 && idx_secondlep == 1) {
    m_invMassPair = objectPair::LEAD_AND_SUBLEAD;
  } else if (idx_firstlep == 0 && idx_secondlep == 2) {
    m_invMassPair = objectPair::LEAD_AND_THIRD;
  } else if (idx_firstlep == 1 && idx_secondlep == 2) {
    m_invMassPair = objectPair::SUBLEAD_AND_THIRD;
  } else {
    throw std::runtime_error("[HWWInvMass2L] inside constructor :: particle indices currently supported are (0,1), (0,2) or (1,2), where 0=leadLep, 1=subleadLep, 2=thirdLep (first otherLep). Check your initialisation.");
  }
}
