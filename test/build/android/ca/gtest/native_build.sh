#!/bin/bash

# Run Command

# ./native_build.sh push=1 clean=1 android_ndk=ndk-absolute_path android_home=sdk_absolute_path

push='1'
clean='1'
release='debug'
total_device=1
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
stand_alone='1'
android_home=${ANDROID_HOME}
android_ndk=${ANDROID_NDK}

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
    arg=${arg_parts[i+1]}
    arg=${arg//+/ }
    if [[ "${arg_parts[i]}" = "push" ]]; then
        push=${arg}
    elif [[ "${arg_parts[i]}" = "clean" ]]; then
        clean=${arg}
    elif [[ "${arg_parts[i]}" = "release" ]]; then
        release=${arg}
    elif [[ "${arg_parts[i]}" = "android_home" ]]; then
        android_home=${arg}
    elif [[ "${arg_parts[i]}" = "android_ndk" ]]; then
        android_ndk=${arg}
    elif [[ "${arg_parts[i]}" = "stand_alone" ]]; then
        stand_alone=${arg}
    fi
    let i=i+2
done

if [[ "${android_home}" = "" ]]; then
    echo ${red}'android_home NOT DEFINED'${reset}
    echo ${red}'Script Exiting...'${reset}
    exit 127
fi

adb_path=${android_home}'/platform-tools/adb'

if [ ! -f "$adb_path" ]; then
    echo ${red}'Invalid android_home. No android_home found'${reset}
    echo ${red}'Script Exiting...'${reset}
    exit 127
fi

if [[ "${android_ndk}" = "" ]]; then
    echo ${red}'android_ndk NOT DEFINED'${reset}
    echo ${red}'Script Exiting...'${reset}
    exit 127
fi

ndk_file=${android_ndk}"/ndk-build"
if [ ! -f "$ndk_file" ]; then
    echo ${red}'Invalid android_ndk. No ndk-build found'${reset}
    echo ${red}'Script Exiting...'${reset}
    exit 127
fi

if [[ "${push}" = "1" ]]; then

    device_ids=$(${adb_path} devices | grep -o '\b[a-f0-9]\+\b')
    
    device_list=(${device_ids})
    total_device=${#device_list[@]}

    if [ $total_device -lt 1 ]; then
        echo ${red}'No device Found'${reset}
    fi    

    echo 'total_device: '${total_device}    
fi

if [[ "${stand_alone}" = "0" ]]; then
    cd build/android/ca/gtest
fi

current_path=`pwd`
cd ../../../../
current_oictest_path=`pwd`
cd $current_path
current_path=`pwd`
echo "pwd: "$current_path
cd ../../../../../
    
current_iotivity_path=`pwd`    

cd $current_path

echo $current_iotivity_path

if [[ "${clean}" = "1" ]]; then
    rm -rf obj
fi

echo '-----------------------Environment Variable-----------------------'
export RELEASE_DIR=$release
export SECTEST_PATH=$current_oictest_path
export IOTIVITY_PATH=$current_iotivity_path
echo $OICTEST_PATH
echo $IOTIVITY_PATH
echo '-----------------------End-----------------------'

rm -rf libs

${android_ndk}/ndk-build

file_list=(libconnectivity_abstraction.so libgnustl_shared.so iotivity_ca_test iotivity_ca_ip_test iotivity_ca_tcp_test iotivity_ca_simulator)

total_file=${#file_list[@]}

success="1"
i=0

while [ $i -lt $total_file ]; do
  
    if [ ! -f libs/armeabi/${file_list[i]}  ]; then
        success="0"
        echo ${red}'File '${file_list[i]}' isn not created !!!'${reset}
        echo ${red}'Build Failed'${reset}
        exit 127
    fi
  
    echo ${green}${file_list[i]}' Build Successful'${reset}
  
    let i=i+1
done


if [[ "${push}" = "1" ]]; then
    i=0
    while [ $i -lt $total_device ]; do
    
        device_id='-s '${device_list[i]}
        echo 'device_id: '${device_id}
        
        ${adb_path} $device_id push ../../../../res/ca_resource/runner.sh /data/local/tmp/
        ${adb_path} $device_id push ../../../../res/ca_resource/config.ini /data/local/tmp/
            
        echo 'total_file: '$total_file
        j=0    
        while [ $j -lt $total_file ]; do
            ${adb_path} $device_id push libs/armeabi/${file_list[j]} /data/local/tmp/    
            echo ${green}${file_list[j]}' pushed /data/local/tmp folder in device '${device_id}${reset}
            let j=j+1
        done    
        
        let i=i+1
    done    
fi

if [[ "${stand_alone}" = "0" ]]; then
    cd ../../../../
fi
