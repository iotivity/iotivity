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

		echo "*********** Build for linux with Security*************"
		scons RELEASE=$3 SECURED=1
	fi

	# Note: for android, as oic-resource uses C++11 feature stoi and to_string,
	# it requires gcc-4.9, currently only android-ndk-r10(for linux)
	# and windows android-ndk-r10(64bit target version) support these features.
	echo "*********** Build Boost for android ***********"
	# disable parallel build for android as gradle depends on scons to finish first
	export SCONSFLAGS="-Q"

	echo "*********** Build for android x86 *************"
	scons TARGET_OS=android TARGET_ARCH=x86 RELEASE=$3 TARGET_TRANSPORT=IP

	echo "*********** Build for android armeabi *************"
	scons TARGET_OS=android TARGET_ARCH=armeabi RELEASE=$3 TARGET_TRANSPORT=IP

	# enable parallel build
	export SCONSFLAGS="-Q -j 4"

	echo "*********** Build for arduino avr *************"
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=mega TARGET_ARCH=avr TARGET_TRANSPORT=IP SHIELD=ETH RELEASE=$3
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=mega TARGET_ARCH=avr TARGET_TRANSPORT=IP SHIELD=WIFI RELEASE=$3

	echo "*********** Build for arduino arm *************"
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=arduino_due_x TARGET_ARCH=arm TARGET_TRANSPORT=IP SHIELD=ETH RELEASE=$3
	scons resource TARGET_OS=arduino UPLOAD=false BOARD=arduino_due_x TARGET_ARCH=arm TARGET_TRANSPORT=IP SHIELD=WIFI RELEASE=$3


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
        echo "     `basename $0` <path-to-android-ndk>"
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
scons resource RELEASE=false -c
scons resource LOGGING=false RELEASE=false
scons resource TEST=1 RELEASE=false
echo "===================== done ====================="


