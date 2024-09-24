//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __PTLEP1SQUAREDOBS__
#define __PTLEP1SQUAREDOBS__
#include "QFramework/TQTreeObservable.h"
#include "TTreeFormula.h"
#include "TString.h"

class PtLep1SquaredObs : public TQTreeObservable {
protected:
  // Put here any data members your class might need and don't forget potential includes.
  TTreeFormula *fFormula = 0;
  TString mVariation = "";
 
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
public:
  PtLep1SquaredObs();
  PtLep1SquaredObs(const TString& name);
  virtual ~PtLep1SquaredObs();
  ClassDefOverride(PtLep1SquaredObs, 1);

};
#endif
