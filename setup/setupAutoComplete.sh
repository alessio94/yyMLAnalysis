#!/bin/bash

######################################################################
# Auto-completion of config files for python scripts
######################################################################


# Names of python scripts that need config files. Auto-complete
# will only work for these scripts and only if they are in the
# $CAFAutoCompleteDirectories.
CAFPythonScriptDirectories="$CAFANALYSISSHARE:$CAFANALYSISBASE/tools"
pythonScripts="prepare.py initialize.py analyze.py visualize.py submit.py statistics.py runGridScanner.py"


_CAFdoAutoCompletion(){
    ##################################################################
    # This function has two purposes:
    # 1. Its return value indicates if the invoking python script
    #    is in $CAFPythonScriptDirectories (i.e. if auto-completion
    #    should be used).
    # 2. It returns a concatenated and formatted (with trailing "/")
    #    list of directories to search possible completions in.
    #
    ##################################################################

    local command=$1
    local CAFAnalysisShare=""

    IFS=:
    
    # Check if $command is known. If it does not point to an
    # executable, exit script.
    local pathToCommand=$(which $command 2>/dev/null)
    # resolve leading "~/" and symbolic links
    pathToCommand="${pathToCommand/#\~/$HOME}"
    if [ ! -x "$pathToCommand" ] ; then
	return 0
    fi
    
    # Check if $command is in $CAFAutocompleteDirectores.
    # If not, exit the script.
    local commandInExecutables=0
    for directory in $CAFPythonScriptDirectories ; do
    	# resolve leading "~/" and symbolic links
    	local dir="${directory/#\~/$HOME}"
    	if [[ $dir != *"/" ]] ; then
    	    dir="$dir/"
    	fi
    	# The following two if statements are a messed up way of
    	# Checking that $command points to a script in $dir.

    	# Check if $command is in $dir or one of its subdirectories.
    	if [[ $pathToCommand == "$dir"* ]] ; then
    	    local trailingPath=${pathToCommand/$dir/}
    	    local slashes=${trailingPath//[^\/]}
    	    # If number of slashes in trailing path is zero, $command
    	    # must be in the same directory as $dir.
    	    if [ ${#slashes} -eq 0 ] ; then
    		# The executable is located in $CAFANALYSISSHARE
    		commandInExecutables=1
    	    fi
    	fi
    done

    for directory in $CAFPythonScriptDirectories ; do
    	# resolve leading "~/" and symbolic links
    	local dir="${directory/#\~/$HOME}"
    	if [[ $dir != *"/" ]] ; then
    	    dir="$dir/"
    	fi
    	if [[ $CAFAnalysisShare != "" ]] ; then
    	    CAFAnalysisShare+=":"
    	fi
    	CAFAnalysisShare+="$dir"
    done

    unset IFS
    echo $CAFAnalysisShare
    return $commandInExecutables
}


_CAFFindPossibleCompletions(){
    ##################################################################
    # This function finds possible suggestions for completion by
    # listing and modifying the files and directories in
    # $CAFAnalysisShare (separated by ":").
    #
    # All matching files and directories are written into $COMPREPLY.
    # If option = 1 or 3 are chosen, a completion is immediately exe-
    # cuted when the substring $immediateCompleteStr is found. The
    # user is not given any options, even if there would be useful
    # ones.
    #
    ##################################################################

    local thisWord=$1
    local thisDir=$2
    local CAFAnalysisShare=$3
    local option=$4
    local immediateCompleteStr=$5

    # echo ""
    # echo "_CAFFindPossibleCompletions called with arguments:"
    # echo "thisWord             : $thisWord"
    # echo "thisDir              : $thisDir"
    # echo "CAFAnalysisShare     : $CAFAnalysisShare"
    # echo "option               : $option"
    # echo "immediateCompleteStr : $immediateCompleteStr"

    

    # The following variables are used:
    # path     : Location that is searched for completions. This
    #            variable loops over the working directory and
    #            directories in $CAFANALYSISSHARE.
    # thisDir  : Already typed directories contained in $thisWord and
    #            relative to $path.
    # thisPath : Combines the two previous variables: $path$thisDir


    IFS=:

    # The content of these two variables will be used for auto-compl.
    local listOfFiles=""
    local listOfDirs=""
    local listOfFilesInDot=""
    local listOfDirsInDot=""

    # Store the $immediateCompleteStr in an array
    # ($immediateCompleteDirs) to simplify looping. 
    local slashes="${immediateCompleteStr//[^\/]}/"
    local immediateCompleteDirs=()
    local arrayIndex=0
    for (( i=0; i<${#slashes}; i++)) ; do
	iPlusOne="$[ i+1 ]"
	tmp=`echo $immediateCompleteStr | cut -d "/" -f$iPlusOne -`
	if [[ "$tmp" != "" ]] ; then
	    if [ "$i" -lt "$[ ${#slashes}-1 ]" ] ; then
		tmp="$tmp/"
	    fi
	    immediateCompleteDirs[arrayIndex]=$tmp
	    arrayIndex="$[ arrayIndex+1 ]"
	fi
    done

    # Loop over all paths in $CAFAnalysisShare. These are used as
    # base location for auto-completion.
    for path in $CAFAnalysisShare ; do
	local thisPath="$path$thisDir"

	# Contains all filenames in $thisPath.
	local listOfFiles_new=`ls -ap $thisPath 2>/dev/null | tr '\r\n' ':'`
	# Contains absolute paths of directories in $thisPath
	local listOfDirs_new=`ls -ad $thisPath*/ 2>/dev/null | tr '\r\n' ':'`

	# All directories and files found in this $path. Used to find
	# $immediateCompleteStr later.
	local foundInThisPath=()

	for file in $listOfFiles_new  ; do
	    if [[ "$file" != *"/" ]] ; then
		if [[ $file != "" ]] ; then
		    if [[ $path == "./" ]] ; then
			if [[ $listOfFilesInDot != "" ]] ; then
			    listOfFilesInDot+=":"
			fi
			listOfFilesInDot+=$thisDir$file
		    else
			if [[ $listOfFiles != "" ]] ; then
			    listOfFiles+=":"
			fi
			listOfFiles+=$thisDir$file
		    fi
		    foundInThisPath+=("$thisDir$file")
		fi
	    fi
	done
	for dir in $listOfDirs_new ; do
	    # Filter "./" and "../" out.
	    if [[ $dir != "" ]] && [[ $dir != "./" ]] && [[ $dir != "../" ]] ; then
		if [[ $path == "./" ]]; then
		    if [[ $listOfDirsInDot != "" ]] ; then
			listOfDirsInDot+=":"
		    fi
		    # Cut off the absolute part of all suggestions
		    listOfDirsInDot+=${dir/$path/}
		else
		    if [[ $listOfDirs != "" ]] ; then
			listOfDirs+=":"
		    fi
		    # Cut off the absolute part of all suggestions
		    listOfDirs+=${dir/$path/}
		fi
		foundInThisPath+=("${dir/$path/}")
	    fi
	done

	# The following part is only used to find and match
	# $immediateCompleteStr in one of the auto-complete paths. If
	# it is found, return this as only suggestion.
	if [ "$option" -eq 1 ] || [ "$option" -eq 3 ] ; then
	    for (( i=0;i<${#immediateCompleteDirs[@]};i++ )) ; do
		for file in "${foundInThisPath[@]}" ; do
		    
		    # Check if any completion matches any part of the
		    # auto-complete string.
		    if [[ "$file" == *"${immediateCompleteDirs[i]}" ]] ; then
			local tmp=""

			# Check if this completion matches the entire
			# parent auto-complete string.
			for (( ii=0;ii<$[ i+1 ];ii++ )) ; do
			    tmp+="${immediateCompleteDirs[ii]}"
			done
			if [[ "${path/#.\//$PWD/}$file" == *"$tmp" ]] ; then

			    # Check if this completion matches the
			    # entire subordinate auto-complete string
			    # step by step (counter $ii).
			    matchedPart="$file"
			    local ranUntilTheEnd=1
			    for (( ii=$[ i+1 ];ii<${#immediateCompleteDirs[@]};ii++ )) ; do
				ranUntilTheEnd=1
				matchedPartPrev="$matchedPart"
				tmpFiles=`ls -ap "$path$matchedPart" 2>/dev/null | tr '\r\n' ':'`
				for tmpFile in $tmpFiles ; do
				    if [[ "$tmpFile" == "${immediateCompleteDirs[ii]}" ]] && [[ "$matchedPart" == "$matchedPartPrev" ]] ; then

					# Found another matching directory.
					matchedPart="$matchedPart$tmpFile"
				    fi
				done

				# Only if a matching directory is found in every for-
				# iteration, consider this match.
				if [[ "$matchedPart" == "$matchedPartPrev" ]] ; then
				    ii=${#immediateCompleteDirs[@]}
				    ranUntilTheEnd=0
				fi
			    done

			    # If a suggestion has been found and it also matches the
			    # last part of $thisWord, use it as exclusive suggestion.
			    if [ "$ranUntilTheEnd" -eq 1 ] ; then
				matchedPart=( $(compgen -W "$matchedPart" -- "$thisWord") )
				if [[ "$matchedPart" != "" ]] ; then
				    COMPREPLY="$matchedPart"
				    if [[ $COMPREPLY == *"/" ]]; then
					COMPREPLY+="/"
				    fi
				    return 0
				fi
			    fi
			fi
		    fi
		done
	    done
	fi
    done

    local fullCompletions=""
    for dir in $listOfDirsInDot; do
	# fullCompletions+=${dir/%\//}":"
	fullCompletions+="$dir:"
    done
    for dir in $listOfDirs; do
	local alreadyThere=0
	for dirInDot in $listOfDirsInDot; do
	    if [[ $dir == $dirInDot ]]; then
		alreadyThere=1
	    fi
	done
	if [[ $alreadyThere -eq 0 ]]; then
	    fullCompletions+=${dir/%\//\/\/}":"
	    # append a trailing slash (will be removed again later)
	    # fullCompletions+=${dir/%\//\/\/}
	fi
    done
    if [[ "$listOfFilesInDot" != "" ]]; then
	fullCompletions+="$listOfFilesInDot:"
    fi
    if [[ "$listOfFiles" != "" ]]; then
	fullCompletions+="$listOfFiles:"
    fi
    fullCompletions=${fullCompletions/%:/}
    unset IFS
    fullCompletions=`echo "$fullCompletions" | tr ":" " "`
    # echo "$fullCompletions"
    # Compgen reduces the list to the words starting with $thisWord.
    # The variable COMPGEN will be used to print suggestions.
    COMPREPLY=( $(compgen -W "$fullCompletions" -- "$thisWord") )

    # echo "compreply: "${COMPREPLY[@]}

    return 0
}


_CAFShortenCompreply(){
    ##################################################################
    #
    # Update: This function is not currently used, but works fine.
    # The same functionality is provided by "-o filenames"
    #
    ##################################################################
    #
    # This function is called at the end, when all possible
    # completions have been created. Current completions are relative
    # to the current directory (or equivalently $CAFANALYSISSHARE).
    # It is nicer to remove the leading path for user-friendliness.
    # But only do it, if suggestions are actually shown (i.e. the
    # current word is not completed automatically).
    #
    # This function checks if auto-completion will find a (sub)string
    # to complete. In that case, it does not change $COMPREPLY and
    # complete the match. If there is no match, it shows the more in-
    # tuitive suggestions by removing the relative path $thisDir.
    #
    ##################################################################

    local thisWord=$1
    local thisDir=$2
    
    local leadingCharFirst=""
    local allLeadingAreSame=0

    for (( i=0; i<${#COMPREPLY[@]}; i++)) ; do
	entry=${COMPREPLY[$i]}

	# Add a space to all filesnames (as opposed to directories).
	if [[ $entry != *"/" ]] ; then
	    COMPREPLY[$i]="$entry "
	fi

	# Check if leading characters of all suggestions are the same.
    	local entryWithoutThisWord=${entry/#$thisWord/}
    	if [[ $leadingCharFirst == "" ]] ; then
    	    leadingCharFirst="${entryWithoutThisWord:0:1}"
    	else
    	    leadingCharThis="${entryWithoutThisWord:0:1}"
    	    if [[ $leadingCharThis != $leadingCharFirst ]] ; then
    		allLeadingAreSame=1
    	    fi
    	fi
    done

    # If leading characters of suggestions are different, no match
    # will be found and the user-friendly text will be printed.
    if [ $allLeadingAreSame -eq 1 ] ; then
    	COMPREPLY=("${COMPREPLY[@]/#$thisDir/}")
    fi

}

_CAFFilterFiles(){
    ##################################################################
    # This function manipulates the content of the global variable
    # $COMPREPLY. It removes all filenames, which do not start with
    # any of the prefixes (first argument) and do not end with any of
    # the extensions (second argument). Different prefixes and
    # extensions are separated by ":". This function does NOT filter
    # directories, only filenames.
    #
    ##################################################################
    
    local prefixes="$1"
    local extensions="$2"

    # Two indices help to ensure that no empty spaces are created in
    # the array $COMPREPLY.
    
    local max=${#COMPREPLY[@]}
    local newIndex=0
    IFS=:
    for (( i=0; i<$max; i++)) ; do
	entry="${COMPREPLY[$i]}"
	local removeEntry=1
	# Directories are not removed
	if [[ "$entry" == *"/" ]] ; then
	    removeEntry=0
	else
	    # Check if one of the extensions or prefixes are there.
	    local extensionMissing=1
	    local prefixMissing=1
	    if [[ "$extensions" == "" ]] ; then
		extensionMissing=0
	    else
		for extension in $extensions ; do
		    if [[ "$entry" == *"$extension" ]] ; then
			extensionMissing=0
		    fi
		done
	    fi
	    if [[ "$extensionMissing" -eq 0 ]] ; then
		if [[ "$prefixes" == "" ]] ; then
		    prefixMissing=0
		else
		    for prefix in $prefixes ; do
			# For prefixes, the leading path is removed
			# to get the beginning of the filename.
			local entryShortened=`echo $entry | rev | cut -d "/" -f1 - | rev`
			if [[ "$entryShortened" == "$prefix"* ]] ; then
			    prefixMissing=0
			fi
		    done
		fi
	    fi
	    # Entry is only kept if prefix AND extension are matched.
	    if [ "$extensionMissing" -eq 0 ] && [ "$prefixMissing" -eq 0 ] ; then
		removeEntry=0
	    fi
	fi
	# Store entries such that no empty spots in arrayare created.
	if [ "$removeEntry" -eq 0 ] ; then
	    if [[ "$newIndex" -lt "$i" ]] ; then
		COMPREPLY[newIndex]=${COMPREPLY[i]}
		unset COMPREPLY[i]
	    fi
	    newIndex="$[ $newIndex+1 ]"
	else
	    unset COMPREPLY[i]
	fi
    done
    unset IFS
}

_CAFSetNoSpaceOpt(){
    if [[ ${#COMPREPLY[@]} -eq 1 ]]; then
	if [[ ${COMPREPLY[0]} = *"/" ]]; then
	    compopt -o nospace
	fi
    fi
}

_CAFRemoveTrailingSlash(){
    local max=${#COMPREPLY[@]}
    for (( i=0; i<$max; i++)) ; do
	COMPREPLY[i]=${COMPREPLY[i]/%\//}
    done
}


_CAFRegularComplete(){
    ##################################################################
    # This function gets executed if auto-completion for the regular
    # CAF execution scripts.
    #
    # In all cases, completion suggestions for the config file are
    # given.
    #
    ##################################################################

    local command=$1
    local thisWord=$2
    local previousWord=$3
    local thisDir=$4
    local CAFAnalysisShare=$5
    local option=$6
    local immediateCompleteStr=$7
    local extensions=$8

    # echo ""
    # echo "_CAFRegularComplete called with arguments:"
    # echo "command          : $command"
    # echo "thisWord         : $thisWord"
    # echo "previousWord     : $previousWord"
    # echo "thisDir          : $thisDir"
    # echo "CAFAnalysisShare : $CAFAnalysisShare"
    # echo "option           : $option"

    local prefixes=""
    if [ "$option" -gt 1 ] ; then
	prefixes="${command/%.py/}"
    fi
    
    _CAFFindPossibleCompletions "$thisWord" "$thisDir" "$CAFAnalysisShare" "$option" "$immediateCompleteStr"
    _CAFFilterFiles "$prefixes" "$extensions"
    _CAFSetNoSpaceOpt
    _CAFRemoveTrailingSlash

    return 0
}

_CAFSubmitComplete(){
    ##################################################################
    # This function gets executed if auto-completion for "submit.py"
    # is requested.
    #
    # If the previous word in the command line is not an option (i.e.
    # does not start with "-"), suggest completion for config file.
    #
    # If the previous word is "--jobs", find the submission options.
    #
    # Otherwise (previous word is an option), find standard
    # completion.
    #
    ##################################################################

    local command=$1
    local thisWord=$2
    local previousWord=$3
    local thisDir=$4
    local CAFAnalysisShare=$5
    local option=$6

    if [[ "$previousWord" != "-"* ]] ; then
	_CAFFindPossibleCompletions "$thisWord" "$thisDir" "$CAFAnalysisShare" "$option" "config/master/"
	_CAFFilterFiles "" ".cfg"
    elif [[ "$previousWord" == "--jobs" ]] ; then
	_CAFFindPossibleCompletions "$thisWord" "$thisDir" "$CAFAnalysisShare" "$option" "config/jobLists/"
	_CAFFilterFiles "" ".txt"
    else
	_CAFFindPossibleCompletions "$thisWord" "$thisDir" "$CAFAnalysisShare" "0" ""
    fi
    _CAFSetNoSpaceOpt
    _CAFRemoveTrailingSlash
    return 0
}

_CAFBasicComplete(){
    ##################################################################
    # This function gets executed if auto-completion is requested for
    # a basic file (i.e. nothing special is implemented, but config
    # files relative to CAFAnalysisShare will be found).
    #
    # In all cases, completion suggestions for the config file are
    # given.
    #
    ##################################################################

    local command=$1
    local thisWord=$2
    local previousWord=$3
    local thisDir=$4
    local CAFAnalysisShare=$5
    local option=$6

    # echo ""
    # echo "_CAFBasicComplete called with arguments:"
    # echo "command          : $command"
    # echo "thisWord         : $thisWord"
    # echo "previousWord     : $previousWord"
    # echo "thisDir          : $thisDir"
    # echo "CAFAnalysisShare : $CAFAnalysisShare"
    # echo "option           : $option"

    local prefixes=""
    if [ "$option" -gt 1 ] ; then
	prefixes="${command/%.py/}"
    fi

    _CAFFindPossibleCompletions "$thisWord" "$thisDir" "$CAFAnalysisShare" "0" ""
    _CAFFilterFiles "$prefixes" ".cfg"
    _CAFSetNoSpaceOpt
    _CAFRemoveTrailingSlash

    return 0
}

_CAFAutoComplete(){
    ##################################################################
    # This function gets called when auto-completion for a script is
    # invoked.
    #
    # The function checks if the python command is known and if it
    # points to a file in $CAFANALYSISSHARE. If not, create the
    # standard auto-complete options using the functions in this
    # file (it would be nicer to use the system auto-complete).
    #
    # Auto-completion is by default run in the mode "option=0". This
    # is overwritten by $CAFAUTOCOMPLETEOPT. For behaviour of modes,
    # look in README.md file.
    #
    # If statements distinguish between different python scripts and
    # call the respective functions, where the auto-complete
    # procedure is defined.
    #
    ##################################################################

    local command=$1
    local thisWord=$2
    local previousWord=$3
    local option=0

    set +e

    # Find the directory of the path that the user has already typed
    # in the command line: $thisDir.
    local thisDir=""
    if [[ $thisWord == *"/"* ]] ; then
	thisDir=`echo $thisWord | rev | cut -d "/" -f2- | rev`
	if [[ $thisDir != "" ]] ; then
	    thisDir+="/"
	fi
    fi

    # Do standard auto-completion if $command is not in CAFANALYSISSHARE
    local CAFAnalysisShare="./:"
    CAFAnalysisShare+=$(_CAFdoAutoCompletion $command)
    local statusCode=$?

    if [ $statusCode -eq 0 ] ; then
	_CAFRegularComplete "$command" "$thisWord" "$previousWord" "$thisDir" "./" "0" "" ""
	return 0
    fi

    # If $CAFAUTOCOMPLETEOPT is an integer between 0 and 3, overwrite
    # $option.
    local re='^[0-3]$'
    if [[ "$CAFAUTOCOMPLETEOPT" =~ $re ]] ; then
	option=$CAFAUTOCOMPLETEOPT
    fi

    if [[ "$command" == *"submit.py" ]] ; then
	_CAFSubmitComplete "$command" "$thisWord" "$previousWord" "$thisDir" "$CAFAnalysisShare" "$option"
    elif [[ "$command" == *"runGridScanner.py" ]] ; then
	# Gridscanner files don't typically start with "runGridScanner". So disable the filtering of config files here.
	if [ $option -gt 1 ] ; then
	    option=$(($option-2))
	fi
	_CAFBasicComplete "$command" "$thisWord" "$previousWord" "$thisDir" "$CAFAnalysisShare" "$option"
    elif [ $command == "statistics.py" ] ; then
	local re='^[23]$'
	if [[ "$option" =~ $re ]] ; then
	    # don't require the filename to match the script name
	    option=$(($option-2))
	fi
	_CAFRegularComplete "$command" "$thisWord" "$previousWord" "$thisDir" "$CAFAnalysisShare" "$option" "config/statistics/" ".txt"
    else
	_CAFRegularComplete "$command" "$thisWord" "$previousWord" "$thisDir" "$CAFAnalysisShare" "$option" "config/master/" ".cfg"
    fi
    return 0
}


# Add all $pythonScripts to the list of commands that use auto-completion. If
# any of them invokes auto-complete, the function _CAFAutoComplete gets called.

IFS=" "
for script in $pythonScripts ; do
    complete -o filenames -F _CAFAutoComplete $script
done
unset pythonScripts
unset IFS
