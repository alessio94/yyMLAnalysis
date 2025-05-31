#!/usr/bin/env bash
#
# download_datasets.sh
#
#  ▸ Usage
#       ./download_datasets.sh                # download into the directories that already exist
#       ./download_datasets.sh /my/base/path  # same, but use a different base directory
#       ./download_datasets.sh --dry-run      # only print what would be done
#       ./download_datasets.sh --debug        # prints additional debug statements
#
#  ▸ What it does
#     – Reads a built-in table of  (relative-path | Rucio DID)
#     – Creates the path if it is missing
#     – cd's into that path
#     – Runs  rucio download <DID>
#     – If the dataset directory is already present, still runs the download;
#       rucio will only fetch files not yet downloaded
#     – If --dry-run is used, it prints what would happen but makes no changes
#
#  ▸ Requires
#     – Rucio command-line client in your PATH and configured (proxy, RSEs …)
#     – Bash ≥ 4 (for associative arrays)
#

set -euo pipefail
trap 'echo "❌ Script failed on line $LINENO with exit code $?" >&2' ERR

BASE_DIR=$(pwd) # use current working directory as default
DRY_RUN=0
DEBUG=0

debug() {
  if [[ "${DEBUG:-0}" == "1" ]]; then
    echo "[DEBUG] $*"
  fi
}

# Parse arguments (order independent)
for arg in "$@"; do
  case "$arg" in
    --dry-run)
      DRY_RUN=1
      ;;
    --debug)
      DEBUG=1
      ;;
    *)
      # If it's not --dry-run or --debug, treat as base directory if not set yet
      if [[ -z "$BASE_DIR" ]]; then
        BASE_DIR="$arg"
      else
        echo "Warning: ignoring unexpected argument '$arg'"
      fi
      ;;
  esac
done

debug "Using BASE_DIR='$BASE_DIR'"
debug "Using DRY_RUN='$DRY_RUN'"
debug "Using DEBUG='$DEBUG'"

######################################################################
#  TABLE:  one  path | dataset  per line  (delimiter is ‘|’)          #
######################################################################
DATASETS='
# ---------- data ----------------------------------------------------
data/2022		    | user.dshope.v1.data22_13p6TeV.periodAllYear.grp22_v02_p6700_TREE
data/2023		    | user.dshope.v1.data23_13p6TeV.periodAllYear.grp23_v01_p6700_TREE

# ---------- mc_nom / mc23a -----------------------------------------
mc_nom/mc23a/yyjets               | user.dshope.v1.yyjets_mc23a.700980.e8537_s4162_r15540_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700870.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700871.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700872.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700873.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700874.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700875.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/Vyy                  | user.dshope.v1.Vyy_mc23a.700876.e8514_s4162_r14622_p6697_TREE
mc_nom/mc23a/ttyy                 | user.dshope.v1.ttyy_mc23a.542859.e8551_s4162_r15540_p6697_TREE
mc_nom/mc23a/ttyy                 | user.dshope.v1.ttyy_mc23a.542860.e8551_s4162_r15540_p6697_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.602421.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.601482.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.601484.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.601483.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.601523.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.601522.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.602422.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.601710.e8559_s4162_r14622_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.545636.e8557_s4162_r15540_p6491_TREE
mc_nom/mc23a/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23a.545639.e8557_s4162_r15540_p6491_TREE

# ---------- mc_nom / mc23d -----------------------------------------
mc_nom/mc23d/yyjets               | user.dshope.v1.yyjets_mc23d.700980.e8537_s4159_r15530_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700870.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700871.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700872.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700873.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700874.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700875.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/Vyy                  | user.dshope.v1.Vyy_mc23d.700876.e8514_s4159_r15224_p6697_TREE
mc_nom/mc23d/ttyy                 | user.dshope.v1.ttyy_mc23d.542859.e8551_s4159_r15530_p6697_TREE
mc_nom/mc23d/ttyy                 | user.dshope.v1.ttyy_mc23d.542860.e8551_s4159_r15530_p6697_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.602421.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.601482.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.601484.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.601483.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.601523.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.601522.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.602422.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.601710.e8559_s4159_r15224_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.545636.e8557_s4159_r15530_p6491_TREE
mc_nom/mc23d/SingleHiggs          | user.dshope.v1.SingleHiggs_mc23d.545639.e8557_s4159_r15530_p6491_TREE
'
######################################################################

debug "DATASETS content:"
debug "$DATASETS"

################################################################################
#  Main loop: split each line into   relative_path   and   DID                 #
################################################################################
echo ">> Downloading datasets into tree rooted at: $BASE_DIR"
echo

# set -x
while IFS='|' read -r RELPATH DID || [[ -n "$RELPATH" ]]; do

    # Skip blank lines and comments
    RELPATH="${RELPATH:-}"
    if [[ -z "${RELPATH//[[:space:]]/}" || "${RELPATH}" =~ ^[[:space:]]*# ]]; then
        debug "Skipping line (empty or comment)"
        continue
    fi

    # Trim whitespace
    RELPATH="$(echo "$RELPATH" | xargs)"
    DID="$(echo "$DID" | xargs)"

    TARGET_DIR="$BASE_DIR/$RELPATH"
    debug "Creating target directory: $TARGET_DIR"
    mkdir -p "$TARGET_DIR"

    echo "==> $DID"
    echo "    ↳ $TARGET_DIR"

    if [[ "$DRY_RUN" -eq 1 ]]; then
        echo "    [dry-run] cd \"$TARGET_DIR\" && rucio download \"$DID\"" >&2
    else
        debug "Changing directory to $TARGET_DIR"
        pushd "$TARGET_DIR" >/dev/null
        debug "Running: rucio download \"$DID\""
        # rucio download "$DID"
        if ! rucio download "$DID"; then
            status=$?
            if [[ $status -eq 2 ]]; then
                echo "ℹ️  $DID already downloaded (exit code 2)"
            else
                echo "❌ Download failed for $DID (exit code $status)" >&2
                popd >/dev/null
                continue
            fi
        else
            echo "Download succeeded for $DID"
        fi
        popd >/dev/null
        echo
    fi

done <<<"${DATASETS#"${DATASETS%%[!$'\n']*}"}" # trim leading newlines from DATASETS string before feeding it into the while loop

echo "✓ All requested datasets processed."
debug "Script finished."
