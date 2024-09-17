#!/usr/bin/env bash
cd /tmp
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
cd /afs/cern.ch/user/n/nischmit/public/EOS/AnalysisExample/build
asetup --restore
source /cvmfs/atlas.cern.ch/repo/sw/software/25.2/AnalysisBaseExternals/25.2.7/InstallArea/x86_64-el9-gcc13-opt/env_setup.sh
source /cvmfs/atlas.cern.ch/repo/sw/software/25.2/AnalysisBaseExternals/25.2.7/InstallArea/x86_64-el9-gcc13-opt/setup.sh
source /afs/cern.ch/user/n/nischmit/public/EOS/AnalysisExample/build/setupAnalysis.sh
cd /eos/home-n/nischmit/AnalysisExample/CAFExample/share
source /afs/cern.ch/user/n/nischmit/public/EOS/AnalysisExample/build/setupAnalysis.sh
analyze.py HHML_example/config/master/analyze-HHML_3L.cfg --restrict /data/?/?/user.aaggarwa.00428648.r14755_p5604_p6269.41068078.PAOD_HH3L._000016.pool/,/data/?/?/user.aaggarwa.00428700.r14755_p5604_p6269.41068078.PAOD_HH3L._000200.pool/ --jobID 1709_1540_data_X_X.part0 --options outputFile=/afs/cern.ch/user/n/nischmit/public/EOS/AnalysisExample/CAFExample/../output/batchOutput/unmerged_1709_1540/unmerged_1709_1540_data_X_X.part0.root prettyPrint=false lineUpdates=false