#!/bin/env python

from CommonAnalysisHelpers import common,analyze

def main(config):
    """execute your analysis according to the given configuration (can be created from a config file)"""

    # print a welcome message
    print(QFramework.TQStringUtils.makeBoldWhite("\nAnalyzing Analysis ROOT File\n"))

    try:
        ROOT.StatusCode.enableFailure()
    except AttributeError:
        pass

    CLI = config.getFolder("CLI+")

    if CLI.getTagIntegerDefault("width",0):
        QFramework.TQLibrary.setConsoleWidth(CLI.getTagInteger("width"))

    # TODO: make a get aliases method? aliases also loaded during cut creation and analysis job booking
    # load the aliases from the config file
    aliases = QFramework.TQTaggable()
    aliases.importTagsWithoutPrefix(config,"cutParameters.")
    aliases.importTagsWithoutPrefix(config,"aliases.")
    QFramework.TQMVAObservable.globalAliases.importTags(aliases)

    # read the channel definitions
    channels = config.getTagVString("channels")

    # load MVA libraries if required
    if config.getTagBoolDefault("loadMVA",True):
        try:
            libMVA = analyze.loadLibMVA(config)
        except Exception as ex:
            libMVA = False
            template = "An exception of type '{0}' occured: {1!s}"
            message = template.format(type(ex).__name__, ",".join(ex.args))
            QFramework.ERROR(message)

    # set some global properties
    if not config.getTagBoolDefault("useTransientTree",True):
        QFramework.TQSample.gUseTransientTree = False
    if config.getTagBoolDefault("useAthenaAccessMode",False):
        QFramework.TQSample.gUseAthenaAccessMode = True
    if config.getTagBoolDefault("loadStore",True):
        try:
            store = ROOT.xAOD.TStore()
            store.setActive()
        except:
            pass

    # load the sample folder from disk
    samples = common.loadSampleFolder(config)

    # make sure that the sample folder is writable before we go any further
    # helps to discover typos ahead of time
    common.testWriteSampleFolder(config, samples)

    # remove the data folder if not desired
    if not config.getTagBoolDefault("doData",True):
        QFramework.INFO("removing data folder")
        samples.deleteObject("data!")

    # flag indicating to run a robust analysis
    robust = CLI.getTagBoolDefault("robust",False)
    # flag indicating to run a dummy analysis
    dummy = CLI.getTagBoolDefault("dummy",False)

    if not robust and not dummy:
        # remove the channels that are not scheduled
        pattern = config.getTagStringDefault("channelFolderPattern","?/$(channel)")
        QFramework.TQFolder.getPathTail(pattern)

        for sf in samples.getListOfSampleFolders(QFramework.TQFolder.concatPaths(pattern, "?")):
            found = False
            for c in channels:
                if QFramework.TQStringUtils.equal(sf.getName(),c):
                    found = True
                    break
            if not found:
                QFramework.INFO("removing folder '{:s}' - unscheduled channel!".format(sf.getPath()))
                sf.detachFromBase()

        # check if all requested channels are present
        for c in channels:
            if not samples.getSampleFolder(QFramework.TQFolder.concatPaths(pattern,c)):
                QFramework.BREAK("channel '{:s}' was requested, but is not present in input!".format(c))

    # apply patches as given by the config
    common.patchSampleFolder(config.getTagVStandardString("patches"), samples)

    # run a reduction step if scheduled, slimming down the sample folder to reduce future memory consumption
    if config.getTagBoolDefault("purgeSamples",False):
        common.reduceSampleFolder(config, samples)

    pathselect = CLI.getTagVStandardString("pathselect")

    # TODO: put this in the reduceSampleFolder method as well?
    # if requested, purge samples (even more)
    if config.getTagBoolDefault("purgeRemainder",False) and pathselect.size() > 0:
        for path in pathselect:
            samples.setTagBool("restrict",True,path)
        samples.purgeWithoutTag("~restrict")
        if not samples.getListOfSampleFolders("?"):
            QFramework.BREAK("sample folder empty after purge - something is wrong!")

    # perform any pre-processing of the sample folder for handling of systematic uncertainties
    # this step is likely to be highly analysis-dependent, so this is just an example implementation
    analyze.prepareSystematicsExample(config, samples)

    # load all the observables that allow access of the physics-content of your samples
    customobservables = analyze.loadObservables(config)

    # load all the cuts that are defined
    cuts = analyze.loadCuts(config)

    # load all the analysis jobs - booking histograms, event counters for cutflows, and much more
    # stores a boolean in config for determining whether or not analysis is cutbased
    # (yes if no MVA detected or at least 1 analysis job is booked)
    analyze.bookAnalysisJobs(config, cuts)

    # cutbased bool set in bookAnalysisJobs above
    runtime = config.getFolder("runtime+")
    cutbased = runtime.getTagBoolDefault("cutbased", False)

    if config.getTagBoolDefault("printCuts",cutbased):
        QFramework.INFO("cuts to visit samples with:")
        cuts.printCut();

    if customobservables or config.getTagBoolDefault("printObservables"):
        QFramework.INFO("custom observables were defined - this is the list of known observables:")
        QFramework.TQTreeObservable.printObservables()

    # run the cutbased analysis
    if cutbased:
        # create an analysis sample visitor that will successively visit all the samples and execute the analysis when used
        visitor = analyze.createAnalysisSampleVisitor(config, cuts)

        # book algorithms that will be executed on the events before any cuts are applied or analysis jobs are executed
        analyze.bookAlgorithms(config, visitor)

        # execute analysis, visiting every sample and running over every event
        # this step might take a VERY LONG time
        #nEvents = analyze.executeAnalysis(config, samples, visitor)
        nsamples = analyze.executeAnalysis(config, samples, visitor)

    # attach the cut definitions to the sample folder
    common.storeCuts(cuts, samples)

    # train any multivariate classifiers
    mvascriptnames = config.getTagVString("MVA")
    if len(mvascriptnames)>0:
        analyze.trainMVA(config, samples, cuts)

    if config.getTagBoolDefault("printObservables",False):
        QFramework.TQObservable.printObservables()

    # retrieve variables that have been set to determine a successful analysis
    # successful by default, error if analysisError has a value or mvaOK is false
    analysisError = runtime.getTagStringDefault("analysisError","")
    mvaOK = runtime.getTagBoolDefault("mvaOK",len(mvascriptnames))

    # write the sample folder to disk
    if len(analysisError) == 0 or mvaOK:
        common.writeSampleFolder(config, samples)
    else: #write alternative output file (the analysis didn't crash but there is something the user should check!
        # TODO: make this another method in common.py? probably
        # TODO: fix TString/std::string mess
        if CLI.getTagBoolDefault("debug",False):
            outfilename = ROOT.TString("debug.root")
        else:
            outfilename = config.getTagStringDefault("outputFile","")
        altFileName = outfilename
        QFramework.TQStringUtils.removeTrailingText(altFileName,".root")
        altFileName.Append(".isDone")
        with open(altFileName.Data(),'w') as f :
            f.write(analysisError.Data() if isinstance(analysisError,ROOT.TString) else analysisError)
            QFramework.WARN("No regular output sample file has been written but the analysis completed without fatal errors. Please check '{:s}' for information on potential errors".format(altFileName.Data()))

    if (config.hasTagString("memoryGraph")):
        ROOT.gROOT.SetBatch(True)
        memGraph = QFramework.TQLibrary.getMemoryGraph()
        memFileName = config.getTagStringDefault("memoryGraph","memoryUsage.pdf")
        QFramework.TQStringUtils.ensureTrailingText(memFileName,".pdf")
        memCanvas = QFramework.TQHistogramUtils.applyATLASstyle(memGraph,"Internal",0.2,0.9,0.9,"timestamp","rss [byte]")
        memCanvas.SaveAs(memFileName.Data())

    # TODO: this doesn't work as intended for now
    # print any keys which were not read during the job
    #common.printUnreadKeys(config)

    # write and print some performance information like memory usage and runtime
    common.printExecutionSummary(config)

    # temporary fix to prevent segfaults in AnaBase 2.3.48 and beyond
    # update: still necessary in 21.2.4
    try:
        ROOT.xAOD.ClearTransientTrees()
    except AttributeError:
        pass

if __name__ == "__main__":

    # create a pre-configured argument parser
    parser = analyze.DefaultArgumentParser()

    import QFramework
    import ROOT
    import imp

    # use the argument parser to read the command line arguments and config options from the config file
    config = common.getConfigOptions(parser.parse_args())

    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # call the main function
    main(config)
