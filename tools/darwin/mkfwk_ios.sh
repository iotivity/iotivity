#!/bin/sh
#===============================================================================
# Author:    Pete Goodliffe
# Copyright: (c) Copyright 2009 Pete Goodliffe
# Licence:   Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#===============================================================================
#VERSION_IOS="${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_NUMBER}.${BUILD_NUMBER}"
VERSION_IOS="0.9.0.1"


OUTDIR=$PWD/out/ios
BUILD=release
LIBCOAP=libcoap
SDKLIB=liboctbstack
LIBC_COMMON=libc_common
LIBCJSON=libcjson
LIBCONNECTIVITY_ABSTRACTION_INTERNAL=libconnectivity_abstraction_internal
LIBCONNECTIVITY_ABSTRACTION=libconnectivity_abstraction
LIBESMEDIATORRICH=libESMediatorRich
LIBLOGGER=liblogger
LIBMBEDCRYPTO=libmbedcrypto
LIBMBEDTLS=libmbedtls
LIBMBEDX509=libmbedx509
LIBNOTIFICATION_CONSUMER_WRAPPER=libnotification_consumer_wrapper
LIBNOTIFICATION_CONSUMER=libnotification_consumer
LIBNOTIFICATION_PROVIDER_WRAPPER=libnotification_provider_wrapper
LIBNOTIFICATION_PROVIDER=libnotification_provider
LIBOC_LOGGER_INTERNAL=liboc_logger_internal
LIBOC_LOGGER=liboc_logger
LIBOCLIB=liboc
LIBOCPROVISION=libocprovision
LIBOCSRM=libocsrm
LIBOCTBSTACK_INTERNAL=liboctbstack_internal
LIBRCS_CLIENT=librcs_client
LIBRCS_COMMON=librcs_common
LIBRCS_CONTAINER=librcs_container
LIBRCS_SERVER=librcs_server
LIBRESOURCE_DIRECTORY=libresource_directory
LIBROUTINGMANAGER=libroutingmanager
LIBSCENE_MANAGER=libscene_manager

LIPO="xcrun -sdk iphoneos lipo"


VERSION_TYPE=Alpha
FRAMEWORK_NAME=iotivity
FRAMEWORK_VERSION=A
FRAMEWORK_CURRENT_VERSION=${VERSION_IOS}
FRAMEWORK_COMPATIBILITY_VERSION=${VERSION_IOS}
FRAMEWORKDIR=out/ios

FRAMEWORK_BUNDLE=$FRAMEWORKDIR/$FRAMEWORK_NAME.framework
rm -rf $FRAMEWORK_BUNDLE

echo "Framework: Setting up directories..."
mkdir -p $FRAMEWORK_BUNDLE
mkdir -p $FRAMEWORK_BUNDLE/Versions
mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION
mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/Resources
mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/Headers
mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/Documentation

echo "Framework: Creating symlinks..."
ln  -s $FRAMEWORK_VERSION               $FRAMEWORK_BUNDLE/Versions/Current
ln  -s Versions/Current/Headers         $FRAMEWORK_BUNDLE/Headers
ln  -s Versions/Current/Resources       $FRAMEWORK_BUNDLE/Resources
ln  -s Versions/Current/Documentation   $FRAMEWORK_BUNDLE/Documentation
ln  -s Versions/Current/$FRAMEWORK_NAME $FRAMEWORK_BUNDLE/$FRAMEWORK_NAME
FRAMEWORK_INSTALL_NAME=$FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/$FRAMEWORK_NAME

lipolite()
{
   PREV="$PWD"
   cd "$1"
   ar -x "$2"
   cd "$PREV"
}


echo "Extracting libraries..."
mkdir $OUTDIR/objs

ARCHS="armv7 armv7s arm64 i386 x86_64"
FATFILE=""

for ARCH in $ARCHS
do
echo "extracting $ARCH"
mkdir $OUTDIR/objs/$ARCH
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBCOAP.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBC_COMMON.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBCJSON.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBCONNECTIVITY_ABSTRACTION_INTERNAL.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBCONNECTIVITY_ABSTRACTION.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBESMEDIATORRICH.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBLOGGER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBOCLIB.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBMBEDCRYPTO.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBMBEDTLS.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBMBEDX509.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBNOTIFICATION_CONSUMER_WRAPPER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBNOTIFICATION_CONSUMER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBNOTIFICATION_PROVIDER_WRAPPER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBNOTIFICATION_PROVIDER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBOC_LOGGER_INTERNAL.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBOC_LOGGER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBOCPROVISION.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBOCSRM.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBOCTBSTACK_INTERNAL.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$SDKLIB.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBRCS_CLIENT.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBRCS_COMMON.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBRCS_CONTAINER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBRCS_SERVER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBRESOURCE_DIRECTORY.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBROUTINGMANAGER.a"
lipolite $OUTDIR/objs/$ARCH "$OUTDIR/$ARCH/$BUILD/$LIBSCENE_MANAGER.a"
ar -r $OUTDIR/objs/$ARCH.a $OUTDIR/objs/$ARCH/*.o
done


echo "Lipoing library into $FRAMEWORK_INSTALL_NAME..."
$LIPO \
	-create \
        -arch armv7 "$OUTDIR/objs/armv7.a" \
        -arch armv7s "$OUTDIR/objs/armv7s.a" \
        -arch arm64 "$OUTDIR/objs/arm64.a" \
        -arch i386 "$OUTDIR/objs/i386.a" \
        -arch x86_64  "$OUTDIR/objs/x86_64.a" \
        -output "$FRAMEWORK_INSTALL_NAME" \
    || abort "Lipo $1 failed"

echo rm -rf objs
find $OUTDIR/objs -name "*.o" | xargs rm

echo "Framework: Copying includes..."
cp -r  resource/include/*.h  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/oc_logger/include/*.h  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/oc_logger/include/*.hpp  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/csdk/stack/include/*.h  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/csdk/include/*.h  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/c_common/*.h  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/csdk/resource-directory/include/*.h  $FRAMEWORK_BUNDLE/Headers
cp -r  resource/oc_logger/include/targets/oc_ostream_logger.h  $FRAMEWORK_BUNDLE/Headers
cp -r  $OUTDIR/arm64/$BUILD/resource/c_common/iotivity_config.h  $FRAMEWORK_BUNDLE/Headers

echo "Framework: Creating plist..."
cat > $FRAMEWORK_BUNDLE/Resources/Info.plist <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDevelopmentRegion</key>
        <string>English</string>
        <key>CFBundleExecutable</key>
        <string>${FRAMEWORK_NAME}</string>
        <key>CFBundleIdentifier</key>
        <string>org.iotivity</string>
        <key>CFBundleInfoDictionaryVersion</key>
        <string>6.0</string>
        <key>CFBundlePackageType</key>
        <string>FMWK</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>${FRAMEWORK_CURRENT_VERSION}</string>
</dict>
</plist>
EOF

echo
echo "    ================================================================="
echo "    Done"
echo
