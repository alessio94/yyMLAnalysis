#!/usr/bin/env python
import argparse
import sys
from CommonAnalysisHelpers import common
toolsPath = common.findConfigPath("../tools", ignoreExecutionDirectory=True)
sys.path.append(toolsPath)
import plotUtils as utils
import math
from copy import deepcopy
import ROOT

##############################################################################
##############################################################################
#
# This tool allows to make quick comparison plots between different processes at different
# cut stages and for several observables. It uses the general concepts of patches and tags
# in CAF including a config file for steering this script.
# 
# In order to make a new plot comparison you need to do the following:
#   -- Adapt the config file 'plotComparisons.cfg' (or write a new one) to your needs
#   -- Adapt the style file 'style-plotComparisons.txt' (or write a new one) to your needs
#   -- Run this script with:
#       'python plotComparisons.py <plotCategories>', 
#       where <plotCategories> specifies the sections in your config file you want
#       plots to be produced for.
#
# Note, that you can also write your own config and style files and point to them accordingly.
# (For pointing to a new config file please specify the '--config' argument when running this script.
# For pointing to a new style file please adapt the 'patches' tag in your config.)
# 
##############################################################################
##############################################################################

# --------------------------------------------------------
# Some useful functions
# --------------------------------------------------------

def getCustomAxisLabel(var, tags):
  xlabel, ylabel = None, None
  if tags.hasTag("xlabel."+var):
    xlabel = tags.getTagStandardStringDefault("xlabel."+var, "No Label")
  if tags.hasTag("ylabel."+var):
    ylabel = tags.getTagStandardStringDefault("ylabel."+var, "No Label")
  return xlabel, ylabel

def getLegendEntry(samplepath, reader, setNameToTrailingFolder = False):
  f = reader.getListOfSampleFolders(samplepath).At(0) # if [em+me] is specified read label from one channel (needs to be the same)
  if not f: QF.BREAK("Something is wrong! Check your configuration!")
  leg_label = ROOT.TString()
  if not f.getTagString("~label", leg_label):
    label = str(samplepath)
    QF.WARN("No label found for sample with path '{:s}'. Using '{:s}'; if you wish to have a different label " \
            "please specify one with the 'label' tag in your style configuration.".format(f.getPath(), ROOT.TString(label)))
    return label
  return str(leg_label)

def setAxesRange(hist, var, tags):
  if tags.hasTag("xrange."+var+".0") and tags.hasTag("xrange."+var+".1"):
    xlow, xhigh = ROOT.Double(), ROOT.Double()
    tags.getTagDouble("xrange."+var+".0", xlow)
    tags.getTagDouble("xrange."+var+".1", xhigh)
    hist.GetXaxis().SetRangeUser(xlow, xhigh)
  if tags.hasTag("yrange."+var+".0") and tags.hasTag("yrange."+var+".1"):
    ylow, yhigh = ROOT.Double(), ROOT.Double()
    tags.getTagDouble("yrange."+var+".0", ylow)
    tags.getTagDouble("yrange."+var+".1", yhigh)
    hist.GetYaxis().SetRangeUser(ylow, yhigh)
  else:
    pass
  
def rebinHistogram(hist, var, tags):
  if tags.hasTag("rebin."+var):
    rebinValue = tags.getTagStringDefault("rebin."+var, "1")
    try:
      rebinValueInt = int(rebinValue)
      hist.Rebin(rebinValueInt)
    except:
      if str(rebinValue) == "VBFDNNrebinning":
        QF.INFO("Trying to rebin provided histograms!")
        binboundaries = [0, 26, 60, 74, 84, 90, 94]
        #binboundaries = [41, 75, 87, 93, 96]
        vec_binboundaries = ROOT.vector('Int_t')()
        for b in binboundaries: vec_binboundaries.push_back(int(b))
        remap = True
        hist = QF.TQHistogramUtils.getRebinned(hist, vec_binboundaries, remap)
  return hist
  
def getNumeratorAndDenominators(histograms, tags):
  # get numerator and denominator to plot ratios for
  indexOfNumerator = tags.getTagIntegerDefault("ratio.indexOfNumerator", 0)
  if indexOfNumerator >= len(histograms):
    QF.WARN("Specified path index for numerator is out of range!" \
            " Falling back to the first specified path (index 0) as numerator for the ratio plot.")
    indexOfNumerator = 0
  numerator = histograms[indexOfNumerator]
  histograms.pop(indexOfNumerator) # remove from histogram list
  denominator = histograms

  return numerator, denominator

# ----------------------------------------------------
# Main
# ----------------------------------------------------

def main(args):

  QF.INFO("Running script plotComparisons.py")

  # -------------------------------------------------------
  # Reading config
  
  parser = common.createConfigParser()
  configpath = ROOT.TString(common.findConfigPath(args.config))
  parsed = parser.read(configpath.Data())
  if not parsed:
    QF.BREAK("Configuration file with path '{:s}' could not be read! Exiting...".format(configpath))
    
  #--------------------------------------------------------
  # Loop over plot categories
  
  tags = QF.TQTaggable()
  # Tags in section 'common' will be overwritten by more specific sections
  if parser.has_section("common"):
    common.getTagsFromConfigParser(parser, tags, "common")

  # load SF if specified in common section
  commonInputFilePath = ROOT.TString()
  if tags.getTagString("input", commonInputFilePath):
    commonInputFilePath = QF.TQPathManager.getPathManager().getTargetPath(commonInputFilePath)
    samples = QF.TQSampleFolder.loadSampleFolder(commonInputFilePath)
  
  for plotCat in args.plotCategory:
      if not parser.has_section(plotCat):
        QF.BREAK("No section with name '{:s}' found in configuration file! Exiting...".format(plotCat))
    
      # Get tags from specific plot category
      common.getTagsFromConfigParser(parser, tags, plotCat)
      inputFilePath = ROOT.TString()
      if tags.getTagString("input", inputFilePath):
        if not inputFilePath  == commonInputFilePath:
          inputFilePath = QF.TQPathManager.getPathManager().getTargetPath(inputFilePath)
          samples = QF.TQSampleFolder.loadSampleFolder(inputFilePath)
      if not samples:
        QF.BREAK("Please specify an input sample folder!")
        
      styleFilePath = tags.getTagStringDefault("patches", common.findConfigPath("../tools/plotComparisons/style-plotComparisons.txt"))
      common.patchSampleFolder([styleFilePath], samples)
      reader = QF.TQSampleDataReader(samples)
      
      # -----------------------------------------------------
      # Style, label and legend settings
      
      atlasStyle = utils.get_style_ATLAS("ATLAS")
      print "\nApplying ATLAS style settings...\n"
      ROOT.gROOT.SetStyle("ATLAS")
      ROOT.gROOT.ForceStyle()
      
      c1 = ROOT.TCanvas("can","can",600,600)
      
      ypos1D = 1 - ROOT.gStyle.GetPadTopMargin() - 0.08
      xpos1D = ROOT.gStyle.GetPadLeftMargin()+0.03
      textsize = 0.04
      
      labelSizeScale = 1
      if tags.getTagBoolDefault("drawRatio", False):
        labelSizeScale = 1.5
      
      label = ["#sqrt{#it{s}} = 13 TeV"]
      additional_labels = []
      if not len(tags.getTagVString("label")) == 0:
        for ilabel in tags.getTagVString("label"):
          label.append(str(ilabel))
          
      legXlow = tags.getTagDoubleDefault("geometry.legend.xlow", 0.52)
      legXhigh = tags.getTagDoubleDefault("geometry.legend.xhigh", 0.85)
      legYlow = tags.getTagDoubleDefault("geometry.legend.ylow", 0.7)
      legYhigh = tags.getTagDoubleDefault("geometry.legend.yhigh", ypos1D)
      leg_cl = ROOT.TLegend(legXlow,legYlow,legXhigh,legYhigh)
      leg_cl.SetBorderSize(0)
      leg_cl.SetFillStyle(0)
      leg_cl.SetTextFont(42)
      leg_cl.SetTextSize(tags.getTagDoubleDefault("legend.textSize", 0.04)*labelSizeScale)
    
      #-----------------------------------------------------------------------
      # Get histograms with helper functions in plotUtils.py
    
      compareType = ROOT.TString()
      tags.getTagString("compare", compareType)
      if not compareType:
        if not compareType in ["processes", "variables", "cuts"]:
          QF.BREAK("No valid comparison type found! " \
                   "You need to specify what to compare with the tag 'compare'! Exiting...")
    
      # get distributions/observables, processes/sample paths and cuts from config
      observables = tags.getTagVString("observables")
      if len(tags.getTagVString("observables")) == 0: QF.BREAK("Please specify observables to plot with the tag 'observables'!")
    
      cuts = tags.getTagVString("cuts")
      if len(tags.getTagVString("cuts")) == 0: QF.BREAK("Please specify the cuts for plotting with the tag 'cuts'!")
      
      processes = tags.getTagVString("samplePaths")
      if len(tags.getTagVString("samplePaths")) == 0: QF.BREAK("Please specifiy processes to plot with the tag 'samplePaths'!")
      # add common path in front of sample paths if specified
      commonPath = ROOT.TString()
      if tags.getTagString("basePath", commonPath):
        processes = [QF.TQFolder.concatPaths(commonPath, proc) for proc in processes]

      totalbkg = ""
      if tags.hasTag("specialPlot.StatUncertaintyDataProcess"):
        totalbkg = tags.getTagStringDefault("specialPlot.StatUncertaintyDataProcess", "bkg/[em+me]/")
        if not ROOT.TString(totalbkg).IsNull():
          if isinstance(processes, ROOT.vector('TString')): processes.push_back(totalbkg)  # for plotting estimated data unc with sqrt(N_bkg)
          else: processes.append(totalbkg)  # for plotting estimated data unc with sqrt(N_bkg)
    
      histsList = utils.getDistributionsAtCutsForProcessesList(processes, cuts, observables, compareType, reader)
    
      if len(histsList) < 1 or (len(histsList) == 1 and len(histsList[0]) == 0):
        QF.BREAK("No histograms could be retrieved, please check your configuration!")
    
      # ---->> Loop over plots
      for histDicts in histsList:
    
        if not "hists" in histDicts.keys():
          QF.BREAK("No proper dictionay found with histogram definitions!")
          
        # ---->> Loop over histograms for special manipulations
        # Do rebinning for all histograms first because it effects other properties like hist range
        for ihist, hist in enumerate(histDicts["hists"]):
          # rebin histogram if specified with tag rebin.*
          histDicts["hists"][ihist] = rebinHistogram(hist, histDicts["variables"][ihist], tags)
        for ihist, hist in enumerate(histDicts["hists"]):
          # option for some special plots
          if tags.getTagBoolDefault("specialPlot.StatUncertainties", False):
            if histDicts["processes"][ihist] == totalbkg:
              if tags.hasTag("specialPlot.StatUncertaintyDataProcess"):
                newhist = hist.Clone()
                for i in range(hist.GetNbinsX()+1):
                  error = math.sqrt(hist.GetBinContent(i)) if hist.GetBinContent(i) > 0 else 0
                  newhist.SetBinContent(i, error)
                  newhist.SetBinError(i, 0)
                newhist.GetYaxis().SetTitle("Statistical Uncertainty")
                histDicts["hists"][ihist] = newhist
            else:
              newhist = hist.Clone()
              for i in range(hist.GetNbinsX()+1):
                newhist.SetBinContent(i, hist.GetBinError(i))
                newhist.SetBinError(i, 0)
              newhist.GetYaxis().SetTitle("Statistical Uncertainty")
              histDicts["hists"][ihist] = newhist
              
        # Get max of hists
        ylow, ymax = utils.findSmartHistRange(histDicts["hists"], normalized = tags.getTagBoolDefault("normed", False), includeOverflowBins = False)
        
        # ---->> Loop over histograms
        for ihist, hist in enumerate(histDicts["hists"]):
          
          # set histogram range
          hist.GetYaxis().SetRangeUser(ylow, ymax)
          
          # set custom axis range if specified with tags xlabel.* or ylabel.*
          setAxesRange(hist, histDicts["variables"][ihist], tags)
              
          # Set labels
          xlabel = hist.GetXaxis().GetTitle()
          ylabel = hist.GetYaxis().GetTitle()
          custom_xlabel, custom_ylabel = getCustomAxisLabel(histDicts["variables"][ihist], tags)
          if custom_xlabel: xlabel = custom_xlabel
          if custom_ylabel: ylabel = custom_ylabel

          # normaliize
          if tags.getTagBoolDefault("normed", True):
            hist.Scale(1./hist.Integral())
            utils.setup_style(hist, xlabel, ylabel+" (Normalized)" )
          else:
            utils.setup_style(hist, xlabel, ylabel)
            
          # get legend name for process
          if compareType == "processes":
            leg_cl.AddEntry(hist, getLegendEntry(histDicts["processes"][ihist], reader, tags.getTagBoolDefault("setLegNameToTrailingFolder", False)), "l")
            # Line colors set with patch file! But can use scheme to set colors that are clearly distinguishable.
            if tags.getTagBoolDefault("setDistinguishableLineColors"):
              colors = utils.getListOfDistinguishableColors()
              if ihist < len(colors):
                hist.SetLineColor(colors[ihist])
              else:
                hist.SetLineColor(ihist)
          if compareType == "cuts":
            leg_cl.AddEntry(hist, hist.GetName(), "l")
            hist.SetLineColor(ihist+1) # TODO: Setting line color can be improved/customized
          if compareType == "variables":
            leg_cl.AddEntry(hist, hist.GetName(), "l")
            hist.SetLineColor(ihist+1) # TODO: Setting line color can be improved/customized

        logX = tags.getTagBoolDefault("logX", False)
        logY = tags.getTagBoolDefault("logY", False)
        plot_options = tags.getTagStandardStringDefault("plotOptions", "histE")
        if tags.getTagBoolDefault("drawRatio", False) and len(histDicts["hists"]) >1:
          ratio_yrange = [0.5, 2]
          if tags.hasTag("ratio.yrange.0"): ratio_yrange[0] = tags.getTagVDouble("ratio.yrange")[0]
          if tags.hasTag("ratio.yrange.1"): ratio_yrange[1] = tags.getTagVDouble("ratio.yrange")[1]
          ratio_ylabel = tags.getTagStandardStringDefault("ratio.ylabel", "Nominal / Variation")
          numerator, denominator = getNumeratorAndDenominators(histDicts["hists"], tags)
          c1,pad1,pad2 = utils.drawWithRatio(numerator, denominator, ratio_yrange = ratio_yrange, ratio_ylabel = ratio_ylabel, plot_options = plot_options, logx = logX, logy = logY, gridlines_in_ratio = True)
          c1.cd()
          pad2.Draw("same")  
          pad1.cd()
        else:
          c1.SetLogx(logX)
          c1.SetLogy(logY)
          histDicts["hists"][0].Draw("histE")
          for h in histDicts["hists"]:
            h.Draw("histe same") 
          histDicts["hists"][0].Draw("histesame")
      
        utils.drawATLASLabel(xpos1D , ypos1D, "Internal", ROOT.kBlack, textsize*1.2*labelSizeScale, 0.25)
        shift = 0.06*(1+(labelSizeScale-1)/2.)
        for i, l in enumerate(label):
          utils.myText(xpos1D, (ypos1D-shift)-i*shift, ROOT.kBlack, l, tags.getTagDoubleDefault("label.textSize", 0.04)*labelSizeScale)
        leg_cl.Draw()
      
        outputfolder = tags.getTagStringDefault("outputFolder", "results/1DShapePlots")
        outputfolder = ROOT.TString(QF.TQPathManager.getPathManager().getTargetPath(outputfolder))
        utils.mkdir_p(outputfolder.Data())
        if compareType == "processes":
          histname = str(histDicts["cuts"][0])+"_"+str(histDicts["variables"][0]) # index does not matter
        if compareType == "cuts" or compareType == "variables":
          validProcessName = ROOT.TString(histDicts["processes"][0])
          validProcessName = validProcessName.ReplaceAll("[", "")
          validProcessName = validProcessName.ReplaceAll("]", "")
          validProcessName = validProcessName.ReplaceAll("+", "")
          validProcessName = QF.TQStringUtils.makeValidIdentifier(validProcessName, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", "_")
          QF.TQStringUtils.removeTrailing(validProcessName, "_") # remove the last _
        if compareType == "cuts":
          histname = str(histDicts["variables"][0])+"_"+str(validProcessName) # index does not matter
        if compareType == "variables":
          histname = str(histDicts["cuts"][0])+"_"+str(validProcessName) # index does not matter
        
        outPath = '{dirct}/{la}'.format(dirct=outputfolder, la=histname)
        plotformat = ["pdf", "png"]
        if tags.hasTag("plotFormat.0"): plotformat = tags.getTagVString("plotFormat")
        for pf in plotformat:
          c1.SaveAs(outPath+"."+pf)
        c1.Clear()
        leg_cl.Clear()
      
if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='make a shape plots for processes shapes')

  parser.add_argument("plotCategory", type=str, nargs="+", help="Plots that should be produced (section in config file)")
  parser.add_argument("--config", type=str, default=common.findConfigPath("../tools/plotComparisons/plotComparisons.cfg"), help="Configuration file to be read (please provide path relative to share or absolute path)")
  args = parser.parse_args()

  # import the following here, otherwise QFramework will screw up the  ArgumentParser '--help' option
  import QFramework as QF
  
  # setup ROOT
  import ROOT
  ROOT.gROOT.SetBatch(True)
  QF.TQLibrary.getQLibrary().setApplicationName("1D shape plots per process");

  # call the main function
  main(args);

  
