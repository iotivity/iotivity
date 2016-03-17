#!/bin/bash
export IOTIVITY_DIR=../../../
LD_LIBRARY_PATH=$IOTIVITY_DIR/out/linux/x86_64/release ./client $1 
