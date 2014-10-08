#! /bin/bash

function clean()
{
	echo "*********** Clean build *************"
	scons -c
	rm out -rf
}

function build()
{
	echo "*********** Build for linux *************"
	scons

	# Note: for android, as oic-resource uses C++11 feature stoi and to_string,
	# it requires gcc-4.9, currently only android-ndk-r10(for linux)
	# and windows android-ndk-r10(64bit target version) support these features.

	echo "*********** Build for android x86 *************"
	scons BUILD_TARGET=Android CPU_ARCH=x86 ANDROID_HOME=$1 ANDROID_NDK=$2

	echo "*********** Build for android armeabi *************"
	scons BUILD_TARGET=Android CPU_ARCH=armeabi ANDROID_HOME=$1 ANDROID_NDK=$2

	echo "*********** Build for android armeabi-v7a *************"
	scons BUILD_TARGET=Android CPU_ARCH=armeabi-v7a ANDROID_HOME=$1 ANDROID_NDK=$2

	echo "*********** Build for android armeabi-v7a-hard *************"
	scons BUILD_TARGET=Android CPU_ARCH=armeabi-v7a-hard ANDROID_HOME=$1 ANDROID_NDK=$2

	echo "*********** Build for arduino avr *************"
	scons BUILD_TARGET=Arduino CPU_ARCH=avr ARDUINO_HOME=$3

	echo "*********** Build for arduino arm *************"
	scons BUILD_TARGET=Arduino CPU_ARCH=arm ARDUINO_HOME=$3

	if [ $(uname -s) = "Darwin" ]
	then
		echo "*********** Build for OSX i386 *************"
		scons BUILD_TARGET=Darwin CPU_ARCH=i386 SYS_VERSION=10.9

		echo "*********** Build for OSX x86_64 *************"
		scons BUILD_TARGET=Darwin CPU_ARCH=x86_64 SYS_VERSION=10.9

		echo "*********** Build for IOS i386 *************"
		scons BUILD_TARGET=IOS CPU_ARCH=i386 SYS_VERSION=7.0

		echo "*********** Build for IOS x86_64 *************"
		scons BUILD_TARGET=IOS CPU_ARCH=x86_64 SYS_VERSION=7.0

		echo "*********** Build for IOS armv7 *************"
		scons BUILD_TARGET=IOS CPU_ARCH=armv7 SYS_VERSION=7.0

		echo "*********** Build for IOS armv7s *************"
		scons BUILD_TARGET=IOS CPU_ARCH=armv7s SYS_VERSION=7.0

		echo "*********** Build for IOS arm64 *************"
		scons BUILD_TARGET=IOS CPU_ARCH=arm64 SYS_VERSION=7.0
	fi
}

function  help()
{
	echo "Usage:"
        echo "  build:"
        echo "     `basename $0` <path-to-android-sdk> <path-to-android-ndk> <path-to-arduino-home>"
        echo "  clean:"
        echo "     `basename $0` -c"
}

if [ $# -eq 1 ]
then
	if [ $1 = '-c' ]
	then
		clean
		exit 0
	else
		help
		exit -1
	fi
elif [ $# -ne 3 ]
then
	help
	exit -1
fi


build $1 $2 $3
echo "===================== done ====================="
