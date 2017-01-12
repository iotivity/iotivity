#!/bin/bash
if which xdotool > /dev/null; then
   echo exists
else
   sudo apt-get install xdotool
fi

export BASE_DIR=/home/ubuntu/Desktop/taf_agent/iotivity
export BUILD_DIR_NAME=release
export LD_LIBRARY_PATH=$BASE_DIR/out/linux/x86_64/$BUILD_DIR_NAME/
export PROVISIONING_DIR=$BASE_DIR/test/bin/linux
export JSON_DIR=$BASE_DIR/test/bin/linux
export IUT_DIR=$BASE_DIR/test/bin/linux/

cd $PROVISIONING_DIR
eval ./$@
