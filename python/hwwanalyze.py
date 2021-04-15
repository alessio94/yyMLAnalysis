
import ROOT
import QFramework
from CommonAnalysisHelpers import common

# Modularize these functions, as they have to be used in the HWWFakeFactorWeight observable
def getIsRestrictedPath(config):
    # check if potential restricted path is specified and whether it points to a fakes folder
    pathselect_tmp, isRestrictedPath = common.getRestrictedPaths(config)
    pathselect = ""
    isFakePath = False
    if isRestrictedPath:
        fakeBasePath = QFramework.TQFolder.concatPaths("bkg/?/?", config.getTagStringDefault("~ddFakes.baseFolderName", "ddFakes"))
        # returns empty list if no matching path was found
        fakePaths = common.getMatchingBasePaths(restrictedPaths = pathselect_tmp, basePaths = fakeBasePath)
        isFakePath = True if fakePaths else False
    return isRestrictedPath, isFakePath

def getRestrictedPathLength(config):
    pathselect, isRestrictedPath = common.getRestrictedPaths(config)
    if not isinstance(pathselect, (list, ROOT.vector('TString'), ROOT.vector('string'))): pathselect = [pathselect]
    return len(pathselect)

def pruneSampleFolder(toBePrunedFolder, specialProcess, initialBaseFolder):
    # prune sample folder from all folders not covered by specialProcess
    fullPath = QFramework.TQFolder.makeValidPath(specialProcess)
    QFramework.TQStringUtils.removeTrailing(fullPath, "/")
    QFramework.TQStringUtils.removeLeading(fullPath, "/")
    trimmedPath = "/".join(str(fullPath).split("/")[initialBaseFolder.countPathLevels(initialBaseFolder.getPath()):])
    if not toBePrunedFolder.getListOfSampleFolders(trimmedPath):
        return False
    for specialProcessFolder in toBePrunedFolder.getListOfSampleFolders(trimmedPath):
        # QFramework.INFO("Running over specified process '{}' for which to include special systematics!".format(specialProcessFolder.getPath()))
        if not specialProcessFolder:
            QFramework.BREAK("Specified process '{}' for which to include special systematics is not present in samplefolder, check your config!".format(specialProcess))
        for ilevel in range(1, specialProcessFolder.countPathLevels(specialProcessFolder.getPath())):
            baseFolder = specialProcessFolder.getBase(ilevel)
            specialProcessBranchFolder = specialProcessFolder.getBase(ilevel-1)
            if not baseFolder:
                continue
            if baseFolder.hasTag("channel"):
                break # don't go further down
            for ifolders in baseFolder.getListOfSampleFolders("?"):
                if not ifolders == specialProcessBranchFolder:
                    toBePrunedFolder.importFromText("$delete('{}!');".format(ifolders.getPath()))
    return True

def prepareSystematics(config, samples):
    """prepare the systematic handling for the HWW analysis"""

    CLI = config.getFolder("CLI+")
    # flag indicating to run a robust analysis
    robust = CLI.getTagBoolDefault("robust",False)
    # flag indicating to run a dummy analysis
    dummy = CLI.getTagBoolDefault("dummy",False)


    if not robust and not dummy:
        subfolders = samples.getListOfSampleFolders("?")
        doNominal = config.getTagBoolDefault("xAODdoNominal",True)
        channels = config.getTagVString("channels")
        mcasvchannels = set([ c.Data() for c in channels ])
        for c in channels:
            for sf in subfolders:
                if sf.getTagBoolDefault("isData",False):
                    # we're in a data SampleFolder
                    # just set some nominal tags for this channel subfolder
                    f = sf.getSampleFolder(c)
                    if not f: continue
                    f.setTagString(".mcasv.channel",f.getTagStringDefault("channel",""))
                    f.setTagString("weightname","nominal")
                    f.setTagString("p4suffix","")
                    continue
                # we're in a MC SampleFolder
                f = sf.getSampleFolder(c)
                if not f:
                    QFramework.WARN("unable to retrieve sample folder '{:s}' from '{:s}'. Is this expected from your path restriction?".format(c,sf.GetName()))
                    sf.printContents()
                    continue #if this is not a valid folder, don't do anything with it, you'll get a null pointer exception otherwise!

                # In the following, the systematic variations are added as separate channels to the sample folder.
                # We first check if the restricted path (if any is specified) is set to a "standard" MC path or a fakes folder.
                # This we do, because we only want to run over sf & p4 variations when processing "standard" MC
                # (and NOT fake factor variations) and only over FF variations (and NOT sf & p4 variations) when processing
                # the fakes/anti-ID region

                isRestrictedPath, isFakePath = getIsRestrictedPath(config)

                # provide warning to the user when an old tag is used for including systematic variations:
                if config.hasTag("xAODsfSystematics") or config.hasTag("xAODp4Systematics") or config.hasTag("xAODsfSystematics.0") or config.hasTag("xAODp4Systematics.0"):
                    QFramework.BREAK("Houston, we have a problem! It seems like you are using a deprecated tag " \
                                     "to specify your systematic variations. The tags 'xAODsfSystematics' and 'xAODp4Systematics' were replaced " \
                                     "by 'xAODsfSystematicsList' and 'xAODp4SystematicsList' and the variations are now specified in " \
                                     "a separate file. You can refer to config/master/ggF/analyze-ggF-FullRun2-EMTopoJets-sys.cfg' for " \
                                     "an example usage.")

                # right now, we are treating p4 systematics and sf systematics as uncorrelated
                # we do only look at the diagonal terms where one systematic is activated
                # crossover terms are neglected for now, hence we have two separate loops

                #----------------------------------------------------------------
                # Include p4 (4-vector) systematics
                fileWithP4Variations = ROOT.TString()
                if config.getTagString("xAODp4SystematicsList", fileWithP4Variations) and not isFakePath:
                    with open(QFramework.TQPathManager.findFileFromEnvVar(fileWithP4Variations, "CAFANALYSISSHARE")) as varFile:
                        p4varList_ = varFile.readlines()
                        p4varList = [x.strip() for x in p4varList_]
                        for p4vars in p4varList:
                            #remove/skip comment lines:
                            if p4vars == "" or p4vars[0]=="#": continue
                            for p4var in p4vars.split(","): # splits up and down variation
                                p4var = p4var.strip()
                                # for each p4 systematic, copy the channel folder
                                if not p4var or len(p4var)<1: continue
                                newname=c+ROOT.TString("_")+p4var
                                newf = f.copy(newname)
                                if not newf:
                                    QFramework.BREAK("unable to copy folder {:s} to new name {:s}".format(f.GetName(),newname))

                                sf.addFolder(newf)
                                # set the appropriate tags
                                candname = f.getTagStringDefault("~cand","")
                                if not candname:
                                    QFramework.BREAK("didn't find 'cand' tag on sample folder '{:s}'".format(f.getPath()))
                                newf.setTagString("p4Variation",p4var)
                                newf.setTagString("weightname","nominal")
                                newf.setTagString("cand",candname+"___"+p4var)
                                newf.setTagString("cand.original",candname)
                                newf.setTagString(".mcasv.channel",newname)
                                newf.setTagString("p4suffix",ROOT.TString("___")+p4var)
                                newf.setTagString("sysVariation",p4var)
                                mcasvchannels.add(newname.Data())

                #----------------------------------------------------------------
                # Include sf (scale-factor) systematics
                fileWithSFVariations = ROOT.TString()
                if config.getTagString("xAODsfSystematicsList", fileWithSFVariations) and not isFakePath:
                    with open(QFramework.TQPathManager.findFileFromEnvVar(fileWithSFVariations, "CAFANALYSISSHARE")) as varFile:
                        sfvarList_ = varFile.readlines()
                        sfvarList = [x.strip() for x in sfvarList_]
                        for sfvars in sfvarList:
                            #remove/skip comment lines:
                            if sfvars == "" or sfvars[0]=="#": continue
                            eventInfoDecoration = False
                            if sfvars.find("!EventInfoDecoration:") >= 0:
                                sfvars = sfvars.replace("!EventInfoDecoration:", "")
                                eventInfoDecoration = True
                            block = ROOT.TString("")
                            sfvars = ROOT.TString(sfvars)
                            variationWithQuotes = QFramework.TQStringUtils.readBlock(sfvars, block, "\"\"")
                            if not block == "":
                                sfvarsList = [block]
                                if not sfvars == "":
                                    QFramework.ERROR("Your variation list with name '{:s}' is not valid. Line with content '{:s}' cannot be read. Please use only one variation per line if you need quotes for specifying the variation/weight name".format(QFramework.TQPathManager.findFileFromEnvVar(specialsfSystLists[i], "CAFANALYSISSHARE"), block+sfvars))
                                    continue
                            if not variationWithQuotes:
                                sfvarsList = str(sfvars).split(",")
                            for sfvar in sfvarsList: # splits up and down sfvar
                                sfvarname = sfvar
                                sfvar = QFramework.TQStringUtils.makeValidIdentifier(sfvar)
                                # for each sf systematic, copy the channel folder
                                if not sfvar or len(sfvar)<1: continue
                                newname = c+"_"+sfvar
                                newf = f.copy(newname)
                                if not newf:
                                    QFramework.BREAK("unable to copy folder {:s} to new name {:s}".format(f.GetName(),newname))
                                sf.addFolder(newf)
                                # set the appropriate tags
                                newf.setTagString("variationType", "xAODsfSystematics")
                                # needed only for HWWMCWeight observable
                                if eventInfoDecoration: newf.setTagBool("eventInfoDecoration", eventInfoDecoration)
                                newf.setTagString("variationName", sfvarname)
                                newf.setTagString("weightname",sfvarname)
                                newf.setTagString(".mcasv.channel",newname)

                                ### todo: still needed? (if not, remove them also from further below) ###
                                newf.setTagString("sfVariation",sfvarname)
                                newf.setTagString("p4suffix","")
                                newf.setTagString("sysVariation",sfvarname)
                                ###########################
                                mcasvchannels.add(newname.Data())

                #----------------------------------------------------------------
                # Include special p4 systematics (for selected processes only)
                specialp4Processes = config.getTagVStandardString("xAODProcessSpecificp4SystematicsPaths")
                specialp4SystLists = config.getTagVStandardString("xAODProcessSpecificp4SystematicsLists")
                printMissingSpecialSystMsg = config.getTagBoolDefault("printMissingSpecialSystMsg",True)
                for i, sp in enumerate(specialp4Processes):
                    if len(config.getTagVString("xAODProcessSpecificp4SystematicsLists")) == 0:
                        QFramework.ERROR("Process defined for which to include special systematics, but no systematics list found! Check your config!")
                        break
                    else:
                        if not len(specialp4SystLists) == len(specialp4Processes):
                            QFramework.ERROR("Process defined for which to include special systematics, but no appropriate number of systematics list found! There need to be the same number of processes (tag = xAODProcessSpecificp4SystematicsPaths) and p4 systematics list (tag = xAODProcessSpecificp4SystematicsLists) specified and the ones at the same position belong together. Check your config...skipping special systematics for now!")
                            break
                        if not ROOT.TString(sp).Contains(sf.getName()): # if sf (bkg, sig, data) matches the one in special process
                            #QFramework.INFO("currently looped over sample folder with path '{}' does not match the process '{}' for which special systematics should be configured, skipping!".format(sf.getName(), sp))
                            continue

                        if len(specialp4SystLists) > 1 and getRestrictedPathLength(config) > 1:
                            QFramework.ERROR("You seem to have tried to run analyze.py with both a list of multiple restricted paths as well as a list of process-specific systematics to be applied. At the moment, this is not supported, please run analyze.py separately for each restricted path.")
                            import sys
                            sys.exit()

                        # we are in correct bkg, sig, or data for the special systematics now and can continue
                        QFramework.INFO("Applying process specific scale systematics from file '{}'".format(specialsfSystLists[i]))
                        with open(common.findConfigPath(specialp4SystLists[i])) as varFile:
                            p4varList_ = varFile.readlines()
                            p4varList = [x.strip() for x in p4varList_]
                            for p4vars in p4varList:
                                #remove/skip comment lines:
                                if p4vars == "" or p4vars[0]=="#": continue
                                for p4var in p4vars.split(","): # splits up and down p4var
                                    p4var = p4var.strip()
                                    # for each p4 systematic, copy the channel folder
                                    if not p4var or len(p4var)<1: continue
                                    newname = c+"_"+p4var
                                    newf = f.copy(newname)
                                    if not newf:
                                        QFramework.BREAK("unable to copy folder {:s} to new name {:s}".format(f.GetName(),newname))
                                    # prune newf before adding to p4!
                                    if not pruneSampleFolder(toBePrunedFolder = newf, specialProcess = sp, initialBaseFolder = f):
                                        if printMissingSpecialSystMsg:
                                            QFramework.INFO("Specified process '{}' for which to include special systematics is not present in currently iterated samplefolder, skipping!".format(sp))
                                        continue
                                    # now add the folder where all SFs are removed where the special systematics don't apply
                                    sf.addFolder(newf)
                                    # set the appropriate tags
                                    candname = f.getTagStringDefault("~cand","")
                                    if not candname:
                                        QFramework.BREAK("didn't find 'cand' tag on sample folder '{:s}'".format(f.getPath()))
                                    newf.setTagString("p4Variation",p4var)
                                    newf.setTagString("weightname","nominal")
                                    newf.setTagString("cand",candname+"___"+p4var)
                                    newf.setTagString("cand.original",candname)
                                    newf.setTagString(".mcasv.channel",newname)
                                    newf.setTagString("p4suffix",ROOT.TString("___")+p4var)
                                    newf.setTagString("sysVariation",p4var)
                                    mcasvchannels.add(newname.Data())
                #----------------------------------------------------------------
                # Include special sf systematics (for selected processes only)
                specialsfProcesses = config.getTagVStandardString("xAODProcessSpecificsfSystematicsPaths")
                specialsfSystLists = config.getTagVStandardString("xAODProcessSpecificsfSystematicsLists")
                for i, sp in enumerate(specialsfProcesses):
                    if len(config.getTagVString("xAODProcessSpecificsfSystematicsLists")) == 0:
                        QFramework.ERROR("Process defined for which to include special systematics, but no systematics list found! Check your config!")
                        break
                    else:
                        if not len(specialsfSystLists) == len(specialsfProcesses):
                            QFramework.ERROR("Process defined for which to include special systematics, but no appropriate number of systematics list found! There need to be the same number of processes (tag = xAODProcessSpecificsfSystematicsPaths) and sf systematics list (tag = xAODProcessSpecificsfSystematicsLists) specified and the ones at the same position belong together. Check your config...skipping special systematics for now!")
                            break
                        if not ROOT.TString(sp).Contains(sf.getName()): # if sf (bkg, sig, data) matches the one in special process
                            #QFramework.INFO("currently looped over sample folder with path '{}' does not match the process '{}' for which special systematics should be configured, skipping!".format(sf.getName(), sp))
                            continue

                        if len(specialsfSystLists) > 1 and getRestrictedPathLength(config) > 1:
                            QFramework.ERROR("You seem to have tried to run analyze.py with both a list of multiple restricted paths as well as a list of process-specific systematics to be applied. At the moment, this is not supported, please run analyze.py separately for each restricted path.")
                            import sys
                            sys.exit()

                        # we are in correct bkg, sig, or data for the special systematics now and can continue
                        QFramework.INFO("Applying process specific scale systematics from file '{}'".format(specialsfSystLists[i]))
                        with open(common.findConfigPath(specialsfSystLists[i])) as varFile:
                            sfvarList_ = varFile.readlines()
                            sfvarList = [x.strip() for x in sfvarList_]
                            for sfvars in sfvarList:
                                #remove/skip comment lines:
                                if sfvars == "" or sfvars[0]=="#": continue
                                eventInfoDecoration = False
                                if sfvars.find("!EventInfoDecoration:") >= 0:
                                    sfvars = sfvars.replace("!EventInfoDecoration:", "")
                                    eventInfoDecoration = True
                                block = ROOT.TString("")
                                sfvars = ROOT.TString(sfvars)
                                variationWithQuotes = QFramework.TQStringUtils.readBlock(sfvars, block, "\"\"")
                                if not block == "":
                                    sfvarsList = [block]
                                    if not sfvars == "":
                                        QFramework.ERROR("Your variation list with name '{:s}' is not valid. Line with content '{:s}' cannot be read. Please use only one variation per line if you need quotes for specifying the variation/weight name".format(common.findConfigPath(specialsfSystLists[i]), block+sfvars))
                                        continue
                                if not variationWithQuotes:
                                    sfvarsList = str(sfvars).split(",")
                                for sfvar in sfvarsList: # splits up and down sfvar
                                    sfvarname = sfvar
                                    sfvar = QFramework.TQStringUtils.makeValidIdentifier(sfvar)
                                    # for each sf systematic, copy the channel folder
                                    if not sfvar or len(sfvar)<1: continue
                                    newname = c+"_"+sfvar
                                    newf = f.copy(newname)
                                    if not newf:
                                        QFramework.BREAK("unable to copy folder {:s} to new name {:s}".format(f.GetName(),newname))
                                    # prune newf before adding to sf! this is important in case restrict option is set to a path for which no
                                    # special systematics are booked
                                    if not pruneSampleFolder(toBePrunedFolder = newf, specialProcess = sp, initialBaseFolder = f):
                                        if printMissingSpecialSystMsg:
                                            QFramework.INFO("Specified process '{}' for which to include special systematics is not present in currently iterated samplefolder, skipping!".format(sp))
                                        continue

                                    # now add the folder where all SFs are removed where the special systematics don't apply
                                    sf.addFolder(newf)

                                    # set the appropriate tags
                                    newf.setTagString("variationType", "xAODsfSystematics")
                                    # needed only for HWWMCWeight observable
                                    if eventInfoDecoration: newf.setTagBool("eventInfoDecoration", eventInfoDecoration)
                                    newf.setTagString("variationName", sfvarname)
                                    newf.setTagString("weightname",sfvarname)
                                    newf.setTagString(".mcasv.channel",newname)

                                    ### todo: still needed? (if not, remove them also from further below) ###
                                    newf.setTagString("sfVariation",sfvarname)
                                    newf.setTagString("p4suffix","")
                                    newf.setTagString("sysVariation",sfvarname)
                                    ###########################
                                    mcasvchannels.add(newname.Data())

                # if no nominal analysis was requested, we can remove the nominal channels
                if not doNominal:
                    f.detachFromBase()
                else:
                    f.setTagString(".mcasv.channel",f.GetName())


    # Add some nominal top level tags, even if systematics aren't being added
    samples.setTagString("variationType", "nominal")
    samples.setTagString("variationName", "nominal")
    samples.setTagString("weightname", "nominal")
    samples.setTagString("sfVariation", "nominal")
    samples.setTagString("p4Variation", "nominal")
    samples.setTagString("p4suffix", "")
    samples.setTagString("sysVariation", "nominal")
    samples.setTagString("ffVariation", "nominal")
    samples.setTagString("ffVariation_SS", "nominal")
    samples.setTagString("ffVariation_trig", "nominal")
    samples.setTagString("ffVariation_trig_SS", "nominal")
    samples.setTagString('ffObsType', "")
    samples.setTagString('ffLepType', "")
    samples.setTagString('ffHistType', "")

    # block systematics for data driven samples
    for sf in samples.getListOfSampleFolders("*"):
        if sf.getTagBoolDefault("isDataDriven",False):
            # it is important that we do NOT overwrite the .mcasv.channel tag here
            # otherwise, we would re-use the same cut hierarchy several times in parallel
            # which could become really confusing and possibly produce subtly wrong results
            sf.setTagString("cand",sf.getTagStringDefault("~cand.original",sf.getTagStringDefault("~cand","")))
            sf.setTagString("weightname","nominal")
            sf.setTagString("variationName","nominal")
            sf.setTagString("variationType","nominal")

    # possibly print how the folder looks like now
    if config.getTagBoolDefault("showChannels",False):
        QFramework.INFO("after taking care of channel and systematics setup, your sample folder looks like this:")
        samples.printContents("r2dt")

    # save the whole collection of channels (including now systematics)
    # for access later when creating the MCASV
    runtime = config.getFolder("runtime+")
    if not dummy:
        for i, channel in enumerate(mcasvchannels):
            if "ROOT" in str(type(channel)):
                from QFramework import WARN
                QFramework.WARN("Please make sure that every mcasvchannel is only booked once. The python set doesn't help when adding c++ ROOT instances (" + str(type(channel)) + ".")
            runtime.setTagString("mcasvchannels."+str(i), channel)

    return
