#!/bin/bash

# Script to recursively format all supported source files using clang-format
# Usage: ./scripts/clang-format-all.sh [directory]

set -e

ROOT_DIR="${1:-.}"

CLANG_FORMAT="clang-format"

if ! command -v "$CLANG_FORMAT" &> /dev/null; then
    echo "Error: $CLANG_FORMAT could not be found"
    echo "Please install clang-format or set the correct path in this script"
    exit 1
fi

echo "Using $(command -v "$CLANG_FORMAT"): $($CLANG_FORMAT --version)"

C_EXTS=(".c" ".h")
CPP_EXTS=(".cpp" ".cc" ".cxx" ".hpp" ".hh" ".hxx" ".ipp" ".h" ".tpp")
CUDA_EXTS=(".cu" ".cuh")
OBJC_EXTS=(".m" ".mm")

ALL_EXTS=("${C_EXTS[@]}" "${CPP_EXTS[@]}" "${CUDA_EXTS[@]}" "${OBJC_EXTS[@]}")

FILE_PATTERN=""
for EXT in "${ALL_EXTS[@]}"; do
    if [ -z "$FILE_PATTERN" ]; then
        FILE_PATTERN="-name \"*$EXT\""
    else
        FILE_PATTERN="$FILE_PATTERN -o -name \"*$EXT\""
    fi
done

EXCLUDE_DIRS=(
    "build"
    "out"
    "extern"
    "third_party"
    "ThirdParty"
    "vendor"
    ".git"
    "node_modules"
    "vcpkg"
)

EXCLUDE_PATTERN=""
for DIR in "${EXCLUDE_DIRS[@]}"; do
    EXCLUDE_PATTERN="$EXCLUDE_PATTERN -not -path \"*/$DIR/*\""
done

echo "Finding files to format..."
FIND_CMD="find \"$ROOT_DIR\" -type f \( $FILE_PATTERN \) $EXCLUDE_PATTERN"
FILES=($(eval "$FIND_CMD"))
FILE_COUNT=${#FILES[@]}

echo "Found $FILE_COUNT files to format"

if [ $FILE_COUNT -eq 0 ]; then
    echo "No files to format. Exiting."
    exit 0
fi

echo "Formatting files..."
for ((i = 0; i < FILE_COUNT; i++)); do
    FILE="${FILES[$i]}"
    echo -ne "[$((i+1))/$FILE_COUNT] Formatting: $FILE\033[0K\r"
    "$CLANG_FORMAT" -i -style=file "$FILE"
done

echo -e "\nAll $FILE_COUNT files formatted successfully!"
exit 0
