#include "CAFExample/HWWMtautauObs.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"
#include "QFramework/TQSample.h"

ClassImp(HWWMtautauObs)

//______________________________________________________________________________________________

HWWMtautauObs::HWWMtautauObs(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWMtautauObs::~HWWMtautauObs(){
  // default destructor
  DEBUGclass("destructor called");
} 

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
/* example block:
//#include "xAODParticleEvent/CompositeParticleContainer.h"
*/

//______________________________________________________________________________________________

int HWWMtautauObs::getLep0Index() {
  return m_lep0Index;
}

int HWWMtautauObs::getLep1Index() {
  return m_lep1Index;
}

//______________________________________________________________________________________________

void HWWMtautauObs::setLep0Index(int lep0Index) {
  m_lep0Index = lep0Index;
}

void HWWMtautauObs::setLep1Index(int lep1Index) {
  m_lep1Index = lep1Index;
}

//______________________________________________________________________________________________

double HWWMtautauObs::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the TQEventObservable only works in an ASG RELEASE, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add an ASG Analysis Release in order to use this feature!"
  return std::numeric_limits<double>::quiet_NaN();
  #else
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!
  
  // first, you can retrieve the data members you needwith the 'retrieve' method
  // as arguments to this function, pass the member pointer to the container as well as the container name
    /* example block:
  // Retrieve CompositeParticle container
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
  DEBUGclass("failed to retrieve candidates!");
  return false;
  }
   */

  // after you have retrieved your data members, you can proceed to calculate the return value
  // probably, you first have to retrieve an element from the container
  /* example block:
  const xAOD::CompositeParticle *Evt = cand->at(0);
  */

  // in the end, you should calculate your return value 
  // of course, you can use other data members of your observable at any time
  /* example block:
     const double retval = Evt->part(0)->pt();
  */

  // Retrieve CompositeParticle container
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return false;
  }

  // Get the event candidate from the container
  const xAOD::CompositeParticle *Evt = cand->at(0);

  // Make sure we have at least 2 particles
  if ( Evt->nParts() < 2 ){
    DEBUGclass("We got less than 2 constituent particles for this CompositeParticle! Return 0");
    return 0.0;
  }

  // Get the two leptons and the missing ET
  const xAOD::IParticle* lep0 = Evt->part(m_lep0Index);
  const xAOD::IParticle* lep1 = Evt->part(m_lep1Index);
  const xAOD::MissingET* met = Evt->missingET();
  const xAOD::IParticle::FourMom_t& lep0P4 = lep0->p4();
  const xAOD::IParticle::FourMom_t& lep1P4 = lep1->p4();

  // Calculate the quantities
  const double mll = (lep0P4+lep1P4).M();
  const double lep0Px = lep0P4.Px();
  const double lep0Py = lep0P4.Py();
  const double lep1Px = lep1P4.Px();
  const double lep1Py = lep1P4.Py();
  const double mpx = met->mpx();
  const double mpy = met->mpy();
  const double x_1 = (lep0Px*lep1Py-lep0Py*lep1Px) / (lep1Py*mpx-lep1Px*mpy+lep0Px*lep1Py-lep0Py*lep1Px);
  const double x_2 = (lep0Px*lep1Py-lep0Py*lep1Px) / (lep0Px*mpy-lep0Py*mpx+lep0Px*lep1Py-lep0Py*lep1Px);
  DEBUGclass("x_1 = %f, and x_2 = %f", x_1, x_2);
  // default return value
  double mtautau = -9.0;
  if(x_1 > 0.0 && x_2 > 0.0){ mtautau = mll/std::sqrt(x_1*x_2); }

  DEBUGclass("Got mTauTau=%f", mtautau);
  return mtautau;
  #endif
}
//______________________________________________________________________________________________

bool HWWMtautauObs::initializeSelf(){
  // initialize this observable
  // called once per sample (input file) so that the observable knows the name of the event candidate
  // will be EventEM or EventME in the case of DF analysis (depending on the current channel)
  TString ContName = "";
  if(!this->fSample->getTagString(this->fDoTruth ? "~cand.original" : "~cand" ,ContName)) return false;
  this->mContName = TString::Format("Event%s%s", this->fDoTruth ? "Truth" : "", ContName.Data());
  
  DEBUGclass("initializing");
  return true;
}

//______________________________________________________________________________________________

bool HWWMtautauObs::finalizeSelf(){
  // finalize this observable
  // remember to undo anything you did in initializeSelf() !
  
  DEBUGclass("finalizing");
  return true;
}
//______________________________________________________________________________________________

HWWMtautauObs::HWWMtautauObs(const TString& name, int lep0Index, int lep1Index):
TQEventObservable(name),
m_lep0Index(lep0Index),
m_lep1Index(lep1Index)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
}
