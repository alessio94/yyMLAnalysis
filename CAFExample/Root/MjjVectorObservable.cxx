#include "CAFExample/MjjVectorObservable.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(MjjVectorObservable)

//______________________________________________________________________________________________

MjjVectorObservable::MjjVectorObservable(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

MjjVectorObservable::~MjjVectorObservable(){
  // default destructor
  DEBUGclass("destructor called");
} 

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
//#include "xAODParticleEvent/CompositeParticleContainer.h"

//______________________________________________________________________________________________

bool MjjVectorObservable::makeCache() const {
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

  // **********************************************************>>
  if (this->getCurrentEntry() == this->fCachedEntry) {
    return true; //nothing to do here, return value already cached
  }

  // Here you should clear the old cache before recalculating it for new event
  /* example block
     myvector.clear();
  */
  // <<********************************************************
  vec_Mjj.clear();
  
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
  for ( int i=2; i < nParts; ++i ){
    for ( int j=i+1; j < nParts; ++j ){
      double tmp_Mjj = Evt->p4(i,j).M();
      vec_Mjj.push_back(tmp_Mjj);
    }
  }

  // ************************************>>
  this->fCachedEntry = this->getCurrentEntry();
  DEBUGclass("Created new set of cached values in instance '%s'",this->GetName());

  return true;
  // next stuff to be removed for vector observable snippet
  // const double retval = MjjMax;

  // DEBUGclass(" retval: %f",retval);

  // DEBUGclass("returning");
  // return retval;
  // <<****************************************
  #endif
}

//______________________________________________________________________________________________

//*******************************************>>
const std::vector<double>* MjjVectorObservable::getVector() const {
  // Function to retrieve the cached vector
  /* example block
     return &myvector;
  */
  
  return &vec_Mjj;
  
  //this should never be executed, we just make the compiler calm down
  throw std::runtime_error("Illegal / unsupported mode selected!");
  return NULL;
}

//______________________________________________________________________________________________

double MjjVectorObservable::getValue() const {
  // Value retrieval function, called on every event for every cut and histogram
  // Since this is a vector observable this should never be executed
  DEBUGclass("entering function getValue");
  throw std::runtime_error("Caught attempt to evaluate vector valued observable in scalar context");
  return std::numeric_limits<double>::quiet_NaN();
}

//______________________________________________________________________________________________

double MjjVectorObservable::getValueAt(int index) const {
  if (!this->makeCache()) {
    ERROR("Failed to obtain return values!");
    return std::numeric_limits<double>::quiet_NaN();
  }

  const std::vector<double>* vec = this->getVector();
	
  if ((int)vec->size() == 0) { // vector is empty, not even filled once
    throw std::runtime_error("Attempted to retrieve value of vector that is empty");
    return std::numeric_limits<double>::quiet_NaN(); 
  }
  if (index >= (int)vec->size()) {
    throw std::runtime_error("Attempted to retrieve value for out of bounds");
    return std::numeric_limits<double>::quiet_NaN();
  }
  return vec->at(index);
}

//______________________________________________________________________________________________

int MjjVectorObservable::getNevaluations() const {
  // This function returns the size of the vector that is goint to be retrieved
  
  if (!this->makeCache()) {
    ERROR("Failed to obtain return values!");
    return -1;
  }
  if (this->fType==TQObservable::ObservableType::scalar) return 1;
  const std::vector<double>* vec = this->getVector();
  if (vec) return vec->size();
  return -1;
}

// <<*********************************************

//______________________________________________________________________________________________

bool MjjVectorObservable::initializeSelf(){

  // called once per sample (input file) so that the observable knows the name of the event candidate
  // will be EventEM or EventME in the case of DF analysis (depending on the current channel)

  TString CandName = "";
  if(!this->fSample->getTagString("~cand",CandName)) return false;
  this->mCandName = "Event"+CandName;

  // *************************************>>
  // initialize cached entry to -1
  fCachedEntry = -1;
  // <<*******************************
  
  return true;
}

//______________________________________________________________________________________________

//***********************************************>>
bool MjjVectorObservable::finalizeSelf(){
  // finalize self - delete accessor
  /* example block
     myvector.clear();
  */
  vec_Mjj.clear();

  return true;
}

// <<**********************************************

//______________________________________________________________________________________________

MjjVectorObservable::MjjVectorObservable(const TString& name):
TQEventObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
}
