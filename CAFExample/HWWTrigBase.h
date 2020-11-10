//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWTRIGBASE__
#define __HWWTRIGBASE__

#include "CAFExample/HWWTrigConfig.h"
#include "TString.h"
#include <vector>
#include <map>

#include "xAODEventInfo/EventInfo.h"

// #include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/ParticleContainer.h"

/*********************************************************
**********************************************************

      HWWTrigBase will serve as mother class to observables which
      want to calculate something trigger-related.

      author: Edvin Sidebo, edvin.sidebo@cern.ch

     **********/

class HWWTrigBase  {
protected:

  std::vector<HWWTrigConfig*> m_trigConfigs;
  TString m_trigpass_prefix;
  TString m_trigmatch_prefix;

  // get trig configs (useful for copy constructor)
  inline std::vector<HWWTrigConfig*> getTrigConfVector() const { return m_trigConfigs; }


public:
  HWWTrigBase();
  HWWTrigBase(const std::vector<HWWTrigConfig*>& trigConfig);

  void addTriggerConfig(HWWTrigConfig* trigConfig);

  virtual ~HWWTrigBase();

private:
  /***   hidden-away functions used privately to execute the below protected ones  ***/
  bool passSingleEl(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  bool passSingleMu(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  bool passDilep(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  bool passAnyTrigger(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;
  bool passAnySingleLeptonTrigger(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const;

  /*** hidden-away trigger-matching functions used privately to evaluate protected functions ***/
  // single lepton trigger matching (will loop over all single lepton triggers)
  bool isMatchedSingleMuParticle(const xAOD::IParticle* muon, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;
  bool isMatchedSingleElParticle(const xAOD::IParticle* electron, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;
  // is the particle matched to any of the triggers?
  bool isMatchedParticle(const xAOD::IParticle* part, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const;

  void DEBUGmsg(TString msg) const;


protected:
  /*** visible functions to be called by child class objects  ***/
   // get correct HWWTrigConfig from member vector, based on run number
  HWWTrigConfig* getTrigConfig(const xAOD::EventInfo* evtInfo) const;
  // does event pass single-el, single-mu or dilep triggers
  bool passSingleEl(const xAOD::EventInfo* evtInfo) const;
  bool passSingleMu(const xAOD::EventInfo* evtInfo) const;
  bool passDilep(const xAOD::EventInfo* evtInfo) const;
  bool passAnyTrigger(const xAOD::EventInfo* evtInfo) const;
  /** trigger matching for individual particles, to be called from child class objects ***/
  bool isMatchedSingleMuParticle(const xAOD::IParticle* muon, const xAOD::EventInfo* evtInfo) const;
  bool isMatchedSingleElParticle(const xAOD::IParticle* electron, const xAOD::EventInfo* evtInfo) const;
  // is the particle matched to any of the triggers?
  bool isMatchedParticle(const xAOD::IParticle* part, const xAOD::EventInfo* evtInfo) const;
  // dilep leg matching, trigger match expression has to be passed
  bool isMatchedDilepLegParticle(const xAOD::IParticle* part, const TString& trigmatch_expression) const;




};
#endif
