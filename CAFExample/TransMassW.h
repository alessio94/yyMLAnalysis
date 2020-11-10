//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TRANSMASSW__
#define __TRANSMASSW__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"

/*******************************

  This observable return transverse mass, given a specific
  lepton. It can be configured to use leading, subleading 
  or third lepton, controlled via an int passed to the constructor:

     1: leading lepton
     2: subleading lepton
     3: third lepton (otherPart(0))

  @ Author: Anamika Aggarwal
  **************************************************/

class TransMassW : public TQEventObservable {
  enum which_lepton{unknown=0, leadlep=1, subleadlep=2, thirdlep=3};

protected:

  // put here data members you wish to retrieve
  // be careful to declare them as follows:
  // mutable ClassName const * varname = 0
  // the "mutable" keyword ensures that this member can be changed also by const functions
  // the "const" keyword ensures that const containers can be retrieved
  // for example, use
  // mutable CompositeParticleContainer const * mCand = 0;

  TString fCandName;

  mutable xAOD::CompositeParticleContainer const * mCand = 0; //!


public:
  virtual double getValue() const override;
  virtual bool initializeSelf() override;
public:
  TransMassW();
  /*** constructor, int for which lepton, mT cut in MeV ***/
  TransMassW(const TString& name, const int lepton_id=0 );
  virtual ~TransMassW();
  ClassDefOverride(TransMassW, 1);

private:
  which_lepton m_lepId;




};
#endif
