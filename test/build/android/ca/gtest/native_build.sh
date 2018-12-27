#!/bin/bash

push='exe'
clean='1'
release_dir='debug'
total_device=1
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
stand_alone='1'
android_home=${ANDROID_HOME}
android_ndk=${ANDROID_NDK}
target_arch='x86'

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
    elif [[ "${arg_parts[i]}" = "iotivity_root" ]]; then
        IOTIVITY_ROOT=${arg}
    elif [[ "${arg_parts[i]}" = "iotivity_test_root" ]]; then
        IOTIVITY_TEST_ROOT=${arg}
    elif [[ "${arg_parts[i]}" = "clean" ]]; then
        clean=${arg}
    elif [[ "${arg_parts[i]}" = "target_arch" ]]; then
        target_arch=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "release_dir" ]]; then
        release_dir=${arg}
    elif [[ "${arg_parts[i]}" = "android_home" ]]; then
        android_home=${arg}
    elif [[ "${arg_parts[i]}" = "stand_alone" ]]; then
        stand_alone=${arg}
    fi
    let i=i+2
done

adb_path=${android_home}'/platform-tools/adb'

if [[ "${push}" = "exe" || "${push}" = "lib" ]]; then

    device_ids=$(${adb_path} devices | awk '/[a-zA-Z0-9]+[ \t]+device$/{print $1}')
    
    device_list=(${device_ids})
    total_device=${#device_list[@]}

    if [ $total_device -lt 1 ]; then
        echo ${red}'No device Found'${reset}
        exit 127
    fi

    echo 'total_device: '${total_device}
fi

cwd=$(pwd)
cd build/android/ca/gtest

if [[ "${clean}" = "1" ]]; then
    rm -rf obj
fi

echo '-----------------------Environment Variable-----------------------'
export RELEASE_DIR=${release_dir}
export IOTIVITY_TEST_ROOT=${IOTIVITY_TEST_ROOT}
export IOTIVITY_ROOT=${IOTIVITY_ROOT}
export IOTIVITY_TARGET_ARCH=${target_arch}
echo ${IOTIVITY_ROOT}
echo ${IOTIVITY_TEST_ROOT}
echo '-----------------------End-----------------------'

rm -rf libs

${android_ndk}/ndk-build

cd ${cwd}

file_list=(libconnectivity_abstraction.so libgnustl_shared.so iotivity_ca_test iotivity_ca_ip_test iotivity_ca_tcp_test iotivity_ca_simulator)

total_file=${#file_list[@]}

i=0

while [ $i -lt $total_file ]; do
  
    if [ ! -f build/android/ca/gtest/libs/${target_arch}/${file_list[i]}  ]; then
        echo ${red}'File '${file_list[i]}' isn not created !!!'${reset}
        echo ${red}'Build Failed'${reset}
        exit 127
    fi
  
    echo ${green}${file_list[i]}' Build Successful'${reset}
  
    let i=i+1
done


if [[ "${push}" = "exe" || "${push}" = "lib" ]]; then
    i=0
    while [ $i -lt $total_device ]; do
    
        device_id='-s '${device_list[i]}
        echo 'device_id: '${device_id}

        ${adb_path} $device_id push ${IOTIVITY_TEST_ROOT}/res/ca_resource/runner.sh /data/local/tmp/
        ${adb_path} $device_id push ${IOTIVITY_TEST_ROOT}/res/ca_resource/config.ini /data/local/tmp/

        echo 'total_file: '${total_file}
        j=0    
        while [ $j -lt $total_file ]; do
            ${adb_path} $device_id push build/android/ca/gtest/libs/${target_arch}/${file_list[j]} /data/local/tmp/
            echo ${green}${file_list[j]}' pushed /data/local/tmp folder in device '${device_id}${reset}
            let j=j+1
        done

        let i=i+1
    done
fi
