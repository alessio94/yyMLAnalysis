#!/bin/env python

from CommonAnalysisHelpers import common,submit
import os, subprocess, math
from datetime import datetime

# This script sends batch jobs to perform tqmerge of many files in two stages.
# The number of files merged per job in the first ('premerging') stage can be
# set via the maxFiles variable. In the second stage the premerged files are
# merged to the final output

def makePreMergeTaskList(args_, preMergeDir, setup):

    inputFiles = []
    for thisInput in args.inputs:
        if os.path.isfile(thisInput):
            inputFiles.append(os.path.abspath(thisInput))
        elif os.path.isdir(thisInput):
            for r, d, f in os.walk(thisInput):
                for thisFile in f:
                    inputFiles.append(os.path.join(os.path.abspath(thisInput), thisFile))
        else:
            print "Ignoring input ", thisInput, ", which appears to be neither a file nor a directory"

    # Set number of files per premerge job to sqrt(total nFiles) if not user configured
    if args.maxFiles == -1: args.maxFiles = int(math.ceil(math.sqrt(len(inputFiles))))

    # Assign input files to pre-merge jobs
    jobFilesDict = {}
    assignedFiles = 0
    for thisFile in inputFiles:
        nPart = int(assignedFiles)/int(args.maxFiles)
        partLabel = args.identifier+".part"+str(nPart)
        if not partLabel in jobFilesDict.keys(): jobFilesDict[partLabel]=[]
        jobFilesDict[partLabel].append(thisFile)
        assignedFiles+=1

    # Configure premerge jobs and add them to list
    retList = []

    for key in jobFilesDict.keys():
        preMergedName = preMergeDir+'/'+key+'.root'
        payload = 'tqmerge '
        for thisFile in jobFilesDict[key]: payload=payload+thisFile+" "
        payload = payload+ '--output '+ preMergedName
        if(args.name):
            payload = payload+ ' -n ' + args.name
        if (args.traceid):
            payload = payload+ ' -t ' + args.traceid
        if(args.downmerge): 
            payload = payload+ ' -m ' + args.downmerge
        if (len(args.patch) > 0):
            payload = payload+ ' -p '+' -p '.join(args.patch)
        if(args.sfname): 
            payload = payload+ ' -s ' + args.sfname
        if(args.depth):
            payload = payload+ ' -d ' + str(args.depth)
        if(args.sum):
            payload = payload+ ' -Sum '
        if(args.quiet):
            payload = payload+ ' -q '
        if(args.verbose):
            payload = payload+ ' -v '

        logFile = os.path.join(args.logpath,key+".log")
        thisTask = submit.task(key,payload, setup=setup, memory=args.memory, time=args.time, inputs=[], outputs=[preMergedName], logFile=logFile, errFile=logFile)
        retList.append(thisTask)

    return retList

def makeFinalMergeTask(args_, retList, setup):

    # Then config final job, which combines the premerged files, and add it to the list
    preMergedNames = []
    for thisTask in retList: 
        preMergedNames.append(thisTask.output[0])
    payload = 'tqmerge '
    for thisPreMergedName in preMergedNames: payload=payload+thisPreMergedName+" "
    payload = payload+ '--output '+ os.path.realpath(args.output) + ' '
    if (args.traceid): payload = payload+ ' -t ' + args.traceid
    logFile = os.path.join(args.logpath,args.identifier+".final.log")
    finalTask = submit.task(args.identifier+".final",payload, setup=setup, memory=args.memoryFinal, time=args.time, inputs=preMergedNames, outputs=[os.path.realpath(args.output)], dependencies=retList, logFile=logFile, errFile=logFile)

    return finalTask


def main(args):
    ctrl = submit.guessSubmissionController()

    # Create directory for temporary (premerged) output
    preMergeDir = os.path.dirname(os.path.realpath(args.output))+'/premerged_'+args.identifier

    # Get setup
    setup = submit.getSetupCommand(args)
    setupPath = os.environ['CAFANALYSISSETUP']
    if len(setupPath)>0: setup.append("source "+setupPath)

    # Compile list of premerge tasks
    retList = makePreMergeTaskList(args, preMergeDir, setup)

    # Create final task
    finalTask = makeFinalMergeTask(args, retList, setup)

    # Submit tasks and remove temporary directory when it's no longer needed
    allDone = ctrl.submitTasks(args,[finalTask])
    if allDone: print "All jobs are done. You may wish to delete the temporary output by calling 'rm -r ", preMergeDir, "'"

    print("Done")
    

if __name__ == "__main__":

    # create a pre-configured argument parser (some parts of the backend code rely on certain arguments being available, this does *NOT* mean that you need to specify each argument explicitly though)
    parser = submit.MinimalArgumentParser()
    #add a few more specific arguments to the argparser:
    parser.add_argument("inputs",type=str,nargs="+",help="list of input directories and/or files")
    parser.add_argument('--output', default='', type=str, help='output file',required=True)
    parser.add_argument('--maxFiles', default='-1', type=int, help='number of files per pre-merge job',required=False)
    parser.add_argument('--memory', default='4000', type=float, help='memory requested per premerge job',required=False)
    parser.add_argument('--memoryFinal', default='8000', type=float, help='memory requested for final job',required=False)
    parser.add_argument('--time', default='60', type=float, help='time requested per job',required=False)
    parser.add_argument('--queue', default='', type=str, help='name of queue',required=False)

    parser.add_argument('-n', '--name', metavar='NAME', type=str, dest="name", help='output folder name')
    parser.add_argument('-t', '--traceid', metavar='TRACEID', type=str, dest="traceid", default="asv", help='trace ID of the analysis sample visitor')
    parser.add_argument('-m', '--downmerge', metavar='DOWNMERGE', type=str, dest="downmerge", default=None, help='tag prefix to trigger downmerging')
    parser.add_argument('-p', '--patch', metavar='style.txt', type=str, dest="patch", default=[], action='append', help='a patch style file to apply to every component')
    parser.add_argument('-s', '--sfname', metavar='SFNAME', type=str, dest="sfname",default='samples', help='name of the sample folder being merged')
    parser.add_argument('-d', '--depth', metavar='DEPTH', type=int, default=2, dest="depth", help='output folder split depth')
    parser.add_argument('-noSum', dest='sum', action='store_false', help='sum elements in TQFolders (counters, histograms)')
    parser.add_argument('-Sum', dest='sum', action='store_true', help='sum elements in TQFolders (counters, histograms)')
    parser.set_defaults(sum=True)
    parser.add_argument('-q', '--quiet', dest='quiet', action='store_true')
    parser.add_argument('-v', '--verbose', dest='verbose', action='store_true',default=False)

    import QFramework
    import ROOT
    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # use the argument parser to read the command line arguments and config options from the config file
    #from os.path import splitext
    args = parser.parse_args()

    # call the main function
    main(args)

