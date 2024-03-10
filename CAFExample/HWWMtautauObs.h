//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWMTAUTAUOBS__
#define __HWWMTAUTAUOBS__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"

class HWWMtautauObs : public TQEventObservable {
protected:
  // Put here data members you wish to use.
  // For example you might want to add a variable
  // for the name of the xAOD container to be retrieved that
  // will be initialized in the initialize function.
  TString mContName = ""; 

  mutable xAOD::CompositeParticleContainer const * mCand = 0;

public:
  virtual double getValue() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
  bool fDoTruth = false;
public:
  HWWMtautauObs();
  HWWMtautauObs(const TString& name, int lep0Index=0, int lep1Index=1);
  virtual ~HWWMtautauObs();
  ClassDefOverride(HWWMtautauObs, 1);

  int getLep0Index();
  int getLep1Index();

  void setLep0Index(int lep0Index);
  void setLep1Index(int lep1Index);

  inline void setDoTruth(bool doTruth= false) { this->fDoTruth=doTruth; };
  inline bool getDoTruth() const { return this->fDoTruth; };

private:
  int m_lep0Index;
  int m_lep1Index;


};
#endif
