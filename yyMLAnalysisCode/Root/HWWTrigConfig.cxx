#include "yyMLAnalysisCode/HWWTrigConfig.h"

// constructor
HWWTrigConfig::HWWTrigConfig(const TString& periodName) :
  periodName(periodName),
  useDilepTrigs(false)
{
}

// destructor 
HWWTrigConfig::~HWWTrigConfig() {
}

// set trigger lists
// data
void HWWTrigConfig::setElectronTriggersData(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addElectronTriggerData(trigname); }
}
void HWWTrigConfig::setMuonTriggersData(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addMuonTriggerData(trigname); }
}
void HWWTrigConfig::setDileptonTriggersData(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addDileptonTriggerData(trigname); }
}

// mc
void HWWTrigConfig::setElectronTriggersMC(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addElectronTriggerMC(trigname); }
}
void HWWTrigConfig::setMuonTriggersMC(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addMuonTriggerMC(trigname); }
}
void HWWTrigConfig::setDileptonTriggersMC(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addDileptonTriggerMC(trigname); }
}

// set both MC and data (if MC and data triggers are the same)
void HWWTrigConfig::setElectronTriggers(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addElectronTrigger(trigname); }
}
void HWWTrigConfig::setMuonTriggers(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addMuonTrigger(trigname); }
}
void HWWTrigConfig::setDileptonTriggers(const std::vector<TString>& triggers) {
  for (const auto& trigname : triggers) { addDileptonTrigger(trigname); }
}

// add trigger lists
// data
void HWWTrigConfig::addElectronTriggerData(const TString& trigName) {
  elecTrigsData.push_back(trigName);
}
void HWWTrigConfig::addMuonTriggerData(const TString& trigName) {
  muonTrigsData.push_back(trigName);
}
void HWWTrigConfig::addDileptonTriggerData(const TString& trigName) {
  dilepTrigsData.push_back(trigName);
}

// mc
void HWWTrigConfig::addElectronTriggerMC(const TString& trigName) {
  elecTrigsMC.push_back(trigName);
}
void HWWTrigConfig::addMuonTriggerMC(const TString& trigName) {
  muonTrigsMC.push_back(trigName);
}
void HWWTrigConfig::addDileptonTriggerMC(const TString& trigName) {
  dilepTrigsMC.push_back(trigName);
}

// add both MC and data (if MC and data triggers are the same)
void HWWTrigConfig::addElectronTrigger(const TString& trigName) {
  elecTrigsData.push_back(trigName);
  elecTrigsMC.push_back(trigName);
}
void HWWTrigConfig::addMuonTrigger(const TString& trigName) {
  muonTrigsData.push_back(trigName);
  muonTrigsMC.push_back(trigName);
}
void HWWTrigConfig::addDileptonTrigger(const TString& trigName) {
  dilepTrigsData.push_back(trigName);
  dilepTrigsMC.push_back(trigName);
}

// set both MC and data (if MC and data triggers are the same)
void HWWTrigConfig::setElectronPtCut(double ptCut) {
  elecPtCut = ptCut;
}
void HWWTrigConfig::setMuonPtCut(double ptCut) {
  muonPtCut = ptCut;
}
void HWWTrigConfig::setDileptonPtCut(double elPtCut, double muPtCut) {
  useDilepTrigs = true;
  dilepElLegPtCut = elPtCut;
  dilepMuLegPtCut = muPtCut;
}
