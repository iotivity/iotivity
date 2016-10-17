#!/bin/bash

# Fail script on any failure
set -e

# Move to script direotory
pushd `dirname $0` > /dev/null

rm -rf docs

# JavaDoc now... from Uze's script
ANDROID_JAR="$ANDROID_HOME/platforms/android-21/android.jar"

#if [ ! -e "$ANDROID_JAR" ]; then
#    echo "Android platform not found. Expected '$ANDROID_JAR'"
#    exit 1
#fi
BASE_PATH="android/android_api/base/src/main/java/"
BASE_PKG="org.iotivity.base"

ES_PATH="service/easy-setup/mediator/richsdk/android/EasySetupCore/src/main/java/"
ES_PKG="org.iotivity.service.easysetup.mediator"

NS_PATH="service/notification/android/notification-service/src/main/java/"
NS_PKG_1="org.iotivity.service.ns.common"
NS_PKG_2="org.iotivity.service.ns.provider"
NS_PKG_3="org.iotivity.service.ns.consumer"

RC_PATH="service/resource-container/android/resource-container/src/main/java/"
RC_PKG="org.iotivity.service.resourcecontainer"

RE_PATH="service/resource-encapsulation/android/service/src/main/java/"
RE_PKG_1="org.iotivity.service"
RE_PKG_2="org.iotivity.service.server"
RE_PKG_3="org.iotivity.service.client"

TM_PATH="service/things-manager/sdk/java/src/"
TM_PKG="org.iotivity.service.tm"


javadoc -splitindex \
        -d ./docs/java \
        -sourcepath $BASE_PATH:$RE_PATH:$RC_PATH:$ES_PATH:$NS_PATH \
        $BASE_PKG $NS_PKG_1 $NS_PKG_2 $NS_PKG_3 $RE_PKG_1 $RE_PKG_2 $RE_PKG_3 $RC_PKG $ES_PKG 
#        -classpath $ANDROID_JAR

# Doxygen now...
# NOTE: For now this is a workaround since I am ready to modify the doxygen setup
pushd resource/docs > /dev/null
doxygen

# Check for warnings or errors
if [ -s doxygen.log ]; then
    echo "Errors running doxygen. Review doxygen.log"
    exit 2
fi

popd > /dev/null
rm -rf docs/cxx
mv resource/docs/docs/html docs/cxx

popd > /dev/null
