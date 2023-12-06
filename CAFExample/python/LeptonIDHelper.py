from QFramework import *
from ROOT import *
import sys
import array
''' initialize and configure HWWLeptonIDHelper,
    so that it can be imported and used by other observables in a common way

    NOTE: this is not an observable, and should not be used as such (i.e. don't
          add it in your runAnalysis config '''


def getListOfTStrings(list_of_python_strings):
  return [ROOT.TString(el) for el in list_of_python_strings]

def vectorize(type, python_list):
  # make an std::vector out of a python list, return it. needed to call c++ functions taking vectors as arguments
  vec = vector(type)()
  if type == "TString*":
    for el in getListOfTStrings(python_list): vec.push_back(el)
  else:
    for el in python_list: vec.push_back(el)
  return vec

class Selections:
  def __init__(self, name):
    self.name = name
    self.electron_id = {}
    self.electron_antiid = {}
    self.muon_id = {}
    self.muon_antiid = {}

# lepton selections
''' syntax explanation for the selection dictionaries:
       {"<descriptive name of selection> (no meaning, but should be unique since python dictionary)":
           ["<auxvar type>", # e.g. 'float'
            "<auxvar name>", # e.g. 'd0sig', you can also do absolute value '|d0sig|', a product 'z0*sinTheta' or normalise to pt 'ptvarcone40/pt'
            "<cutvalue>", # e.g. 6. for d0sig
            "<operator>", # e.g. "<" for d0sig. Supported types are '>','>=','<','<=','=', '==' (same as '='), 'TRUE', 'FALSE' (the latter will enforce cast of auxvar to bool)
            "<isValidOnlyInPtSlice>", # OPTIONAL, bool, if true the cut will only be active in pt subtregion, as given by the two coming cuts
            "<ptcut_low>", # OPTIONAL, float, lower ptcut in MeV
            "<ptcut_up>" # OPTIONAL, float, upper ptcut in MeV
            ]
        }
'''
#=========================================
#========== 2L selections
#=========================================
selections_2L = Selections("2L")

# electron id
selections_2L.electron_id = {
               #"overlaps"                : ["char", "overlaps", 0, "FALSE"],
               "author"                  : ["unsigned short", "author", 1, "=="],
               "d0"                      : ["float", "|d0sig|", 5., "<"],
               "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt"         : ["char",  "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt"        : ["char",  "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               "isolation"               : ["char", "passIsoFCTight", 1, "TRUE"]
               }



# electron anti-id
selections_2L.electron_antiid = {
                   #"overlaps"   : ["char", "overlaps", 0, "FALSE"],
                   "author"     : ["unsigned short", "author", 1, "=="],
                   "d0"         : ["float", "|d0sig|", 5., "<"],
                   "z0"         : ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood" : ["char",  "isLHLoose", 1, "TRUE"]
                   }

# muon id
selections_2L.muon_id = {
           #"overlaps"                : ["char", "overlaps", 0, "FALSE"],
           "qualityLowPt"            : ["int",   "Quality", 0, "=", True, 0., 1e9],
           "d0"                      : ["float", "|d0sig|",   3., "<"],
           "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolation"               : ["char", "passIsoFCTight", 1, "TRUE"]
           }

selections_2L.muon_antiid = {
           #"overlaps"      : ["char", "overlaps", 0, "FALSE"],
           "quality"       : ["int",   "Quality", 1, "<=", True, 0., 1e9],
           "d0"            : ["float", "|d0sig|",   15., "<"],
           "z0"            : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#========= END 2L selections

#=========================================
#========== 2L selections
#=========================================
selections_2L_allAuthor = Selections("2L_allAuthor")

# electron id
selections_2L_allAuthor.electron_id = {
               #"overlaps"                : ["char", "overlaps", 0, "FALSE"],
               #"author"                  : ["unsigned short", "author", 1, "=="],
               "d0"                      : ["float", "|d0sig|", 5., "<"],
               "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt"         : ["char",  "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt"        : ["char",  "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               "isolationPtvarconeLowPt" : ["float", "ptvarcone40/pt", 0.06, "<", True, 0., 25000.],
               "isolationtopoEtconeLowPt": ["float", "topoetcone20/pt", 0.11, "<", True, 0., 25000.],
               "isolationHighPt"         : ["char",  "passIsoGradient", 1, "TRUE", True, 25000., 1e9]
               }



# electron anti-id
selections_2L_allAuthor.electron_antiid = {
                   #"overlaps"   : ["char", "overlaps", 0, "FALSE"],
                   #"author"     : ["unsigned short", "author", 1, "=="],
                   "d0"         : ["float", "|d0sig|", 5., "<"],
                   "z0"         : ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood" : ["char",  "isLHLoose", 1, "TRUE"]
                   }

# muon id
selections_2L_allAuthor.muon_id = {
           #"overlaps"                : ["char", "overlaps", 0, "FALSE"],
           "qualityLowPt"            : ["int",   "Quality", 0, "=", True, 0., 1e9],
           "d0"                      : ["float", "|d0sig|",   3., "<"],
           "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolationPtvarcone"      : ["float", "ptvarcone30/pt",  0.06, "<",    True, 0., 1e9],
           "isolationtopoEtcone"     : ["float", "topoetcone20/pt", 0.09, "<",    True, 0., 1e9]
           }

selections_2L_allAuthor.muon_antiid = {
           #"overlaps"      : ["char", "overlaps", 0, "FALSE"],
           "quality"       : ["int",   "Quality", 1, "<=", True, 0., 1e9],
           "d0"            : ["float", "|d0sig|",   15., "<"],
           "z0"            : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#========= END 2L selections




#=========================================
#========== 2L selections with anti-id electron LHVeryLoose+BLayer
#=========================================
selections_2L_VeryLooseBLayer = Selections("2LVeryLooseBLayer")

# electron id
selections_2L_VeryLooseBLayer.electron_id = {
               #"overlaps"                : ["char", "overlaps", 0, "FALSE"],
               "author"                  : ["unsigned short", "author", 1, "=="],
               "d0"                      : ["float", "|d0sig|", 5., "<"],
               "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt"         : ["char",  "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt"        : ["char",  "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               "isolationPtvarconeLowPt" : ["float", "ptvarcone40/pt", 0.06, "<", True, 0., 25000.],
               "isolationtopoEtconeLowPt": ["float", "topoetcone20/pt", 0.11, "<", True, 0., 25000.],
               "isolationHighPt"         : ["char",  "passIsoGradient", 1, "TRUE", True, 25000., 1e9]
               }

# electron anti-id
selections_2L_VeryLooseBLayer.electron_antiid = {
                   #"overlaps"   : ["char", "overlaps", 0, "FALSE"],
                   "author"     : ["unsigned short", "author", 1, "=="],
                   "d0"         : ["float", "|d0sig|", 5., "<"],
                   "z0"         : ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood" : ["char",  "isLHVeryLoose", 1, "TRUE"],
                   "blayer"     : ["char",  "passBLayerRequirement", 1, "TRUE"]
                   }
# muon id
selections_2L_VeryLooseBLayer.muon_id = {
           #"overlaps"                : ["char", "overlaps", 0, "FALSE"],
           "qualityLowPt"            : ["int",   "Quality", 0, "=", True, 0., 1e9],
           "d0"                      : ["float", "|d0sig|",   3., "<"],
           "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolationPtvarcone"      : ["float", "ptvarcone30/pt",  0.06, "<",    True, 0., 1e9],
           "isolationtopoEtcone"     : ["float", "topoetcone20/pt", 0.09, "<",    True, 0., 1e9]
           }

selections_2L_VeryLooseBLayer.muon_antiid = {
           #"overlaps"      : ["char", "overlaps", 0, "FALSE"],
           "quality"       : ["int",   "Quality", 1, "<=", True, 0., 1e9],
           "d0"            : ["float", "|d0sig|",   15., "<"],
           "z0"            : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#========= END 2L VeryLooseBLayer selections


#=========================================
#========== 2L selections with anti-id electron LHVeryLoose+BLayer, and noMuOR modifications
#=========================================
selections_2L_VeryLooseBLayerNoMuOR = Selections("2LVeryLooseBLayerNoMuOR")

# electron id
selections_2L_VeryLooseBLayerNoMuOR.electron_id = {
               "overlaps"                : ["char", "overlaps", 0, "FALSE"],
               "author"                  : ["unsigned short", "author", 1, "=="],
               "d0"                      : ["float", "|d0sig|", 5., "<"],
               "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt"         : ["char", "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt"        : ["char", "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               "isolationPtvarconeLowPt" : ["float", "ptvarcone40/pt", 0.06, "<", True, 0., 25000.],
               "isolationtopoEtconeLowPt": ["float", "topoetcone20/pt", 0.11, "<", True, 0., 25000.],
               "isolationHighPt"         : ["char",  "passIsoGradient", 1, "TRUE", True, 25000., 1e9]
               }

# electron anti-id
selections_2L_VeryLooseBLayerNoMuOR.electron_antiid = {
                   "overlaps"   : ["char", "overlaps", 0, "FALSE"],
                   "author"     : ["unsigned short", "author", 1, "=="],
                   "d0"         : ["float", "|d0sig|", 5., "<"],
                   "z0"         : ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood" : ["char",  "isLHVeryLoose", 1, "TRUE"],
                   "blayer"     : ["char",  "passBLayerRequirement", 1, "TRUE"]
                   }
# muon id
selections_2L_VeryLooseBLayerNoMuOR.muon_id = {
           "overlap"                                    : ["char",  "overlaps", 0, "FALSE"],
           "qualityLowPt"                               : ["int",   "Quality", 0, "=", True, 0., 1e9],
           "d0"                                         : ["float", "|d0sig|",   3., "<"],
           "z0"                                         : ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolationPtvarcone"                         : ["float", "ptvarcone30/pt",  0.06, "<",    True, 0., 1e9],
           "isolationtopoEtcone"                        : ["float", "topoetcone20/pt", 0.09, "<",    True, 0., 1e9]
           }

selections_2L_VeryLooseBLayerNoMuOR.muon_antiid = {
           #"overlap"                                    : ["char",  "overlaps", 0, "FALSE"],
           "overlapsNoMuNearJetRemoval"                 : ["char", "overlapsNoMuNearJetRemoval", 0, "FALSE"],
           #"overlapsNoMuNearJetRemovalNoBJetPrecedence" : ["char", "overlapsNoMuNearJetRemovalNoBJetPrecedence", 0, "FALSE"],
           "quality"                                    : ["int", "Quality", 1, "<=", True, 0., 1e9],
           "d0"                                         : ["float", "|d0sig|",   15., "<"],
           "z0"                                         : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#========= END 2L VeryLooseBLayerNoMuOR selections

#=========================================
#========== WH selections
#=========================================
selections_WH = Selections("WH")

# electron id
selections_WH.electron_id = {
               "author" : ["unsigned short", "author", 1, "=="],
               "d0": ["float", "|d0sig|", 5., "<"],
               "z0": ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt": [ "char", "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt": [ "char", "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               #"isolationGradient": ["char", "passIsoGradient", 1, "TRUE"]
               "isolationEle": ["char", "passIsoFCLoose", 1, "TRUE"]
               }

# electron anti-id
selections_WH.electron_antiid = {
                   "author" : ["unsigned short", "author", 1, "=="],
                   "d0": ["float", "|d0sig|", 5., "<"],
                   "z0": ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood": ["char", "isLHLoose", 1, "TRUE"]
                   }
# muon id
selections_WH.muon_id = {
           "qualityLowPt": ["int", "Quality", 1, "<=", True, 0., 25000.],
           "qualityHighPt": ["int", "Quality", 1, "<=", True, 25000., 1e9],
           "d0":            ["float", "|d0sig|",   3., "<"],
           "z0":            ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolationMuon":     ["char", "passIsoFCLoose", 1, "TRUE"]
           }

selections_WH.muon_antiid = {
           "qualityLowPt": ["int", "Quality", 1, "<=", True, 0., 25000.],
           "qualityHighPt": ["int", "Quality", 1, "<=", True, 25000., 1e9],
           "d0":            ["float", "|d0sig|",   6., "<"],
           "z0":            ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#============ END WH selections

#=========================================
#========== ZH selections
#=========================================
selections_ZH = Selections("ZH")

# electron id
selections_ZH.electron_id = {
               "author" : ["unsigned short", "author", 1, "=="],
               "d0": ["float", "|d0sig|", 5., "<"],
               "z0": ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt": [ "char", "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt": [ "char", "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               #"isolationGradient": ["char", "passIsoGradient", 1, "TRUE"]
               "isolationEle": ["char", "passIsoFCLoose", 1, "TRUE"]
               }

# electron anti-id
selections_ZH.electron_antiid = {
                   "author" : ["unsigned short", "author", 1, "=="],
                   "d0": ["float", "|d0sig|", 5., "<"],
                   "z0": ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood": ["char", "isLHVeryLoose", 1, "TRUE"]
                   }
# muon id
selections_ZH.muon_id = {
           "qualityLowPt": ["int", "Quality", 1, "<=", True, 0., 25000.], # "Medium" muons
           "qualityHighPt": ["int", "Quality", 1, "<=", True, 25000., 1e9],
           "d0":            ["float", "|d0sig|",   3., "<"],
           "z0":            ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolationMuon":     ["char", "passIsoFCLoose", 1, "TRUE"]
           ###"isolationMuon": ["char", "passIsoFixedCutTightTrackOnly", 1, "TRUE"]
           }

selections_ZH.muon_antiid = {
           "qualityLowPt": ["int", "Quality", 0, "<=", True, 0., 25000.], # "Loose" muons
           "qualityHighPt": ["int", "Quality", 0, "<=", True, 25000., 1e9],
           #"d0":            ["float", "|d0sig|",   6., "<"],
           "d0":            ["float", "|d0sig|",   15., "<"],
           "z0":            ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#============ END ZH selections


#==============================================================================
#========== Z boson candidate selections for the Zjets fake-lepton estimate
#==============================================================================
selections_ZCand = Selections("ZCand")

# electron id
selections_ZCand.electron_id = {
               "author"       : ["unsigned short", "author", 1, "=="],
               #"isolationEle" : ["char", "passIsoFixedCutTightTrackOnly", 1, "TRUE"],
               #"likelihood"   : ["char", "isLHLoose", 1, "TRUE"],
               "likelihood"   : ["char", "isLHMedium", 1, "TRUE"],
               "d0"           : ["float", "|d0sig|", 5., "<"],
               "z0"           : ["float", "|z0*sinTheta|", 0.5, "<"]
               }

# electron anti-id
selections_ZCand.electron_antiid = {
                   "author"     : ["unsigned short", "author", 1, "=="],
                   "d0"         : ["float", "|d0sig|", 5., "<"],
                   "z0"         : ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood" : ["char", "isLHLoose", 1, "TRUE"]
                   #"likelihood" : ["char",  "isLHVeryLoose", 1, "TRUE"]
                   }
# muon id
selections_ZCand.muon_id = {
           "quality"	   : ["int", "Quality", 1, "<="], # "Medium" muons
           #"isolation"     : ["char", "passIsoGradient", 1, "TRUE"],
           #"isolationMuon" : ["char", "passIsoFixedCutTightTrackOnly", 1, "TRUE"],
           "d0"            : ["float", "|d0sig|",   3., "<"],
           "z0"            : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

selections_ZCand.muon_antiid = {
           #"quality"	   : ["int", "Quality", 2, "<="], # "Loose" muons
           "quality"	   : ["int", "Quality", 1, "<="], # "Medium" muons
           #"qualityLowPt"  : ["int", "Quality", 1, "<=", True, 0., 25000.],
           #"qualityHighPt" : ["int", "Quality", 1, "<=", True, 25000., 1e9],
           "d0"            : ["float", "|d0sig|",   6., "<"],
           "z0"            : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#============ END ZCand selections ============================================


#=========================================
#========== VgammaCR selections (electrons from photon conversions)
#=========================================
selections_VgammaCR = Selections("VgammaCR")

# electron id
selections_VgammaCR.electron_id = {
               "author"                   : ["unsigned short", "author", 16, "=="],
               "d0"                       : ["float", "|d0sig|", 5., "<"],
               "z0"                       : ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt"          : ["char", "isLHTightNoBLayer", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt"         : ["char", "isLHMediumNoBLayer", 1, "TRUE", True, 25000., 1e9],
               "BLayerRequirement"        : ["char", "passBLayerRequirement", 0, "FALSE", False, 0., 1e9],
               "isolationPtvarconeLowPt"  : ["float", "ptvarcone40/pt", 0.06, "<", True, 0., 25000.],
               "isolationtopoEtconeLowPt" : ["float", "topoetcone20/pt", 0.11, "<", True, 0., 25000.],
               "isolationHighPt"          : ["char",  "passIsoGradient", 1, "TRUE", True, 25000., 1e9]
               }

# electron anti-id
selections_VgammaCR.electron_antiid = {
                   "author"            : ["unsigned short", "author", 16, "=="],
                   "d0"                : ["float", "|d0sig|", 5., "<"],
                   "z0"                : ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood"        : ["char",  "isLHLoose", 1, "TRUE"],
                   "BLayerRequirement" : ["char", "passBLayerRequirement", 0, "FALSE", False, 0., 1e9]
                   }

# muon id
selections_VgammaCR.muon_id = {
           "qualityLowPt"            : ["int",   "Quality", 0, "=", True, 0., 1e9],
           "d0"                      : ["float", "|d0sig|",   3., "<"],
           "z0"                      : ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolationPtvarcone"      : ["float", "ptvarcone30/pt",  0.06, "<",    True, 0., 1e9],
           "isolationtopoEtcone"     : ["float", "topoetcone20/pt", 0.09, "<",    True, 0., 1e9]
           }

selections_VgammaCR.muon_antiid = {
           "quality"	   : ["int", "Quality", 1, "<=", True, 0., 1e9],
           "d0"            : ["float", "|d0sig|",   15., "<"],
           "z0"            : ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#========= END VgammaCR selections

#========== define your own selections here, if you wish (and remember to add to the dictionary below)
# selections_myFancySel = Selections("myFancySelections")
# selections_myFancySel.electron_id = {
#                                      ...
#                                      }
# ...

#=========================================
#========== High mass selections
#=========================================
selections_HighMass = Selections("HighMass")

# electron id
selections_HighMass.electron_id = {
               "d0": ["float", "|d0sig|", 5., "<"],
               "z0": ["float", "|z0*sinTheta|", 0.5, "<"],
               "likelihoodLowPt": [ "char", "isLHTight", 1, "TRUE", True, 0., 25000.],
               "likelihoodHighPt": [ "char", "isLHMedium", 1, "TRUE", True, 25000., 1e9],
               "isolationGradient": ["char", "passIsoGradient", 1, "TRUE"]
               }
# electron anti-id
selections_HighMass.electron_antiid = {
                   "d0": ["float", "|d0sig|", 5., "<"],
                   "z0": ["float", "|z0*sinTheta|", 0.5, "<"],
                   "likelihood": ["char", "isLHLoose", 1, "TRUE"]
                   }
# muon id
selections_HighMass.muon_id = {
           "qualityLowPt": ["int", "Quality", 0, "=", True, 0., 25000.],
           "qualityHighPt": ["int", "Quality", 1, "<=", True, 25000., 1e9],
           "d0":            ["float", "|d0sig|",   3., "<"],
           "z0":            ["float", "|z0*sinTheta|", 0.5, "<"],
           "isolation":     ["char", "passIsoGradient", 1, "TRUE"]
           }
selections_HighMass.muon_antiid = {
           "qualityLowPt": ["int", "Quality", 0, "=", True, 0., 25000.],
           "qualityHighPt": ["int", "Quality", 1, "<=", True, 25000., 1e9],
           "d0":            ["float", "|d0sig|",   6., "<"],
           "z0":            ["float", "|z0*sinTheta|", 0.5, "<"]
           }

#============ END HighMass selections


all_selections = {}
# add selections to the dictionary
all_selections[selections_2L.name] = selections_2L # 2L
all_selections[selections_2L_allAuthor.name] = selections_2L_allAuthor # 2L without author requirement
all_selections[selections_WH.name] = selections_WH # WH
all_selections[selections_ZH.name] = selections_ZH # ZH
all_selections[selections_ZCand.name] = selections_ZCand # ZCand
all_selections[selections_HighMass.name] = selections_HighMass # HighMass
all_selections[selections_VgammaCR.name] = selections_VgammaCR # Selection of electrons converted from photons
all_selections[selections_2L_VeryLooseBLayer.name] = selections_2L_VeryLooseBLayer # 2L with VeryLoose+BLayerHitRequirement for anti-id electrons
all_selections[selections_2L_VeryLooseBLayerNoMuOR.name] = selections_2L_VeryLooseBLayerNoMuOR


#====== add your own fancy selections
# all_selections[selections_myFancySel.name] = selections_myFancySel


def setElectronID(lepIDHelper, selection_key):
  electron_id_cuts = all_selections[selection_key].electron_id
  for var in electron_id_cuts:
    #print("HWWLeptonIDHelper :: adding {} cut to id electrons...".format(var))
    list_of_req = electron_id_cuts[var]
    type_str = list_of_req[0]
    auxvar = list_of_req[1]
    cutvalue = list_of_req[2]
    operator_str = list_of_req[3]
    if len(list_of_req) == 4:
      # valid-everywhere cut
      lepIDHelper.addCut_ElectronID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str))
    elif len(list_of_req) == 7:
      # pt-dependent cut
      ptcut_low = list_of_req[5]
      ptcut_up = list_of_req[6]
      lepIDHelper.addCut_ElectronID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str), True, ptcut_low, ptcut_up)
    else:
      BREAK("ERROR in HWWLepIDHelper :: requirement list should be either of length 4 or length 7. Check your id-electron selection lists")

  #print("DONE adding electron id cuts.")

def setElectronAntiID(lepIDHelper, selection_key):
  electron_antiid_cuts = all_selections[selection_key].electron_antiid
  for var in electron_antiid_cuts:
    #print("HWWLeptonIDHelper :: adding {} cut to anti-id electrons...".format(var))
    list_of_req = electron_antiid_cuts[var]
    type_str = list_of_req[0]
    auxvar = list_of_req[1]
    cutvalue = list_of_req[2]
    operator_str = list_of_req[3]
    if len(list_of_req) == 4:
      # valid-everywhere cut
      lepIDHelper.addCut_ElectronAntiID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str))
    elif len(list_of_req) == 7:
      # pt-dependent cut
      ptcut_low = list_of_req[5]
      ptcut_up = list_of_req[6]
      lepIDHelper.addCut_ElectronAntiID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str), True, ptcut_low, ptcut_up)
    else:
      BREAK("ERROR in HWWLepIDHelper :: requirement list should be either of length 4 or length 7. Check your antiid-electron selection lists")

  #print("DONE adding electron anti-id cuts.")

def setMuonID(lepIDHelper, selection_key):
  muon_id_cuts = all_selections[selection_key].muon_id
  for var in muon_id_cuts:
    #print("HWWLeptonIDHelper :: adding {} cut to id muons...".format(var))
    list_of_req = muon_id_cuts[var]
    type_str = list_of_req[0]
    auxvar = list_of_req[1]
    cutvalue = list_of_req[2]
    operator_str = list_of_req[3]
    if len(list_of_req) == 4:
      # valid-everywhere cut
      lepIDHelper.addCut_MuonID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str))
    elif len(list_of_req) == 7:
      # pt-dependent cut
      ptcut_low = list_of_req[5]
      ptcut_up = list_of_req[6]
      lepIDHelper.addCut_MuonID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str), True, ptcut_low, ptcut_up)
    else:
      BREAK("ERROR in HWWLepIDHelper :: requirement list should be either of length 4 or length 7. Check your id-muon selection lists")

  #print("DONE adding muon id cuts.")

def setMuonAntiID(lepIDHelper, selection_key):
  muon_antiid_cuts = all_selections[selection_key].muon_antiid
  for var in muon_antiid_cuts:
    #print("HWWLeptonIDHelper :: adding {} cut to anti-id muons...".format(var))
    list_of_req = muon_antiid_cuts[var]
    type_str = list_of_req[0]
    auxvar = list_of_req[1]
    cutvalue = list_of_req[2]
    operator_str = list_of_req[3]
    if len(list_of_req) == 4:
      # valid-everywhere cut
      lepIDHelper.addCut_MuonAntiID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str))
    elif len(list_of_req) == 7:
      # pt-dependent cut
      ptcut_low = list_of_req[5]
      ptcut_up = list_of_req[6]
      lepIDHelper.addCut_MuonAntiID(TString(var), TString(type_str), TString(auxvar), cutvalue, TString(operator_str), True, ptcut_low, ptcut_up)
    else:
      BREAK("ERROR in HWWLepIDHelper :: requirement list should be either of length 4 or length 7. Check your antiid-muon selection lists")

  #print("DONE adding muon anti-id cuts.")

def getLepIDHelper(selection_key):
  # return a pointer to a HWWLeptonIDHelper object
  # other observables should call this class and pass
  # the returned object through their constructor

  from CAFExample import HWWLeptonIDHelper

  lepIDHelper = HWWLeptonIDHelper()

  selection_key = str(selection_key)
  # check that given key actually exists
  if selection_key not in all_selections:
    BREAK("Inside HWWLepIDHelper :: Given key {} doesn't exist! Available options are {}".format(selection_key, all_selections.keys()))

  # configure it
  # .. electron
  setElectronID(lepIDHelper, selection_key)
  setElectronAntiID(lepIDHelper, selection_key)
  # ... muon
  setMuonID(lepIDHelper, selection_key)
  setMuonAntiID(lepIDHelper, selection_key)

  if not lepIDHelper.allOK():
    print("ERROR initializing HWWLeptonIDHelper!")
    sys.exit(0)

  # here, we have to do a HACK:
  # make a vector of HWWLeptonIDHelper objects, put  the one object in,
  # return the first and only object in the vector. This somehow prevents
  # this object from being removed from memory (by the python garbage collector?)
  lepIDHelperVec = vectorize("HWW::HWWLeptonIDHelper*",[lepIDHelper])

  return lepIDHelperVec[0]

# add the usual addObservables() function to be able to give a meaningful error
# message when wrongly trying to include this helper as an observable
def addObservables():
  ERROR("HWWLepIDHelper is a helper, not an observable. You should not add it in your runAnalysis config file.")
  return False

if __name__ == "__main__":
  print "Inside main of HWWLeptonIDHelper snippet"
  foo = getLepIDHelper()
