import ROOT
import sys

from CAFExample import HWWTrigConfig

def getListOfTStrings(list_of_python_strings):
  return [ROOT.TString(el) for el in list_of_python_strings]

def vectorize(type, python_list):
  # make an std::vector out of a python list, return it. needed to call c++ functions taking vectors as arguments
  vec = ROOT.std.vector(type)()
  if type == "TString*":
    for el in getListOfTStrings(python_list): vec.push_back(el)
  else:
    for el in python_list: vec.push_back(el)
  return vec

''' trigger config settings

    configure trigger-related settings, so that other observables
    can import and use them, such as HWWTriggerSelection and HWWTriggerMatchingObs.
    These observables takes a vector of HWWTrigConfig objects in their constructor,
    each object corresponding to a specific data period. In this snippet, you can
    create your own list of triggers and implement a function which returns this
    list in vector format. Then import this module and call this function from wherever 
    you are initialising your trigger selection/matching observables.

    The HWWTrigConfig object assumes there are three types of triggers: 
    single electron, single muon and di-lepton, each for both data and MC
    (in case you use different triggers for data and MC)

    NOTE: this is not an observable, and should not be used as such (i.e. don't 
          add it in your runAnalysis config '''

# period identifiers
g_periods = ["2015", "2016", "2017", "2018"]

# corresponding run numbers
runNrLow = {"2015": 276261, "2016": 297729, "2017": 325713, "2018": 348885}
runNrUp = {"2015": 284485, "2016": 320000, "2017": 340453, "2018": 364292}

# the list of analysis triggers
# 2015
single_el_2015 = ["HLT_e24_lhmedium_L1EM20VH", "HLT_e60_lhmedium", "HLT_e120_lhloose"]
single_mu_2015 = ["HLT_mu20_iloose_L1MU15", "HLT_mu50"]
dilep_2015 = ["HLT_e17_lhloose_mu14"]
# 2016
single_el_2016 = ["HLT_e26_lhtight_nod0_ivarloose", "HLT_e60_lhmedium_nod0", "HLT_e140_lhloose_nod0"]
single_mu_2016 = ["HLT_mu26_ivarmedium", "HLT_mu50"]
dilep_2016 = ["HLT_e17_lhloose_nod0_mu14"]
# 2017 
single_el_2017 = ["HLT_e26_lhtight_nod0_ivarloose", "HLT_e60_lhmedium_nod0", "HLT_e140_lhloose_nod0"]
single_mu_2017 = ["HLT_mu26_ivarmedium", "HLT_mu50"]
dilep_2017 = ["HLT_e17_lhloose_nod0_mu14"]
# 2018 
single_el_2018 = ["HLT_e26_lhtight_nod0_ivarloose", "HLT_e60_lhmedium_nod0", "HLT_e140_lhloose_nod0"]
single_mu_2018 = ["HLT_mu26_ivarmedium", "HLT_mu50"]
dilep_2018 = ["HLT_e17_lhloose_nod0_mu14"]

# dictionaries sorted by period, holding the corresponding trigger lists
single_el_data = {"2015": single_el_2015, "2016": single_el_2016, "2017": single_el_2017, "2018": single_el_2018}
single_el_mc   = single_el_data
single_mu_data = {"2015": single_mu_2015, "2016": single_mu_2016, "2017": single_mu_2017, "2018": single_mu_2018}
single_mu_mc   = single_mu_data
dilep_data     = {"2015": dilep_2015, "2016": dilep_2016, "2017": dilep_2017, "2018": dilep_2018}
dilep_mc       = dilep_data
# dilep dictionary with only the first of the two dilep triggers ("HLT_e17...")
dilep1_data     = {"2015": [dilep_2015[0]], "2016": [dilep_2016[0]], "2017": [dilep_2017[0]], "2018": [dilep_2018[0]]}
dilep1_mc       = dilep1_data

# corresponding pt cuts for trigger SFs (for combined OR trigger expressions). They seem to always be 1 GeV larger than lowest trigger threshold
ptcut_single_el = {"2015": 25000., "2016": 27000., "2017": 27000., "2018": 27000.}
ptcut_single_mu = {"2015": 20000.*1.05, "2016": 26000.*1.05, "2017": 26000.*1.05, "2018": 26000.*1.05}
ptcut_dilep_elleg = {"2015": 18000., "2016": 18000., "2017": 18000., "2018": 18000.}
# TODO: should be 1.05*<lowest trigger threshold>, but we won't loosen cuts already applied during skimming - FIX LATER
ptcut_dilep_muleg = {"2015": 15000., "2016": 15000., "2017": 15000., "2018": 15000.}
# ptcut's for dilep1 should be 18000. (TODO: check what trigger SFs actually exist and how they are configured. Should use this file too)
ptcut_dilep1_elleg = {"2015": 18000., "2016": 18000., "2017": 18000., "2018": 18000.}
ptcut_dilep1_muleg = ptcut_dilep_muleg

# list of prescaled triggers used for dijet FF measurement
single_el_prescale_2015 = ["HLT_e12_lhvloose_nod0_L1EM10VH","HLT_e20_lhvloose_nod0","HLT_e60_lhvloose_nod0"]
single_el_prescale_2016 = ["HLT_e12_lhvloose_nod0_L1EM10VH","HLT_e20_lhvloose_nod0","HLT_e60_lhvloose_nod0","HLT_e140_lhvloose_nod0"]
single_el_prescale_2017 = ["HLT_e12_lhvloose_nod0_L1EM10VH","HLT_e20_lhvloose_nod0","HLT_e60_lhvloose_nod0","HLT_e140_lhvloose_nod0"]
single_el_prescale_2018 = ["HLT_e12_lhvloose_nod0_L1EM10VH","HLT_e20_lhvloose_nod0","HLT_e60_lhvloose_nod0","HLT_e140_lhvloose_nod0"]
single_mu_prescale = ["HLT_mu14","HLT_mu24","HLT_mu50"]



single_el_prescale_2015_lhloose = ["HLT_e12_lhloose_L1EM10VH"]
single_el_prescale_2016_lhloose = ["HLT_e12_lhloose_nod0_L1EM10VH"]
single_el_prescale_2017_lhloose = ["HLT_e12_lhloose_nod0_L1EM10VH"]
single_el_prescale_2018_lhloose = ["HLT_e12_lhloose_nod0_L1EM10VH"]
ptcut_dilep_single_el = {"2015": 15000., "2016": 15000., "2017": 15000., "2018": 15000.}




# corresponding pt cuts for trigger unprescaling
ptcut_single_el_prescale = 15000.
ptcut_single_mu_prescale = 15000.

ptcut_single_el_prescale_lhloose = 15000.

single_el_prescale_data = {"2015":single_el_prescale_2015, "2016":single_el_prescale_2016, "2017":single_el_prescale_2017, "2018":single_el_prescale_2018}
single_el_prescale_mc = single_el_prescale_data
single_mu_prescale_data = {"2015": single_mu_prescale, "2016": single_mu_prescale, "2017": single_mu_prescale, "2018": single_mu_prescale}
single_mu_prescale_mc = single_mu_prescale_data
single_el_prescale_lhloose_data = {"2015":single_el_prescale_2015_lhloose, "2016":single_el_prescale_2016_lhloose, "2017":single_el_prescale_2017_lhloose, "2018":single_el_prescale_2018_lhloose}
single_el_prescale_lhloose_mc = single_el_prescale_lhloose_data



def checkPeriod(period):
  if period not in g_periods:
    print("Unrecognised period identifier! Existing periods are ", g_periods)
    sys.exit(0)

def setSingleEllhloosePrescaledTriggers(trigConf, period):
  ''' set the single electron triggers on the HWWTrigConf object '''
  checkPeriod(period)
  for t in single_el_prescale_lhloose_data[period]: trigConf.addElectronTriggerData(t)
  for t in single_el_prescale_lhloose_mc[period]: trigConf.addElectronTriggerMC(t)
  trigConf.setElectronPtCut(ptcut_single_el_prescale_lhloose)


def setSingleElTriggers(trigConf, period):
  ''' set the single electron triggers on the HWWTrigConf object '''

  checkPeriod(period)
  for t in single_el_data[period]: trigConf.addElectronTriggerData(t)
  for t in single_el_mc[period]: trigConf.addElectronTriggerMC(t)
  trigConf.setElectronPtCut(ptcut_single_el[period])

def setSingleMuTriggers(trigConf, period):
  ''' set the single muon triggers on the HWWTrigConf object '''
  checkPeriod(period)
  for t in single_mu_data[period]: trigConf.addMuonTriggerData(t)
  for t in single_mu_mc[period]: trigConf.addMuonTriggerMC(t)
  trigConf.setMuonPtCut(ptcut_single_mu[period])

def setDilepTriggers(trigConf, period):
  ''' set the dilepton triggers on the HWWTrigConf object '''
  checkPeriod(period)
  for t in dilep_data[period]: trigConf.addDileptonTriggerData(t)
  for t in dilep_mc[period]: trigConf.addDileptonTriggerMC(t)
  trigConf.setDileptonPtCut(ptcut_dilep_elleg[period], ptcut_dilep_muleg[period])

def setDilepTriggers1(trigConf, period):
  # like above fcn, but use only the first of the dilep triggers ("HLT_e17...")
  checkPeriod(period)
  for t in dilep1_data[period]: trigConf.addDileptonTriggerData(t)
  for t in dilep1_mc[period]: trigConf.addDileptonTriggerMC(t)
  trigConf.setDileptonPtCut(ptcut_dilep1_elleg[period], ptcut_dilep1_muleg[period])

def setSingleElPrescaledTriggers(trigConf, period):
  ''' set the single electron triggers on the HWWTrigConf object '''
  checkPeriod(period)
  for t in single_el_prescale_data[period]: trigConf.addElectronTriggerData(t)
  for t in single_el_prescale_mc[period]: trigConf.addElectronTriggerMC(t)
  trigConf.setElectronPtCut(ptcut_single_el_prescale)



def setSingleMuPrescaledTriggers(trigConf, period):
  ''' set the single muon triggers on the HWWTrigConf object '''
  checkPeriod(period)
  for t in single_mu_prescale_data[period]: trigConf.addMuonTriggerData(t)
  for t in single_mu_prescale_mc[period]: trigConf.addMuonTriggerMC(t)
  trigConf.setMuonPtCut(ptcut_single_mu_prescale)

def getTrigConf(period):
  ''' create a HWWTrigConf object, set its run numbers based on passed period identifier, return it '''
  checkPeriod(period)
  
  # create the HWWTrigConfig object
  trigConfig = HWWTrigConfig(period)
  
  # set period (run numbers)
  trigConfig.setRunNumbers(runNrLow[period], runNrUp[period])
  
  return trigConfig

def getTrigConfigList(periods = g_periods):
  # list of HWWTrigConfig objects, single lepton triggers only
  trigConfigList = []

  for period in periods: 
    trigConf = getTrigConf(period)
    setSingleElTriggers(trigConf, period)
    setSingleMuTriggers(trigConf, period)
    trigConfigList.append(trigConf)

  return trigConfigList

def getTrigConfigListDilep(periods = g_periods):
  # list of HWWTrigConfig objects, both single and dilepton triggers
  trigConfigList = []
  for period in periods: 
    trigConf = getTrigConf(period)
    setSingleElTriggers(trigConf, period)
    setSingleMuTriggers(trigConf, period)
    setDilepTriggers(trigConf, period)
    trigConfigList.append(trigConf)

  return trigConfigList

def getTrigConfigListDilep_el_leg(periods = g_periods):
  # list of HWWTrigConfig objects, both single and dilepton triggers
  trigConfigList = []
  for period in periods: 
    print("getTrigConfigListDilep_el_leg: period = ")
    print(period)
    trigConf = getTrigConf(period)
    setDileptonSingleElTriggers(trigConf, period)
    trigConfigList.append(trigConf)

  return trigConfigList


def getTrigConfigListDilep1(periods = g_periods):
  # list of HWWTrigConfig objects, single and one of the dilepton triggers ("HLT_e17...")
  trigConfigList = []
  for period in periods: 
    trigConf = getTrigConf(period)
    setSingleElTriggers(trigConf, period)
    setSingleMuTriggers(trigConf, period)
    setDilepTriggers1(trigConf, period)
    trigConfigList.append(trigConf)

  return trigConfigList

def getTrigConfigListDilep1__OnlyDilep(periods = g_periods):
  # list of HWWTrigConfig objects, with one of each of the dilepton triggers ("HLT_e17...")
  trigConfigList = []
  for period in periods: 
    trigConf = getTrigConf(period)
    setDilepTriggers1(trigConf, period)
    trigConfigList.append(trigConf)

  return trigConfigList

def getTrigPrescaleConfigList(periods = g_periods):
  # list of HWWTrigConfig objects for prescaled triggers used in dijet nominal FF measurement
  trigConfList = []
  for period in periods:
    trigConf = getTrigConf(period)
    setSingleElPrescaledTriggers(trigConf, period)
    setSingleMuPrescaledTriggers(trigConf, period)
    trigConfList.append(trigConf)
  return trigConfList

def getTriglhloosePrescaleConfigList(periods = g_periods):
  # list of HWWTrigConfig objects for prescaled triggers used in dijet nominal FF measurement
  trigConfList = []
  for period in periods:
    trigConf = getTrigConf(period)
    setSingleEllhloosePrescaledTriggers(trigConf, period)
    print("getTriglhloosePrescaleConfigList: period = ", period)
    print("getTriglhloosePrescaleConfigList: trigConf = ",trigConf)
    trigConfList.append(trigConf)
  return trigConfList

# add the usual addObservables() function to be able to give a meaningful error
# message when wrongly trying to include this helper as an observable
def addObservables():
  from QFramework import ERROR
  ERROR("HWWTrigConfig provides helper functions, it's not an observable. You should not add it in your runAnalysis config file.")
  return False

# vector of trigger configs, this is the one that will be used by other observables
def getTrigConfigs(period = ""):
  # if specified period, return only those triggers
  if period in g_periods:
    return getTrigConfigList([period])
  # else, return full list of triggers
  return getTrigConfigList()

# like above, but including dilep triggers
def getTrigConfigsDilep(period = ""):
  # if specified period, return only those triggers
  if period in g_periods:
    return getTrigConfigListDilep(period)
  # else, return full list of triggers
  return getTrigConfigListDilep()

# like above, but including only one of the dilep triggers ("HLT_e17_...")
def getTrigConfigsDilep1(period = ""):
  # if specified period, return only those triggers
  if period in g_periods:
    return getTrigConfigListDilep1(period)
  # else, return full list of triggers
  return getTrigConfigListDilep1()

# like above, but including only one of the dilep triggers ("HLT_e17_...") (and NO single lepton triggers)
def getTrigConfigsDilep1__OnlyDilep(period = ""):
  # if specified period, return only those triggers
  if period in g_periods:
    return getTrigConfigListDilep1__OnlyDilep(period)
  # else, return full list of triggers
  return getTrigConfigListDilep1__OnlyDilep()

def getTrigConfigsDilep_el_leg(period = ""):
  # if specified period, return only those triggers
  if period in g_periods:
    return getTrigConfigListDilep_el_leg([period])
  # else, return full list of triggers
  return getTrigConfigListDilep_el_leg()


# vector of trigger configs for dijet FF measurement (prescaled triggers)

def getTrigConfigsPrescale(period = ""):
  if period in g_periods:
    return getTrigPrescaleConfigList([period])
  return getTrigPrescaleConfigList()

def getTriglhlooseConfigsPrescale(period = ""):
  if period in g_periods:
    return getTriglhloosePrescaleConfigList([period])
  return getTriglhloosePrescaleConfigList()

  
if __name__ == "__main__":
  print("Inside main of HWWTrigConfig snippet")
  #getTrigConfigListDilep1()


