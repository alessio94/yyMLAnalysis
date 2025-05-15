//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __PASSWZVETO__
#define __PASSWZVETO__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"

/*******************************

  This observable returns a boolean for vetoing WZ events,
  based on an single-lepton mT cut. It can be configured to
  use leading, subleading or third lepton, controlled via
  an int passed to the constructor:

     1: leading lepton
     2: subleading lepton
     3: third lepton (otherPart(0))

  @ Author: Javier Llorente, slightly modified by Edvin Sidebo
  **************************************************/

class PassWZVeto : public TQEventObservable {
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

  mutable xAOD::CompositeParticleContainer const * mCand = 0;


public:
  virtual double getValue() const override;
  /*** truth version of getValue ***/
  double vetoTruth(const xAOD::MissingET* metObj, const xAOD::TruthParticle* truthPart) const ;
  virtual bool initializeSelf() override;
public:
  PassWZVeto();
  /*** constructor, int for which lepton, mT cut in MeV ***/
  PassWZVeto(const TString& name, const int lepton_id=0, const double mTCut_fakeElectron=50000., const double mTCut_fakeMuon=30000. );
  virtual ~PassWZVeto();
  ClassDefOverride(PassWZVeto, 1);

private:
  const double m_mTCut_fakeElectron=50000.;
  const double m_mTCut_fakeMuon=30000.;
  which_lepton m_lepId;




};
#endif
