//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWBTAGCOUNTEROBS__
#define __HWWBTAGCOUNTEROBS__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

class HWWBTagCounterObs : public TQEventObservable {

public:
  HWWBTagCounterObs();
  HWWBTagCounterObs(const TString& name,
                    std::string bTagName = "MV2c10",
                    std::string bTagWP = "FixedCutBEff_85",
                    std::string jetAuthor = "AntiKt4EMTopoJets",
                    bool useOtherJets = true,
                    bool useHardCuts = false,
                    float ptCut = 20000.0,
                    float bTagCut = 0.0);
  virtual ~HWWBTagCounterObs();
  ClassDefOverride(HWWBTagCounterObs, 1);
  virtual double getValue() const override;

  /* Getters */
  std::string getBTagName() const { return m_bTagVarName; }
  std::string getBTagWP() const { return m_operatingPoint; }
  float getBTagCut() const { return m_bTagCut; }
  float getPtCut() const { return m_ptCut; }
  std::string getJetAuthor() const { return m_jetAuthor; }
  bool getUseOtherJets() const { return m_useOtherJets; }
  bool getUseHardCuts() const { return m_useHardCuts; }

  /* Setters */
  void setPtCut(float ptCut) { m_ptCut = ptCut; }
  void setUseHardCuts(bool useHardCuts) { m_useHardCuts = useHardCuts; }
  void setJetAuthor(std::string jetAuthor) { m_jetAuthor = jetAuthor; }
  void setUseOtherJets(bool useOtherJets) { m_useOtherJets = useOtherJets; }

  /* Non trivial Setters */
  void setBTagName(std::string bTagName, std::string bTagWP);
  void setBTagCut(float bTagCut);

protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;

private:
 /* Functions */
 // Helper function to calcualte if a Jet is a bJet
 bool PassBTagCuts(const xAOD::Jet* jet) const;

 /* PxAOD Container Information */
 TString mContName = "";
 mutable const xAOD::CompositeParticleContainer* mCand = 0;

 /* BTagger informaiton */
 mutable ToolHandle<IBTaggingSelectionTool> m_btagtool;
 std::string m_bTagVarName = "";     // The name of the b-tag variable
 std::string m_operatingPoint = "";  // Working Point name (e.g. FixedCutBEff_77, FixedCutBEff_85, ...)
 std::string m_jetAuthor = "";       // Jet type (e.g. AntiKt4EMTopoJets, AntiKt4EMPFlowJets, ...)

 /* Flags */
 bool m_wasInitialized = false;  // Have we initialized before?
 bool m_useOtherJets = true;     // Do we look at other sub-threshold Jets?
 bool m_useHardCuts = false;     // Allow hard cuts to implement custom working points?

 /* Kinematics */
 float m_ptCut = 0;    // pT Kinematic cut
 float m_bTagCut = 0;  // If useHardCuts, this value is compared to the tagWeight
};
#endif
