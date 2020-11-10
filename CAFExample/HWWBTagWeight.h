//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWBTAGWEIGHT__
#define __HWWBTAGWEIGHT__
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

class HWWBTagWeight : public HWWSFBase {
protected:
  // name of the decoration in PxAODs
  std::string m_bTagSFName = "";
  std::unique_ptr<SG::AuxElement::Accessor<float>> m_bTagSFDecor=0;//!
  bool m_useOtherJets = true;

  double getBTagWeight(const xAOD::IParticle* p) const ;

  // These variables are set in defineVariationTrigger(...). In initializeSelf(...),
  // they are compared to sample folder tags to switch systematic variations on.
  // The result is saved in m_variationDirection and m_variationID for easier
  // access.
  TString m_variationTypeMatch = "";
  TString m_variationNameMatch = "";
public:

  void defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch);
  void setElectronScaleFactors(TH1* hist, TH1* syshist);

  virtual double getValue() const override;
  // Observables that inherit from HWWSFBase use the methods
  // initializeSF() and finalizeSF() in place of
  // initializeSelf() and finalizeSelf()
  virtual bool finalizeSF() override;
  virtual bool initializeSF() override;

  HWWBTagWeight();
  HWWBTagWeight(const TString& expression, const std::string& bTagSFName, bool useOtherJets);
  virtual ~HWWBTagWeight();

  ClassDefOverride(HWWBTagWeight,1);
};

#endif
