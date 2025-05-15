//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __MJJVECTOROBSERVABLE__
#define __MJJVECTOROBSERVABLE__
#include "CAFxAODUtils/TQEventObservable.h"

// #ifndef __CINT__
// #ifdef ASG_RELEASE
#include "xAODParticleEvent/CompositeParticleContainer.h"
// #endif
// #endif

class MjjVectorObservable : public TQEventObservable {
protected:
  // put here data members you wish to retrieve
  // be careful to declare them as follows:
  // mutable ClassName const * varname = 0
  // the "mutable" keyword ensures that this member can be changed also by const functions
  // the "const" keyword ensures that const containers can be retrieved
  /* example block
     mutable CompositeParticleContainer const * mCand = 0;
     mutable std::vector<float> myvector = std::vector<float>();
  */

  mutable Long64_t fCachedEntry;
  
  TString mCandName = "";

// #ifndef __CINT__
// #ifdef ASG_RELEASE

  mutable xAOD::CompositeParticleContainer const * mCand = 0;
  mutable std::vector<double> vec_Mjj = std::vector<double>();

// #endif
// #endif
  
  bool makeCache() const;

public:
  
  const std::vector<double>* getVector() const;
  virtual double getValue() const override;
  virtual double getValueAt(int index) const override;
  virtual int getNevaluations() const override;
  TQObservable::ObservableType fType = TQObservable::ObservableType::vector;
  inline virtual TQObservable::ObservableType getObservableType() const override {
    return fType;
  }
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
  
public:
  MjjVectorObservable();
  MjjVectorObservable(const TString& name);
  virtual ~MjjVectorObservable();
  ClassDefOverride(MjjVectorObservable, 1);


};
#endif
