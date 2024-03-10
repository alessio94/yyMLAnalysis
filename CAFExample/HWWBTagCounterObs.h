//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWBTAGCOUNTEROBS__
#define __HWWBTAGCOUNTEROBS__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"

class HWWBTagCounterObs : public TQEventObservable {
protected:
  // Put here data members you wish to use.
  // For example you might want to add a variable
  // for the name of the xAOD container to be retrieved that
  // will be initialized in the initialize function.
  TString mContName = "";

  mutable xAOD::CompositeParticleContainer const * mCand = 0;

public:
  virtual double getValue() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
  ToolHandle<IBTaggingSelectionTool>  m_btagtool; //!
  bool m_wasInitialized = false; //!

public:
  HWWBTagCounterObs();
  HWWBTagCounterObs(const TString& name, std::string BTagName="MV2c10",
		    std::string BTagWP = "FixedCutBEff_85",
		    float PtCut=20000.0, /*bool UseOtherJets=false,*/
		    bool UseHardCuts=false, float BTagCut=0.0,
		    std::string jetAuthor = "AntiKt4EMTopoJets");
  virtual ~HWWBTagCounterObs();
  ClassDefOverride(HWWBTagCounterObs, 1);

  std::string getBTagName();
  std::string getBTagWP();
  float getBTagCut();
  float getPtCut();
  std::string getJetAuthor();
  bool getUseOtherJets();
  bool getUseHardCuts();
  void setBTagName(std::string BTagName, std::string BTagWP);
  void setBTagCut(float BTagCut);
  void setPtCut(float PtCut);
  //void setUseOtherJets(bool UseOtherJets);
  void setUseHardCuts(bool UseHardCuts);
  void setJetAuthor(std::string JetAuthor = "AntiKt4EMTopoJets");

private:

  /// The name of the b-tag variable
  std::string m_bTagVarName;
  // Working Point name (e.g. FixedCutBEff_77, FixedCutBEff_85, ...)
  std::string m_OperatingPoint;
  /// The cut value
  float m_ptCut;
    /// Flag to say that we should also look at the other sub-threshold jets
  bool m_useOtherJets;
  /// Flag to do hard cuts to allow for custom working points.  (Only supports MV2 currently)
  bool m_useHardCuts;
  float m_bTagCut;
  // jet type (e.g. AntiKt4EMTopoJets, AntiKt4EMPFlowJets, ...)
  std::string m_jetAuthor;

};
#endif
