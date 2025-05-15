//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWLEPTONIDHELPER__
#define __HWWLEPTONIDHELPER__ 1

#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "TString.h"
#include "yyMLAnalysisCode/HWWSelection.h"

// using namespace Selections;

// Include the base class (only put in to enforce python bindings to be created, don't know how to get them in other way)
// class ObservableBaseTool;
// #include "HWWDualUseUtils/HWWObservableBaseTool.h"

namespace HWW {

  class HWWLeptonIDHelper { //: public TObject {
    // /// Create a proper constructor for Athena
    // ASG_TOOL_CLASS0( HWWLeptonIDHelper )

    private:

    // HWWLeptonIDHelper is responsible for cleaning up the memory allocated in vector
    std::map<xAOD::Type::ObjectType, std::vector<Selection*> > m_id_selections;
    std::map<xAOD::Type::ObjectType, std::vector<Selection*> > m_antiid_selections;

    // At the risk of overcomplicating things, it may be nice to know that the cuts are being added properly so this flag turns on debug statements when adding cuts
    // However, you may want to know if it's being call on ID electron, anti-ID muon, etc., so those are controlled by the _DEBUG flag in the cxx file
    // There's no overhead for turning on the debug with m_debug = true as you add a finite number of cuts
    // I tried moving the template function declarations to the cxx file, but I could not get python to recognize the template functions (and I think the explicit instantiations have
    // to be in this file)
    bool m_debug = false;

    // private add-selection methods to be used by the more verbose public ones called from outside (see below)
    // template functions, need full definition here in header file
    template <class T>
    void addCut(std::vector<Selection*>& selections_to_add_to, const TString& cutname, const TString& type_str, const TString& auxvarname, const T& cutvalue, const TString& operator_str, const bool validOnlyInPtRegion, const float& ptcut_low, const float& ptcut_up) {
      // didn't manage to pass char from python, so enforce this if that's the type
      Selection* sel = nullptr;
      if (type_str == "char") {
        sel = new AuxDataSelection<char>(cutname, auxvarname, cutvalue, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      } else if (type_str == "unsigned short") {
        sel = new AuxDataSelection<unsigned short>(cutname, auxvarname, cutvalue, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      } else if (type_str == "float") {
        sel = new AuxDataSelection<float>(cutname, auxvarname, cutvalue, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      } else if (type_str == "int") {
        sel = new AuxDataSelection<int>(cutname, auxvarname, cutvalue, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      } else {
        sel = new AuxDataSelection<T>(cutname, auxvarname, cutvalue, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      }
      selections_to_add_to.push_back(sel);
    }

    public:
    // print the current configuration of all anti-id and id selections
    void showconfig() const;

    // id and anti-id checks
    bool isID(const xAOD::IParticle* ipart) const;
    bool isAntiID(const xAOD::IParticle* ipart) const;
    bool isIDMuon(const xAOD::IParticle* ipart) const;
    bool isAntiIDMuon(const xAOD::IParticle* ipart) const;
    bool isIDElectron(const xAOD::IParticle* ipart) const;
    bool isAntiIDElectron(const xAOD::IParticle* ipart) const;

    /*** add cut functions: two per id/anti-id and flavour. Reflect what's being set in function names so that call from outside is clear ***/
    template <class T>
    void addCut_ElectronID(const TString cutname, const TString type_str, const TString auxvarname, const T value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.) {
      // static counter to keep track of how many times this fcn has been called.
      static unsigned int ncalled = 0;
      std::vector<Selection*>& selections_id_electron = m_id_selections[xAOD::Type::Electron];
      addCut(selections_id_electron, cutname, type_str, auxvarname, value, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      ncalled++;
      if (m_debug) std::cout << "Inside HWWLeptonIDHelper :: successfully added selection to id electrons. " << std::endl; //" Nr of selections for anti-id electrons = " << ncalled << "." << std::endl;
    }

    template <class T>
    void addCut_ElectronAntiID(const TString cutname, const TString type_str, const TString auxvarname, const T value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.) {
      // static counter to keep track of how many times this fcn has been called.
      static unsigned int ncalled = 0;
      std::vector<Selection*>& selections_antiid_electron = m_antiid_selections[xAOD::Type::Electron];
      addCut(selections_antiid_electron, cutname, type_str, auxvarname, value, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      ncalled++;
      if (m_debug) std::cout << "Inside HWWLeptonIDHelper :: successfully added selection to anti-id electrons. " << std::endl; //" Nr of selections for anti-id electrons = " << ncalled << "." << std::endl;
    }


    template <class T>
    void addCut_MuonID(const TString cutname, const TString type_str, const TString auxvarname, const T value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.) {
      // static counter to keep track of how many times this fcn has been called.
      static unsigned int ncalled = 0;
      std::vector<Selection*>& selections_id_muon = m_id_selections[xAOD::Type::Muon];
      addCut(selections_id_muon, cutname, type_str, auxvarname, value, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      ncalled++;
      if (m_debug) std::cout << "Inside HWWLeptonIDHelper :: successfully added selection to id muons. " << std::endl; //" Nr of selections for anti-id muons = " << ncalled << "." << std::endl;
    }

    template <class T>
    void addCut_MuonAntiID(const TString cutname, const TString type_str, const TString auxvarname, const T value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.) {
      // static counter to keep track of how many times this fcn has been called.
      static unsigned int ncalled = 0;
      std::vector<Selection*>& selections_antiid_muon = m_antiid_selections[xAOD::Type::Muon];
      addCut(selections_antiid_muon, cutname, type_str, auxvarname, value, operator_str, validOnlyInPtRegion, ptcut_low, ptcut_up);
      ncalled++;
      if (m_debug) std::cout << "Inside HWWLeptonIDHelper :: successfully added selection to anti-id muons. " << std::endl; //" Nr of selections for anti-id muons = " << ncalled << "." << std::endl;
      // if (ncalled > 2 && m_debug) {
      //   showconfig();
      // }
    }

    // sanity function, to be called after all setter functions have been called
    bool allOK() const;

    HWWLeptonIDHelper(); // default constructor
    HWWLeptonIDHelper(const HWWLeptonIDHelper& helper); // copy constructor

    virtual ~HWWLeptonIDHelper();

    // can't get ClassDef to work, causes linker error when loading QFramework library ("cling::DynamicLibraryManager::loadLibrary(): /afs/cern.ch/work/p/psidebo/HWW/HWWAnalysis/RootCoreBin/obj/x86_64-slc6-gcc49-opt/HWWDualUseUtils/lib/libHWWDualUseUtils.so: undefined symbol: _ZN3HWW17HWWLeptonIDHelper8StreamerER7TBufferterminate called after throwing an instance of 'std::string'")
    // ClassDefT(HWWLeptonIDHelper, 1);

  };

/** apparently one needs to declare one example (with any type) template specification of the template functions to make them visible from python.
    See https://root-forum.cern.ch/t/class-with-a-template-member-function/20101
**/
// private addCut function(s)
template void HWWLeptonIDHelper::addCut<int>(std::vector<Selection*>& selections_to_add_to, const TString& cutname, const TString& type_str, const TString& auxvarname, const int& cutvalue, const TString& operator_str, const bool validOnlyInPtRegion, const float& ptcut_low, const float& ptcut_up);
// public particle-id/antiid-specific public functions
template void HWWLeptonIDHelper::addCut_MuonAntiID<int>(const TString cutname, const TString type_str, const TString auxvarname, const int value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.);
template void HWWLeptonIDHelper::addCut_MuonID<int>(const TString cutname, const TString type_str, const TString auxvarname, const int value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.);
template void HWWLeptonIDHelper::addCut_ElectronAntiID<int>(const TString cutname, const TString type_str, const TString auxvarname, const int value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.);
template void HWWLeptonIDHelper::addCut_ElectronID<int>(const TString cutname, const TString type_str, const TString auxvarname, const int value, const TString operator_str, const bool validOnlyInPtRegion=false, const float ptcut_low=0., const float ptcut_up=0.);

 } // end namespace HWW
#endif //> !__HWWLEPTONIDHELPER__
