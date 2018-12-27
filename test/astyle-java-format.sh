#!/bin/sh

astyle --style=java --indent=spaces=4 --min-conditional-indent=0 --align-pointer=name --align-reference=name --indent-classes --indent-switches --indent-namespaces --pad-oper --pad-header --keep-one-line-blocks --keep-one-line-statements --convert-tabs --mode=java --max-code-length=100 --suffix=none "*.java" --recursive

echo ============================================================

echo Removing trailing spaces started
find . -type f  -name "*.java" -exec sed --in-place 's/[[:space:]]\+$//' {} \+
echo ------------------------------------------------------------
echo Removing trailing spaces ended
