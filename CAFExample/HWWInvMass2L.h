//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWINVMASS2L__
#define __HWWINVMASS2L__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"

/*****

  This observable will help to compute the invariant mass between
  the leading/subleading lepton and a lepton in the otherPart()
  container.

  @ Author: Edvin Sidebo, edvin.sidebo@cern.ch


****/

class HWWInvMass2L : public TQEventObservable {
protected:
  /** enum identifying which two objects to use ***/
  enum objectPair{LEAD_AND_SUBLEAD=0, LEAD_AND_THIRD, SUBLEAD_AND_THIRD};

  objectPair m_invMassPair; // will be set in constructor

  // the "mutable" keyword ensures that this member can be changed also by const functions
  // the "const" keyword ensures that const containers can be retrieved
  mutable xAOD::CompositeParticleContainer const * mCand = 0; //!

public:
  virtual double getValue() const override;
  TString mCandName = "";
public:
  virtual bool initializeSelf() override;
  HWWInvMass2L();
  /*** constructor with indices of the leptons to go in the invariant mass calculation: 0=lead lep, 1=sublead lep, 2=first otherLep ***/
  HWWInvMass2L(const TString& name, const int idx_firstlep=0, const int idx_secondlep=1);
  virtual ~HWWInvMass2L();
  ClassDefOverride(HWWInvMass2L, 1);

private:
  /*** compute the invariant mass ***/
  double getInvMass(const xAOD::IParticle* part_one, const xAOD::IParticle* part_two) const;


};
#endif
