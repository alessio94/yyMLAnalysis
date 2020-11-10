from QFramework import TQTreeObservable, INFO, ERROR
import ROOT

class HWWModeOfRunning:
  LeadLepID        = 0
  SubleadLepID     = 1
  ThirdLepID       = 2
  FourthLepID      = 3
  OtherLepID       = 4
  LeadLepAntiID    = 5
  SubleadLepAntiID = 6
  ThirdLepAntiID   = 7
  FourthLepAntiID  = 8
  OtherLepAntiID   = 9

class HWWAnalysisType:
  TwoLep  = 0
  WH      = 1
  ZH      = 2

def vectorize(type, python_list):
  # make an std::vector out of a python list, return it. needed to call c++ functions taking vectors as arguments
  vec = vector(type)()
  for el in python_list: vec.push_back(el)
  return vec

def addObservables(config):
  from QFramework import TQObservable,INFO,ERROR,BREAK,WARN

  #======================
  #===== import lepton ID helper snippet, wil pass object to observable
  try:
    from CAFExample import LeptonIDHelper
    from CAFExample import HWWLeptonIDObservable
  except Exception as error:
    ERROR("Couldn't import LeptonIDHelper module!\n"+"Message from python:\n"+str(error))
    return False

  lepIDAnalysisType = config.getTagStringDefault("lepIDAnalysisType", "2L")
  if lepIDAnalysisType == '2L':
      lepIDAnalysisType = HWWAnalysisType.TwoLep
  elif lepIDAnalysisType == 'WH':
      lepIDAnalysisType = HWWAnalysisType.WH
  elif lepIDAnalysisType == 'ZH':
      lepIDAnalysisType = HWWAnalysisType.ZH
  else:
      ERROR('In HWWLeptonIDObservable.addObservables(...) : unknown analysis type: ' + lepIDAnalysisType)
      return False

  lepIDSelectionTag = config.getTagStringDefault("lepIDSelectionTag", "2L")
  lepIDHelper = LeptonIDHelper.getLepIDHelper(lepIDSelectionTag)
  INFO("in HWWLeptonIDObservable: Using lepton ID selection '{}' (see CAFExample/python/LeptonIDHelper.py for details)".format(lepIDSelectionTag))

  INFO("adding HWWLeptonIDObservable")
  #=== The integers must match the correct modes inside HWWLeptonIDObservable.cxx
  leadLepID = HWWLeptonIDObservable("leadLepID", lepIDHelper, HWWModeOfRunning.LeadLepID, lepIDAnalysisType)
  if not TQObservable.addObservable(leadLepID): return False
  subleadLepID = HWWLeptonIDObservable("subleadLepID", lepIDHelper, HWWModeOfRunning.SubleadLepID, lepIDAnalysisType)
  if not TQObservable.addObservable(subleadLepID): return False
  # otherLepton(index_default), default should be = 0
  otherLep0ID = HWWLeptonIDObservable("otherLep0ID", lepIDHelper, HWWModeOfRunning.OtherLepID, lepIDAnalysisType)
  if not TQObservable.addObservable(otherLep0ID): return False
  # MB: This the otherLep1ID running mode does not appear to be supported in the .cxx file, so I would not uncomment the below
  # otherLep1ID   = HWWLeptonIDObservable("otherLep1ID", lepIDHelper, 2, 0, 1, 1) # otherLepton(1)
  # if not TQObservable.addObservable(otherLep1ID): return False
  thirdLepID = HWWLeptonIDObservable("thirdLepID", lepIDHelper, HWWModeOfRunning.ThirdLepID, lepIDAnalysisType)
  if not TQObservable.addObservable(thirdLepID): return False
  fourthLepID = HWWLeptonIDObservable("fourthLepID", lepIDHelper, HWWModeOfRunning.FourthLepID, lepIDAnalysisType)
  if not TQObservable.addObservable(fourthLepID): return False

  leadLepAntiID = HWWLeptonIDObservable("leadLepAntiID", lepIDHelper, HWWModeOfRunning.LeadLepAntiID, lepIDAnalysisType)
  if not TQObservable.addObservable(leadLepAntiID): return False
  subleadLepAntiID = HWWLeptonIDObservable("subleadLepAntiID", lepIDHelper, HWWModeOfRunning.SubleadLepAntiID, lepIDAnalysisType)
  if not TQObservable.addObservable(subleadLepAntiID): return False
  # otherLepton(index_default), default should be = 0
  otherLep0AntiID = HWWLeptonIDObservable("otherLep0AntiID", lepIDHelper, HWWModeOfRunning.OtherLepAntiID, lepIDAnalysisType)
  if not TQObservable.addObservable(otherLep0AntiID): return False
  # MB: This the otherLep1ID running mode does not appear to be supported in the .cxx file, so I would not uncomment the below
  # otherLep1AntiID   = HWWLeptonIDObservable("otherLep1AntiID", lepIDHelper, 5, 0, 1, 1) # otherLepton(1)
  # if not TQObservable.addObservable(otherLep1AntiID): return False
  thirdLepAntiID = HWWLeptonIDObservable("thirdLepAntiID", lepIDHelper, HWWModeOfRunning.ThirdLepAntiID, lepIDAnalysisType)
  if not TQObservable.addObservable(thirdLepAntiID): return False
  fourthLepAntiID = HWWLeptonIDObservable("fourthLepAntiID", lepIDHelper, HWWModeOfRunning.FourthLepAntiID, lepIDAnalysisType)
  if not TQObservable.addObservable(fourthLepAntiID): return False

  return True

if __name__ == "__main__":
  print "Inside main of HWWLeptonIDObservable python snippet"
  if not addObservables():
    print("Couldn't add HWWLeptonIDObservables!")
  else:
    print("Successfully added HWWLeptonIDObservables")
