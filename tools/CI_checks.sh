#!/bin/bash

# This script searches all files that match share/config/samples/maps/*/*.map
# and prints a warning if they have overlapping/duplicate DSID mappings


HWWAnalysis_checkMapFiles(){

    listOfDuplicateDSIDs=""

    mapsPath="share/xAOD/config/samples/maps/xAOD-Example.map"

    # CAFANALYSISBASE might not be set in a CI job. In that case, hope that we are in the right location
    for file in `[ -z "$CAFANALYSISBASE" ] && ls $mapsPath || ls $CAFANALYSISBASE/$mapsPath` ; do
        for line_orig in `cat $file | sed 's/ /_/g'` ; do

            # ignore spaces in the beginning
            line=$(echo "$line_orig" | sed 's/^\(_\| \)*//')

            # if the line starts with #, ignore it
            echo "$line" | sed '/^#/{q10}' > /dev/null
            [[ $? -eq 10 ]] && continue

            # search for 6-digit (maybe with extension like "w10") DSID in $file
            DSID=$(echo $line | sed 's/\([0-9]\{6\}\)\(w[0-9]\{2\}\)\?\(_\| \).*/\1\2/')
            nMatches=$(grep -r "$DSID" $file | grep -v "^ *#" | wc -l)

            # if more than one line matches this DSID and the DSID hasn't been reported before, add it to the list of DSIDs and print warning
            ! [[ nMatches -eq 1 ]] && ! [[ $(echo "$listOfDuplicateDSIDs" | grep "$DSID") ]] && listOfDuplicateDSIDs="$listOfDuplicateDSIDs,$DSID" && echo "duplicate: $DSID, file: $file" > /dev/stderr && grep -r "$DSID" $file > /dev/stderr

        done
    done

    set -e

    [ "$listOfDuplicateDSIDs" == "" ]
    return $?
}
