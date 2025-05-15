//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWTRIGGERWEIGHT__
#define __HWWTRIGGERWEIGHT__
#include "CAFxAODUtils/TQEventObservable.h"

#ifndef __CINT__
#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "PATCore/PATCoreEnums.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "AsgTools/AnaToolHandle.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "yyMLAnalysisCode/HWWSFBase.h"

#include <stdio.h>
using namespace xAOD;
#else
// put here forward declares for the classes you might need, e.g.
class xAOD::IParticle;
class xAOD::ParticleContainer;
class CompositeParticle;
class CompositeParticleContainer;
#endif


class HWWTriggerWeight : public HWWSFBase {

public:
  class TriggerSetup{ // nested
  public:
    TString m_triggerPeriod;
    TString m_campaign;
    std::map<TString, TString> m_electronToolSetup;
    std::vector<TString> m_triggers;
    TriggerSetup();
    TriggerSetup(TString triggerPeriod, TString campaign);
    ~TriggerSetup();
    void addTrigger(TString trigger);
    void addElectronToolSetup(TString triggerID, TString electronToolSetupString);
  };

  virtual double getValue() const override;
  // Observables that inherit from HWWSFBase use the methods
  // initializeSF() and finalizeSF() in place of
  // initializeSelf() and finalizeSelf()
  virtual bool finalizeSF() override;
  virtual bool initializeSF() override;
  mutable asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> m_triggerTool;//!
  static HWWTriggerWeight::TriggerSetup& createNewTriggerSetup(TString triggerPeriod, TString campaign);

protected:


  TString fExpression = "";
  TString mCandName = "";
  std::map<std::string, std::string> triggerKeys ;//!
  std::map<std::string, std::string> myThresholds;//!
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffToolsHandles ;//!
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronSFToolsHandles;//!
  std::vector<asg::AnaToolHandle<IAsgElectronEfficiencyCorrectionTool> > electronToolsFactory; //!
  ToolHandleArray<CP::IMuonTriggerScaleFactors> muonToolsHandles;//!
  std::vector<asg::AnaToolHandle<CP::IMuonTriggerScaleFactors> > muonToolsFactory;//! // for RAII
  std::map<std::string,std::string> legsPerTool ;//!

  // The number of leptons considered for the SF calculation.
  int m_nLeptonWeights = -1;

  // Number of errors and warnings in getValue. If they are too many, exit to
  // not cause huge outputs or logfiles.
  mutable int m_errorCount = 0;
  mutable int m_outOfValidityRangeCount = 0;

  TString m_variationTypeMatchElectrons = "";
  TString m_variationNameMatchElectrons = "";
  TString m_variationTypeMatchMuons = "";
  TString m_variationNameMatchMuons = "";

  // Can be set in initializeSF(...) if no trigger setup is available
  bool m_returnOneForThisSample = false;

  // This variable is static so that the instances of TriggerSetup don't need
  // to get copied all the time. But it can make sense to have different
  // trigger setups for different instances of HWWTriggerWeight. In that case,
  // make this non-static.
  static std::vector<TriggerSetup> triggerSetups;
  TString m_initializedForCampaign = "";

  template<class T> bool varyTools(ToolHandleArray<T>& tools);
  void varyElectronTools();
  void varyMuonTools();
  void registerVariations(int nVariations);

public:
  void defineVariationTriggerElectrons(const TString& variationTypeMatchElectrons, const TString& variationNameMatchElectrons);
  void defineVariationTriggerMuons(const TString& variationTypeMatchMuons, const TString& variationNameMatchMuons);

  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expr) override;

  HWWTriggerWeight();
  HWWTriggerWeight(int nLeptonWeights);
  HWWTriggerWeight(const TString& expression, int nLeptonWeights);
  virtual ~HWWTriggerWeight();

  ClassDefOverride(HWWTriggerWeight,1);
};
#endif
