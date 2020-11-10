#!/usr/bin/env python
import argparse
import inspect
import sys

from CommonAnalysisHelpers import common
from QFramework import TQPathManager

alias = "buildWorkspace"

def main(args):
  # print a welcome message
  print(QFramework.TQStringUtils.makeBoldWhite("starting statistical treatment of the analysis results"))
  pathManager = TQPathManager.getPathManager()
  configFiles = args.config
  configs = []
  for configFile in configFiles:
    configFile = configFile.strip()
    if len(configFile) < 1 : continue
    # load the config
    configPath = common.findConfigPath(configFile)
    config = QFramework.TQFolder.newFolder("config");
    errorMessage = ROOT.TString();
    from os.path import isfile
    if not isfile(configPath):
      QFramework.ERROR("no such file: " +configPath)
      continue
    if not config.importFromTextFile(configPath, errorMessage):
      QFramework.ERROR("Failed to parse configuration file: " + errorMessage.Data())
      continue
    for change in args.change:
      config.importFromText(change)
    if args.fitIdentifier: config.importFromText("$replace(\"*:*\",fitIdentifier=\""+args.fitIdentifier+"\");")
    QFramework.INFO("Imported config file '{:s}'".format(configPath))
    configs.append( (config,configPath) )
  
  for config,configPath in configs:
    # create the statistics manager
    mgr = SFramework.TSStatisticsManager()

    QFramework.INFO("Starting Statistics Manager on '{:s}'".format(configPath))
    # run the statistics manager
    try:
      mgr.run(config)
    except Exception as ex:
      mgr.getModels().writeToFile(pathManager.getTargetPath("statistics.emergency.models.root"))
      mgr.getWorkspaces().writeToFile(pathManager.getTargetPath("statistics.emergency.workspaces.root"))
      mgr.getResults().writeToFile(pathManager.getTargetPath("statistics.emergency.results.root"))
      extype,exsource,extext = QFramework.parseException(ex)
      QFramework.BREAK("Failed to run statistics manager for config, exception of type '{:s}' occurred: '{:s}'".format(extype,extext))
    QFramework.INFO("Done processing '{:s}'!".format(configPath))

if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='Create a workspace for the HSG3 Run2 Analysis.')
  parser.add_argument('config', metavar='CONFIG', type=str, nargs="+",
            default="config/buildWorkspace.cfg",
            help='path to the analysis config file to be processed')
  parser.add_argument('--fitIdentifier', '-id', metavar='IDENTIFIER', type=str, default=None, help='Identifier for fit, becomes folder name of workspace')
  parser.add_argument("--change", metavar='text', type=str, help='changes to apply to the input config file', nargs="*", default=[])
  args = parser.parse_args()
  sys.argv=[]

  import QFramework
  try:
    import SFramework
  except Exception as ex:
    QFramework.BREAK("Failed to load SFramework - are you sure it is included in your library?")
  import ROOT
  QFramework.TQLibrary.getQLibrary().setApplicationName(alias);
  ROOT.gROOT.SetBatch(True)

  #try:
  main(args)
  #except:
  #  QFramework.BREAK("unable to access statistics manager - SFramework available?")


  
  
