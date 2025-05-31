#!/usr/bin/env bash
#
# list_dataset_files.sh
#
# Usage:
#   ./list_dataset_files.sh --group 2022                   # saves to files_2022.txt
#   ./list_dataset_files.sh --group mc23a --output mc23a.txt
#   ./list_dataset_files.sh --group 2023
#   ./list_dataset_files.sh --group mc23d --output custom.txt
#

set -euo pipefail

GROUP=""
OUTPUT_FILE=""

# Parse arguments
while [[ $# -gt 0 ]]; do
  case "$1" in
    --group)
      GROUP="$2"
      shift 2
      ;;
    --output)
      OUTPUT_FILE="$2"
      shift 2
      ;;
    *)
      echo "❌ Unknown argument: $1"
      exit 1
      ;;
  esac
done

# Check required argument
if [[ -z "$GROUP" ]]; then
  echo "❌ Missing required argument: --group <2022|2023|mc23a|mc23d>"
  exit 1
fi

# Set default output file name if not provided
if [[ -z "$OUTPUT_FILE" ]]; then
  OUTPUT_FILE="files_${GROUP}.txt"
fi

# Determine the folder to scan
case "$GROUP" in
  2022)
    SEARCH_DIR="data/2022"
    ;;
  2023)
    SEARCH_DIR="data/2023"
    ;;
  mc23a)
    SEARCH_DIR="mc_nom/mc23a"
    ;;
  mc23d)
    SEARCH_DIR="mc_nom/mc23d"
    ;;
  *)
    echo "❌ Unknown group: $GROUP"
    exit 1
    ;;
esac

# Verify directory exists
if [[ ! -d "$SEARCH_DIR" ]]; then
  echo "❌ Directory '$SEARCH_DIR' does not exist"
  exit 1
fi

# List absolute paths and write to file
echo "📦 Listing files under '$SEARCH_DIR' → '$OUTPUT_FILE'"
find "$SEARCH_DIR" -type f -exec realpath {} \; | sort > "$OUTPUT_FILE"
echo "✅ Done: $OUTPUT_FILE created with absolute paths."
