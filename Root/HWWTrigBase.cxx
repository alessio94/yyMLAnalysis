#include "CAFExample/HWWTrigBase.h"

#include "QFramework/TQLibrary.h"
// uncomment the following line to enable debug printouts
 // #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);


// can't get DEBUG messages to work for this class -- maybe needs to inherit from TQobservable? Also check with what run numbers they are configured.
// use my own instead
void HWWTrigBase::DEBUGmsg(TString msg) const {
  std::cout << "DEBUG HWWTrigBase *** " << msg << std::endl;
}

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
	if(evtInfo->isAvailable<unsigned int>("RandomRunNumber")){
    //if(evtinfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
    runNumber = evtInfo->auxdata<unsigned int> ("RandomRunNumber");
  } else {
    runNumber = evtInfo->runNumber();
  }
  // find out based on run number which is the correct HWWTrigConf
  // DEBUGclass("runNumber = %d", runNumber); // can't get debug messages to work for this class
	for (const auto& trigconf : m_trigConfigs) {
		//std::cout << runNumber << "is the RunNumber" << std::endl;
		if (runNumber >= trigconf->runNumberLow && runNumber <= trigconf->runNumberUp) {

			return trigconf;
		}
	}
	// should never end up here
	return nullptr;
}

//______________________________________________________________________________________________
bool HWWTrigBase::isMatchedSingleElParticle(const xAOD::IParticle* electron, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // loop over triggers and return passed if electron matches any of them, and also passes pt cut
  if (electron->pt() > trigConf->ptcut_singleel) {

    const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    // loop over single electron triggers
    for(const auto& trigger: (isMC ? trigConf->trigSingleEl_MC: trigConf->trigSingleEl_Data)) {
      if (static_cast<bool>(electron->auxdata<char>(std::string(m_trigmatch_prefix + trigger)))) {
        // std::cout << "matched to single electron trigger " << std::endl;
        return true;
      }
    }
  }
  return false;
}

//______________________________________________________________________________________________
// from-outside version of above function
bool HWWTrigBase::isMatchedSingleElParticle(const xAOD::IParticle* electron, const xAOD::EventInfo* evtInfo) const {
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
    throw std::runtime_error("In HWWTrigBase::isMatchedSingleElParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ... call private fcn
  return isMatchedSingleElParticle(electron, trigConf, evtInfo);
}

//______________________________________________________________________________________________
bool HWWTrigBase::isMatchedSingleMuParticle(const xAOD::IParticle* muon, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // loop over triggers and return passed if muon matches any of them, and also passes pt cut
  if (muon->pt() > trigConf->ptcut_singlemu) {

    const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

    // loop over single muon triggers
    for(const auto& trigger: (isMC ? trigConf->trigSingleMu_MC : trigConf->trigSingleMu_Data)) {
      if (static_cast<bool>(muon->auxdata<char>(std::string(m_trigmatch_prefix + trigger)))) {
        // std::cout << "matched to single muon trigger " << std::endl;
        return true;
      }
    }
  }
  return false;
}

//______________________________________________________________________________________________
// from-outside version of above fcn.
bool HWWTrigBase::isMatchedSingleMuParticle(const xAOD::IParticle* muon, const xAOD::EventInfo* evtInfo) const {
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
    throw std::runtime_error("In HWWTrigBase::isMatchedSingleMuParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ... call private function
  return isMatchedSingleMuParticle(muon, trigConf, evtInfo);
}

//______________________________________________________________________________________________
bool HWWTrigBase::isMatchedDilepLegParticle(const xAOD::IParticle* part, const TString& trigmatch_expression) const {
  // trigmatch expression passed, simply match electron
  if (static_cast<bool>(part->auxdata<char>(trigmatch_expression.Data()))) return true;
  return false;
}

//______________________________________________________________________________________________
bool HWWTrigBase::isMatchedParticle(const xAOD::IParticle* part, const HWWTrigConfig* trigConf, const xAOD::EventInfo* evtInfo) const {
  // is the particle matched to any of the triggers?
  // single muon
  if (part->type() == xAOD::Type::Muon && isMatchedSingleMuParticle(part, trigConf, evtInfo)) {
    // std::cout << "Matched to single muon trigger" << std::endl;
    return true;
  }
  // single electron
  if (part->type() == xAOD::Type::Electron && isMatchedSingleElParticle(part, trigConf, evtInfo)) {
    // std::cout << "Matched to single electron trigger" << std::endl;
    return true;
  }
  // dilepton triggers
  // get dilepton trigger list for this event
  const std::vector<TString>& triggers = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ? trigConf->trigDilep_MC : trigConf->trigDilep_Data;
  unsigned int nDilepTriggers = triggers.size();
  // loop over triggers
  // TString trigger;
  for (unsigned int iDilep(0); iDilep < nDilepTriggers; iDilep++) {
    // Electron
    if (part->type() == xAOD::Type::Electron) {
      // the pt is event-level cut, check that first
      if (part->pt() > trigConf->ptcut_dilep_elleg) {
        if (isMatchedDilepLegParticle(part, this->m_trigmatch_prefix + triggers[iDilep])) return true;
      }
    } else if (part->type() == xAOD::Type::Muon) {
      // the pt is event-level cut, check that first
      if (part->pt() > trigConf->ptcut_dilep_muleg) {
        if (isMatchedDilepLegParticle(part, this->m_trigmatch_prefix + triggers[iDilep])) return true;
      }
    }
  }
  return false;
}

//______________________________________________________________________________________________
// called-from-outside version of above function
bool HWWTrigBase::isMatchedParticle(const xAOD::IParticle* part, const xAOD::EventInfo* evtInfo) const {
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
    throw std::runtime_error("In HWWTrigBase::isMatchedParticle :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  // ...  call private function
  return isMatchedParticle(part, trigConf, evtInfo);
}


bool HWWTrigBase::passSingleEl(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConf) const {

  const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  bool passed = false;
  // DEBUGclass("Inside passSingleElTrigger");
  for(const auto& trig: (isMC ? trigConf->trigSingleEl_MC : trigConf->trigSingleEl_Data)){
    passed |= static_cast<bool>(evtInfo->auxdata<char>(std::string(m_trigpass_prefix + trig)));
    // DEBUGclass(TString::Format("Inside passSingleElTrigger :: checking trigger %s", trig.Data()).Data());
  }
  // DEBUGclass(std::string("passSingleElTrigger :: returning " + passed));
  return passed;
}

bool HWWTrigBase::passSingleEl(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::passSingleEl :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  return passSingleEl(evtInfo, trigConf);
}

bool HWWTrigBase::passSingleMu(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConf) const {

	const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
	bool passed = false;

  for(const auto& trig: (isMC ? trigConf->trigSingleMu_MC : trigConf->trigSingleMu_Data)){
    passed |= static_cast<bool>(evtInfo->auxdata<char>(std::string(m_trigpass_prefix + trig)));
  }

  return passed;
}

bool HWWTrigBase::passSingleMu(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        // std::cout << "Event weight " << evtInfo->mcEventWeight() << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::passSingleEl :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  return passSingleMu(evtInfo, trigConf);
}


bool HWWTrigBase::passDilep(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConf) const {

	const bool isMC = evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
	bool passed = false;

  for(const auto& trig: (isMC ? trigConf->trigDilep_MC : trigConf->trigDilep_Data)){
    passed |= static_cast<bool>(evtInfo->auxdata<char>(std::string(m_trigpass_prefix + trig)));
  }
  return passed;
}

bool HWWTrigBase::passDilep(const xAOD::EventInfo* evtInfo) const {

  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {
    // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        //std::cout << "WARNING in HWWTrigBase :: RandomRunNumber == 0, interpreting as trigger miss." << std::endl;
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::passDilep :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }
  if (trigConf->m_useDilepTriggers) return passDilep(evtInfo, trigConf);
  // DEBUGmsg("You flagged not to use dilep triggers. Returning false.");
  return false;
}


bool HWWTrigBase::passAnyTrigger(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const {
  return (passSingleEl(evtInfo, trigConfig) || passSingleMu(evtInfo, trigConfig) || passDilep(evtInfo, trigConfig));
}

bool HWWTrigBase::passAnySingleLeptonTrigger(const xAOD::EventInfo* evtInfo, const HWWTrigConfig* trigConfig) const {
  return (passSingleEl(evtInfo, trigConfig) || passSingleMu(evtInfo, trigConfig));
}

bool HWWTrigBase::passAnyTrigger(const xAOD::EventInfo* evtInfo) const {
  const HWWTrigConfig* trigConf = getTrigConfig(evtInfo);
  if (!trigConf) {

    if (evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      if (evtInfo->auxdata<unsigned int>("RandomRunNumber") == 0) {
        // there seem to be events in MC with RandomRunNr = 0. Coming from mu not represented and thus having weight=0? Previous implementation must then have said false.
        return false;
      }
    }
    throw std::runtime_error("In HWWTrigBase::passDilep :: invalid HWWTrigConfig pointer! Are you sure you passed the TrigConfig objects to your observable? Also check with what run numbers they are configured.");
  }

  if (trigConf->m_useDilepTriggers) {
    // DEBUGmsg("Inside HWWTrigBase :: you are using dilepton triggers");
    return passAnyTrigger(evtInfo, trigConf);
  }
  return passAnySingleLeptonTrigger(evtInfo, trigConf);
}
