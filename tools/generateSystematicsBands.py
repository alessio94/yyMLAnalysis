#!/usr/bin/env python

# This script generates bands corresponding to systematic uncertainties.
# The bands are generated using an analyzed SF with systematics as an input.
# The output can then be used to include the bands when visualizing a 
# nominal-only analyzed SF.
# For the distributions available in the sys SF shape and rate variations are
# considered for the systematic bands. For other distributions, rate variations
# are considered (separately per process)

import sys
import argparse
import os

def main(args):
  from QFramework import INFO,BREAK,ERROR,START,END
  from QFramework import TQSystematicsHandler,TQTaggable,TQFolder,TQMessageStream,TQStringUtils

  systlist = TQFolder()
  for sysfile in args.systematicsList:
    systlist.importFromTextFile(sysfile)

  systlist.setTagString("SampleFolder",args.inputFile)
  INFO("loading input from '{:s}'".format(args.inputFile))  
  handler = TQSystematicsHandler('systematics',systlist)
  handler.setTagBool("verbose",True)
  handler.addSample("default",args.pathPattern)

  # make sure all aliases expand to the empty string for nominal
  if not systlist.getFolder("Variations/Nominal"):
    nom = systlist.getFolder("Variations/Nominal+")
    for alias in TQStringUtils.getTagPlaceholders(args.pathPattern):
      nom.setTagString(alias,"")    
      
  if args.verbose:
    handler.setTagBool("verbose",True)

  for hist in args.moreHistos:
    handler.addHistogram(*hist)

  # Add all the cut stages at which you want to extract the systematic uncertainties to the handler
  cuts = []
  for cut in args.cuts:
    cuts.append(cut)
  if args.reloadcuts:
    cutfolder = handler.getSampleFolder("Nominal").getFolder("info/cuts/?")
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
  handler.collect()
  # This should then be comparably quick
  handler.compute()

  # write the histogram repository to a file if requested
  if args.repofile:
    handler.repository().writeToFile(args.repofile,False)

  # write the handler contents itself to a file if requested
  if args.handlerfile:
    handler.writeToFile(args.handlerfile,False)    
    
  # Export the systematics to an instance of TQFolder 
  INFO("writing outputs to file '{:s}'".format(args.outputFile))
  systematics = handler.exportSystematics("default",False,True)

  # write logging information
  info = systematics.getFolder("info+")
  info.setInfoTags()
  nom = handler.getSampleFolder("Nominal")
  for f in nom.getListOfFolders("info/config/?"):
    f.detachFromBase()
    info.addFolder(f)

  # write the 'yellow-band' to file
  systematics.writeToFile(args.outputFile,True,-1)

  # Produce a table with the ranking of systematic uncertainties at some cut stage table
  # Table.printPlain() to write it as a LaTeX and a CSV file
  for cut in cuts:
    table = handler.getTable(cut,"default")
    if table:
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
  parser.add_argument('--systematics',  metavar='systematics.txt', nargs="+", type=str, help='text file with all systematics defined', dest="systematicsList",required=True)
  parser.add_argument('--output', '-o', metavar='systematics.root', type=str, help='name of the output file', dest="outputFile", default="systematics.root")
  parser.add_argument('--tables', metavar='rankings', type=str, help='output directory for the systematics ranking tables', dest="outputRankings",default="rankings")
  parser.add_argument('--pathPattern', metavar="/some/path", type=str, help='pattern for the systematics paths', dest="pathPattern",default="bkg/[em$(Variation)+me$(Variation)]")
  parser.add_argument('--histogram', nargs=3, action="append", type=str, help='additional histogram specifications', dest="moreHistos", default=[])
  g = parser.add_mutually_exclusive_group(required=True)
  g.add_argument('--usecuts', metavar='cut', type=str, help='list of cuts to produce systematics at', dest="cuts",nargs="+", default=[])
  g.add_argument('--reloadcuts', action="store_true", help='use all cuts from the /info/cuts folder', dest="reloadcuts",default=False)
  g.add_argument('--loadcuts', metavar='cut', type=str, help='list of cut definition files with cuts to produce systematics at', dest="cutdefs",nargs="+", default=[])
  parser.add_argument('--verbose', '-v', action="store_const", const=True, default=False, help='provide the user with verbose output')
  parser.add_argument('--writeVariations', metavar='repository.root', type=str, help='write the original systematic histograms to a file for inspection', dest="repofile", default=None)
  parser.add_argument('--writeContents', metavar='contents.root', type=str, help='write the computed contents to a file', dest="handlerfile", default=None)
  args = parser.parse_args()
  sys.argv = []
  from QFramework import TQLibrary
  TQLibrary.getQLibrary().setApplicationName("createSystematics")
  import ROOT
  ROOT.gROOT.SetBatch(True)
  main(args)
