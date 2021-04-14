#include "CAFExample/FakeLeptonMT.h"
#include "TObjString.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

ClassImp(FakeLeptonMT)

//______________________________________________________________________________________________

FakeLeptonMT::FakeLeptonMT(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

FakeLeptonMT::~FakeLeptonMT(){
  // default destructor
  DEBUGclass("destructor called");
}


//______________________________________________________________________________________________

TObjArray* FakeLeptonMT::getBranchNames() const {
  // retrieve the list of branch names
  // ownership of the list belongs to the caller of the function
  DEBUGclass("retrieving branch names");
  TObjArray* bnames = new TObjArray();
  bnames->SetOwner(false);

  // add the branch names needed by your observable here, e.g.
  bnames->Add(new TObjString("fakecandLep_pt"));
  bnames->Add(new TObjString("fakecandLep_phi"));
  bnames->Add(new TObjString("metObj_met"));
  bnames->Add(new TObjString("metObj_phi"));

  return bnames;
}

//______________________________________________________________________________________________

double FakeLeptonMT::getValue() const {
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!

  // here, you should calculate your return value
  // of course, you can use other data members of your observable at any time
  /* example block for TTreeFormula method:
     const double retval = this->fFormula->EvalInstance();
  */
  /* exmple block for TTree::SetBranchAddress method:
  const double retval = this->fBranch1 + this->fBranch2;
  */

  const double retval = this->fFormula->EvalInstance();

  DEBUGclass("returning");
  return retval;
}
//______________________________________________________________________________________________

bool FakeLeptonMT::initializeSelf(){
  // initialize this observable on a sample/tree
  DEBUGclass("initializing");

  // since this function is only called once per sample, we can
  // perform any checks that seem necessary
  if(!this->fTree){
    DEBUGclass("no tree, terminating");
    return false;
  }

  // if you want to use a TTreeFormula, can may construct it here
  /* example block for TTreeFormula method:
  this->fFormula = new TTreeFormula("myFormula", "branch1 + branch2",this->fTree);
  */

  // create string expression that calculates fakelepton MT
  TString fakeCandLep_phi = "fakecandLep_phi";
  TString metObj_met = "metObj_met";
  TString metObj_phi = "metObj_phi";

  TString deltaPhi = "TVector2::Phi_mpi_pi( fabs("+fakeCandLep_phi+" - "+metObj_phi+") )";
  TString MT_squared = "2*fakecandLep_pt*metObj_met*(1-cos("+deltaPhi+"))";
  TString MT = "sqrt("+MT_squared+")";

  DEBUGclass("Configured expression: %s", MT.Data());

  this->fFormula = new TTreeFormula("transvserse_mass", MT.Data(), this->fTree);

  return true;
}

//______________________________________________________________________________________________

bool FakeLeptonMT::finalizeSelf(){
  // finalize this observable on a sample/tree
  DEBUGclass("finalizing");
  // here, you should undo anything that you did while initializing
  /* example block for TTreeFormula method:
  delete this->fFormula;
  this->fFormula = NULL;
  */

  delete this->fFormula;
  this->fFormula = NULL;

  return true;
}
//______________________________________________________________________________________________

FakeLeptonMT::FakeLeptonMT(const TString& name):
TQTreeObservable(name)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
}
