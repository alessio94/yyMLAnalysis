#!/bin/env python

from CommonAnalysisHelpers import common,initialize

def main(config):
    """initialize the sample folder for your analysis according to the given configuration (can be created from a config file)
    by linking the MC input files to samples, determining their normalization, and adding the data input files"""

    # print a welcome message
    print(QFramework.TQStringUtils.makeBoldWhite("\nInitializing Analysis ROOT File\n"))

    # load the sample folder from disk
    samples = common.loadSampleFolder(config)

    # make sure that the sample folder is writable before we go any further
    # helps to discover typos ahead of time
    common.testWriteSampleFolder(config, samples)

    # if the initialize step is parallelized,
    # it's nice to have a flag to be able to optionally attempt to add the data
    if config.getTagBoolDefault("addData",True):
        # add all the data to the sample folder
        initialize.addData(config, samples)
    else:
        QFramework.WARN("skipping data addition as requested!")

    # apply pre-initialize patches as given by the config
    common.patchSampleFolder(config.getTagVStandardString("preInit_patches"), samples)

    # run the monte carlo initialization
    #  - collect meta-information including the sum-of-weights from the input files
    #  - compute the corresponding normalization factor for each sample
    if config.getTagBoolDefault("initialize",True):
        initialize.initializeMonteCarlo(config, samples)
    else:
        QFramework.WARN("skipping initialization as requested!")

    # apply post-initialize patches as given by the config
    common.patchSampleFolder(config.getTagVStandardString("postInit_patches"), samples)

    # TODO: do we want to use a purgeSamples flag to allow the user to choose whether or not to purge?
    # run a reduction step if scheduled, slimming down the sample folder to reduce future memory consumption
    if config.getTagBoolDefault("purgeSamples",False):
        common.reduceSampleFolder(config, samples)

    # produce some output for the user to have a look at the result
    QFramework.INFO("Done with initializing samples! Please investigate the top-level structure "  \
                    "of your sample folder to make sure everything is in place!")
    samples.printContents(config.getTagStringDefault("printFolderTags","r1dt"))

    # write the sample folder to disk
    common.writeSampleFolder(config, samples)

    # TODO: this doesn't work as intended for now
    # print any keys which were not read during the job
    #common.printUnreadKeys(config)

    common.printExecutionSummary(config)

    # temporary fix to prevent segfaults in AnaBase 2.3.48 and beyond
    # update: still necessary in 21.2.4
    try:
        ROOT.xAOD.ClearTransientTrees()
    except AttributeError:
        pass

if __name__ == "__main__":

    # create a pre-configured argument parser
    parser = initialize.DefaultArgumentParser()

    import QFramework
    import ROOT

    # use the argument parser to read the command line arguments and config options from the config file
    config = common.getConfigOptions(parser.parse_args())

    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # call the main function
    main(config)
