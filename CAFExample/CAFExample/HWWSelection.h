//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __HWWSELECTION__
#define __HWWSELECTION__ 1

#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "TString.h"
// #include "QFramework/TQObservable.h"
#include <iostream>

/***

  This file defines some selection classes which will be
  used by the HWWLeptonIDHelper.

  @ Author: Edvin Sidebo, edvin.sidebo@cern.ch

  **/


// enum for different operators in selections
enum Operator{LESSTHAN, LESSTHANEQUALTO, LARGERTHAN, LARGERTHANEQUALTO, EQUALTO, TRUE, FALSE, AND, OR};
// operator overload for printing
std::ostream& operator<<(std::ostream & os, const Operator& op);

// abstract base class which others will inherit from
class Selection  {
  public:
    // default constructor
    Selection() : m_name("dummy_namy") {}
    // copy constructor
    Selection(const Selection& sel) :
      // TObject(sel),
      m_name(sel.m_name),
      m_operator(sel.m_operator)
       {}
    // constructor without operator
    Selection(const TString& name) :
      m_name(name) {}
    // constructor with operator
    Selection(const TString& name, const TString& operator_str) :
      m_name(name) {
        // set operator
        if (operator_str == "<") {
          m_operator = Operator::LESSTHAN;
        } else if (operator_str == "=" || operator_str == "==") {
          m_operator = Operator::EQUALTO;
        } else if (operator_str == "<=") {
          m_operator = Operator::LESSTHANEQUALTO;
        } else if (operator_str == ">") {
          m_operator = Operator::LARGERTHAN;
        } else if (operator_str == ">=") {
          m_operator = Operator::LARGERTHANEQUALTO;
        } else if (operator_str == "TRUE") {
          m_operator = Operator::TRUE;
        } else if (operator_str == "FALSE") {
            m_operator = Operator::FALSE;
        // AND and OR operators, useful for MultiSelection (see below). Not intended to be called from outside
        } else if (operator_str == "AND") {
          m_operator = Operator::AND;
        } else if (operator_str == "OR") {
          m_operator = Operator::OR;
        } else {
          throw std::runtime_error("ERROR in class Selection :: unvalid operator string passed! Supported types are '>','>=','<','<=','=', '==' (save as '='), 'TRUE', 'FALSE', 'AND' and 'OR'. Check the initialization of this Selection.");
          // m_operator = Operator::UNDEFINED;
        }
        // // get decorator observable
        // TString auxvar_expression = ...
        // m_decObs = TQObservable::getObservable(auxvar_expression, NULL);
      }
      virtual ~Selection() {}
  protected:
    const TString m_name;
    Operator m_operator;
    // TQObservable* m_decObs = nullptr;
  public:
    virtual bool pass(const xAOD::IParticle* ipart) const=0; // must be implemented by derived class
    virtual void show() const=0;  // must be implemented by derived class
  // ClassDef(Selection, 1);
};


// class for making cuts on any aux variable
// template class so that we can retrieve any auxdata type
template <class T>
class AuxDataSelection : public Selection {
  public:
    // default constructor
    AuxDataSelection() {}
    // nominal constructor
    AuxDataSelection(const TString& name, const TString& varname, const T& cut, const TString& operator_str, const bool validOnlyInPtRegion=false, float ptcut_low=0., float ptcut_up=0.) :
      Selection(name, operator_str),
      m_var(varname),
      m_cut(cut),
      m_validOnlyInPtRegion(validOnlyInPtRegion),
      m_ptcut_low(ptcut_low),
      m_ptcut_up(ptcut_up),
      m_doAbsValue(false),
      m_normaliseToPt(false),
      m_productOfManyVars(false),
      m_nVars(1)
        {
          // check for absolute value, if so, set flag and remove '|'s from auxvar name
          if (m_var.CountChar('|') == 2 || m_var.CountChar('|') == 4) {
            m_doAbsValue=true;
            m_var.ReplaceAll("|", "");
          }
          // check for normalisation to pt ("/pt" at the end)
          if (m_var.EndsWith("/pt")) {
            m_normaliseToPt=true;
            m_var.ReplaceAll("/pt","");
          }

          // check for products
          int nMultiplications = m_var.CountChar('*');
          if (nMultiplications==1) {
            // increment nVars counter
            m_nVars += nMultiplications;
            m_var_array = new TString[m_nVars];
            Ssiz_t idx_mult_symbol = m_var.Index('*');
            m_var_array[0] = m_var(0, idx_mult_symbol);
            m_var_array[1] = m_var(idx_mult_symbol+1, m_var.Length()-1);
          } else if (nMultiplications>1) {
            throw std::runtime_error("ERROR in HWWSelection :: more than one multiplication is not supported!");
          }
    }
    virtual ~AuxDataSelection() {
      if (m_var_array) {
        delete m_var_array;
        m_var_array=nullptr;
      }
    }

  public:
    virtual bool pass(const xAOD::IParticle* ipart) const {
      // if pt-region-dependent cut, check that we are inside region. Otherwise return true
      if (m_validOnlyInPtRegion) {
        if (!(ipart->pt() > m_ptcut_low && ipart->pt() <= m_ptcut_up)) return true;
      }
      T value;
      // check for multiple vars (product)
      if (m_nVars>1) {
        if ( ipart->isAvailable<T>(m_var_array[0].Data()) && ipart->isAvailable<T>(m_var_array[1].Data()) ){
          value = ipart->auxdata<T>(m_var_array[0].Data())*ipart->auxdata<T>(m_var_array[1].Data());
        }
        else{
          value = T(); // Use the default constructor of the current type
        }
      } else {
        if ( ipart->isAvailable<T>(m_var.Data()) ){
          value = ipart->auxdata<T>(m_var.Data());
        }
        else{
          value = T(); // Use the default constructor of the current type
        }
      }
      // check for absolute value
      if (m_doAbsValue) value = fabs(value);
      // check for pt normalisation
      if (m_normaliseToPt) value /= ipart->pt();

      // do the comparison
      switch(m_operator) {
        case Operator::LESSTHAN:
          if (value < m_cut) return true;
          break;
        case Operator::LARGERTHAN:
          if (value > m_cut) return true;
          break;
        case Operator::EQUALTO:
          if (value == m_cut) return true;
          break;
        case Operator::TRUE:
          // cast to bool, require to pass (cut value ignored, not needed)
          if (static_cast<bool>(value)) return true;
          break;
        case Operator::FALSE:
          // cast to bool, require to fail (cut value ignored, not needed)
          if (!static_cast<bool>(value)) return true;
          break;
        case Operator::LESSTHANEQUALTO:
          if (value <= m_cut) return true;
          break;
        case Operator::LARGERTHANEQUALTO:
          if (value >= m_cut) return true;
          break;
        default:
          throw std::runtime_error("ERROR in AuxDataSelection :: unrecognised operator! Did you pass a valid operator when you  added your selection?.");
      }

      return false;
    }
    virtual void show() const {
      std::cout << "*** AuxDataSelection " << m_name << ", operator " << m_operator <<  std::endl;
      std::cout << "********************* auxdata " << m_var << ", cut value " << m_cut << std::endl;
      if (m_validOnlyInPtRegion) {
        std::cout << "********************* valid only in pt region defined by lower cut " << m_ptcut_low << " and upper cut " << m_ptcut_up << " (both in MeV)" << std::endl;
      }
    }

  private:
    TString m_var;
    const T m_cut;
    const bool m_validOnlyInPtRegion=false;
    const float m_ptcut_low;
    const float m_ptcut_up;
    bool m_doAbsValue=false;
    bool m_normaliseToPt=false;
    bool m_productOfManyVars=false;
    int m_nVars; //
    TString* m_var_array = nullptr; // if more than one var, this will be used to initialize array of auxvar names
  // public:
  // ClassDefT(AuxDataSelection, 1);

};
#endif //> !__HWWSELECTION__

// /** class with vector of selections, used either with OR or AND **/
// class MultiSelection : public Selection {
//   public:
//     // default constructor
//     MultiSelection() {}
//     // constructor passing name and operator only, selections to be added later by calling addSelection
//     MultiSelection(const TString& name, const TString& operator_str_andor) :
//       Selection(name, operator_str_andor)
//       {}
//         // constructor passing in everything needed: name, operator and selections
//     MultiSelection(const TString& name, const std::vector<Selection*>& selections, const TString& operator_str_andor) :
//       Selection(name, operator_str_andor),
//       m_selections(selections)
//       {}
//     virtual ~MultiSelection() {
//       // call delete on Selection's in vector? TODO: decide on ownership. Or better yet, use smart pointers
//     }
//   public:
//   void addSelection(Selection* selection) {
//     m_selections.push_back(selection);
//   }
//   virtual bool pass(const xAOD::IParticle* ipart) const {
//     bool passed = false;
//     switch (m_operator) {
//       case Operator::AND:
//         for (const auto& sel : m_selections) {
//           passed &= sel->pass(ipart);
//         }
//         break;
//       case Operator::OR:
//         for (const auto& sel : m_selections) {
//           passed |= sel->pass(ipart);
//         }
//         break;
//       default:
//         throw std::runtime_error("ERROR in MultiSelection :: unrecognised operator! Must be AND or OR");
//     }
//     return passed;
//   }
//   virtual void show() const {
//     std::cout << "*** MultiSelection " << m_name << ", operator " << m_operator << std::endl;
//     std::cout << "*** member selections:  " << std::endl;
//     for (const auto sel : m_selections ) {
//       sel->show();
//     }
//     std::cout << "****" << std::endl;
//   }
// private:
//   std::vector<Selection*> m_selections;
// public:
//   // ClassDef(MultiSelection, 1);
// };







// // check if pt is within window
// class PtSelection : public Selection {
//
//
//   public:
//     // default constructor
//     PtSelection() : m_ptcut_low(0.), m_ptcut_up(0.) {}
//
//     // nominal constructor
//     PtSelection(const TString& name, const float ptcut_low, const float ptcut_up) :
//       Selection(name, TString("TRUE")), // dummy operator string
//       m_ptcut_low(ptcut_low),
//       m_ptcut_up(ptcut_up) {}
//     virtual ~PtSelection() {}
//
//   private:
//     const float m_ptcut_low;
//     const float m_ptcut_up;
//
//   public:
//     virtual bool pass(const xAOD::IParticle* ipart) const {
//       return ipart->pt() > m_ptcut_low && ipart->pt() <= m_ptcut_up;
//       // switch (m_operator) {
//       //   case Operator::LESSTHAN:
//       //     return ipart->pt() < m_ptcut;
//       //   case Operator::LESSTHANEQUALTO:
//       //     return ipart->pt() <= m_ptcut;
//       //   case Operator::LARGERTHAN:
//       //     return ipart->pt() > m_ptcut;
//       //   case Operator::LARGERTHANEQUALTO:
//       //     return ipart->pt() >= m_ptcut;
//       //   default:
//       //     throw std::runtime_error("ERROR in HWWSelection::PtSelection :: expected operator of type larger-than or less-than. Check your call");
//       //
//       // }
//     }
//     virtual void show() const {
//       std::cout << "*** PtSelection " << m_name << std::endl;
//       std::cout << "*************** cutvalue (low) = " << m_ptcut_low << " MeV, cutvalue (up) = " << m_ptcut_up << " MeV" << std::endl;
//     }
//   public:
//   // ClassDef(PtSelection, 1);
// };
