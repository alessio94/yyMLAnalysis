#!/usr/bin/env python2

# This script generates bands corresponding to systematic uncertainties.
# The bands are generated using an analyzed SF with systematics as an input.
# The output can then be used to include the bands when visualizing a 
# nominal-only analyzed SF.
# For the distributions available in the sys SF shape and rate variations are
# considered for the systematic bands. For other distributions, rate variations
# are considered (separately per process)

import sys
import argparse
from os.path import join as pjoin

def main(args):
  from QFramework import INFO,BREAK,ERROR,START,END
  from QFramework import TQSystematicsHandler,TQTaggable,TQFolder,TQMessageStream
  from ROOT import TString
  from ROOT.std import pair as cpair
  pair = cpair('TString','TString')

  INFO("starting to crawl for systematics")
  handler = TQSystematicsHandler ('systematics')
  handler.setTagBool("verbose",True)
  nomtags = TQTaggable("variation='', Variation=''")
  handler.setNominalFilePath(nomtags.replaceInText(args.inputFile))
  handler.setNominalSampleFolderPath(nomtags.replaceInText(args.pathPattern))
  if args.verbose:
    handler.setTagBool("verbose",True)

  systlist = TQFolder.loadFromTextFile(args.systematicsList)
  if not systlist:
    BREAK("unable to load file: '{:s}'".format(args.systematicsList))
  variations = systlist.getFolder("Variations")
  for syst in systlist.getListOfFolders("Systematics/?"):
    up = None
    down = None
    s_up = TString()
    s_down = TString()
    if syst.getTagString("Up",s_up):
      up   = variations.getFolder(s_up)
    if syst.getTagString("Down",s_down):
      down = variations.getFolder(s_down)
    if up and down:
      up_pathPattern = up.replaceInText(args.pathPattern)
      dn_pathPattern = down.replaceInText(args.pathPattern)
      if syst.GetName().__contains__("fJVT"):
        continue
      if syst.GetName().__contains__("ATLAS_JER_"):
        up_pathPattern.ReplaceAll("em[_","[em+em[_")
        up_pathPattern.ReplaceAll("me[_","[me+me[_")
        up_pathPattern.ReplaceAll("PDsmear__1up]","PDsmear__1up]]")
        dn_pathPattern.ReplaceAll("em[_","[em+em_")
        dn_pathPattern.ReplaceAll("me[_","[me+me_")
        dn_pathPattern.ReplaceAll("PDsmear__1down]","PDsmear__1down]]")
      handler.addSystematic(
          syst.GetName(),
          pair(up.replaceInText(args.inputFile),up_pathPattern),
          pair(down.replaceInText(args.inputFile),dn_pathPattern)
      )
    elif up:
      handler.addSystematic(
          syst.GetName(),
          pair(up.replaceInText(args.inputFile),up.replaceInText(args.pathPattern)),
      )
    elif down:
      handler.addSystematic(
          syst.GetName(),
          pair(down.replaceInText(args.inputFile),down.replaceInText(args.pathPattern))
      )
    else:
      ERROR("problem with systematic {:s}".format(syst.GetName()))

  # Add all the cut stages at which you want to extract the systematic uncertainties to the handler
  cuts = []
  for cut in args.cuts:
    cuts.append(cut)
  if args.reloadcuts:
    cutfolder= handler.getSampleFolder("Nominal",0).getFolder("info/cuts")
  else:
    cutfolder = TQFolder("cuts")
  for cutdef in args.cutdefs:
    cutfolder.importFromTextFile(cutdef)
  cutlist = cutfolder.getListOfFolders("?/*")
  if cutlist:
    for cut in cutlist:
      cuts.append(cut.GetName())
  for cut in cuts:
    handler.addCut(cut)

  # Have the handler print the systematic uncertainties to the console to be sure that you did 'the right thing'
  INFO("list of systematics:")
  handler.printSystematics()

  # Have the handler do it's magic
  # Depending on the size of your input files and the speed of your system, this step might take some time
  START("l","collecting systematics")
  handler.collectSystematics()
  END(TQMessageStream.OK)

  # Export the systematics to an instance of TQFolder 
  INFO("writing outputs to file '{:s}'".format(args.outputFile))
  systematics = handler.exportSystematics()

  # write logging information
  info = systematics.getFolder("info+")
  info.setInfoTags()
  nom = handler.getSampleFolder("Nominal",0)
  for f in nom.getListOfFolders("info/config/?"):
    f.detachFromBase()
    info.addFolder(f)

  # write the 'yellow-band' to file
  systematics.writeToFile(args.outputFile,True,0)

  # Produce a table with the ranking of systematic uncertainties at some cut stage table
  # Table.printPlain() to write it as a LaTeX and a CSV file
  for cut in cuts:
    table = handler.getTable(cut)
    table.setTagString("colAlign", "l")
    table.writeLaTeX(str(TQFolder.concatPaths(args.outputRankings,cut))+".tex", "ensureDirectory=true")
    table.writeHTML(str(TQFolder.concatPaths(args.outputRankings,cut))+".html", "ensureDirectory=true")
   
  INFO("all done")

  if args.verbose:
    handler.printContents("rdt")

    
if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='Create a systematics ranking and systematics.root file',formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('--input', '-i', metavar='samples.root:samples', type=str, help='name of the input sample folder', dest="inputFile",required=True)
  parser.add_argument('--systematics',  metavar='systematics.txt', type=str, help='text file with all systematics defined', dest="systematicsList",default="config/statistics/systematics-all.txt")
  parser.add_argument('--output', '-o', metavar='systematics.root', type=str, help='name of the output file', dest="outputFile", default="systematics.root")
  parser.add_argument('--tables', metavar='rankings', type=str, help='output directory for the systematics ranking tables', dest="outputRankings",default="rankings")
  parser.add_argument('--pathPattern', metavar="/some/path", type=str, help='pattern for the systematics paths', dest="pathPattern",default="bkg/[em$(Variation)+me$(Variation)]")
  g = parser.add_mutually_exclusive_group(required=True)
  g.add_argument('--usecuts', metavar='cut', type=str, help='list of cuts to produce systematics at', dest="cuts",nargs="+", default=[])
  g.add_argument('--reloadcuts', action="store_true", help='use all cuts from the /info/cuts folder', dest="reloadcuts")
  g.add_argument('--loadcuts', metavar='cut', type=str, help='list of cut definition files with cuts to produce systematics at', dest="cutdefs",nargs="+", default=[])
  parser.add_argument('--verbose', '-v', action="store_const", const=True, default=False, help='provide the user with verbose output')
  args = parser.parse_args()
  sys.argv = []
  from QFramework import TQLibrary
  TQLibrary.getQLibrary().setApplicationName("createSystematics")
  import ROOT
  ROOT.gROOT.SetBatch(True)
  main(args)
