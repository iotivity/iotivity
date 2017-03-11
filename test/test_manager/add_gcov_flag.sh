#!/bin/sh

#set path

iotivity_root=$1

#cd $ROOT_DIR

API_BUILD_SCRIPT=${iotivity_root}/build_common/linux/SConscript

#BackUP Scons
mkdir tmp_api
mkdir tmp_tc

cp $API_BUILD_SCRIPT tmp_api

sed -i "/Import('env')/a env.AppendUnique(CCFLAGS = ['-ftest-coverage' , '-fprofile-arcs'])\nenv.AppendUnique(LIBS = ['gcov'])" $API_BUILD_SCRIPT
