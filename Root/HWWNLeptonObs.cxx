#include "CAFExample/HWWNLeptonObs.h"
#include <limits>

// uncomment the following line to enable debug printouts
// #define _DEBUG_
// you can perform debug printouts with statements like this
// DEBUG("error number %d occurred",someInteger);

// be careful to not move the _DEBUG_ flag behind the following line
// otherwise, it will show no effect
#include "QFramework/TQLibrary.h"
#include "QFramework/TQSample.h"

ClassImp(HWWNLeptonObs)

//______________________________________________________________________________________________

HWWNLeptonObs::HWWNLeptonObs(){
  // default constructor
  DEBUGclass("default constructor called");
}

//______________________________________________________________________________________________

HWWNLeptonObs::~HWWNLeptonObs(){
  // default destructor
  DEBUGclass("destructor called");
}

#define XAOD_STANDALONE 1
// put here any EDM includes you might need, e.g.
/* example block:
//#include "xAODParticleEvent/CompositeParticleContainer.h"
*/

//______________________________________________________________________________________________

bool HWWNLeptonObs::getCountElectrons() {
  return m_countElectrons;
}

bool HWWNLeptonObs::getCountMuons() {
  return m_countMuons;
}

int HWWNLeptonObs::getMode() {
  return m_mode;
}

double HWWNLeptonObs::getEle_maxZsinTheta() {
  return m_maxz_ele;
}

double HWWNLeptonObs::getEle_maxd0Sig() {
  return m_maxd_ele;
}

double HWWNLeptonObs::getEle_LH() {
  return m_LH_ele;
}

double HWWNLeptonObs::getMu_maxZsinTheta() {
  return m_maxz_mu;
}

double HWWNLeptonObs::getMu_maxd0Sig() {
  return m_maxd_mu;
}

double HWWNLeptonObs::getIsolation() {
  return m_iso;
}

double HWWNLeptonObs::getMinPt() {
  return m_minPt;
}

//______________________________________________________________________________________________

void HWWNLeptonObs::setCountElectrons(bool CountElectrons) {
  m_countElectrons = CountElectrons;
}

void HWWNLeptonObs::setCountMuons(bool CountMuons) {
  m_countMuons = CountMuons;
}

void HWWNLeptonObs::setMode(int Mode) {
  m_mode = Mode;
}

void HWWNLeptonObs::setEle_maxZsinTheta(double Ele_maxZsinTheta) {
  m_maxz_ele = Ele_maxZsinTheta;
}

void HWWNLeptonObs::setEle_maxd0Sig(double Ele_maxd0Sig) {
  m_maxd_ele = Ele_maxd0Sig;
}

void HWWNLeptonObs::setEle_LH(double Ele_LH) {
  m_LH_ele = Ele_LH;
}

void HWWNLeptonObs::setMu_maxZsinTheta(double Mu_maxZsinTheta) {
  m_maxz_mu = Mu_maxZsinTheta;
}

void HWWNLeptonObs::setMu_maxd0Sig(double Mu_maxd0Sig) {
  m_maxd_mu = Mu_maxd0Sig;
}

void HWWNLeptonObs::setIsolation(double Isolation) {
  m_iso = Isolation;
}

void HWWNLeptonObs::setMinPt(double MinPt) {
  m_minPt = MinPt;
}

//______________________________________________________________________________________________

double HWWNLeptonObs::getValue() const {
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
  // Retrieve CompositeParticle container
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
  DEBUGclass("failed to retrieve candidates!");
  return false;
  }
   */

  // after you have retrieved your data members, you can proceed to calculate the return value
  // probably, you first have to retrieve an element from the container
  /* example block:
  const xAOD::CompositeParticle *Evt = cand->at(0);
  */

  // in the end, you should calculate your return value
  // of course, you can use other data members of your observable at any time
  /* example block:
     const double retval = Evt->part(0)->pt();
  */

  // Retrieve CompositeParticle container
  const xAOD::CompositeParticleContainer *cand = 0;
  if(!this->fEvent->retrieve(cand, this->mContName.Data()).isSuccess()){
    DEBUGclass("failed to retrieve candidates!");
    return false;
  }

  // Get the event candidate from the container
  const xAOD::CompositeParticle *Evt = cand->at(0);

  // Get the number of other leptons
  const int nMuons     = Evt->nMuons() + Evt->nOtherMuons();
  const int nElectrons = Evt->nElectrons() + Evt->nOtherElectrons();
  const int nLeptons   = m_countMuons * nMuons + m_countElectrons * nElectrons;

  if (m_mode==0) {
    DEBUGclass("Got %d muons, and %d electrons .... without cuts",nMuons,nElectrons);
    return static_cast<double>(nLeptons);
  }
  // now make more convoluted calculations
  int nJets=0;
  int nVetoMuon=0;
  int nVetoEle =0;
  bool passed3Lep=false;
  float mll=0;
  float mll_low=999999;
  float VetoLepPt=999999.9, VetoLepEta=99.9, VetoLepPhi=99.9;
  for (unsigned int i=0; i<Evt->nOtherParts(); i++) {
    if ( Evt->otherPart(i)->type()==xAOD::Type::Jet ) {
      nJets++;
      continue;
    }
    if ( Evt->otherPart(i)->type()==xAOD::Type::Muon ) {
      const xAOD::Muon* muon=Evt->otherMuon(i);
      if(muon->pt()<m_minPt) continue;

      if ( fabs(muon->eta())>2.5) continue;
      float z0    =muon->auxdata<float>("z0");
      float sTheta=muon->auxdata<float>("sinTheta");
      float d0sig =muon->auxdata<float>("d0sig");
      if ( fabs(z0*sTheta)>m_maxz_mu ) continue;
      if ( fabs(d0sig)>m_maxd_mu ) continue;

      char isol=0;
      if(m_iso==1) {
        isol = muon->auxdata<char>("passIsoLoose_VarRad");
      } else if(m_iso==2) {
        isol = muon->auxdata<char>("passIsoTightTrackOnly_VarRad");
      }else if(m_iso==3) {
        isol = muon->auxdata<char>("passIsoTight_VarRad");
      }else {
        float ptcone30=muon->auxdata<float>("ptvarcone30");
        isol = ( ptcone30/muon->pt()>0.20 );
      }
      if(!isol) continue;

      // If useMuonQuality, skip all muons that have looser quality than required
      if ((m_useMuonQuality) && (muon->auxdata<int>("Quality") > m_muonQuality)) continue;

      nVetoMuon+=1;
      VetoLepPt = muon->pt()/1000.;
      VetoLepEta = muon->eta();
      VetoLepPhi = muon->phi();

      //// this is for 3lep selection
      if (m_mode>3) {
        for (unsigned int t=0; t<Evt ->nParts(); t++) {
          if ( Evt->part(t)->type()!=xAOD::Type::Muon ) continue;
          TLorentzVector sigMuon=Evt->part(t)->p4();
          sigMuon+=muon->p4();
          if ( fabs(sigMuon.M()-91e3)<15e3 && Evt->muon(t)->charge()!=muon->charge() ) {
            passed3Lep=true;
            mll=sigMuon.M();
          }
          if ( Evt->muon(t)->charge()!=muon->charge() ) {
            if (sigMuon.M() < mll_low) mll_low=sigMuon.M();
          }
        }
      }
    } // end of muon part
    else {
      const xAOD::Electron* ele=Evt->otherElectron(i);
      if(ele->pt()<m_minPt) continue;
      int LH;
      if(m_LH_ele==1) {
        LH=ele->auxdata<char>("isLHLoose");
      } else if(m_LH_ele==2) {
        LH=ele->auxdata<char>("isLHMedium");
      } else if(m_LH_ele==3) {
        LH=ele->auxdata<char>("isLHTight");
      } else {
        if(ele->pt()<25e3)
          LH=ele->auxdata<char>("isLHTight");
        else
          LH=ele->auxdata<char>("isLHMedium");
      }
      if ( LH==0 ) continue;

      float fEta=fabs(ele->eta());
      if ( fEta>2.47 || (fEta>1.37 && fEta<1.52) ) continue;

      float z0    =ele->auxdata<float>("z0");
      float sTheta=ele->auxdata<float>("sinTheta");
      float d0sig =ele->auxdata<float>("d0sig");
      if ( fabs(z0*sTheta)>m_maxz_ele ) continue;

      if ( fabs(d0sig)>m_maxd_ele ) continue;

      char isol=0;
      if(m_iso==1){
        isol = ele->auxdata<char>("passIsoFCLoose");
      } else if(m_iso==2){
        isol = ele->auxdata<char>("passIsoGradient");
      } else if(m_iso==3) {
        isol = ele->auxdata<char>("passIsoFCTight");
      } else {
        float ptcone30=ele->auxdata<float>("ptvarcone20");
        isol = ( ptcone30/ele->pt()>0.20 );
      }
      if(!isol) continue;
      //      float ptcone30=ele->auxdata<float>("ptvarcone20");
      //      if ( ptcone30/ele->pt()>0.20 ) continue;

      if(nVetoMuon==0 || ele->pt()/1000. > VetoLepPt) {
        VetoLepPt = ele->pt()/1000.0;
        VetoLepEta = ele->eta();
        VetoLepPhi = ele->phi();
      }
      nVetoEle+=1;

      //// this is for 3lep selection
      if (m_mode>3) {
        for (unsigned int t=0; t<Evt ->nParts(); t++) {
          if ( Evt->part(t)->type()!=xAOD::Type::Electron ) continue;
          TLorentzVector sigEle=Evt->part(t)->p4();
          sigEle+=ele->p4();
          if ( fabs(sigEle.M()-91e3)<15e3 && Evt->electron(t)->charge()!=ele->charge()  ) {
            passed3Lep=true;
            mll=sigEle.M();
          }
          if ( Evt->electron(t)->charge()!=ele->charge() ) {
            if (sigEle.M() < mll_low) mll_low=sigEle.M();
          }
        }
      }
    } // end of electron part
  }
  if (m_mode==1) return static_cast<double>(nVetoMuon);
  if (m_mode==2) return static_cast<double>(nVetoEle );
  if (m_mode==3) return static_cast<double>(nVetoMuon+nVetoEle);
  if (m_mode==4) return static_cast<double>(passed3Lep);
  if (m_mode==5) return static_cast<double>(mll);
  if (m_mode==6) return static_cast<double>(mll_low);
  if (m_mode==7) return static_cast<double>(VetoLepPt);
  if (m_mode==8) return static_cast<double>(VetoLepEta);
  if (m_mode==9) return static_cast<double>(VetoLepPhi);
  if (m_mode==10) return static_cast<double>(nJets);

  DEBUGclass(" unrecognized running mode %s .... returnining 0 ", m_mode);
  const double retval = 0;

  return retval;
  #endif
}
//______________________________________________________________________________________________

bool HWWNLeptonObs::initializeSelf(){
  // initialize this observable
  // called once per sample (input file) so that the observable knows the name of the event candidate
  // will be EventEM or EventME in the case of DF analysis (depending on the current channel)
  TString ContName = "";
  if(!this->fSample->getTagString("~cand",ContName)) return false;
  this->mContName = "Event"+ContName;

  DEBUGclass("initializing");
  return true;
}

//______________________________________________________________________________________________

bool HWWNLeptonObs::finalizeSelf(){
  // finalize this observable
  // remember to undo anything you did in initializeSelf() !

  DEBUGclass("finalizing");
  return true;
}
//______________________________________________________________________________________________

HWWNLeptonObs::HWWNLeptonObs(const TString& name, bool CountElectrons, bool CountMuons, int Mode,
                              double Ele_maxZsinTheta, double Ele_maxd0Sig, int Ele_LH,
                              double Mu_maxZsinTheta, double Mu_maxd0Sig, int Isolation,
                              double MinPt):
TQEventObservable(name),
m_countElectrons(CountElectrons),
m_countMuons(CountMuons),
m_mode(Mode),
m_maxz_ele(Ele_maxZsinTheta),
m_maxd_ele(Ele_maxd0Sig),
m_LH_ele(Ele_LH),
m_maxz_mu(Mu_maxZsinTheta),
m_maxd_mu(Mu_maxd0Sig),
m_iso(Isolation),
m_minPt(MinPt)

{
  // nominal constructor
  DEBUGclass("constructor called with '%s'",name.Data());
}
