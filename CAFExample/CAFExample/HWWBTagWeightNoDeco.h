//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWBTAGWEIGHTNODECO__
#define __HWWBTAGWEIGHTNODECO__

// CAF includes:
#include "CAFExample/HWWSFBase.h"

#ifndef __CINT__
#define XAOD_STANDALONE 1

// EDM includes:
#include "xAODJet/Jet.h"

// Tool includes:
#include "AsgTools/AnaToolHandle.h"
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "FTagAnalysisInterfaces/IBTaggingEfficiencyTool.h"

#else
class HWWSFBase;
template<T>
class ToolHandle;
class IBTaggingEfficiencyTool;
class IBTaggingSelectionTool;
namespace xAOD {
  class Jet;
}
#endif

// ROOT includes:
#include "TString.h"

// STL includes:
#include <string>

class HWWBTagWeightNoDeco : public HWWSFBase {
protected:

  mutable ToolHandle<IBTaggingEfficiencyTool> m_btagtool;      //!
  mutable ToolHandle<IBTaggingSelectionTool> m_btagselecttool; //!

  float getBTagWeight(const xAOD::Jet* jet) const;
  std::string getTtbarDSID(const uint32_t mcChannelNumber) const;

  // These variables are set in defineVariationTrigger(...). In initializeSelf(...),
  // they are compared to sample folder tags to switch systematic variations on.
  // The result is saved in m_variationDirection and m_variationID for easier
  // access.
  TString m_variationTypeMatch = "";
  TString m_variationNameMatch = "";

public:

  void defineVariationTrigger(const TString& variationTypeMatch, const TString& variationNameMatch);

  virtual double getValue() const override;
  // Observables that inherit from HWWSFBase use the methods
  // initializeSF() and finalizeSF() in place of
  // initializeSelf() and finalizeSelf()
  virtual bool finalizeSF() override;
  virtual bool initializeSF() override;

  HWWBTagWeightNoDeco();
  HWWBTagWeightNoDeco(const TString& expression, const std::string& BTagName, const std::string& BTagWP, const float ptCut = 20000.0, const float etaCut = 2.5, const std::string& jetAuthor = "AntiKt4EMPFlowJets");
  virtual ~HWWBTagWeightNoDeco();

  ClassDefOverride(HWWBTagWeightNoDeco, 1);

private:
  std::string m_bTagVarName    = "";
  std::string m_OperatingPoint = "";
  float m_ptCut                = 20000.0;
  float m_etaCut               = 2.5;
  std::string m_jetAuthor      = "";

  // These are hardcoded and are not intended to be set in the python snippet
  std::string m_CDI            = "xAODBTaggingEfficiency/13p6TeV/2023-22-13p6TeV-MC21-CDI_Test_2023-08-1_v1.root";
  std::string m_systStrat      = "SFEigen";
  std::string m_systRed        = "Medium";
  int m_outputLvl              = MSG::WARNING;

};

#endif
