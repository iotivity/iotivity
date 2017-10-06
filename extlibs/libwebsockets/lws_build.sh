#!/bin/sh
# libwebsockets build script
# This script takes two parameters.
# First parameter indicates the usage of SSL. If the value is 1, then SSL will be enabled.
# Second parameter indicates the build type. If the value is 1, then the library will be built in RELEASE mode.
# Third parameter indicates the target operating system. 0 for linux, 1 for android and 2 for tizen.

# Get into the libwebsockets library
cd libwebsockets

# If build directory for libwebsockets library exists, then remove it.
# It makes the library to get rebuilt every time.
# This is to ensure that the library gets built properly for different platforms(linux/android/tizen) without a prior clean operation.
# Actual problem: If library is already built for linux platform and we try to rebuild it for android platform, then the source files of the library
# will not be rebuilt for android platform and the corresponding static/shared libraries of libwebsockets will not be generated.
# So in order to get the library rebuilt for android platform, removal of build directory is required.
build_dir="$PWD/build"
if [ -d "$build_dir" ]; then
    rm -rf build
fi

# Create build directory
mkdir build

# Get into the build directory
cd build

# Build and install the library
ssl=0
if [ $1 = 1 ]; then
	ssl=1
fi

build_type=DEBUG
if [ $2 = 1 ]; then
	build_type=RELEASE
fi

android_toolchain=''
if [ $3 = 1 ]; then
	android_toolchain='-DCMAKE_TOOLCHAIN_FILE=../android-toolchain.cmake'
fi

cmake .. -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_INSTALL_PREFIX:PATH=. $android_toolchain -DLWS_WITH_SSL=$ssl -DLWS_WITHOUT_TESTAPPS=1

make
