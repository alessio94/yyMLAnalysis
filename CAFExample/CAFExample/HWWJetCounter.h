//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWJETCOUNTER__
#define __HWWJETCOUNTER__
#include "CAFxAODUtils/TQEventObservable.h"
// #include "HWWDualUseUtils/HWWLeptonIDHelper.h"

#ifndef __CINT__
#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "AthContainers/AuxElement.h"
using namespace xAOD;
#else
// put here forward declares for the classes you might need, e.g.
// class CompositeParticle;
// class CompositeParticleContainer;
#endif

enum class JetType {
  nominal,
  FJVT,
  FJVTTight
};

class HWWJetCounter : public TQEventObservable {

protected:
  // put here data members you wish to retrieve
  // be careful to declare them as follows:
  // mutable ClassName const * varname = 0
  // the "mutable" keyword ensures that this member can be changed also by const functions
  // the "const" keyword ensures that const containers can be retrieved
  // for example, use
public:
  virtual double getValue() const override;
  virtual bool initializeSelf() override;

  void setCutValue(float val, float fwdval);
  void setEtaRange(float min, float max);

protected:
  void setJetType(const TString& jetTypeString);
  bool muonOverlapsWithJet(const xAOD::IParticle* jet, const xAOD::IParticle* muon) const;
  bool fDoTruth = false;
  TString fExpression = "";
  JetType m_jetType = JetType::nominal;
  TString m_candName = "";
  float m_cutVal = 25000.;
  float m_fwdcutVal = 30000.;
  float m_etamin = 0.;
  float m_etamax = 2.5;
private:
  /** reject overlaps with first other muon. intended to be used for NoMuOR sample for Z+jets muon FF computation **/
  const bool m_rejectJetsOverlappingOtherMuon = false;
  /** reject overlaps with first other muon. intended to be used for NoMuOR sample for W+jets CR sample **/
  const bool m_rejectJetsOverlappingSignalMuons = false;
  // LeptonIDHelper for anti-id and id selections
  // at the time of writing, intended to be used to
  // perform CAF-level mu-jet overlap removal between jets
  // and muons in the other lepton container.
  // (in principle this object can of course be used for other things)
  // const HWW::HWWLeptonIDHelper* m_leptonIDHelper = nullptr;


public:

  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expression) override;
  inline void setDoTruth(bool doTruth= false) { this->fDoTruth=doTruth; };
  inline bool getDoTruth() const { return this->fDoTruth; };
  HWWJetCounter();
  // jetTypeString is one of "nominal", "FJVT", or "FJVTTight"
  // removeOROtherMuon=true means don't count jets which overlap with
  // muon in otherPart container. Such muons exist in specially produced, mu-jet OR modified, samples
  HWWJetCounter(const TString& expression, const TString& jetTypeString, float cutVal = 25000., float fwdcutVal = 30000.
                ,const bool removeOROtherMuon=false, const bool removeORSignalMuons=false);
                // , const HWW::HWWLeptonIDHelper* leptonIDHelper = nullptr);
  virtual ~HWWJetCounter();

  ClassDefOverride(HWWJetCounter,2);
};

#endif
