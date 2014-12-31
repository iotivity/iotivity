#! /bin/bash

# Ideally we will capture the exit code of each step and try them all before failing
# the build script.  For now, use set -e and fail the build at first failure.
set -e

function clean()
{
	echo "*********** Clean build *************"
	scons -c
	rm -rf out
}

function build()
{
	if [ $(uname -s) = "Linux" ]
        then
		echo "*********** Build for linux *************"
		scons RELEASE=$3
	fi

	# Note: for android, as oic-resource uses C++11 feature stoi and to_string,
	# it requires gcc-4.9, currently only android-ndk-r10(for linux)
	# and windows android-ndk-r10(64bit target version) support these features.
	if [ "$BUILD_FOR_ANDROID" = "true" ]
		then

		echo "*********** Build Boost for android ***********"
		pushd extlibs
		./buildDependencies.sh
		popd

		echo "*********** Build for android x86 *************"
		scons TARGET_OS=android TARGET_ARCH=x86 ANDROID_NDK=$1 RELEASE=$3

		echo "*********** Build for android armeabi *************"
		scons TARGET_OS=android TARGET_ARCH=armeabi ANDROID_NDK=$1 RELEASE=$3

		echo "*********** Build for android armeabi-v7a *************"
		scons TARGET_OS=android TARGET_ARCH=armeabi-v7a ANDROID_NDK=$1 RELEASE=$3

		echo "*********** Build for android armeabi-v7a-hard *************"
		scons TARGET_OS=android TARGET_ARCH=armeabi-v7a-hard ANDROID_NDK=$1 RELEASE=$3
	fi
	
	echo "*********** Build for arduino avr *************"
	scons TARGET_OS=arduino TARGET_ARCH=avr ARDUINO_HOME=$2 RELEASE=$3

	echo "*********** Build for arduino arm *************"
	scons TARGET_OS=arduino TARGET_ARCH=arm ARDUINO_HOME=$2 RELEASE=$3

	if [ $(uname -s) = "Darwin" ]
	then
		echo "*********** Build for OSX *************"
		scons TARGET_OS=darwin SYS_VERSION=10.9 RELEASE=$3

		echo "*********** Build for IOS i386 *************"
		scons TARGET_OS=ios TARGET_ARCH=i386 SYS_VERSION=7.0 RELEASE=$3

		echo "*********** Build for IOS x86_64 *************"
		scons TARGET_OS=ios TARGET_ARCH=x86_64 SYS_VERSION=7.0 RELEASE=$3

		echo "*********** Build for IOS armv7 *************"
		scons TARGET_OS=ios TARGET_ARCH=armv7 SYS_VERSION=7.0 RELEASE=$3

		echo "*********** Build for IOS armv7s *************"
		scons TARGET_OS=ios TARGET_ARCH=armv7s SYS_VERSION=7.0 RELEASE=$3

		echo "*********** Build for IOS arm64 *************"
		scons TARGET_OS=ios TARGET_ARCH=arm64 SYS_VERSION=7.0 RELEASE=$3
	fi
}

function  help()
{
	echo "Usage:"
        echo "  build:"
        echo "     `basename $0` <path-to-android-ndk> <path-to-arduino-home>"
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
elif [ $# -ne 2 ]
then
	help
	exit -1
fi

# Suppress "Reading ..." message and enable parallel build
export SCONSFLAGS="-Q -j 4"
build $1 $2 true
build $1 $2 false
echo "===================== done ====================="
