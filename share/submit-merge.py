#!/bin/env python

from SubmissionHelpers import submit
import os, subprocess, math
from datetime import datetime

# This script sends batch jobs to perform tqmerge of many files in two stages.
# The number of files merged per job in the first ('premerging') stage can be
# set via the maxFiles variable. In the second stage the premerged files are
# merged to the final output

def main(args):
    from CommonAnalysisHelpers.submit import makePreMergeTaskList,makeFinalMergeTask
    
    ctrl = submit.guessSubmissionController(args)

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
    if args.final_local:
        from SubmissionHelpers.submissionControllers.local import LocalController
        local_ctrl = LocalController()
        allDone = local_ctrl.submitTasks(args,[finalTask])
    else:
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
    parser.add_argument('--vmemorypernode', default=-1, type=float, help='memory to be requested per node in the job (in MB)')
    parser.add_argument('--memoryFinal', default='8000', type=float, help='memory requested for final job',required=False)
    parser.add_argument('--time', default='60', type=float, help='time requested per job',required=False)
    parser.add_argument('--queue', default='', type=str, help='name of queue',required=False)
    parser.add_argument('--final-local', action="store_true",help="run the final merge locally", default=False)
    parser.add_argument('-n', '--name', metavar='NAME', type=str, dest="name", help='output folder name')
    parser.add_argument('-t', '--traceid', metavar='TRACEID', type=str, dest="traceid", default="analyze", help='trace ID of the analysis sample visitor')
    parser.add_argument('-m', '--downmerge', metavar='DOWNMERGE', type=str, dest="downmerge", default=None, help='tag prefix to trigger downmerging')
    parser.add_argument('-p', '--patch', metavar='style.txt', type=str, dest="patch", default=[], action='append', help='a patch style file to apply to every component')
    parser.add_argument('-s', '--sfname', metavar='SFNAME', type=str, dest="sfname",default='samples', help='name of the sample folder being merged')
    parser.add_argument('-d', '--depth', metavar='DEPTH', type=int, default=2, dest="depth", help='output folder split depth')
    parser.add_argument('-noSum', dest='sum', action='store_false', help='sum elements in TQFolders (counters, histograms)')
    parser.add_argument('-Sum', dest='sum', action='store_true', help='sum elements in TQFolders (counters, histograms)')
    parser.set_defaults(sum=True)
    parser.add_argument('-q', '--quiet', dest='quiet', action='store_true')
    parser.add_argument('-v', '--verbose', dest='verbose', action='store_true',default=False)

    # use the argument parser to read the command line arguments and config options from the config file
    #from os.path import splitext
    args = parser.parse_args()
    
    import QFramework
    import ROOT
    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # call the main function
    main(args)

