//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWNLEPTONOBS__
#define __HWWNLEPTONOBS__
#include "CAFxAODUtils/TQEventObservable.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"

class HWWNLeptonObs : public TQEventObservable {
protected:
  // Put here data members you wish to use.
  // For example you might want to add a variable
  // for the name of the xAOD container to be retrieved that
  // will be initialized in the initialize function.
  TString mContName = "";

  mutable xAOD::CompositeParticleContainer const * mCand = 0;

public:
  virtual double getValue() const override;
protected:
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
public:
  HWWNLeptonObs();
  HWWNLeptonObs(const TString& name, bool CountElectrons=true, bool CountMuons=true, int Mode=0,
    double Ele_maxZsinTheta=0.5, double Ele_maxd0Sig=5.0, int Ele_LH=0,
    double Mu_maxZsinTheta=0.5, double Mu_maxd0Sig=3.0, int Isolation=2,
    double MinPt=15000.0);
  virtual ~HWWNLeptonObs();
  ClassDefOverride(HWWNLeptonObs, 1);

  bool getCountElectrons();
  bool getCountMuons();
  int getMode();
  double getEle_maxZsinTheta();
  double getEle_maxd0Sig();
  double getEle_LH();
  double getMu_maxZsinTheta();
  double getMu_maxd0Sig();
  double getIsolation();
  double getMinPt();

  void setCountElectrons(bool CountElectrons);
  void setCountMuons(bool CountMuons);
  void setMode(int Mode);
  void setEle_maxZsinTheta(double Ele_maxZsinTheta);
  void setEle_maxd0Sig(double Ele_maxd0Sig);
  void setEle_LH(double Ele_LH);
  void setMu_maxZsinTheta(double Mu_maxZsinTheta);
  void setMu_maxd0Sig(double Mu_maxd0Sig);
  void setIsolation(double Isolation);
  void setMinPt(double MinPt);

  bool m_useMuonQuality = false;
  int  m_muonQuality = 0;
private:

  /// Whether to include electrons in total count
  bool m_countElectrons;
  /// Whether to include muons in total count
  bool m_countMuons;

  int m_mode; //Running mode - 0 default, 1 Veto muons, 2 Veto electrons

  double m_maxz_ele, m_maxd_ele; // cut on z0*sinTheta and d0 significance
  int m_LH_ele; // 1 Loose, 2 Medium, 3 Tight, 4 tight for <25GeV, Medium for >25GeV
  double m_maxz_mu, m_maxd_mu;  // cut on z0*sinTheta and d0 significance
  int m_iso; // 1 passIsoGradientLoose, 2 passIsoGradient, 3 passIsoTight
  double m_minPt; // Minimum pT of the veto lepton

};
#endif
