#include "CAFExample/HWWSFBase.h"

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

ClassImp(HWWSFBase)

HWWSFBase::DataPeriod HWWSFBase::getDataPeriod(unsigned int runNumber){
	// TODO: not ready for 2017 and 2018 data
	if ((276261 < runNumber) && (runNumber < 284485)){
		return DataPeriod::is2015;
	}
	else if ((297729 < runNumber) && (runNumber < 302919)){
		return DataPeriod::is2016;
	}
	else if ((302918 < runNumber)){
		return DataPeriod::is2016D;
	}
	else{
		return DataPeriod::isUndefined;
	}
}

std::map<TString, int> HWWSFBase::nVariations;

//______________________________________________________________________________________________

HWWSFBase::HWWSFBase(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWSFBase::HWWSFBase(const TString& expression):
  TQEventObservable(expression)
{
  // constructor with expression argument
  DEBUGclass("constructor called with '%s'",expression.Data());

  this->setExpression(expression);
}

//______________________________________________________________________________________________

HWWSFBase::~HWWSFBase(){
  // default destructor
  DEBUGclass("destructor called");
}

//______________________________________________________________________________________________

bool HWWSFBase::initializeSelf(){
  DEBUGclass("Initializing HWWSFBase for sample with path '%s'",fSample->getPath().Data());
  TString candName = "";
  if(!this->fSample->getTagString("~cand",candName)) return false;
  this->m_candName = "Event"+candName;
  // read variation type and name from sample folder
  m_variationType = this->fSample->getTagStringDefault("~variationType", "nominal");
  m_variationName = this->fSample->getTagStringDefault("~variationName", "nominal");
  m_variationAndCandName = m_variationName + "," + m_candName;

  if (m_variationType.EqualTo("xAODsfSystematics")){
    if (nVariations.find(m_variationAndCandName) == nVariations.end())
      nVariations[m_variationAndCandName] = 0;
  }

  this->fExpressionInternal = TString::Format("%s:%s",this->GetName(),this->m_candName.Data());
  DEBUGclass("expression is '%s'",this->fExpression.Data());
  this->fCachedEntry = -999;
  this->fCachedValue = 1.0;

  // Note that this condition is just saving resources. The
  // scale factors would correctly return 1 if this were not here.
  if (this->fSample->getTagBoolDefault("~isData", false) ||
      this->fSample->getTagBoolDefault("~isDataFakes", false)){
    DEBUGclass("This is data and we can skip the initialize method.");
    this->isData = true;
    return true;
  }

  return initializeSF();
}

//______________________________________________________________________________________________

bool HWWSFBase::deleteAccessorAutomatically(SG::AuxElement::Accessor<float> **accessor){
  if ((!accessor) || (!(*accessor))){
    WARNclass("You passed a %snull pointer. Won't delete accessor automatically.", (accessor) ? "pointer to ":"");
    return false;
  }
  accessors_float.push_back(accessor);
  return true;
}

//______________________________________________________________________________________________

void HWWSFBase::registerVariation(){
  DEBUGclass("registerVariation() called for %s", this->getExpression().Data());
  if (!m_variationType.EqualTo("xAODsfSystematics")){
    WARNclass("Cannot register systematic variation if variationType is not equal to xAODsfSystematics.");
    return;
  }
  if (nVariations.find(m_variationAndCandName) == nVariations.end()){
    WARNclass("Cannot find systematic variation %s and candidate name %s. Bookkeeping won't work properly.", m_variationName.Data(), m_candName.Data());
    return;
  }
  DEBUGclass("Incrementing the counter for %s to %i.", m_variationAndCandName.Data(), nVariations[m_variationAndCandName]+1);
  nVariations[m_variationAndCandName]++;
}

//______________________________________________________________________________________________

bool HWWSFBase::finalizeSelf(){
  // Automatically delete all registered accessors
  DEBUGclass("Finalizing HWWSFBase");

  this->simulationWarningPrintedForThisSample = false;

  // since we skipped initialize, we can also skip finalize
  if (this->isData){
    this->isData = false;
    return true;
  }

  bool returnValue = finalizeSF();

  // This is a loop over a static variable. It will only be executed once, even though finalizeSelf might be executed multiple times.
  for (auto variation:nVariations){
    if (variation.second == 1)
      continue;
    else if (variation.second == 0){
      WARNclass("You ran over systematic variation and candidate %s, but did you really vary anything?", variation.first.Data());
      WARNclass("This warning could also be produced because the method 'registerVariation();' was not called in initializeSelf of the corresponding scale factor. Please include this method call when a SF variation is used.");
    }
    else if (variation.second > 1)
      WARNclass("You ran over systematic variation and candidate %s and %i variables were varied. Is this understood?", variation.first.Data(), variation.second);
  }
  nVariations.clear();

  return returnValue;
}

//______________________________________________________________________________________________

bool HWWSFBase::getDefaultValue(double& value, const xAOD::CompositeParticleContainer* &cand, const xAOD::CompositeParticle* &evt, const xAOD::EventInfo* &evtInfo) const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");
  // the following protective if-block should NEVER evaluate true
  // for performance reasons, we disable it per preprocessor directive unless in debugging mode
  #ifdef _DEBUG_
  if(!this->fEvent){
    ERRORclass("failed to access xAOD::TEvent member!");
    value = 0;
    return true;
  }
  #endif

  if (this->isData){
    DEBUGclass("This is data. Returning unity.");
    return 1.0;
  }

  if (!this->getUseWeight()){
    DEBUGclass("Weight switched off. Returning unity.");
    value = 1.0;
    return true;
  }

  DEBUGclass("retrieving container");
  // Retrieve the event candidates
  if( !this->fEvent->retrieve(cand, this->m_candName.Data()).isSuccess() ){
    ERRORclass("failed to retrieve candidates!");
    value = 0;
    return true;
  }

  DEBUGclass("retrieving event candidate from container");
  if(cand->size() == 0){
    ERRORclass("in %s: event '%d' in '%s' has no candidate of type '%s'!",this->GetName(),(int)(this->getCurrentEntry()),this->fSample->getPath().Data(),this->m_candName.Data());
    value = 0.;
    return true;
  }
  evt = cand->at(0);

  // Retrieve the EventInfo container
  if( !this->fEvent->retrieve(evtInfo, "EventInfo").isSuccess() ){
    ERRORclass("failed to retrieve event info!");
    value = 0;
    return true;
  }

  // Don't do anything for data
  if( !evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ){
    DEBUGclass("returning unity weight for data");
    // we check above if isData and return 1 in this case.
    if (!this->simulationWarningPrintedForThisSample){
      WARNclass("This xAOD type is not labeled simulation, but the sample folder tags don't indicate that this is data. This might point to an inconsistency somewhere. Setting the scale factor to 1.");
      this->simulationWarningPrintedForThisSample = true;
    }
    value = 1.;
    return true;
  }

  if(this->getCurrentEntry() == this->fCachedEntry) {
    DEBUGclass("skipping reevalution for event %d, returning %f", this->getCurrentEntry(), this->fCachedValue);
    value = this->fCachedValue;
    return true;
  }
  DEBUGclass("No default value found. Returning 1.");
  value = 1.0;
  return false;
}

//______________________________________________________________________________________________

const TString& HWWSFBase::getExpression() const {
  // retrieve the expression associated with this observable
  return this->fExpression;
}

//______________________________________________________________________________________________

void HWWSFBase::setExpression(const TString& expression) {
  this->fExpression = expression;
}

//______________________________________________________________________________________________

bool HWWSFBase::hasExpression() const {
  // check if this observable type knows expressions
  return true;
}

//______________________________________________________________________________________________

void HWWSFBase::setUseWeight(bool useWeight){
  this->m_useWeight = useWeight;
}

//______________________________________________________________________________________________

bool HWWSFBase::getUseWeight() const {
  return this->m_useWeight;
}
