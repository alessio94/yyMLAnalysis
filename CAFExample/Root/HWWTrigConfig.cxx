#include "CAFExample/HWWTrigConfig.h"

// constructor
HWWTrigConfig::HWWTrigConfig(TString periodName) :
  period_name(periodName),
  // trig_prefix("pass_"),
  m_useDilepTriggers(false)
{

}

// destructor
HWWTrigConfig::~HWWTrigConfig() {

}


// set trigger lists
// data
void HWWTrigConfig::setTriggersSingleEl_Data(const std::vector<TString>& triggers, double ptcut) {
  for (auto& trigname : triggers) addTriggerSingleEl_Data(trigname);
  setTriggersSingleEl(ptcut);
}
void HWWTrigConfig::setTriggersSingleMu_Data(const std::vector<TString>& triggers, double ptcut) {
  for (auto& trigname : triggers) addTriggerSingleMu_Data(trigname);
  setTriggersSingleMu(ptcut);
} //trigSingleMu_Data = triggers; }
void HWWTrigConfig::setTriggersDilep_Data(const std::vector<TString>& triggers, double ptcut_ele, double ptcut_mu)    {
  for (auto& trigname : triggers) addTriggerDilep_Data(trigname);
  setTriggersDilep(ptcut_ele,ptcut_mu);
}

// mc
void HWWTrigConfig::setTriggersSingleEl_MC(const std::vector<TString>& triggers, double ptcut) {
  for (auto& trigname : triggers) addTriggerSingleEl_MC(trigname);
  setTriggersSingleEl(ptcut);
}
void HWWTrigConfig::setTriggersSingleMu_MC(const std::vector<TString>& triggers, double ptcut) {
  for (auto& trigname : triggers) addTriggerSingleMu_MC(trigname);
  setTriggersSingleMu(ptcut);
}
void HWWTrigConfig::setTriggersDilep_MC(const std::vector<TString>& triggers, double ptcut_ele, double ptcut_mu)    {
  for (auto& trigname : triggers) addTriggerDilep_MC(trigname);
  setTriggersDilep(ptcut_ele,ptcut_mu);
}

// set both MC and data (if MC and data triggers are the same)
void HWWTrigConfig::setTriggersSingleEl(const std::vector<TString>& triggers, double ptcut) {
  for (auto& trigname : triggers) { addTriggerSingleEl(trigname); }
  setTriggersSingleEl(ptcut);
}
void HWWTrigConfig::setTriggersSingleMu(const std::vector<TString>& triggers, double ptcut) {
  for (auto& trigname : triggers) { addTriggerSingleMu(trigname); }
  setTriggersSingleMu(ptcut);
}
void HWWTrigConfig::setTriggersDilep(const std::vector<TString>& triggers, double ptcut_ele, double ptcut_mu)    {
  for (auto& trigname : triggers) { addTriggerDilep(trigname); }
  setTriggersDilep(ptcut_ele,ptcut_mu);
}

// add trigger lists
// data
void HWWTrigConfig::addTriggerSingleEl_Data(const TString& trigName) {
  trigSingleEl_Data.push_back(trigName);
}
void HWWTrigConfig::addTriggerSingleMu_Data(const TString& trigName) {
  trigSingleMu_Data.push_back(trigName);
} //trigSingleMu_Data = triggers; }
void HWWTrigConfig::addTriggerDilep_Data(const TString& trigName)    {
  trigDilep_Data.push_back(trigName);
}

// mc
void HWWTrigConfig::addTriggerSingleEl_MC(const TString& trigName) {
  trigSingleEl_MC.push_back(trigName);
}
void HWWTrigConfig::addTriggerSingleMu_MC(const TString& trigName) {
  trigSingleMu_MC.push_back(trigName);
}
void HWWTrigConfig::addTriggerDilep_MC(const TString& trigName)    {
  trigDilep_MC.push_back(trigName);
}

// add both MC and data (if MC and data triggers are the same)
void HWWTrigConfig::addTriggerSingleEl(const TString& trigName) {
  trigSingleEl_Data.push_back(trigName);
  trigSingleEl_MC.push_back(trigName);
}
void HWWTrigConfig::addTriggerSingleMu(const TString& trigName) {
  trigSingleMu_Data.push_back(trigName);
  trigSingleMu_MC.push_back(trigName);
}
void HWWTrigConfig::addTriggerDilep(const TString& trigName)    {
  trigDilep_Data.push_back(trigName);
  trigDilep_MC.push_back(trigName);
}


// set both MC and data (if MC and data triggers are the same)
void HWWTrigConfig::setTriggersSingleEl(double ptcut) {
  ptcut_singleel = ptcut;
}
void HWWTrigConfig::setTriggersSingleMu(double ptcut) {
  ptcut_singlemu = ptcut;
}
void HWWTrigConfig::setTriggersDilep(double ptcut_ele, double ptcut_mu)    {
  m_useDilepTriggers = true;
  ptcut_dilep_elleg = ptcut_ele;
  ptcut_dilep_muleg = ptcut_mu;
}
