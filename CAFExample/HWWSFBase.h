//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWSFBASE__
#define __HWWSFBASE__
#include "CAFxAODUtils/TQEventObservable.h"

#ifndef __CINT__
#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
#include "PileupReweighting/PileupReweightingTool.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "AsgTools/ToolHandle.h"
#include "AthContainers/AuxElement.h"
//#include "NNLOReweighter/NNLOReweighter.h"
using namespace xAOD;
#else
// put here forward declares for the classes you might need, e.g.
// class CompositeParticle;
// class CompositeParticleContainer;
#endif

class HWWSFBase : public TQEventObservable {
protected:
  TString fExpression = "";
  TString fExpressionInternal = "";
  mutable Long64_t fCachedEntry; //!
  mutable double fCachedValue; //!
  TString m_candName = "";
  bool m_useWeight = true;

  bool isData = false;

  TString sfName = "";

  TString m_variationType = "";
  TString m_variationName = "";
  TString m_variationAndCandName = "";

  // Two variables for bookkeeping in HWWSFBase. The accessor vectors keep track
  // of all created accessors and deletes them automatically in finalizeSelf().
  // To use this, call deleteAccessorAutomatically(...) in initializeSelf() of
  // the sf variation implementation (not in this file!).
  // The nVariations map records in which systematic variations something was
  // varied. This is an important check to make sure that all systematic
  // variations are really executed. To use it, call registerVariation() from
  // initializeSelf() whenever you vary a scale factor.
  std::vector<SG::AuxElement::Accessor<float>**> accessors_float;//!
  static std::map<TString, int> nVariations;

  mutable bool simulationWarningPrintedForThisSample = false;

public:
  enum DataPeriod{
    isUndefined,
    is2015,
    is2016,
    is2016D
  };

  static DataPeriod getDataPeriod(unsigned int runNumber);

  bool deleteAccessorAutomatically(SG::AuxElement::Accessor<float> **accessor);
  void registerVariation();

  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expression) override;
  void setUseWeight(bool useWeight);
  bool getUseWeight() const;

  virtual bool initializeSelf() override final;
  virtual bool finalizeSelf() override final;

  virtual bool initializeSF() = 0;
  virtual bool finalizeSF() = 0;

  bool getDefaultValue(double& value, const xAOD::CompositeParticleContainer* &cand, const xAOD::CompositeParticle* &evt, const xAOD::EventInfo* &evtInfo) const;

  HWWSFBase();
  HWWSFBase(const TString& expression);
  virtual ~HWWSFBase();

  ClassDefOverride(HWWSFBase,1);

};

#endif
