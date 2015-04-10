#!/bin/bash

set -e

# Change to extlibs directory
cd "$(dirname "$0")"

EXTDIR=$(pwd)

# Check for cereal existence
if [ ! -d "cereal" ]; then
    git clone https://github.com/USCiLab/cereal.git cereal
    pushd cereal
    git reset --hard 7121e91e6ab8c3e6a6516d9d9c3e6804e6f65245
    git apply ../../resource/patches/cereal_gcc46.patch
    popd
fi

# Pick the preferred version of boost to use
BOOST_MAJOR=1
BOOST_MINOR=57
BOOST_REVISION=0

BOOST_VERSION="${BOOST_MAJOR}.${BOOST_MINOR}.${BOOST_REVISION}"
BOOST_NAME="boost_${BOOST_MAJOR}_${BOOST_MINOR}_${BOOST_REVISION}"
BOOST_FILE="${BOOST_NAME}.zip"

function downloadBoost {
    echo "Downloading boost v${BOOST_VERSION}"
    wget --progress=bar --continue --output-document=${BOOST_FILE} http://downloads.sourceforge.net/project/boost/boost/${BOOST_VERSION}/${BOOST_FILE}?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F${BOOST_VERSION}%2F\&ts=1419450713\&use_mirror=iweb
}

function unpackBoost {
    if [ ! -f "${BOOST_FILE}" ]; then
        downloadBoost
    fi

    echo "Unpacking boost v${BOOST_VERSION}"
    unzip ${BOOST_FILE} >> build.log
    pushd ${BOOST_NAME}
    ./bootstrap.sh
    popd
}

function buildBoost {
    if [ ! -d "${BOOST_NAME}" ]; then
        unpackBoost
    fi

    TOOLCHAIN=${ANDROID_NDK}/toolchains/${TOOLSET}-${VERSION}/prebuilt/linux-x86/bin
    echo "Copying user configs to boost"
    cp ../resource/patches/user-config-${TOOLSET}.jam ${BOOST_NAME}/tools/build/v2/user-config.jam

    OLDPATH=$PATH
    PATH=$TOOLCHAIN:$PATH

    pushd ${BOOST_NAME}
    ./b2 clean
    ./b2 -q \
        target-os=linux \
        link=static \
        threading=multi \
        --layout=system \
        --prefix="./../../out/boost" \
        -s PLATFORM=android-${PLATFORM} \
        -s VERSION=${VERSION} \
        --with-thread \
        install
    popd

    PATH=$OLDPATH

    mkdir -p ${INCPATH}
    cp -R ../out/boost/include/* ${INCPATH}
    mkdir -p ${LIBPATH}
    cp -R ../out/boost/lib/*     ${LIBPATH}
}

function checkBoost {
    PLATFORM=$1
    TOOLSET=$2
    VERSION=$3

    INCPATH="$(dirname "$0")/../out/android/include"
    LIBPATH="$(dirname "$0")/../out/android/lib/${TOOLSET}"

    if [ ! -d "${INCPATH}" ];
    then
        buildBoost
    fi
    if [ ! -d "${LIBPATH}" ];
    then
        buildBoost
    fi
}

checkBoost 19 arm-linux-androideabi 4.9
checkBoost 19 x86 4.9

