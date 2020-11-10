#include "CAFExample/HWWLeptonIDObservable.h"
#include <limits>
// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);
// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"
ClassImp(HWWLeptonIDObservable)

//______________________________________________________________________________________________
HWWLeptonIDObservable::HWWLeptonIDObservable() :
fIndexLeadLep(0),
fIndexSubleadLep(1),
fIndexThirdLep(2),
fIndexFourthLep(3),
fIndexOtherLep(0)
{
  // default constructor
  DEBUGclass("default constructor called");
}
//______________________________________________________________________________________________
// copy constructor
HWWLeptonIDObservable::HWWLeptonIDObservable(const HWWLeptonIDObservable& obs) :
// LeptonIDHelper for anti-id and id selections
  TQEventObservable(obs),
  fLeptonIDHelper(new HWW::HWWLeptonIDHelper(*obs.fLeptonIDHelper)),
  fHWWLepIDModeOfRunning(obs.fHWWLepIDModeOfRunning),
  fHWWAnalysisType(obs.fHWWAnalysisType),
  fIndexLeadLep(obs.fIndexLeadLep),
  fIndexSubleadLep(obs.fIndexSubleadLep),
  fIndexThirdLep(obs.fIndexThirdLep),
  fIndexFourthLep(obs.fIndexFourthLep),
  fIndexOtherLep(obs.fIndexOtherLep),
  mCand(obs.mCand),
  mCandName(obs.mCandName) {

  DEBUGclass("Inside copy constructor");
  // check that a valid value was given
  switch (fHWWLepIDModeOfRunning) {
    case HWWLepIDModeOfRunning::LeadLepID:
      break;
    case HWWLepIDModeOfRunning::SubleadLepID:
      break;
    case HWWLepIDModeOfRunning::ThirdLepID:
      break;
    case HWWLepIDModeOfRunning::FourthLepID:
      break;
    case HWWLepIDModeOfRunning::OtherLepID:
      break;
    case HWWLepIDModeOfRunning::LeadLepAntiID:
      break;
    case HWWLepIDModeOfRunning::SubleadLepAntiID:
      break;
    case HWWLepIDModeOfRunning::ThirdLepAntiID:
      break;
    case HWWLepIDModeOfRunning::FourthLepAntiID:
      break;
    case HWWLepIDModeOfRunning::OtherLepAntiID:
      break;
    default:
      throw std::runtime_error("[HWWLeptonIDObservable] Invalid mode-of-running value given!");
  }
  switch (fHWWAnalysisType) {
    case HWWAnalysisType::TwoLep:
      break;
    case HWWAnalysisType::WH:
      break;
    case HWWAnalysisType::ZH:
      break;
    default:
      throw std::runtime_error("ERROR in HWWLeptonIDObservable :: Unknown analysis type given!");
  }
  if (!fLeptonIDHelper) { throw std::runtime_error("ERROR in HWWLeptonIDObservable :: you must pass a valid pointer to your HWWLeptonIDHelper object!"); }

  DEBUGclass("Running with mode of running: %d", fHWWLepIDModeOfRunning);
  DEBUGclass("Running with anlysis type: %d:", fHWWAnalysisType);
}

//______________________________________________________________________________________________
HWWLeptonIDObservable::~HWWLeptonIDObservable(){
  // default destructor
  DEBUGclass("destructor called");
}
#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
//#include "xAODParticleEvent/CompositeParticleContainer.h"
//______________________________________________________________________________________________
double HWWLeptonIDObservable::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering function");
  // the TQEventObservable only works in an ASG RELEASE, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add an ASG Analysis Release in order to use this feature!"
  return std::numeric_limits<double>::quiet_NaN();
  #else
  // in the rest of this function, you should retrieve the data and calculate your return value
  // here is the place where most of your custom code should go
  // a couple of comments should guide you through the process
  // when writing your code, please keep in mind that this code can be executed several times on every event
  // make your code efficient. catch all possible problems. when in doubt, contact experts!
  // first, you can retrieve the data members you needwith the 'retrieve' method
  // as arguments to this function, pass the member pointer to the container as well as the container name
  /* example block:
  xAOD::CompositeParticleContainer* cont = NULL;
  this->retrieve(cont, "EECands");
  */
  // after you have retrieved your data members, you can proceed to calculate the return value
  // probably, you first have to retrieve an element from the container
  /* example block:
  const CompositeParticle* p = cont->at(0);
  */
  // in the end, you should calculate your return value
  // of course, you can use other data members of your observable at any time
  /* exmple block:
  const double retval = p->electron(0)->passSelection(this->fExpression.Data());
  */
  if(!this->fEvent->retrieve(this->mCand, this->mCandName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return 0;
  }
  // DEBUGclass("retrieve candidates! %s",this->mCandName.Data());
  const xAOD::CompositeParticle *Evt = this->mCand->at(0);
  // DEBUGclass(" this->mCand->size() %i",this->mCand->size());

  if(this->mCand->size()==0) return 0.;
  // lepton of interest
  const xAOD::IParticle* lepOfInterest = nullptr;
  // get it from event candidate depending on mode of running

  DEBUGclass("Retrieving candidate for %d", fHWWLepIDModeOfRunning);
  switch (fHWWLepIDModeOfRunning) {
    // leading lepton
    case HWWLepIDModeOfRunning::LeadLepID: case HWWLepIDModeOfRunning::LeadLepAntiID:
        lepOfInterest = Evt->part(fIndexLeadLep);
        break;
    // subleading lepton
    case HWWLepIDModeOfRunning::SubleadLepID: case HWWLepIDModeOfRunning::SubleadLepAntiID:
        lepOfInterest = Evt->part(fIndexSubleadLep);
        break;
    // third lepton (for VH)
    case HWWLepIDModeOfRunning::ThirdLepID: case HWWLepIDModeOfRunning::ThirdLepAntiID:
        switch (fHWWAnalysisType) {
            case HWWAnalysisType::WH: case HWWAnalysisType::ZH:
                lepOfInterest = Evt->part(fIndexThirdLep);
                break;
            default:
                throw std::runtime_error("ERROR in HWWLeptonIDObservable :: HWWAnalysisType::TwoLepton should not be trying to access third lepton mode of running.");
        }
        // BREAK
        break;
    // fourth lepton (for ZH)
    case HWWLepIDModeOfRunning::FourthLepID: case HWWLepIDModeOfRunning::FourthLepAntiID:
        switch (fHWWAnalysisType) {
            case HWWAnalysisType::ZH:
                lepOfInterest = Evt->part(fIndexFourthLep);
                break;
            default:
                throw std::runtime_error("ERROR in HWWLeptonIDObservable :: HWWAnalysisType::TwoLepton or HWWAnalysisType::WH should not be trying to access fourth lepton mode of running.");
        }
        // BREAK
        break;
    // other lepton (for 2L)
    case HWWLepIDModeOfRunning::OtherLepID: case HWWLepIDModeOfRunning::OtherLepAntiID:
        switch (fHWWAnalysisType) {
            case HWWAnalysisType::TwoLep:
                lepOfInterest = Evt->otherPart(fIndexOtherLep);
                break;
            default:
                throw std::runtime_error("ERROR in HWWLeptonIDObservable :: Other lepton mode of running is currently restricted to HWWAnalysisType::TwoLepton -- does this make sense?");
        }
        // BREAK
        break;
    default:
        DEBUGclass("In event retrieval error.");
        throw std::runtime_error("ERROR in HWWLeptonIDObservable :: unrecognised mode of running. Check your configuration of this observable.");
  }

  // check that particle exists
  if (!lepOfInterest) throw std::runtime_error("[HWWLeptonIDObservable] failed to retrieve iParticle!");

  // compute pass or fail
  bool pass(false);
  switch (fHWWLepIDModeOfRunning) {
    // id
    case HWWLepIDModeOfRunning::LeadLepID: case HWWLepIDModeOfRunning::SubleadLepID: case HWWLepIDModeOfRunning::ThirdLepID: case HWWLepIDModeOfRunning::FourthLepID: case HWWLepIDModeOfRunning::OtherLepID:
        pass = fLeptonIDHelper->isID(lepOfInterest);
        break;
    // anti-id
    case HWWLepIDModeOfRunning::LeadLepAntiID: case HWWLepIDModeOfRunning::SubleadLepAntiID: case HWWLepIDModeOfRunning::ThirdLepAntiID: case HWWLepIDModeOfRunning::FourthLepAntiID: case HWWLepIDModeOfRunning::OtherLepAntiID:
        pass = fLeptonIDHelper->isAntiID(lepOfInterest);
        break;
    default:
        DEBUGclass("In event cut error.");
        throw std::runtime_error("ERROR in HWWLeptonIDObservable :: unrecognised mode of running. Check your configuration of this observable.");
  }
  DEBUGclass("returning: %d", pass);
  return static_cast<double>(pass);
  #endif
}
//______________________________________________________________________________________________
// getClone override (so we don't have to rely on root streamer)
TQObservable* HWWLeptonIDObservable::getClone() const  {
  // retrieve a clone of this observable
  TQObservable* obs = NULL;
  if(this->hasExpression()){
    obs = this->tryCreateInstanceVirtual(this->getExpression());
  }
  if(!obs){
    // obs = (TQObservable*)(this->Clone()); DON'T USE ROOT STREAMER, INSTEAD USE COPY constructor
    obs = new HWWLeptonIDObservable(*this);
  }
  if(obs->isInitialized()) obs->finalize();
  return obs;
}

//______________________________________________________________________________________________
bool HWWLeptonIDObservable::initializeSelf(){
  TString CandName = "";
  if(!this->fSample->getTagString("~cand",CandName)) return false;
  this->mCandName = "Event"+CandName;
  return true;
}
//______________________________________________________________________________________________
HWWLeptonIDObservable::HWWLeptonIDObservable(const TString& name, const HWW::HWWLeptonIDHelper* lepIDHelper,
                                    const int mode_of_running, const int analysis_type,
                                    const unsigned int indexLeadLep, const unsigned int indexSubleadLep,
                                    const unsigned int indexThirdLep, const unsigned int indexFourthLep,
                                    const unsigned int indexOtherLep):
  TQEventObservable(name),
  fLeptonIDHelper(lepIDHelper),
  fIndexLeadLep(indexLeadLep),
  fIndexSubleadLep(indexSubleadLep),
  fIndexThirdLep(indexThirdLep),
  fIndexFourthLep(indexFourthLep),
  fIndexOtherLep(indexOtherLep)
{
  // constructor with name argument
  DEBUGclass("constructor called with '%s'",name.Data());
  fHWWLepIDModeOfRunning = static_cast<HWWLepIDModeOfRunning>(mode_of_running);
  fHWWAnalysisType = static_cast<HWWAnalysisType>(analysis_type);
  // check that a valid value was given
  switch (fHWWLepIDModeOfRunning) {
    case HWWLepIDModeOfRunning::LeadLepID:
      break;
    case HWWLepIDModeOfRunning::SubleadLepID:
      break;
    case HWWLepIDModeOfRunning::ThirdLepID:
      break;
    case HWWLepIDModeOfRunning::FourthLepID:
      break;
    case HWWLepIDModeOfRunning::OtherLepID:
      break;
    case HWWLepIDModeOfRunning::LeadLepAntiID:
      break;
    case HWWLepIDModeOfRunning::SubleadLepAntiID:
      break;
    case HWWLepIDModeOfRunning::ThirdLepAntiID:
      break;
    case HWWLepIDModeOfRunning::FourthLepAntiID:
      break;
    case HWWLepIDModeOfRunning::OtherLepAntiID:
      break;
    default:
      throw std::runtime_error("[HWWLeptonIDObservable] Invalid mode-of-running value given!");
  }
  switch (fHWWAnalysisType) {
    case HWWAnalysisType::TwoLep:
      break;
    case HWWAnalysisType::WH:
      break;
    case HWWAnalysisType::ZH:
      break;
    default:
      throw std::runtime_error("ERROR in HWWLeptonIDObservable :: Unknown analysis type given!");
  }
  if (!fLeptonIDHelper) { throw std::runtime_error("ERROR in HWWLeptonIDObservable :: you must pass a valid pointer to your HWWLeptonIDHelper object!"); }

  DEBUGclass("Running with mode %d", fHWWLepIDModeOfRunning);
  DEBUGclass("Running with analysis type %d", fHWWAnalysisType);
}
