#!/usr/bin/env python

##########################################################
#
# This tool takes the histograms file used by HistFactory
# for building the workspace and produces nice plots and cutflows out of it.
# The histograms file is saved during the creation of the workspace.
#
# Usage:
# python tools/statistics/plotFitInputs.py --input path/to/input/rootfile --output path/to/output/folder --processes path/to/processes/txt/file --path folder/inside/root/file
# Example:
# python tools/statistics/plotFitInputs.py --input $CAFOUTPUTDIR/workspaces/ggf-default-c16a/histograms.root -o $CAFOUTPUTDIR/workspaces/ggf-default-c16a/prefitPlots --processes config/visualization/processes/ggF/fitinputs-couplings.txt --path HWWRun2GGF_histograms/Nominal
#
# For further options see ArgumentParser below
#
##########################################################

import argparse
from os.path import join as pjoin
import math

_protected = []
def protect(obj):
  _protected.append(obj)

def isNameBlacklisted(nameToCheck, blacklist):

  return True if True in [QFramework.TQStringUtils.matches(nameToCheck, blr) for blr in blacklist] else False

def main(args):
  
  infile = ROOT.TFile.Open(args.inputfile,"READ")
  if not infile:
    QFramework.ERROR("unable to open '{:s}'".format(args.inputfile))
    exit(0);

  if args.inputpath:
    indir = infile.Get(args.inputpath)
    if not indir:
      QFramework.ERROR("unable to retrieve '{:s}'".format(args.inputpath))
      exit(0);
  else:
    indir = infile


  unblinded = args.unblind
  samples = QFramework.TQSampleFolder("histograms")
  printer = QFramework.TQCutflowPrinter(samples)
  printer.setVerbose(False)
  plotter = QFramework.TQDefaultPlotter(samples)
  
  if args.verbose:
    plotter.setTagBool("verbose",True)
  plotter.setTagBool("ensureDirectory",True)
  plotter.setTagBool("style.showKS",False)
  plotter.setTagBool("style.showSub",True)
  plotter.setTagString("style.subPlot","ratio")
  plotter.setTagString("style.ratio.denominator","totalStack")    
  plotter.setTagInteger("style.nLegendCols",args.ncols)
  plotter.setTagBool("style.stackSignal",True)
  # for plots that mimic postfit style
  plotter.setTagBool("style.manualStacking", True)
  plotter.setTagBool("style.reverseStacking", False)  
  #plotter.setTagBool("style.showMissing", False) 
  #plotter.setTagBool("labels.info", False)   
  plotter.setTagBool("style.unsorted", False) 
  plotter.setTagBool("style.listDataFirst", True) 
  plotter.setTagDouble("style.legend.textSize", 0.02)
  plotter.setTagBool("style.listSignalFirst", True)
  # # to fix the y-limits of the main plot
  plotter.setTagDouble("style.min", 0.1)
  # plotter.setTagDouble("style.max", 1e4)

  if args.postfit:
    plotter.setTagBool("errors.showStat",False)  
    plotter.setTagString("errors.showSys","total")
    

  if args.processes:
    plotter.importProcessesFromFile(args.processes)
    if not unblinded:
      plotter.removeProcessesByName("*ata*")
  else:
    processes = set()
    for region in indir.GetListOfKeys():
      histograms = indir.Get(region.GetName())
      for ihist, histname in enumerate(sorted(histograms.GetListOfKeys(),key=lambda k:k.GetName())):
        pname = str(histname.GetName())
        if not pname.endswith("_count") and not pname == "total":
          processes.add(pname)
    for p in sorted(list(processes)):
      plotter.addBackground(p)        
      
  systematics = QFramework.TQFolder("total")
  protect(systematics)
  processes = []
  for name in plotter.getListOfProcessNames():
    p = QFramework.TQTaggable(plotter.getProcessTags(name.GetName()))
    processes.append(p);
  printer.importProcesses(plotter)
  print("================ Processes considered for plots ===============")
  plotter.printProcesses()

  plotter.setTagString("labels.atlas","Internal")

  plist = []

  histcontents = QFramework.TQTable()
  nrows = 0
  ibin = -1
  # for region in sorted(indir.GetListOfKeys(),key=lambda k:k.GetName()):
  for region in indir.GetListOfKeys():
    if isNameBlacklisted(region.GetName(), args.blacklistRegions.split(",")): continue
    printer.addCut(region.GetName())
    histograms = indir.Get(region.GetName())
    totalBkg = []
    totalBkgErr = []
    iValidHist = 0
    for ihist, histname in enumerate(sorted(histograms.GetListOfKeys(),key=lambda k:k.GetName())):
      pname = str(histname.GetName())
      if isNameBlacklisted(pname, args.blacklistSamples.split(",")): continue
      hist = histograms.Get(pname)
      if hist: iValidHist = iValidHist + 1
      else: continue
      bincontents = [0 for i in range(hist.GetXaxis().GetNbins())]
      bincontents_err = [0 for i in range(hist.GetXaxis().GetNbins())]
      t = QFramework.TQTaggable(plotter.getProcessTags(pname.replace("*", "")))
      histcontents.setEntry(0, iValidHist, t.getTagStringDefault(".title", pname))
      for ibin in range(1, hist.GetXaxis().GetNbins()+1): # ignore over/underflow bins
#         print "ibin ",ibin,pname 
        bincontent = hist.GetBinContent(ibin)
        error = hist.GetBinError(ibin)
        bincontent = hist.GetBinContent(ibin)
        if bincontent < 1e-10: bincontent = 0
        if error < 1e-10: error = 0
          
          
        histcontents.setEntry(nrows + ibin, 0, "{} bin={}".format(region.GetName(), ibin))
#         print "{} bin={}".format(region.GetName(), ibin),nrows + ibin,0
        if not "Data" in pname or unblinded:
          histcontents.setEntry(nrows + ibin, iValidHist, "{:.2f} $\\pm$ {:.2f}".format(bincontent, error))
          bincontents[ibin-1] = bincontent
          bincontents_err[ibin-1] = error
      if t.getTagBoolDefault(".isBackground", False):
        i = 0
        for cont,err in zip(bincontents, bincontents_err):
          if len(totalBkg) > i:
            totalBkg[i] = totalBkg[i] + cont
            totalBkgErr[i] = math.sqrt(totalBkgErr[i]**2 + err**2)
          else:
            totalBkg.append(cont)
            totalBkgErr.append(err)
          i = i+1
      if pname == "total":
        syshist = QFramework.TQHistogramUtils.getUncertaintyHistogram(hist)
        syshist.Divide(hist)
        syshist.SetDirectory(0)
        syshist.SetName("h")
        sysfolder = systematics.getFolder(region.GetName()+"+")
        sysfolder.setTagDouble("yield",0.)
        protect(syshist)
        protect(sysfolder)        
        sysfolder.addObject(syshist)
      if not pname.endswith("_count"):
        hist = histograms.Get(pname)
        if not hist.InheritsFrom(ROOT.TH1.Class()): 
          continue
        if args.xlabel:
          hist.GetXaxis().SetTitle(args.xlabel)
#         hist.GetXaxis().SetTitle( (hist.GetXaxis().GetTitle()).replace("obs_x_","Remapped discriminant variable of ") )
        hist.GetXaxis().SetTitle( (hist.GetXaxis().GetTitle()).replace("obs_x_","Remapped discriminant variable of ") )
        isBkg = False
        histf = samples.getSampleFolder(pname+"+").getFolder(".histograms/"+region.GetName()+"+")
        hist.SetDirectory(0)
        hist.SetName("h")
        histf.addObject(hist)
        cntf = samples.getSampleFolder(pname+"+").getFolder(".cutflow+")
        cnt = QFramework.TQHistogramUtils.histogramToCounter(hist);
        cnt.SetName(region.GetName())
        cnt.SetTitle(region.GetName())
        cntf.addObject(cnt)
#        for p in processes:
#          name = p.getTagStringDefault(".path","")
#          if QFramework.TQStringUtils.matches(pname,name):
#            QFramework.TQHistogramUtils.extractStyle(hist,p)
#            isBkg = p.getTagBoolDefault(".isBackground",False)
#            hist.SetTitle(p.getTagStringDefault(".title",hist.GetTitle()).Data())

    if ibin >= 0:
        histcontents.setEntry(0, iValidHist+1, "Total Bkg")
        for i in range(len(totalBkg)):
          histcontents.setEntry(nrows+i+1, iValidHist + 1, "{:.2f} $\\pm$ {:.2f}".format(totalBkg[i], totalBkgErr[i]))
        nrows = nrows + ibin # for filling of table with hist content
      
  if not samples.getListOfFolders("?"):
    print("no samples found, please check path given!")
    exit(0)

  options = QFramework.TQTaggable("")
  for opt in args.options:
    options.importTags(opt)
  if args.xbins:
    for i in range(len(args.xbins)-1):
      label = "{:g}-{:g} {:s}".format(args.xbins[i],args.xbins[i+1],args.xunit)
      options.setTagString("relabelX."+str(i),label)

  if args.verbose:
    options.printTags()
      
  if args.postfit:
    plotter.loadSystematics(systematics)

  for region in indir.GetListOfKeys():
    plotter.setTagBool("style.logScale", False)
    ok1 = plotter.plotAndSaveAs(pjoin(region.GetName(), "h" ), pjoin(args.outputdir,region.GetName())+".pdf", options)
    plotter.setTagBool("style.logScale", True)
    ok2 = plotter.plotAndSaveAs(pjoin(region.GetName(), "h" ), pjoin(args.outputdir,region.GetName())+"_log.pdf", options)
    ok = ok1 and ok2
  if not ok:
    print("Something went wrong during histogram plotting, check your sample folder content below...")
    samples.printContents("rdt")
  if args.writesf:
    samples.writeToFile(args.writesf)
  
  if args.writestyle:
    with open(args.writestyle,'w') as outfile:
      for p in processes:
        s = str(p.exportTagsAsString())
        outfile.write(s.replace("#","\\#")+"\n")

  yields = printer.createTable()
  raw = printer.createTable()
#   print("yields:",yields)
  
  print("writing tables to "+args.outputdir)
  yields.writeLaTeX(QFramework.TQFolder.concatPaths(args.outputdir,"yields.tex"),"standalone=true")
  yields.writeCSV(QFramework.TQFolder.concatPaths(args.outputdir,"yields.csv"))
  yields.writeHTML(QFramework.TQFolder.concatPaths(args.outputdir,"yields.html"))
  yields.writePlain(QFramework.TQFolder.concatPaths(args.outputdir,"yields.txt"))
  raw.writeLaTeX(QFramework.TQFolder.concatPaths(args.outputdir,"raw.tex"),"standalone=true")
  raw.writeCSV(QFramework.TQFolder.concatPaths(args.outputdir,"raw.csv"))
  raw.writeHTML(QFramework.TQFolder.concatPaths(args.outputdir,"raw.html"))
  raw.writePlain(QFramework.TQFolder.concatPaths(args.outputdir,"raw.txt"))
  histcontents.writeLaTeX(QFramework.TQFolder.concatPaths(args.outputdir,"histcontents.tex"))
  histcontents.writeCSV(QFramework.TQFolder.concatPaths(args.outputdir,"histcontents.csv"))
  histcontents.writeHTML(QFramework.TQFolder.concatPaths(args.outputdir,"histcontents.html"))
  histcontents.writePlain(QFramework.TQFolder.concatPaths(args.outputdir,"histcontents.txt"))
if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='produce a nice visualization of the fit inputs')
  parser.add_argument('--input', '-i', metavar='INPUT.ROOT', type=str, dest="inputfile", required=True, help='ROOT file with input histograms (plain root file, typically histograms.root')
  parser.add_argument('--output', '-o', metavar='OUTPUT/PATH', type=str, dest="outputdir", required=True, help='output directory')
  parser.add_argument('--writeSampleFolder', metavar='samples.root', type=str, dest="writesf", required=False, help='write the sample folder with the histograms')  
  parser.add_argument('--xlabel', '-x', metavar='TITLE', type=str, dest="xlabel", required=False, help='x label for the plots', default=None)
  parser.add_argument('--xbins', '-b', metavar='NUM', type=float, nargs="+", dest="xbins", required=False, help='bin boundaries', default=None)
  parser.add_argument('--xunit', '-u', metavar='GeV', type=str, dest="xunit", required=False, help='axis unit', default="GeV")    
  parser.add_argument('--ncols', metavar='NCOLS', type=int, dest="ncols", required=False, help='number of legend columns for the plots', default=2)
  parser.add_argument('--path', '-p', metavar='PATH/TO/FOLDER', type=str, dest="inputpath", default=None, help='name of directory inside ROOT file')
  parser.add_argument('--processes', '-l', metavar='DEFINTION.TXT', type=str, dest="processes", required=False, help='process definition file')
  parser.add_argument('--writestyle', metavar='DEFINTION.TXT', type=str, dest="writestyle", default=None, help='output style file')
  parser.add_argument('--options', metavar='a=b', type=str, nargs="+",dest="options", help='plotting options',default=[])
  parser.add_argument('--verbose', dest="verbose", action="store_const", const=True, default=False,help='be verbose')
  parser.add_argument('--unblind', dest="unblind", action="store_const", const=True, default=False,help='show data')
  parser.add_argument('--postfit', dest="postfit", action="store_const", const=True, default=False,help='mark plots as postfit')
  parser.add_argument('--blacklistRegions', dest="blacklistRegions", type=str, default="", help='comma-separated list of names (can be wildcarded with *) of regions that should be excluded')
  parser.add_argument('--blacklistSamples', dest="blacklistSamples", type=str, default="", help='comma-separated list of names (can be wildcarded with *) of samples that should be excluded')
  args = parser.parse_args()

  # setup ROOT
  import QFramework
  import ROOT
  ROOT.gROOT.SetBatch(True)
  QFramework.TQLibrary.getQLibrary().setApplicationName("plotFitInputs");

  # call the main function
  main(args);
