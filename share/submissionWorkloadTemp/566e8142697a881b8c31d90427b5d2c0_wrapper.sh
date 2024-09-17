#!/bin/bash -l
cd /eos/home-n/nischmit/AnalysisExample/CAFExample/share
function cleanup(){
rm /eos/home-n/nischmit/AnalysisExample/CAFExample/share/submissionWorkloadTemp/566e8142697a881b8c31d90427b5d2c0.sh /eos/home-n/nischmit/AnalysisExample/CAFExample/share/submissionWorkloadTemp/566e8142697a881b8c31d90427b5d2c0_wrapper.sh
rv=$?
rmdir /eos/home-n/nischmit/AnalysisExample/CAFExample/share/submissionWorkloadTemp 2>/dev/null
exit $rv
}
trap "cleanup; exit 0" EXIT
trap "cleanup; echo 'The payload was requested to exit via SIGTERM, please check your ressource allocation';  exit \$?" TERM INT SIGTERM SIGCONT SIGKILL
source /eos/home-n/nischmit/AnalysisExample/CAFExample/share/submissionWorkloadTemp/566e8142697a881b8c31d90427b5d2c0.sh
