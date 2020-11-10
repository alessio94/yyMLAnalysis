#include "CAFExample/HWWLeptonIDHelper.h"
#include <algorithm>
// #include "CAFExample/HWWObservableBaseTool.h"

// Uncomment the following line to enable debug printouts
// #define _DEBUG_
// You can perform debug printouts with statements like this:
// DEBUG("error number %d occurred", someInteger);

// For debug messages
#include "QFramework/TQLibrary.h"

// ClassImpT(HWW::HWWLeptonIDHelper, 1)

/** default constructor, put the default arguments here **/
HWW::HWWLeptonIDHelper::HWWLeptonIDHelper()
  // HWW::ObservableBaseTool(name)
{
  DEBUG("HWWLeptonIDHelper::HWWLeptonIDHelper(...) : Inside default constructor");


  // initialize selection map with empty vectors with selections
  m_id_selections[xAOD::Type::Electron] = std::vector<Selection*>{};
  m_antiid_selections[xAOD::Type::Electron] = std::vector<Selection*>{};
  m_id_selections[xAOD::Type::Muon] = std::vector<Selection*>{};
  m_antiid_selections[xAOD::Type::Muon] = std::vector<Selection*>{};

}

// copy constructor
HWW::HWWLeptonIDHelper::HWWLeptonIDHelper(const HWWLeptonIDHelper& helper) :
// copy over the members
  // TObject(helper),
  // copy the vector -- means that new object will have pointers to the same selection
  // original. This should be OK.
  m_id_selections(helper.m_id_selections),
  m_antiid_selections(helper.m_antiid_selections)

{
  DEBUG("HWWLeptonIDHelper::HWWLeptonIDHelper(...) : Inside copy constructor");
  // m_id_selections(helper.m_id_selections),
  // m_antiid_selections(helper.m_antiid_selections)
  // m_id_selections[xAOD::Type::Electron] = std::vector<Selection*>{};
  // m_antiid_selections[xAOD::Type::Electron] = std::vector<Selection*>{};
  // m_id_selections[xAOD::Type::Muon] = std::vector<Selection*>{};
  // m_antiid_selections[xAOD::Type::Muon] = std::vector<Selection*>{};
  // copy over Selections
  // for (const auto& sel : helper.m_id_selections.at(xAOD::Type::Electron)) {
  //   Selection* sel_new = new Selection(sel);
  //   m_id_selections[xAOD::Type::Electron].push_back(sel_new);
  // }
  // for (const auto& sel : helper.m_antiid_selections.at(xAOD::Type::Electron)) {
  //   Selection* sel_new = new Selection(sel);
  //   m_antiid_selections[xAOD::Type::Electron].push_back(sel_new);
  // }
  // for (const auto& sel : helper.m_id_selections.at(xAOD::Type::Muon)) {
  //   Selection* sel_new = new Selection(sel);
  //   m_id_selections[xAOD::Type::Muon].push_back(sel_new);
  // }
  // for (const auto& sel : helper.m_antiid_selections.at(xAOD::Type::Muon)) {
  //   Selection* sel_new = new Selection(sel);
  //   m_antiid_selections[xAOD::Type::Muon].push_back(sel_new);
  // }
}

//______________________________________________________________________________________________

HWW::HWWLeptonIDHelper::~HWWLeptonIDHelper(){
  // free memory (TODO: use smart pointers)
  for (Selection* sel: m_id_selections[xAOD::Type::Electron]) {
    delete sel;
    sel = nullptr;
  }
  for (Selection* sel: m_id_selections[xAOD::Type::Muon]) {
    delete sel;
    sel = nullptr;
  }
  for (Selection* sel: m_antiid_selections[xAOD::Type::Electron]) {
    delete sel;
    sel = nullptr;
  }
  for (Selection* sel: m_antiid_selections[xAOD::Type::Muon]) {
    delete sel;
    sel = nullptr;
  }
}

void HWW::HWWLeptonIDHelper::showconfig() const {
  std::map<TString, xAOD::Type::ObjectType> types = {
    {"electron", xAOD::Type::Electron},
    {"muon",    xAOD::Type::Muon}
  };
  std::cout << "*** HWWLeptonIDHelper :: config \n" << std::endl;
  for (const auto& flavour : types) {
    const std::vector<Selection*>& sels_id = m_id_selections.at(flavour.second);
    std::cout << "HWWLeptonIDHelper :: id " << flavour.first << ":" << std::endl;
    for (const Selection* sel : sels_id) {
      sel->show();
    }

    const std::vector<Selection*>& sels_antiid = m_antiid_selections.at(flavour.second);
    std::cout << "HWWLeptonIDHelper :: anti-id " << flavour.first << ":" << std::endl;
    for (const Selection* sel : sels_antiid) {
      sel->show();
    }
  }
}

bool HWW::HWWLeptonIDHelper::isID(const xAOD::IParticle* ipart) const {
  if (ipart->type() == xAOD::Type::Electron) return isIDElectron(ipart);
  else if (ipart->type() == xAOD::Type::Muon) return isIDMuon(ipart);
  else {
    throw std::runtime_error(TString::Format("Inside HWWLeptonIDHelper::isID >> Expected xAOD::Type::Muon or xAOD::Type::Electron but found type %d!", ipart->type()).Data());
  }
}

bool HWW::HWWLeptonIDHelper::isAntiID(const xAOD::IParticle* ipart) const {
  if (ipart->type() == xAOD::Type::Electron) return isAntiIDElectron(ipart);
  else if (ipart->type() == xAOD::Type::Muon) return isAntiIDMuon(ipart);
  else {
    throw std::runtime_error(TString::Format("Inside HWWLeptonIDHelper::isAntiID >> Expected xAOD::Type::Muon or xAOD::Type::Electron but found type %d!", ipart->type()).Data());
  }
}

bool HWW::HWWLeptonIDHelper::allOK() const {
  return true;
}

bool HWW::HWWLeptonIDHelper::isIDElectron(const xAOD::IParticle* ipart) const {
  // if (m_debug) std::cout << "*** HWWLeptonIDHelper :: about to check selections for id electron..." << std::endl;
  DEBUG("Inside HWWLeptonIDHelper::isIDElectron(...)");
  const std::vector<Selection*>& id_selections_electron = m_id_selections.at(xAOD::Type::Electron);
  for (const auto& sel : id_selections_electron) {
    if (!sel->pass(ipart)) return false;
  }
  // passed all selections, return true
  return true;
}

bool HWW::HWWLeptonIDHelper::isIDMuon(const xAOD::IParticle* ipart) const {
  DEBUG("Inside HWWLeptonIDHelper::isIDMuon(...)");
  const std::vector<Selection*>& id_selections_muon = m_id_selections.at(xAOD::Type::Muon);
  for (const auto& sel : id_selections_muon) {
    if (!sel->pass(ipart)) return false;
  }
  // passed all selections, return true
  return true;
}

bool HWW::HWWLeptonIDHelper::isAntiIDElectron(const xAOD::IParticle* ipart) const {
  // anti-id electron should fail the identified electron...
  DEBUG("Inside HWWLeptonIDHelper::isAntiIDElectron(...)");
  if (isIDElectron(ipart)) return false;
  // ... but still pass some looser requirements
  const std::vector<Selection*>& antiid_selections_electron = m_antiid_selections.at(xAOD::Type::Electron);
  for (const auto& sel : antiid_selections_electron) {
    if (!sel->pass(ipart)) return false;
  }
  // passed all selections, return true
  return true;
}

bool HWW::HWWLeptonIDHelper::isAntiIDMuon(const xAOD::IParticle* ipart) const {
  // anti-id electron should fail the identified muon...
  DEBUG("Inside HWWLeptonIDHelper::isAntiIDMuon(...)");
  if (isIDMuon(ipart)) return false;
  // ... but still pass some looser requirements
  const std::vector<Selection*>& antiid_selections_muon = m_antiid_selections.at(xAOD::Type::Muon);
  for (const auto& sel : antiid_selections_muon) {
    if (!sel->pass(ipart)) return false;
  }
  // passed all selections, return true
  return true;
}
