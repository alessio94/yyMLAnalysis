#!/usr/bin/env python

############################################################
#
# Script that adapts an lwtnn (https://github.com/lwtnn/lwtnn) model json file
# to the needs of CAFCore and HWWAnalysisCode. This essentially means that
# the 'name' entries of the json file are modified by adding a CAFCore-like
# observable expression found in a ntuple definition file. 
# This is also described here:
# https://gitlab.cern.ch/atlas-caf/caflwtnn/tree/master
#
############################################################

import os
import json

debug = False

def readNTupleDefinitionFile(nTupleDefinitionFile):
    """
    reads the ntuple definition file when the ntuples for the DNN training were produced and
    tries to read the observable expressions in a python dictionary.
    This is very tailored and prone to fail in case of changes to the
    ntuple definition file.
    """
    from CommonAnalysisHelpers import common
    f = open(common.findConfigPath(args.nTupleDefinitionFile))
    line = f.readlines()[0]
    myl = line.replace("float", "").replace("int", "").replace("bool", "").split(",")
    ntupDict = {}
    for m in myl:
        if len(m.split("<<")) < 2:
            print "WARNING: {} has no '<<'".format(m.split("<<"))
            continue
        key = m.split("<<")[0].strip()
        definition = m.split("<<")[1].strip().replace(";","")
        ntupDict[key] = definition
    return ntupDict

def main(args):

    # load initial input variables json file
    print "INFO: read initial input variables file to manipulate from '{}'".format(args.networkInputFile)
    with open(args.networkInputFile, 'r') as f:
        variables = json.load(f)

    # get variable expressions as dictionary e.g. dict = {"Mll":"$(Mll)", ...}
    print "INFO: read ntuple definition file for retrieving observable expressions from '{}'".format(args.nTupleDefinitionFile)
    variableExpressions = readNTupleDefinitionFile(args.nTupleDefinitionFile)

    # loop over all entries and add CAF observable expressions to 'name' key from above dictionary
    print "INFO: adding observable expressions to variables file..."
    for varDict in variables["inputs"]:
        name = varDict["name"]
        if debug: print "INFO: variable name = {}".format(name)
        if name in variableExpressions.keys():
            if debug: print "INFO: Found key in varDict with name '{}'".format(name)
            varDict["name"] = "{}={}".format(name, variableExpressions[name])
        else:
            print "WARNING: No observable expression found for observable with name '{}'. Consider adding one manually, skipping this entry for now...".format(name)

    # write out manipulated file under new name which is either specified by argument or constructed from input filename
    if args.networkOutputFile: outfile = args.networkOutputFile
    else: outfile = "{}_edited.json".format(args.networkInputFile.replace(".json", ""))
    with open(outfile, "w") as jsonfile:
        print "INFO: write edited input variables file to '{}'".format(outfile)
        json.dump(variables, jsonfile)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument("--networkInputFile", type=str, required = True)
    parser.add_argument("--networkOutputFile", type=str, default="")
    parser.add_argument("--nTupleDefinitionFile", type=str, default="config/nTuples/VBF/mva-ntuple.txt")
    args = parser.parse_args()
    main(args)
