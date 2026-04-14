# ZpMET CAF Analysis Setup
# ANA-EXOT-2023-28 — Z' + MET, Run 3

## Directory structure

    ZpMET/
    ├── config/
    │   ├── master/
    │   │   ├── prepare-ZpMET.cfg        ← Step 1
    │   │   ├── initialize-ZpMET.cfg     ← Step 2
    │   │   ├── analyze-ZpMET.cfg        ← Step 3
    │   │   └── visualize-ZpMET.cfg      ← Step 4
    │   ├── samples/
    │   │   ├── ZpMET-samples.txt        ← DSID → SampleFolder path mapping
    │   │   ├── XSec/
    │   │   │   └── ZpMET_XS_13p6TeV.csv ← cross-section table
    │   │   └── inputFileLists/
    │   │       └── ZpMET-mcFileList.txt  ← list of input ntuple paths
    │   ├── cuts/
    │   │   └── ZpMET-cuts.def           ← event selection tree
    │   ├── histograms/
    │   │   └── ZpMET-histograms.txt     ← histogram definitions
    │   └── visualization/
    │       ├── style/
    │       ├── processes/
    │       └── cuts/

## Running the workflow

From inside CAFExample/share/ (after sourcing setupAnalysis.sh):

    prepare.py    ZpMET/config/master/prepare-ZpMET.cfg
    initialize.py ZpMET/config/master/initialize-ZpMET.cfg
    analyze.py    ZpMET/config/master/analyze-ZpMET.cfg
    visualize.py  ZpMET/config/master/visualize-ZpMET.cfg

## Before running — TODOs

1. **Branch names**: verify variable names in ZpMET-cuts.def match your
   EasyJet ntuple branches. Check with:
       tqroot -sfr <your_ntuple.root>
       // in ROOT: t->Print()  or  t->Scan("*",""," ",5)

2. **TTree name**: confirm the TTree name inside your ntuples
   (default assumed: "output"). Update mcFileListTreeName in initialize cfg.

3. **XSec file**: create ZpMET_XS_13p6TeV.csv with columns:
       DSID, xsec[pb], kFactor, filterEfficiency

4. **Input file list**: create ZpMET-mcFileList.txt listing your EOS ntuple
   paths, one per line, e.g.:
       root://eosatlas.cern.ch//eos/atlas/.../ZpMET/Run3/ntuples/signal_546587.root

5. **Background DSIDs**: fill in bkg entries in ZpMET-samples.txt

6. **Weight branches**: update weightExpression in cuts.def to match
   your actual weight branch names (pileup, lepton SF, etc.)
