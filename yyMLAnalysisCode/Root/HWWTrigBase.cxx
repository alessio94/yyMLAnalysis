#include "yyMLAnalysisCode/HWWTrigBase.h"

// Uncomment the following line to enable debug printouts
// #define _DEBUG_
#include "QFramework/TQLibrary.h"

//______________________________________________________________________________________________

// constructor
HWWTrigBase::HWWTrigBase(const std::vector<HWWTrigConfig*>& trigConfigs):
  HWWTrigBase()
{
  for(const auto& t:trigConfigs){
    this->addTriggerConfig(t);
  }
}

//______________________________________________________________________________________________

// constructor
HWWTrigBase::HWWTrigBase():
  m_trigpass_prefix("pass_"),
  m_trigmatch_prefix("trigMatch_")
{
  DEBUGclass("constructor of HWWTrigBase");
}

//______________________________________________________________________________________________

void HWWTrigBase::addTriggerConfig(HWWTrigConfig* trigConfig){
  this->m_trigConfigs.push_back(trigConfig);
}

//______________________________________________________________________________________________

// destructor
HWWTrigBase::~HWWTrigBase() {

}

//______________________________________________________________________________________________

HWWTrigConfig* HWWTrigBase::getTrigConfig(const xAOD::EventInfo* evtInfo) const {
  unsigned int runNumber;
  if (evtInfo->isAvailable<unsigned int>("RandomRunNumber")){
    //if(evtinfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
    runNumber = evtInfo->auxdata<unsigned int> ("RandomRunNumber");
  } else {
    runNumber = evtInfo->runNumber();
  }
  // find out based on run number which is the correct HWWTrigConf
  DEBUGclass(TString::Format("runNumber = %d", runNumber).Data());
  for (const auto& trigconf : m_trigConfigs) {
    if (runNumber >= trigconf->runNumberLow && runNumber <= trigconf->runNumberUp) {
      return trigconf;
    }
  }
  // should never end up here
  return nullptr;
}

//______________________________________________________________________________________________

bool HWWTrigBase::isMatchedAnySingleElParticle(const xAOD::IParticle* electron, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // loop over triggers and return passed if offline electron matches any of them, and also passes pt cut
  const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  // loop over single electron triggers
  const std::vector<TString>& triggers = isMC ? trigConf->elecTrigsMC : trigConf->elecTrigsData;
  // check offline electron pT cut
  if (electron->pt() >= trigConf->elecPtCut) {
    for (unsigned int itrig(0); itrig < triggers.size(); ++itrig) {
      DEBUGclass(TString::Format("checking trigger %s", triggers[itrig].Data()).Data());
      if (isMatchedTriggerParticle(electron,triggers[itrig])) {
        DEBUGclass("matched to single electron trigger");
        return true;
      }
    }
  }
  return false;
}

//______________________________________________________________________________________________

// from-outside version of above function
bool HWWTrigBase::isMatchedAnySingleElParticle(const xAOD::IParticle* electron, const xAOD::EventInfo* evtInfo) const {
  // get trigConf...
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isMatchedAnySingleElParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ... call private fcn
  return isMatchedAnySingleElParticle(electron, trigConf, evtInfo);
}

//______________________________________________________________________________________________

bool HWWTrigBase::isMatchedAnySingleMuParticle(const xAOD::IParticle* muon, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // loop over triggers and return passed if offline muon matches any of them, and also passes pt cut
  const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  // loop over single muon triggers
  const std::vector<TString>& triggers = isMC ? trigConf->muonTrigsMC : trigConf->muonTrigsData;
  // check offline muon pT cut
  if (muon->pt() >= trigConf->muonPtCut) {
    for (unsigned int itrig(0); itrig < triggers.size(); itrig++) {
      DEBUGclass(TString::Format("checking trigger %s", triggers[itrig].Data()).Data());
      if (isMatchedTriggerParticle(muon,triggers[itrig])) {
        DEBUGclass("matched to single muon trigger");
        return true;
      }
    }
  }
  return false;
}

//______________________________________________________________________________________________

// from-outside version of above fcn.
bool HWWTrigBase::isMatchedAnySingleMuParticle(const xAOD::IParticle* muon, const xAOD::EventInfo* evtInfo) const {
  // get trigConf...
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isMatchedAnySingleMuParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ... call private function
  return isMatchedAnySingleMuParticle(muon, trigConf, evtInfo);
}

//______________________________________________________________________________________________

bool HWWTrigBase::isMatchedTriggerParticle(const xAOD::IParticle* part, const TString& trigger) const {
  // trigmatch expression passed, simply match electron
  if (static_cast<bool>(part->isAvailable<char>((m_trigmatch_prefix+trigger).Data()))) {
    if (static_cast<bool>(part->auxdata<char>((m_trigmatch_prefix+trigger).Data()))) {
      DEBUGclass("matched to dilepton trigger");
      return true;
    }
  }
  return false;
}

//______________________________________________________________________________________________

bool HWWTrigBase::isMatchedAnyDilepLegParticle(const xAOD::IParticle* part, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // get dilepton trigger list for this event
  const std::vector<TString>& dilepTriggers = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ? trigConf->dilepTrigsMC : trigConf->dilepTrigsData;
  unsigned int nDilepTriggers = dilepTriggers.size();
  // loop over triggers
  // TString trigger;
  for (unsigned int iDilep(0); iDilep < nDilepTriggers; iDilep++) {
    // Electron
    if (part->type() == xAOD::Type::Electron) {
      // the pt is event-level cut, check that first
      if (part->pt() > trigConf->dilepElLegPtCut) {
        if (isMatchedTriggerParticle(part, dilepTriggers[iDilep])) return true;
      }
    } else if (part->type() == xAOD::Type::Muon) {
      // the pt is event-level cut, check that first
      if (part->pt() > trigConf->dilepMuLegPtCut) {
        if (isMatchedTriggerParticle(part, dilepTriggers[iDilep])) return true;
      }
    }
  }
  return false;
}

//______________________________________________________________________________________________

// from-outside version of above function
bool HWWTrigBase::isMatchedAnyDilepLegParticle(const xAOD::IParticle* part, const xAOD::EventInfo* evtInfo) const {
  // get trigConf...
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isMatchedAnyDilepLegParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ... call private fcn
  return isMatchedAnyDilepLegParticle(part, trigConf, evtInfo);
}

//______________________________________________________________________________________________

bool HWWTrigBase::isMatchedAnyParticle(const xAOD::IParticle* part, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // is the particle matched to any of the triggers?
  // single muon
  if (part->type() == xAOD::Type::Muon && isMatchedAnySingleMuParticle(part, trigConf, evtInfo)) {
    return true;
  }
  // single electron
  if (part->type() == xAOD::Type::Electron && isMatchedAnySingleElParticle(part, trigConf, evtInfo)) {
    return true;
  }
  // dilepton triggers
  if (isMatchedAnyDilepLegParticle(part, trigConf, evtInfo)) {
    return true;
  }
  return false;
}

//______________________________________________________________________________________________

// called-from-outside version of above function
bool HWWTrigBase::isMatchedAnyParticle(const xAOD::IParticle* part, const xAOD::EventInfo* evtInfo) const {
  // get trigConf...
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isMatchedAnyParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ...  call private function
  return isMatchedAnyParticle(part, trigConf, evtInfo);
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedTrigger(const xAOD::EventInfo* evtInfo, const TString& trigger) const {
  return static_cast<bool>(evtInfo->auxdata<char>(std::string(m_trigpass_prefix+trigger.Data())));
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnySingleEl(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConf) const {
  const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  bool passed = false;
  DEBUGclass("Inside isPassedAnySingleElTrigger");
  for(const auto& trig: (isMC ? trigConf->elecTrigsMC : trigConf->elecTrigsData)){
    DEBUGclass(TString::Format("Inside isPassedAnySingleElTrigger :: checking trigger %s", trig.Data()).Data());
    passed |= isPassedTrigger(evtInfo,trig);
  }
  DEBUGclass(TString::Format("Inside isPassedAnySingleElTrigger :: returning %d", passed).Data());
  return passed;
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnySingleEl(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isPassedAnySingleEl :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  return isPassedAnySingleEl(evtInfo, trigConf);
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnySingleMu(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConf) const {
  const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  bool passed = false;
  DEBUGclass("Inside isPassedAnySingleMuTrigger");
  for(const auto& trig: (isMC ? trigConf->muonTrigsMC : trigConf->muonTrigsData)){
    DEBUGclass(TString::Format("Inside isPassedAnySingleMuTrigger :: checking trigger %s", trig.Data()).Data());
    passed |= isPassedTrigger(evtInfo,trig);
  }
  DEBUGclass(TString::Format("Inside isPassedAnySingleMuTrigger :: returning %d", passed).Data());
  return passed;
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnySingleMu(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isPassedAnySingleEl :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  return isPassedAnySingleMu(evtInfo, trigConf);
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnyDilep(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConf) const {
  const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  bool passed = false;
  for(const auto& trig: (isMC ? trigConf->dilepTrigsMC : trigConf->dilepTrigsData)){
    DEBUGclass(TString::Format("Inside isPassedAnyDilepTrigger :: checking trigger %s", trig.Data()).Data());
    passed |= isPassedTrigger(evtInfo,trig);
  }
  DEBUGclass(TString::Format("Inside isPassedAnyDilepTrigger :: returning %d", passed).Data());
  return passed;
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnyDilep(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        //std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isPassedAnyDilep :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  if (trigConf->useDilepTrigs) return isPassedAnyDilep(evtInfo, trigConf);
  DEBUGclass("You flagged not to use dilep triggers. Returning false.");
  return false;
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAny(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const {
  return (isPassedAnySingleEl(evtInfo, trigConfig) || isPassedAnySingleMu(evtInfo, trigConfig) || isPassedAnyDilep(evtInfo, trigConfig));
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAnySingleLep(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const {
  return (isPassedAnySingleEl(evtInfo, trigConfig) || isPassedAnySingleMu(evtInfo, trigConfig));
}

//______________________________________________________________________________________________

bool HWWTrigBase::isPassedAny(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::isPassedAnyDilep :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  if (trigConf->useDilepTrigs) {
    DEBUGclass("Inside HWWTrigBase :: you are using dilepton triggers");
    return isPassedAny(evtInfo, trigConf);
  }
  return isPassedAnySingleLep(evtInfo, trigConf);
}
