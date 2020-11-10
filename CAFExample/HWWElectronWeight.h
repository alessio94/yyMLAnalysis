//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWELECTRONWEIGHT__
#define __HWWELECTRONWEIGHT__
#include "CAFExample/HWWSFBase.h"

#ifndef __CINT__
#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "AsgTools/ToolHandle.h"
#include "AthContainers/AuxElement.h"

using namespace xAOD;
#else
// put here forward declares for the classes you might need, e.g.
// class CompositeParticle;
// class CompositeParticleContainer;
#endif

// This class is intended to be general so that it can be used for
// different electron weights. If you need a dedicated implementation
// for a certain scale factor, please write a new class.

class HWWElectronWeight : public HWWSFBase {
protected:
  bool m_lowPtElectronsSeparately = false;
  double m_ptCut = 0;

  // name of the decoration in PxAODs
  std::string m_electronSFName = "";
  std::string m_electronSFNameLowPt = "";
  std::unique_ptr<SG::AuxElement::Accessor<float>> m_electronSFDecor=0;//!
  std::unique_ptr<SG::AuxElement::Accessor<float>> m_electronSFDecorLowPt=0;//!

  double getElectronWeight(const xAOD::IParticle* p) const ;

  // These variables are set in defineVariationTrigger(...). In initializeSelf(...),
  // they are compared to sample folder tags to switch systematic variations on.
  // The result is saved in m_variationDirection and m_variationID for easier
  // access.
  TString m_variationTypeMatch = "";
  TString m_variationNameMatch = "";
  TString m_variationTypeMatchLowPt = "";
  TString m_variationNameMatchLowPt = "";

public:

  void defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch);
  void defineVariationTriggerLowPt(const TString& variationTypeMatchLowPt, const TString& variationNameMatchLowPt);

  virtual double getValue() const override;
  // Observables that inherit from HWWSFBase use the methods
  // initializeSF() and finalizeSF() in place of
  // initializeSelf() and finalizeSelf()
  virtual bool finalizeSF() override;
  virtual bool initializeSF() override;

  HWWElectronWeight();
  HWWElectronWeight(const TString& expression, const std::string& electronSFName);
  HWWElectronWeight(const TString& expression, const std::string& electronSFName, const std::string& electronSFNameLowPt, const double ptCut);
  virtual ~HWWElectronWeight();

  ClassDefOverride(HWWElectronWeight,1);
};

#endif
