#!/bin/env python

import argparse


def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("sourceDir", help="absolute path of source directory", type=str)
    parser.add_argument("setupDir", help="absolute path of setup directory", type=str)
    parser.add_argument("CAFCoreSetupDir", help="absolute path of directory containing CAFCore's setup script cafsetup.sh", type=str)
    parser.add_argument("binDir",help="absolute path of binary directory", type=str)
    args = parser.parse_args()
    return args


if __name__ == "__main__":
    """
    This file is automatically called during cmake. It writes a setup script that user executes before running.
    """

    from os.path import join as pjoin
    
    args = parseArguments()
    scriptPath = pjoin(args.binDir, "setupAnalysis.sh")

    with open(scriptPath, "w") as f:
        f.write("#!/bin/bash\n")
        f.write("# this is an auto-generated setup script\n\n")

        f.write("storeOptions=\"$-\"\n")
        f.write("export CAFANALYSISBASE=\""+args.sourceDir+"\"\n")
        f.write("export ANALYSIS_BASE_DIR=\""+args.sourceDir+"\"\n")

        # Create environment variable for location of executables (separate
        # multiple paths with ":")
        f.write("export CAFANALYSISSHARE=\""+args.sourceDir+"/share\"\n\n")

        # Add location of executables to $PATH
        f.write("# Add paths from $CAFANALYSISSHARE to $PATH (while avoiding duplicates)\n")
        f.write("set +e\n")
        f.write("for directory in `echo $CAFANALYSISSHARE:$CAFANALYSISBASE/tools:$CAFANALYSISBASE/tools/statistics:$CAFANALYSISBASE/tools/plotComparisons:$CAFANALYSISBASE/tools/TheorySystematics:$CAFANALYSISBASE/tools/FakeFactors | tr \":\" \" \"` ; do\n")
        f.write("\t# Remove trailing slash\n")
        f.write("\tdir=${directory%/}\n")
        f.write("\t# If $dir is not in $PATH yet, add it at the end.\n")
        f.write('\techo $PATH | tr ":" "\\n" | grep -q $dir\n')
        f.write("\tif [[ $? -ne 0 ]] ; then\n")
        f.write("\t\texport PATH=$PATH:$dir\n")
        f.write("\tfi\n")
        f.write("done\n")
        f.write("if [[ \"$storeOptions\" == *\"e\"* ]] ; then\n")
        f.write("\tset -e\n")
        f.write("fi\n\n")

        autoCompletePath = pjoin(args.setupDir,"setupAutoComplete.sh")
        setupLocalPath = pjoin(args.setupDir,"setupLocal.sh")
        cafsetupPath = pjoin(args.CAFCoreSetupDir,"cafsetup.sh")

        # Source other scripts
        f.write("if [ -f "+autoCompletePath+" ]; then\n\tsource "+autoCompletePath+"\nfi\n")
        f.write("if [ -f "+cafsetupPath+" ]; then \n\tsource "+cafsetupPath+"\nfi\n")
        f.write("if [ -f "+setupLocalPath+" ]; then \n\tsource "+setupLocalPath+"\nfi\n")
        for addpackage in ["SubmissionHelpers","RooFitUtils","RooFitExtensions","RooUnfold", "CommonSystSmoothingTool"]:
            thepath = pjoin(args.binDir,addpackage,"setup.sh")
            f.write("if [ -f "+thepath+" ]; then \n\tsource "+thepath+"\nfi\n")

        #add the path to this script (overwriting what might have been set by cafsetup.sh !) :
        f.write("\n#export path to this script for easier re-setup, e.g., for batch submission\n")
        f.write("export PYTHONPATH=$PYTHONPATH:"+args.binDir+"\n")
        f.write("export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"+args.binDir+"\n")
        f.write("export CAFANALYSISSETUP="+scriptPath+"\n")
        f.write("export ANALYSIS_SETUP="+scriptPath+"\n")
 
        #For Nikhef batch submission to not clog tmp dir
        f.write("if [ -z $TMPDIR ] && [[ $(hostname) == *.nikhef.nl* ]]; then\n\texport CAF_USE_TEMP_DIR=0 \nfi\n")

        f.write("export ANALYSIS_BUILD_DIR="+args.binDir+"\n")
        f.write("export ANALYSIS_BASE_DIR="+args.sourceDir+"\n")
