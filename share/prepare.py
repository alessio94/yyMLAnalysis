#!/bin/env python

from CommonAnalysisHelpers import common,prepare

def main(config):
    """create the sample folder for your analysis according to the given configuration (can be created from a config file)
    by populating it with a selection of DSIDs chosen from a cross section file"""

    # print a welcome message
    print(QFramework.TQStringUtils.makeBoldWhite("\nPreparing Analysis ROOT File\n"))

    # create a new sample folder
    samples = common.createSampleFolder(config)

    # make sure that the sample folder is writable before we go any further
    # helps to discover typos ahead of time
    common.testWriteSampleFolder(config, samples)

    # add the monte carlo to the sample folder as given by the cross section file
    # no actual input files will be linked yet
    prepare.addMonteCarlo(config, samples)

    # if requested, print the XSection information
    if config.getTagBoolDefault("printXsec",False):
        common.printXSecParser(samples)

    # make sure we have at least something in the sample folder
    if samples.isEmpty():
        QFramework.BREAK("your sample folder is empty. please make sure your configuration is sensible and try again!")

    # apply patches as given by the config
    common.patchSampleFolder(config.getTagVStandardString("patches"), samples)

    # produce some output for the user to have a look at the result
    QFramework.INFO("please investigate the top-level structure of your sample folder to make sure everything is in place!")
    samples.printContents(config.getTagStringDefault("printFolderTags","r1dt"))

    # write the sample folder to disk
    common.writeSampleFolder(config, samples)

    common.printExecutionSummary(config)

    # TODO: this doesn't work as intended for now
    # print any keys which were not read during the job
    #common.printUnreadKeys(config)

    # temporary fix to prevent segfaults in AnaBase 2.3.48 and beyond
    # update: still necessary in 21.2.4
    try:
        ROOT.xAOD.ClearTransientTrees()
    except AttributeError:
        pass

if __name__ == "__main__":

    # create a pre-configured argument parser
    parser = prepare.DefaultArgumentParser()

    import QFramework
    import ROOT

    # use the argument parser to read the command line arguments and config options from the config file
    config = common.getConfigOptions(parser.parse_args())

    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # call the main function
    main(config)
