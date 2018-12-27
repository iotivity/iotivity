#!/bin/bash

# ./build.sh device_id=android_devices_id clean=true

deviceId=''
push="true"
clean="true"
source=""
iotivity_dir="iotivity"
given_adapter=""
release='1'
stand_alone='true'
security_mode='justworks'
ndk_path=$ANDROID_NDK

for i in `seq 1 $#` 
do
	eval arg=\$$i
	arg=${arg// /+}
    args+=$arg" "
done

arg_parts=(${args//=/ })
len=${#arg_parts[@]}

i=0
while [ $i -lt $len ]; do
    arg_parts[i]=${arg_parts[i],,}
    let i=i+2
done

i=0
while [ $i -lt $len ]; do
    if [[ "${arg_parts[i]}" = "device_id" ]]; then
        deviceId="-s "${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "push" ]]; then
        push=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "clean" ]]; then
        clean=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "target_source" ]]; then
        source=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "iotivity_dir" ]]; then
        iotivity_dir=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "release" ]]; then
        release=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "stand_alone" ]]; then
        stand_alone=${arg_parts[i+1],,}     
    elif [[ "${arg_parts[i]}" = "ndk_path" ]]; then
        ndk_path=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "security_mode" ]]; then
        security_mode=${arg_parts[i+1],,}        
    fi
    let i=i+2
done

if [[ "${stand_alone}" = "false" ]] || [[ "${stand_alone}" = "0" ]]; then
    cd build/android/iut
fi    

ndk_file=${ndk_path}"/ndk-build"
if [ ! -f "$ndk_file" ]; then
    echo 'Invalid ANDROID_NDK. No ndk-build found'
    echo 'Script Exiting...'
    exit 127
fi

current_path=`pwd`
echo 'current_path: '$current_path

cd ../../../
test_project_root=`pwd`
echo 'test_project_root: '$test_project_root
	
cd ..
iotivity_path=`pwd`
echo 'iotivity_path: '$iovitiy_path

cd $current_path

if [[ "${clean}" = "true" ]] || [[ "${clean}" = "1" ]]; then
    rm -rf libs
    rm -rf obj
fi

if [[ "${release}" = "true" ]] || [[ "${release}" = "1" ]]; then
    release_dir='release'
else
    release_dir='debug'    
fi

export IOTIVITY_PATH=$iotivity_path
export RELEASE_DIR=$release_dir
export TEST_PROJECT_ROOT=$test_project_root

echo '-----------------------Environment Variable-----------------------'
echo $TEST_PROJECT_ROOT
echo $IOTIVITY_PATH
echo $RELEASE_DIR
echo '-----------------------End-----------------------'

rm -rf libs

${ndk_path}/ndk-build

if [ -f libs/armeabi/ConformanceSimulator ]; then
    echo 'Build Successful'
else
    echo 'Build Failed'
    exit 127
fi

if [[ "${push}" = "true" ]] || [[ "${push}" = "1" ]]; then
    echo 'Pushing files in adnroid device ...'

    adb $deviceId push libs/armeabi/libconnectivity_abstraction.so /data/local/tmp/obj/local/armeabi/libconnectivity_abstraction.so
    adb $deviceId push libs/armeabi/libgnustl_shared.so /data/local/tmp/obj/local/armeabi/
    adb $deviceId push libs/armeabi/liboc.so /data/local/tmp/obj/local/armeabi/
    adb $deviceId push libs/armeabi/liboc_logger.so /data/local/tmp/obj/local/armeabi/
    adb $deviceId push libs/armeabi/libocpmapi.so /data/local/tmp/obj/local/armeabi/
    adb $deviceId push libs/armeabi/liboctbstack.so /data/local/tmp/obj/local/armeabi/
    adb $deviceId push libs/armeabi/ConformanceSimulator /data/local/tmp/
    
    if [[ "${security_mode}" = "justworks" ]]; then
        db_filename='oic_svr_db_server_justworks.dat'
    elif [[ "${arg_parts[i]}" = "randompin" ]]; then
        db_filename='oic_svr_db_server_randompin.dat'
    fi
    
    adb $deviceId push $iotivity_path/resource/csdk/security/provisioning/sample/oic_svr_db_server_justworks.dat /usr/apps/com.oic.cs/bin/oic_svr_db_server_justworks.dat
    adb $deviceId push $iotivity_path/resource/csdk/security/provisioning/sample/oic_svr_db_server_randompin.dat /usr/apps/com.oic.cs/bin/oic_svr_db_server_randompin.dat
    adb $deviceId push $iotivity_path/resource/csdk/security/provisioning/sample/$db_filename /usr/apps/com.oic.cs/bin/oic_svr_db_server.dat
    
fi

mkdir -p $test_project_root/bin/android/iut/
cp -rf $test_project_root/build/android/iut/libs/armeabi/* $test_project_root/bin/android/iut
rm -rf $test_project_root/build/android/iut/libs

if [[ "${stand_alone}" = "false" ]] || [[ "${stand_alone}" = "0" ]]; then
    cd ../../../
fi    
