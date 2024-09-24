#include "CAFExample/PtLep1SquaredObs.h"
#include "TObjString.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"
#include "QFramework/TQSample.h"

ClassImp(PtLep1SquaredObs)

//______________________________________________________________________________________________

PtLep1SquaredObs::PtLep1SquaredObs(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

PtLep1SquaredObs::~PtLep1SquaredObs(){
  // default destructor
  DEBUGclass("destructor called");
} 

//______________________________________________________________________________________________

TObjArray* PtLep1SquaredObs::getBranchNames() const {
  // retrieve the list of branch names 
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = new TObjArray();
  bnames->SetOwner(false);

  // add the branch names needed by your observable here, e.g.
  bnames->Add(new TObjString("el_pt_" + mVariation));
  
  return bnames;
}

//______________________________________________________________________________________________

double PtLep1SquaredObs::getValue() const {
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!
  
  // here, you should calculate your return value
  // of course, you can use other data members of your observable at any time
  const double retval = this->fFormula->EvalInstance();

  DEBUGclass("returning");
  return retval;
}
//______________________________________________________________________________________________

bool PtLep1SquaredObs::initializeSelf(){
  // initialize this observable
  // called once per sample (input file) so that the observable knows the name of the variation
  DEBUGclass("initializing");

  // Obtain the variation name
  TString variation = "";
  if (!this->fSample->getTagString("~Variation", variation)) {
    ERRORclass("Could not get tag string ~Variation.");
    return false;
  }
  this->mVariation = variation;

  // since this function is only called once per sample, we can
  // perform any checks that seem necessary
  if (!this->fTree){
    DEBUGclass("no tree, terminating");
    return false;
  }

  // create string expression that calculates pTlep1^2
  TString PtLep1Squared = "pow(el_pt_" + mVariation + "[0], 2)";
  DEBUGclass("Configured expression: %s", PtLep1Squared.Data());

  this->fFormula = new TTreeFormula("PtLep1Squared", PtLep1Squared.Data(), this->fTree);

  return true;
}

//______________________________________________________________________________________________

bool PtLep1SquaredObs::finalizeSelf(){
  // finalize this observable
  // remember to undo anything you did in initializeSelf() !
  
  DEBUGclass("finalizing");

  delete this->fFormula;
  this->fFormula = 0;

  return true;
}
//______________________________________________________________________________________________

PtLep1SquaredObs::PtLep1SquaredObs(const TString& name):
TQTreeObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
}
