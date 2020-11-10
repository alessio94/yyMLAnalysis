#!/bin/env python

from CommonAnalysisHelpers import common,visualize

def main(config):
    """visualize the results of your analysis according to the given configuration (can be created from a config file)"""

    # print a welcome message
    print(QFramework.TQStringUtils.makeBoldWhite("\nVisualizing Analysis ROOT File\n"))

    # load the sample folder from disk
    samples = common.loadSampleFolder(config)

    # make sure that the sample folder is writable before we go any further
    # helps to discover typos ahead of time
    if config.hasTagString("outputFile"):
        common.testWriteSampleFolder(config, samples)

    # apply patches as given by the config
    common.patchSampleFolder(config.getTagVStandardString("patches"), samples)

    # location to dump results
    outputdir = visualize.getOutputDir(config)

    # retrieve properties of channels provided in config
    channelsDict = visualize.getDictOfChannels(config)

    # retrieve properties of campaigns provided in config
    campaignsDict = visualize.getDictOfCampaigns(config)

    # TODO: check that all channels specified in channelsDict is actually found in samples

    # initialize object which will read the results from the sample folder
    reader = visualize.initializeSampleDataReader(config, samples)

    # generate fake data if requested
    if config.getTagStringDefault("fakeData.source",""):
        visualize.generateFakeData(config)

    # calculate normalization factors based on a series of config files and save this information in the sample folder
    visualize.generateNormalizationFactors(config, samples, campaignsDict=campaignsDict)

    # create a document which will summarize all of the results
    summary = visualize.createSummaryDocument(samples, outputdir)

    # create a diagram summarizing the cuts
    visualize.createCutDiagram(config, samples, summary, outputdir)

    # print a cutflow for each combination of processes and cuts defined in the config for each channel
    visualize.printCutflows(config, reader, summary, channelsDict, outputdir, campaignsDict=campaignsDict)

    # print event level quantities defined by the user
    visualize.printEventLists(config, reader, channelsDict, outputdir, campaignsDict=campaignsDict)

    # make all of the plots with the processes stacked which are requested by the user
    visualize.makePlots(config, reader, summary, channelsDict, outputdir, campaignsDict=campaignsDict)

    # make a selection of advanced plots to more easily compare specific processes defined by the user
    visualize.makeComparisonPlots(config, reader, outputdir)

    # write the sample folder to disk
    if config.hasTagString("outputFile"):
        common.writeSampleFolder(config, samples)

    # finalize the summary document
    visualize.finalizeSummaryDocument(summary, config)

    common.printExecutionSummary(config)

    QFramework.INFO("all done!")

    # TODO: this doesn't work as intended for now
    # print any keys which were not read during the job
    #common.printUnreadKeys(config)

if __name__ == "__main__":

    # create a pre-configures argument parser
    parser = visualize.DefaultArgumentParser()

    import QFramework
    import ROOT

    # use the argument parser to read the command line arguments and config options from the config file
    config = common.getConfigOptions(parser.parse_args())

    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # TODO: This was in readAnalysis, is this still needed?
    ROOT.gROOT.SetBatch(True)

    # call the main function
    main(config)
