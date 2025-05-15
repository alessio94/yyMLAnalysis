//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWTRIGBASE__
#define __HWWTRIGBASE__

#include "yyMLAnalysisCode/HWWTrigConfig.h"
#include "TString.h"
#include <vector>

#include "xAODEventInfo/EventInfo.h"
#include "xAODParticleEvent/ParticleContainer.h"

/*********************************************************
**********************************************************

      HWWTrigBase will serve as mother class to observables which
      want to calculate something trigger-related.

      author: Edvin Sidebo, edvin.sidebo@cern.ch

     **********/

class HWWTrigBase {
protected:

  std::vector<HWWTrigConfig*> m_trigConfigs;
  TString m_trigpass_prefix;
  TString m_trigmatch_prefix;

  // get trig configs (useful for copy constructor)
  inline std::vector<HWWTrigConfig*> getTrigConfVector() const { return m_trigConfigs; }

public:
  HWWTrigBase();
  HWWTrigBase(const std::vector<HWWTrigConfig*>& trigConfig);
  virtual ~HWWTrigBase();
  void addTriggerConfig(HWWTrigConfig* trigConfig);

private:
  /***  hidden-away functions used privately to execute the below protected ones  ***/

  // does the event pass any electron/muon trigger?
  bool isPassedAnySingleEl(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  bool isPassedAnySingleMu(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  // does the event pass any single-lepton trigger?
  bool isPassedAnySingleLep(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  // does the event pass any dilepton trigger?
  bool isPassedAnyDilep(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  // does the event pass any trigger?
  bool isPassedAny(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;

  // is the particle matched to any electron/muon trigger?
  bool isMatchedAnySingleElParticle(const xAOD::IParticle* electron, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;
  bool isMatchedAnySingleMuParticle(const xAOD::IParticle* muon, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;
  // is the particle matched to any dilepton trigger (one of its leg particles)?
  bool isMatchedAnyDilepLegParticle(const xAOD::IParticle* part, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;
  // is the particle matched to any trigger?
  bool isMatchedAnyParticle(const xAOD::IParticle* part, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;

protected:
  /*** visible functions to be called by child class objects  ***/

  // get correct HWWTrigConfig from member vector, based on run number
  HWWTrigConfig* getTrigConfig(const xAOD::EventInfo* evtInfo) const;

  // does the event pass a specific trigger?
  bool isPassedTrigger(const xAOD::EventInfo* evtInfo, const TString& trigger) const;
  // does the event pass any electron/muon trigger?
  bool isPassedAnySingleEl(const xAOD::EventInfo* evtInfo) const;
  bool isPassedAnySingleMu(const xAOD::EventInfo* evtInfo) const;
  // does the event pass any dilepton trigger?
  bool isPassedAnyDilep(const xAOD::EventInfo* evtInfo) const;
  // does the event pass any trigger?
  bool isPassedAny(const xAOD::EventInfo* evtInfo) const;

  // is the particle matched to a specific trigger?
  bool isMatchedTriggerParticle(const xAOD::IParticle* part, const TString& trigger) const;
  // is the particle matched to any electron/muon trigger?
  bool isMatchedAnySingleElParticle(const xAOD::IParticle* electron, const xAOD::EventInfo* evtInfo) const;
  bool isMatchedAnySingleMuParticle(const xAOD::IParticle* muon, const xAOD::EventInfo* evtInfo) const;
  // is the particle matched to any dilepton trigger (one of its leg particles)?
  bool isMatchedAnyDilepLegParticle(const xAOD::IParticle* part, const xAOD::EventInfo* evtInfo) const;
  // is the particle matched to any triggers?
  bool isMatchedAnyParticle(const xAOD::IParticle* part, const xAOD::EventInfo* evtInfo) const;

  ClassDef(HWWTrigBase,1);
};
#endif
