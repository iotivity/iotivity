#!/bin/sh

set -f
set -e

[ ! -z ${EXEC_MODE} ] || EXEC_MODE=false

topdir="${PWD}"


main_()
{
    cat<<EOF
#
# WARNING: This helper script is DISCOURAGED to be used
#
#  Instead please use the supported build tool "scons"
#
#  However in some specific cases it's not possible
#  so this script is providing a fallback option.
#
#  Reminder: it's not safe to access online resources at build time,
#  so by default the script will tell what to do (like scons),
#  to enable tasks executions it should be run with EXEC_MODE enabled:
#
#  EXEC_MODE=true ./prep.sh
#
#  More details at:
#  https://wiki.iotivity.org/build
#
EOF

    which bash > /dev/null
    which git > /dev/null
    which unzip > /dev/null
    which wget > /dev/null

    find ./extlibs/ -maxdepth 2 -a -iname "prep.sh" | xargs -n1 bash
}


main_
