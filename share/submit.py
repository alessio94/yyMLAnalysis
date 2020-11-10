#!/bin/env python

from CommonAnalysisHelpers import common,submit
import os

def main(args):
    """submit your analysis to a batch system"""
    #read the main analysis config file, we need some information about what our analysis is intended to work on
    executable_name = args.executable.split(".")[0] # remove the '.py'
    config = common.getConfigOptions(args, executable_name)
    #the lazy way to get all the needed instructions to ensure the environment on the batch node behaves like your current analysis setup:
    setup = submit.getSetupCommand(args)
    #run the setup script for our analysis (CAFExample), should this be moved to submit.getSetupCommand ?
    setupPath = os.environ['CAFANALYSISSETUP']
    if len(setupPath)>0: setup.append("source "+setupPath)

    pathManager = QFramework.TQPathManager.getPathManager()
    outputFileNameTemplate=pathManager.getTargetPath("{output}/unmerged_{globalIdentifier}/unmerged_{{identifier}}.root".format(output=args.output, globalIdentifier=args.identifier))

    templateCommand="{executable} {config} --restrict {{restrict}} --jobID {{identifier}} --options outputFile={outFileTemplate} {{options}}".format(executable=args.executable, config=",".join(args.config), outFileTemplate=outputFileNameTemplate)

    if args.options:
        templateCommand += " " + " ".join(args.options) #forward options from command line to individual jobs

    #convenience method to make a somewhat smart splitting of jobs (the maxSampleCount and maxSampleSize arguments allow to control the splitting into sub-jobs. This can yield a significant improvement in turn-over time!):
    #note: this method is somewhat targeted at the analyze.py step/ written with that one in mind
    if executable_name == "initialize":
        taskList = submit.makeTaskList(args, args.jobs, config, templateCommand, setup=setup, inputs=[], outputs=[outputFileNameTemplate])
    else:
        taskList = submit.makeSmartTaskList(args, args.jobs, config, templateCommand, maxSampleCount=args.maxSampleCount, maxSampleSize=args.maxSampleSize, setup=setup, inputs=[], outputs=[outputFileNameTemplate])
    ctrl = submit.guessSubmissionController()
    allDone = ctrl.submitTasks(args,taskList)
    if allDone: common.mergeFilesQuery(args, config)
    print("Done")


if __name__ == "__main__":

    # create a pre-configured argument parser (some parts of the backend code rely on certain arguments being available, this does *NOT* mean that you need to specify each argument explicitly though)
    parser = submit.MinimalArgumentParser()
    #add a few more specific arguments to the argparser:
    parser.add_argument("config",type=str,nargs="+",metavar="config.cfg",help="config to be submitted",default=[])
    parser.add_argument('--jobs', default='flatNTuple/config/jobLists/jobs_ZjetsFF_analyze.txt', help='name of the queue to submit to',required=True)
    parser.add_argument('-o','--options',dest='options',type=str,nargs="+",default=[],metavar="key=value",help="changes to be made to the configuration")
    parser.add_argument('--maxSampleSize', default=-1, type=float, help='split jobs such that each job processes only so many input files that their cummulative size is below this value (in MB)')
    parser.add_argument('--maxSampleCount', default=-1, type=float, help='split jobs such that each job processes only up to this many input files')
    parser.add_argument("--executable",type=str,metavar="executable.py",help="executable to be submitted (default = analyze.py)", default="analyze.py")
    parser.add_argument('--mergeConfig', default="common/submission/merge.cfg", type=str, help='merge file to be read to build merge command string')
    parser.add_argument('--merge', action="store_const", default=False, const=True, help='merge files when they have succeeded automatically')

    import QFramework
    import ROOT
    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # use the argument parser to read the command line arguments and config options from the config file
    #from os.path import splitext
    args = parser.parse_args()

    # call the main function
    main(args)
