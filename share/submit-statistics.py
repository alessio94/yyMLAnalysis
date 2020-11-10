#!/bin/env python

from CommonAnalysisHelpers import common,submit
from CommonAnalysisHelpers.task import task,taskStatus

import QFramework

def makeChunks(configs,n):
    import random
    rng = random.Random(42) # set a fixed random seed to get the same shufflings everytime
    rng.shuffle(configs)
    return [configs[i:i + n] for i in xrange(0, len(configs), n)]


def makeID(identifiers):
    common = None
    for i in identifiers:
        if not common:
            common = i
        else:
            for c in range(1,min(len(i),len(common))):
                if common[0:c] != i[0:c]:
                    common = common[0:c-1]
                    break
    ident = common
    for i in identifiers:
        if len(common) >= len(i):
            continue
        else:
            ident = ident + "_" + i[len(common)-1:]
    ident = ident.replace(".","_")
    while "__" in ident:
        ident = ident.replace("__","_")
    if len(ident) > 200:
        return ident[0:200]+"_etc"
    else:
        return ident

def main(args):
    """submit your statistics analysis to a batch system"""
    from os import listdir
    from os import environ
    from os.path import isfile,splitext
    from os.path import join as pjoin
    configdir = pjoin(args.directory,"configs")
    configs = [pjoin(configdir,f) for f in listdir(configdir) if isfile(pjoin(configdir, f))]
    common.mkdir(args.logpath)
    
    #the lazy way to get all the needed instructions to ensure the environment on the batch node behaves like your current analysis setup:
    setup = submit.getSetupCommand(args)
    #run the setup script for our analysis
    setupPath = environ['CAFANALYSISSETUP']
    if len(setupPath)>0: setup.append("source "+setupPath)

    tasks = []
    chunks = makeChunks(configs,args.chunk)
    for chunk in chunks:
        inputs = []
        outputs = []
        commands = []
        identifiers = []
        for config in chunk:
            thiscfg = QFramework.TQFolder("config")
            thiscfg.importFromTextFile(config)
            identifier = thiscfg.getTagStandardStringDefault("identifier",splitext(config)[0])
            addoutput = thiscfg.getTagVStandardString("expectedOutput")
            addinput = thiscfg.getTagVStandardString("expectedInput")        
            if len(addoutput)==0 or len(addinput)==0:
                print("faulty config file "+config+" does not have <expectedInput>/<expectedOutput> tag")
                continue
            inputs.extend(addinput)
            outputs.extend(addoutput)
            identifiers.append(identifier)
            commands.append("statistics.py "+config)
        t = task(identifier=args.identifier+"_"+makeID(identifiers),args=args,setup=setup,outputs=outputs,inputs=inputs,payload=commands)
        tasks.append(t)

    ctrl = submit.guessSubmissionController()
    ctrl.submitTasks(args,tasks)
    print("Done")
    

if __name__ == "__main__":

    # create a pre-configured argument parser (some parts of the backend code rely on certain arguments being available, this does *NOT* mean that you need to specify each argument explicitly though)
    parser = submit.MinimalArgumentParser(executable="statistics.py")
    #add a few more specific arguments to the argparser:
    parser.add_argument("directory",type=str,metavar="/path/to/jobs",help="directory of configs be submitted")
    parser.add_argument("--chunk",type=int,metavar="N",help="number of configs to be grouped into a single chunk and submitted as a job",default=1)
    parser.add_argument('--queue', default='', type=str, help='name of queue',required=False)
    import QFramework
    import ROOT
    # ignore command line arguments since ROOT is very greedy here (and tends to choke from it!)
    ROOT.PyConfig.IgnoreCommandLineOptions = True

    # use the argument parser to read the command line arguments and config options from the config file
    #from os.path import splitext
    args = parser.parse_args()
    
    # call the main function
    main(args)

