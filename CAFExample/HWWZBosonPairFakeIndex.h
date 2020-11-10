//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWZBOSONPAIRFAKEINDEX__
#define __HWWZBOSONPAIRFAKEINDEX__
#include "CAFxAODUtils/TQEventObservable.h"
#include "CAFExample/HWWLeptonIDHelper.h"
#include "CAFExample/HWWTrigConfig.h"
#include "CAFExample/HWWTrigBase.h"
// #include "CAFExample/HWWTriggerClassifier.h"

#ifndef __CINT__
#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODEventInfo/EventInfo.h"
#else
// put here forward declares for the classes you might need, e.g.
class CompositeParticle;
class CompositeParticleContainer;
#endif

/*******************************

  This observable returns an enum corresponding to the index of the fake candidate lepton,
  based on which pair of leptons is the best Z boson candidate:
  lep0+lep1, lep0+otherLep0 or lep1+otherLep0.
  A Z-boson candidate pair will need to be SFOS, both be ID, and have at least one
  of the pairs matched to one of the single lepton triggers.


  The return values are:
    0: fakeIndex = 0: no fake candidate found (either because there are only two leptons, or because no Z candidate was found)
    1: fakeIndex = 1: leading lepton lep0 is fake (lep1+otherLep0 is the best Z candidate pair)
    2: fakeIndex = 2: subleading lepton lep1 is fake (lep0+otherLep0 is the best Z candidate pair)
    3: fakeIndex = 3: otherLep0 is fake (lep0+lep1 is the best Z candidate pair)


  Author: Edvin Sidebo, with help from Javier Llorente, edvin.sidebo@cern.ch
  **************************************************/

using namespace HWW; // use HWW namespace from dual use utils

enum zBosonPairIdentifier{NO_PAIR_FOUND=0, LEP0_AND_LEP1=1, LEP0_AND_OTHERLEP0=2, LEP1_AND_OTHERLEP0=3};
enum fakeIndex{NO_FAKE_FOUND=0, LEADLEP=1, SUBLEADLEP=2, OTHERLEP0=3};


class HWWZBosonPairFakeIndex : public TQEventObservable, public HWWTrigBase {
  // inherits from HWWTrigBase to access isMatchedParticle() function

protected:
  // put here data members you wish to retrieve
  // be careful to declare them as follows:
  // mutable ClassName const * varname = 0
  // the "mutable" keyword ensures that this member can be changed also by const functions
  // the "const" keyword ensures that const containers can be retrieved
  // for example, use
  mutable xAOD::CompositeParticleContainer const * mCand = 0;

  mutable Long64_t fCachedEntry; //!
  mutable double fCachedValue; //!

  // lepton id helper to check which object is anti-id
  const HWWLeptonIDHelper* fLeptonIDHelper = nullptr;

  TString fCandName;

  /*** lepton pair is SFOF ***/
  bool isSFOS(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const;
  /*** lepton pair is ID (both leptons) ***/
  bool isID(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const;
  /*** at least one of the leptons is trigger matched ***/
  bool isTriggerMatched(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair, const xAOD::EventInfo* evtInfo) const;
  /*** |Mll-MZ| is within Z-window ***/
  bool isWithinZWindow(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair, const xAOD::Type::ObjectType fakeType) const;
  /***** get Mll ***/
  float getMll(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const;
  /**** get |Mll-MZ| ***/
  float getAbsMllMZDiff(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const;
  /*** fulfils all Z-candidate requirements ***/
  bool isGoodZCand(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair, const xAOD::EventInfo* evtInfo, const xAOD::Type::ObjectType fakeType) const;
  /*** fill map with Zboson candidate pairs ****/
  void makeZBosonCandPairs(std::map< zBosonPairIdentifier, float>& zPairCands,
                                          const xAOD::CompositeParticle* Evt,
                                          const xAOD::IParticle* otherLep0, const xAOD::EventInfo* evtInfo) const;




public:
  virtual bool initializeSelf() override;
  virtual double getValue() const override;
  // getClone override (so we don't have to rely on root streamer)
  TQObservable* getClone() const  override;

public:

  // default constructor
  HWWZBosonPairFakeIndex();
  // copy constructor
  HWWZBosonPairFakeIndex(const HWWZBosonPairFakeIndex& obs);
  // nominal constructor
  HWWZBosonPairFakeIndex(const TString& name, const std::vector<HWWTrigConfig*>& trigConfigs, const HWWLeptonIDHelper* lepIDHelper);
  // nominal constructor
  HWWZBosonPairFakeIndex(const TString& name, const HWWLeptonIDHelper* lepIDHelper);
  virtual ~HWWZBosonPairFakeIndex();
  ClassDefOverride(HWWZBosonPairFakeIndex, 1);

private:
  bool evtNrUnique(const xAOD::EventInfo* evtInfo) const;
  // const float fMllMZWindow = 25.; // Z window in GeV
  const float fMZWindow_low_muon = 70.;
  const float fMZWindow_low_electron = 80.;
  const float fMZWindow_up = 110.;
  const float fZMass = 91.1876; // Z mass in GeV
  bool debugMissingEvts=false;


};
#endif
