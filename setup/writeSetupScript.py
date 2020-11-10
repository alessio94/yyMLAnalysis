#!/bin/env python

import argparse
import os


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

    args = parseArguments()
    scriptPath = os.path.join(args.binDir, "setupAnalysis.sh")

    with open(scriptPath, "w") as f:
        f.write("#!/bin/bash\n")
        f.write("# this is an auto-generated setup script\n\n")

        f.write("storeOptions=\"$-\"\n")
        f.write("export CAFANALYSISBASE=\""+args.sourceDir+"\"\n")
        # Create environment variable for location of executables (separate
        # multiple paths with ":")
        f.write("export CAFANALYSISSHARE=\""+args.sourceDir+"/share\"\n\n")

        # Add location of executables to $PATH
        f.write("# Add paths from $CAFANALYSISSHARE to $PATH (while avoiding duplicates)\n")
        f.write("set +e\n")
        f.write("for directory in `echo $CAFANALYSISSHARE:$CAFANALYSISBASE/tools:$CAFANALYSISBASE/tools/statistics | tr \":\" \" \"` ; do\n")
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

        autoCompletePath = os.path.join(args.setupDir,"setupAutoComplete.sh")
        setupLocalPath = os.path.join(args.setupDir,"setupLocal.sh")
        cafsetupPath = os.path.join(args.CAFCoreSetupDir,"cafsetup.sh")

        # Source other scripts
        f.write("if [ -f "+autoCompletePath+" ]; then\n\tsource "+autoCompletePath+"\nfi\n")
        f.write("if [ -f "+cafsetupPath+" ]; then \n\tsource "+cafsetupPath+"\nfi\n")
        f.write("if [ -f "+setupLocalPath+" ]; then \n\tsource "+setupLocalPath+"\nfi\n")
        #add the path to this script (overwriting what might have been set by cafsetup.sh !) :
        f.write("\n#export path to this script for easier re-setup, e.g., for batch submission\n")
        f.write("export PYTHONPATH=$PYTHONPATH:"+args.binDir+"\n")
        f.write("export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:"+args.binDir+"\n")
        f.write("export CAFANALYSISSETUP="+scriptPath+"\n")
