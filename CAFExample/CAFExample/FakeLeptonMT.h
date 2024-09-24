//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __FAKELEPTONMT__
#define __FAKELEPTONMT__
#include "QFramework/TQTreeObservable.h"
#include "TTreeFormula.h"

class FakeLeptonMT : public TQTreeObservable {
protected:
  // put here any data members your class might need
  TTreeFormula *fFormula = 0;
  
public:
  virtual double getValue() const override;
  virtual TObjArray* getBranchNames() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
public:
  FakeLeptonMT();
  FakeLeptonMT(const TString& name);
  virtual ~FakeLeptonMT();
  ClassDefOverride(FakeLeptonMT, 1);


};
#endif
