#include "CAFExample/HWWSelection.h"

// this is an xAOD helper class

// ClassImp(Selection)
// ClassImp(PtSelection)
// ClassImpT(AuxDataSelection, 1)
// ClassImp(MultiSelection)

std::ostream& operator<<(std::ostream & os, const Operator& op) {
  switch(op) {
    case Operator::LESSTHAN:
      os << "LESSTHAN";
      break;
    case Operator::LARGERTHAN:
      os << "LARGERTHAN";
      break;
    case Operator::EQUALTO:
      os << "EQUALTO";
      break;
    case Operator::TRUE:
      os << "TRUE";
      break;
    case Operator::FALSE:
      os << "FALSE";
      break;
    case Operator::LESSTHANEQUALTO:
      os << "LESSTHANEQUALTO";
      break;
    case Operator::LARGERTHANEQUALTO:
      os << "LARGERTHANEQUALTO";
      break;
    default:
      os << "UNDEFINED";
  }
  return os;
}
