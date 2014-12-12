#!/bin/sh
if echo $1 | grep -q -i "make"
then
cd ../lib/libcoap-4.1.1/
sudo make -f makefile_arduino.mak PLATFORM=$2 ARDUINO_DIR=$4
cd ../../build/arduino
sudo make PLATFORM=$2 TRANSPORT=$3 ARDUINO_DIR=$4 APP_NAME=$5
else
cd ../lib/libcoap-4.1.1/
sudo make $1 -f makefile_arduino.mak PLATFORM=$2 ARDUINO_DIR=$4
cd ../../build/arduino/
sudo make $1 PLATFORM=$2 TRANSPORT=$3 ARDUINO_DIR=$4 APP_NAME=$5
fi