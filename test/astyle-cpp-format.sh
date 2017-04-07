#!/bin/sh

astyle --style=allman --indent=spaces=4 --min-conditional-indent=0 --align-pointer=name --align-reference=name --indent-classes --indent-switches --indent-cases --indent-namespaces --pad-oper --pad-header --keep-one-line-blocks --keep-one-line-statements --convert-tabs --mode=c --max-code-length=100 --suffix=none "*.cpp" "*.h" "*.c" --recursive

echo ============================================================

echo Removing trailing spaces started
find . -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.h" \) -exec sed --in-place 's/[[:space:]]\+$//' {} \+
echo ------------------------------------------------------------
echo Removing trailing spaces ended
