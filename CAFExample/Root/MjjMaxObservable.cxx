#include "CAFExample/MjjMaxObservable.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(MjjMaxObservable)

//______________________________________________________________________________________________

MjjMaxObservable::MjjMaxObservable(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

MjjMaxObservable::~MjjMaxObservable(){
  // default destructor
  DEBUGclass("destructor called");
} 

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
//#include "xAODParticleEvent/CompositeParticleContainer.h"

//______________________________________________________________________________________________

double MjjMaxObservable::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");

  // the TQEventObservable only works in an ASG environment which HAS_XAOD, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add '-DASG_RELEASE' to your packages 'Makefile.RootCore'"
  return std::numeric_limits<double>::quiet_NaN();
  #else
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!

  // first, you can retrieve the data members you needwith the 'retrieve' method:

  // Retrieve CompositeParticle container
  if(!this->fEvent->retrieve(this->mCand, this->mCandName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return false;
  }

  DEBUGclass("retrieved candidates! %s",this->mCandName.Data());

  DEBUGclass(" this->mCand->size(): %i",this->mCand->size());

  // after you have retrieved your data members, you can proceed to calculate the return value
  // probably, you first have to retrieve an element from the container:

  // Retrieve event candidate
  const xAOD::CompositeParticle *Evt = this->mCand->at(0);

  int nParts = Evt->nParts();

  DEBUGclass(" Evt->nParts(): %i",Evt->nParts());

  // Parts of event candidates:
  //
  // part(0):    leading lepton
  // part(1):    subleading lepton
  // part(2...): jets
  //
  // otherPart(x): additional leptons

  // in the end, you should calculate your return value
  // of course, you can use other data members of your observable at any time

  // calculate maximum jet pair mass
  // if nJets < 2 (nParts < 4), return a value of 0
  double MjjMax = 0;

  for ( int i=2; i < nParts; ++i ){
    for ( int j=i+1; j < nParts; ++j ){

      double tmp_Mjj = Evt->p4(i,j).M();

      if (tmp_Mjj > MjjMax){
        MjjMax = tmp_Mjj;
      }

    }
  }

  const double retval = MjjMax;

  DEBUGclass(" retval: %f",retval);

  DEBUGclass("returning");
  return retval;
  #endif
}
//______________________________________________________________________________________________

bool MjjMaxObservable::initializeSelf(){

  // called once per sample (input file) so that the observable knows the name of the event candidate
  // will be EventEM or EventME in the case of DF analysis (depending on the current channel)

  TString CandName = "";
  if(!this->fSample->getTagString("~cand",CandName)) return false;
  this->mCandName = "Event"+CandName;

  return true;
}

MjjMaxObservable::MjjMaxObservable(const TString& name):
TQEventObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
}
