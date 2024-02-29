//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWTRIGCONFIG__
#define __HWWTRIGCONFIG__

#include "TString.h"
#include <vector>

/*** This class holds some variables which are settings
     for HWW triggers, like trigger names and corresponding data periods.
     An object of this class can then be bassed to constructors
     of trigger-related observables, such that these settings can
     easily be shared among observables. 
     ***/

class HWWTrigConfig {
public:
  unsigned int runNumberLow = 0;
  unsigned int runNumberUp = 0;

  std::vector<TString> elecTrigsData;
  std::vector<TString> muonTrigsData;
  std::vector<TString> dilepTrigsData;

  std::vector<TString> elecTrigsMC;
  std::vector<TString> muonTrigsMC;
  std::vector<TString> dilepTrigsMC;

  double elecPtCut;
  double muonPtCut;
  double dilepElLegPtCut;
  double dilepMuLegPtCut;

  TString periodName;
  bool useDilepTrigs;

public:
  HWWTrigConfig(const TString& periodName);
  HWWTrigConfig() {} // seems a default argument-less constructor is needed by ROOT?
  virtual ~HWWTrigConfig();

  void setRunNumbers(const unsigned int low, const unsigned int up) { runNumberLow = low; runNumberUp = up; }

  // set triggers
  // data
  void setElectronTriggersData(const std::vector<TString>& triggers);
  void setMuonTriggersData(const std::vector<TString>& triggers);
  void setDileptonTriggersData(const std::vector<TString>& triggers);
  // mc
  void setElectronTriggersMC(const std::vector<TString>& triggers);
  void setMuonTriggersMC(const std::vector<TString>& triggers);
  void setDileptonTriggersMC(const std::vector<TString>& triggers);
  // set both MC and data (if MC and data triggers are the same)
  void setElectronTriggers(const std::vector<TString>& triggers);
  void setMuonTriggers(const std::vector<TString>& triggers);
  void setDileptonTriggers(const std::vector<TString>& triggers);

  // set pT cuts
  void setElectronPtCut(double ptCut);
  void setMuonPtCut(double ptCut);
  void setDileptonPtCut(double elPtCut, double muPtCut);

  // add trigger lists
  // data
  void addElectronTriggerData(const TString& trigger);
  void addMuonTriggerData(const TString& trigger);
  void addDileptonTriggerData(const TString& trigger);
  
  // mc
  void addElectronTriggerMC(const TString& trigger);
  void addMuonTriggerMC(const TString& trigger);
  void addDileptonTriggerMC(const TString& trigger);

  // add both MC and data (if MC and data triggers are the same)
  void addElectronTrigger(const TString& trigger);
  void addMuonTrigger(const TString& trigger);
  void addDileptonTrigger(const TString& trigger);

};
#endif
