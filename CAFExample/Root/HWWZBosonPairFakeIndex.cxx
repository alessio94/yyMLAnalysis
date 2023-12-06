#include "CAFExample/HWWZBosonPairFakeIndex.h"
#include <limits>


// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"

// #include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"

ClassImp(HWWZBosonPairFakeIndex)

//______________________________________________________________________________________________
// default constructor
HWWZBosonPairFakeIndex::HWWZBosonPairFakeIndex(){
  // default constructor
  DEBUGclass("default constructor called");
}
//______________________________________________________________________________________________
// copy constructor
HWWZBosonPairFakeIndex::HWWZBosonPairFakeIndex(const HWWZBosonPairFakeIndex& obs) :
  TQEventObservable(obs),
  HWWTrigBase(obs.getTrigConfVector()),
  mCand(obs.mCand),
  fLeptonIDHelper(obs.fLeptonIDHelper),
  fCandName(obs.fCandName)

  {}


//______________________________________________________________________________________________

HWWZBosonPairFakeIndex::~HWWZBosonPairFakeIndex(){
  // default destructor
  DEBUGclass("destructor called");
}


#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
//#include "xAODParticleEvent/CompositeParticleContainer.h"

//______________________________________________________________________________________________

bool HWWZBosonPairFakeIndex::initializeSelf() {

  TString s;
  if(!this->fSample->getTagString("~cand",s)){
    return false;
  }
  this->fCandName = "Event"+s;

  this->fCachedEntry = -999;
  this->fCachedValue = 1.0;

  return true;
}



bool HWWZBosonPairFakeIndex::isSFOS(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const {
  if (zPair.first->type() == xAOD::Type::Electron && zPair.second->type() == xAOD::Type::Electron) { // both electrons
    // cast to electrons, check OS
    const xAOD::Electron * elec1 = static_cast<const xAOD::Electron*>(zPair.first);
    const xAOD::Electron * elec2 = static_cast<const xAOD::Electron*>(zPair.second);
    return elec1->charge() + elec2->charge() == 0;
  } else if (zPair.first->type() == xAOD::Type::Muon && zPair.second->type() == xAOD::Type::Muon) { // both muons
    // cast to muons, check OS
    const xAOD::Muon * muon1 = static_cast<const xAOD::Muon*>(zPair.first);
    const xAOD::Muon * muon2 = static_cast<const xAOD::Muon*>(zPair.second);
    return muon1->charge() + muon2->charge() == 0;
  }
  DEBUGclass("[HWWZBosonPairFakeIndex] isSFOS() :: returning false");
  return false; // not SF, return false
}

bool HWWZBosonPairFakeIndex::isID(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const {
  if ( fLeptonIDHelper->isID(zPair.first) && fLeptonIDHelper->isID(zPair.second) ) return true;
  DEBUGclass("[HWWZBosonPairFakeIndex] isID() :: returning false");
  return false;
}

bool HWWZBosonPairFakeIndex::isTriggerMatched(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair, const xAOD::EventInfo* evtInfo) const {
  if (  HWWTrigBase::isMatchedParticle( zPair.first, evtInfo) || HWWTrigBase::isMatchedParticle(zPair.second, evtInfo )) return true;
  DEBUGclass("[HWWZBosonPairFakeIndex] isTriggerMatched() :: returning false");
  return false;
}

bool HWWZBosonPairFakeIndex::isWithinZWindow(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair, const xAOD::Type::ObjectType fakeType) const {
  // compute the Mll-MZ difference and return true if within the z-window
  // return getAbsMllMZDiff(zPair) < fMllMZWindow;
  const float mll = getMll(zPair);
  return fakeType == xAOD::Type::Electron
        ?  fMZWindow_low_electron < mll &&  mll < fMZWindow_up
        :  fMZWindow_low_muon < mll     &&  mll < fMZWindow_up;
}

float HWWZBosonPairFakeIndex::getMll(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const {
  // compute the Mll of the pair and return
  TLorentzVector lepton1; lepton1.SetPtEtaPhiE(zPair.first->pt(), zPair.first->eta(), zPair.first->phi(), zPair.first->e());
  TLorentzVector lepton2; lepton2.SetPtEtaPhiE(zPair.second->pt(), zPair.second->eta(), zPair.second->phi(), zPair.second->e());
  return (lepton1 + lepton2).M() * 0.001; // mll in GeV
}

float HWWZBosonPairFakeIndex::getAbsMllMZDiff(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair) const {
  return fabs ( getMll(zPair) - fZMass);
}

//______________________________________________________________________________________________
bool HWWZBosonPairFakeIndex::isGoodZCand(const std::pair< const xAOD::IParticle*, const xAOD::IParticle* >& zPair, const xAOD::EventInfo* evtInfo, const xAOD::Type::ObjectType fakeType) const {
  // debugging
  if (debugMissingEvts && evtNrUnique(evtInfo)) {
    if (!isSFOS(zPair)) std::cout<<"evtNr unique --> zPair is not SFOS!" << std::endl;
    if (!isID(zPair))  std::cout << "evtNr unique --> zPair is not ID!" << std::endl;
    if (!isTriggerMatched(zPair, evtInfo))  std::cout << "evtNr unique --> zPair is not trigger matched!" << std::endl;
    if (!isWithinZWindow(zPair, fakeType))  std::cout << "evtNr unique --> zPair is not within Zmass window!" << std::endl;

  }

  // check that this pair is SFOS, has both leptons ID, at least one lepton trigger matched and
  // mll within Z window
  return  (
          isSFOS(zPair)
      &&  isID(zPair)
      &&  isTriggerMatched(zPair, evtInfo)
      &&  isWithinZWindow(zPair, fakeType)
    );
}

//______________________________________________________________________________________________
void HWWZBosonPairFakeIndex::makeZBosonCandPairs(std::map< zBosonPairIdentifier, float>& zPairCands,
                                        const xAOD::CompositeParticle* Evt,
                                        const xAOD::IParticle* otherLep0, const xAOD::EventInfo* evtInfo) const {

  // make the lep0+lep1 pair
  std::pair< const xAOD::IParticle*, const xAOD::IParticle* > zPair = { Evt->part(0), Evt->part(1) } ;
  xAOD::Type::ObjectType fakeType = otherLep0->type();

  // if this pair fulfils Z candidate requirements, add to map
  if (isGoodZCand(zPair, evtInfo, fakeType)) zPairCands[ zBosonPairIdentifier::LEP0_AND_LEP1 ] = getAbsMllMZDiff(zPair);

  // make the lep0+otherLep0 pair
  zPair = { Evt->part(0), otherLep0 } ;
  fakeType = Evt->part(1)->type();
  // if this pair fulfils Z candidate requirements, add to map
  if (isGoodZCand(zPair, evtInfo, fakeType)) zPairCands[ zBosonPairIdentifier::LEP0_AND_OTHERLEP0 ] = getAbsMllMZDiff(zPair);

  // make the lep1+otherLep0 pair
  zPair = { Evt->part(1), otherLep0 } ;
  fakeType = Evt->part(0)->type();
  // if this pair fulfils Z candidate requirements, add to map
  if (isGoodZCand(zPair, evtInfo, fakeType)) zPairCands[ zBosonPairIdentifier::LEP1_AND_OTHERLEP0 ] = getAbsMllMZDiff(zPair);


}


bool HWWZBosonPairFakeIndex::evtNrUnique(const xAOD::EventInfo* evtInfo) const {
  std::array<long long unsigned int, 37> evtNumbers={
    432008049,
    1453973682,
    298001122,
    321101339,
    2092289953,
    265488622,
    5330127577,
    1068647710,
    3888873152,
    742462728,
    509747772,
    5245843827,
    671351973,
    154585070,
    390970075,
    672427615,
    5563343559,
    2269801154,
    3338475416,
    3280916268,
    1466877244,
    1334635691,
    1358073203,
    2621715034,
    843665139,
    215255880,
    4411707094,
    4773264112,
    2119100093,
    1231407261,
    3454514929,
    2445649061,
    2426355135,
    311878445,
    1567256743,
    578245962,
    3061906790
  };
  long long unsigned int evtNr = static_cast<long long unsigned int>(evtInfo->eventNumber());

  return ( evtNumbers.end() != std::find(evtNumbers.begin(), evtNumbers.end(), evtNr) );
}
//______________________________________________________________________________________________


double HWWZBosonPairFakeIndex::getValue() const {
  // value retrieval function, called on every event for every cut and histogram
  DEBUGclass("entering getValue() function");

  // the TQEventObservable only works in an ASG RELEASE, hence
  // we encapsulate the implementation in an ifdef/ifndef block
  #ifndef HAS_XAOD
  #warning "using plain ROOT compilation scheme - please add an ASG Analysis Release in order to use this feature!"
  return std::numeric_limits<double>::quiet_NaN();
  #else

  if(this->getCurrentEntry() == this->fCachedEntry) {
    DEBUGclass("skipping reevalution for event %d, returning %f", this->getCurrentEntry(), this->fCachedValue);
    return this->fCachedValue;
  }

  if (!this->fEvent->retrieve(this->mCand,this->fCandName.Data()).isSuccess()){
    DEBUGclass("Failed to retrieve event candidate!");//" %s", this->fCandName.Data()).Data());
    return 0.;
  }
  // get the event candidate
  const xAOD::CompositeParticle *Evt = this->mCand->at(0);

  const xAOD::EventInfo* evtInfo = NULL;
  if (!this->fEvent->retrieve(evtInfo,"EventInfo").isSuccess()) {
    throw std::runtime_error("HWWZBosonPairFakeIndex :: Failed to retrieve event info!");
  }

  // get the otherLep0, if it exists (if we find a muon or electron)
  const xAOD::IParticle* otherPart0 = Evt->otherPart(0);
  const xAOD::IParticle* otherLep0 = nullptr;
  if (otherPart0) { // otherPart0 exists...
    if (otherPart0->type() == xAOD::Type::Electron || otherPart0->type() == xAOD::Type::Muon) { // ... and is muon or electron
      otherLep0 = otherPart0;
    } else {
      DEBUGclass(Form("[HWWZBosonPairFakeIndex] otherPart0 found, but is not electron or muon. Type = %d", otherPart0->type()));
      DEBUGclass(Form("[HWWZBosonPairFakeIndex] lep0 type =  %d", Evt->part(0)->type()));
      if (Evt->part(0)->isAvailable<int>("hwwTruthType")) {
        DEBUGclass(Form("[HWWZBosonPairFakeIndex] lep0 hwwTruthType found =  %d", Evt->part(0)->auxdata<int>("hwwTruthType")));
      }
      if (Evt->part(0)->isAvailable<int>("truthType")) {
        DEBUGclass(Form("[HWWZBosonPairFakeIndex] lep0 truthType found =  %d", Evt->part(0)->auxdata<int>("truthType")));
      }
    }
  }

  // if no otherLep is found, we return 0 (no fake found)
  if (!otherLep0) {
    if (evtNrUnique(evtInfo))
      std::cout << "evt nr unique --> otherLep0 invalid ptr!" << std::endl;
    DEBUGclass("[HWWZBosonPairFakeIndex] otherLep0 not found! Returning 0.");
    return static_cast<double>(fakeIndex::NO_FAKE_FOUND);
  }

  // get the combinations which fulfil the Zboson requirements
  // map with the pair enum as key, |mll-mZ|  as value
  std::map< zBosonPairIdentifier, float > zPairCands;
  makeZBosonCandPairs(zPairCands, Evt, otherLep0, evtInfo);

  if (debugMissingEvts && evtNrUnique(evtInfo)) {
    if (zPairCands.size() == 0) {
      std::cout << "evtNr unique --> didn't find any Zcand! " << std::endl;
    }
  }


  // step through map (which could be empty) and find the pair closest to the z mass
  zBosonPairIdentifier bestZPair = zBosonPairIdentifier::NO_PAIR_FOUND; // default: no candidate found
  float min_mllmZ_diff = 999999.;
  // loop over candidate combinations, record the one which is closest to the Zboson mass
  for (const auto& pair_mll : zPairCands) {
    if (pair_mll.second < min_mllmZ_diff) {
      bestZPair = pair_mll.first;
      min_mllmZ_diff = pair_mll.second;
    }
  }

  // set the fake index, depending on z-pair identifier
  fakeIndex theFakeIndex = fakeIndex::NO_FAKE_FOUND;
  switch (bestZPair) {
    case zBosonPairIdentifier::NO_PAIR_FOUND:
      break;
    case zBosonPairIdentifier::LEP0_AND_LEP1:
      theFakeIndex = fakeIndex::OTHERLEP0;
      break;
    case zBosonPairIdentifier::LEP0_AND_OTHERLEP0:
      theFakeIndex = fakeIndex::SUBLEADLEP;
      break;
    case zBosonPairIdentifier::LEP1_AND_OTHERLEP0:
      theFakeIndex = fakeIndex::LEADLEP;
      break;
    default:
      throw std::runtime_error("[HWWZBosonPairFakeIndex] unrecognised z-pair index! shouldn't end up here.");
  }

  this->fCachedEntry = this->getCurrentEntry();
  this->fCachedValue = static_cast<double> (theFakeIndex);

  // return index of fake
  DEBUGclass("returning %d", theFakeIndex);
  return static_cast<double> ( theFakeIndex );

  #endif
}
//______________________________________________________________________________________________
// getClone override (so we don't have to rely on root streamer)
TQObservable* HWWZBosonPairFakeIndex::getClone() const  {
  // retrieve a clone of this observable
  TQObservable* obs = NULL;
  if(this->hasExpression()){
    obs = this->tryCreateInstanceVirtual(this->getExpression());
  }
  if(!obs){
    // obs = (TQObservable*)(this->Clone()); DON'T USE ROOT STREAMER, INSTEAD USE COPY constructor
    obs = new HWWZBosonPairFakeIndex(*this);
  }
  if(obs->isInitialized()) obs->finalize();
  return obs;
}



//______________________________________________________________________________________________
// nominal constructor
HWWZBosonPairFakeIndex::HWWZBosonPairFakeIndex(const TString& name, const std::vector<HWWTrigConfig*>& trigConfigs,
                                        const HWWLeptonIDHelper* lepIDHelper) :
TQEventObservable(name),
HWWTrigBase(trigConfigs),
fLeptonIDHelper(lepIDHelper)

{
  // constructor with name argument
  DEBUGclass("[HWWZBosonPairFakeIndex] constructor called with '%s'",name.Data());

  // check that valid lepton id helper was passed
  if (!fLeptonIDHelper) throw std::runtime_error("ERROR in [HWWZBosonPairFakeIndex] :: you must pass along a valid pointer to a HWWLeptonIDHelper object.");

}

//______________________________________________________________________________________________
// nominal constructor
HWWZBosonPairFakeIndex::HWWZBosonPairFakeIndex(const TString& name,
                                        const HWWLeptonIDHelper* lepIDHelper) :
TQEventObservable(name),
fLeptonIDHelper(lepIDHelper)
{
  // constructor with name argument
  DEBUGclass("[HWWZBosonPairFakeIndex] constructor called with '%s'",name.Data());

  // check that valid lepton id helper was passed
  if (!fLeptonIDHelper) throw std::runtime_error("ERROR in [HWWZBosonPairFakeIndex] :: you must pass along a valid pointer to a HWWLeptonIDHelper object.");

}
