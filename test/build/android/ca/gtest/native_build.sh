#!/bin/bash

# Run Command

# ./native_build.sh push=true clean=true android_ndk=ndk-absolute_path 

push="false"
clean="true"
total_device=1
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
    
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
    elif [[ "${arg_parts[i]}" = "android_ndk" ]]; then
        ANDROID_NDK=${arg}        
    fi
    let i=i+2
done

if [[ "${ANDROID_NDK}" = "" ]]; then
    echo ${red}'ANDROID_NDK NOT DEFINED'${reset}
    echo ${red}'Script Exiting...'${reset}
    exit 127
fi

ndk_file=${ANDROID_NDK}"/ndk-build"
if [ ! -f "$ndk_file" ]; then
    echo ${red}'Invalid ANDROID_NDK. No ndk-build found'${reset}
    echo ${red}'Script Exiting...'${reset}
    exit 127
fi

if [[ "${push}" = "true" || "${push}" = "1" ]]; then

    device_ids=$(adb devices | grep -o '\b[a-f0-9]\+\b')
    
    device_list=(${device_ids})
    total_device=${#device_list[@]}

    if [ $total_device -lt 1 ]; then
        echo ${red}'No device Found'${reset}
    fi    

    echo 'total_device: '${total_device}    
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

if [[ "${clean}" = "true" || "${clean}" = "1" ]]; then
    rm -rf obj
fi

export IP_ADAPTER_FLAG=IP_ADAPTER
export EDR_ADAPTER_FLAG=NO_EDR_ADAPTER
export LE_ADAPTER_FLAG=NO_LE_ADAPTER
export SECTEST_PATH=$current_oictest_path
export IOTIVITY_PATH=$current_iotivity_path

echo '-----------------------Environment Variable-----------------------'
echo $OICTEST_PATH
echo $IOTIVITY_PATH
echo $LE_ADAPTER_FLAG
echo $IP_ADAPTER_FLAG
echo $EDR_ADAPTER_FLAG
echo '-----------------------End-----------------------'

rm -rf libs

${ANDROID_NDK}/ndk-build

file_list=(libHelperInterface.so libTcpHelperInterface.so libTinyDtls.so libgnustl_shared.so iotivity_ca_test iotivity_ca_ip_test iotivity_ca_tcp_test iotivity_ca_simulator)

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


if [[ "${push}" = "true" || "${push}" = "1" ]]; then
    
    i=0
    while [ $i -lt $total_device ]; do
    
        device_id='-s '${device_list[i]}
        echo 'device_id: '${device_id}
        
        adb $device_id push ../../../../res/ca_resource/runner.sh /data/local/tmp/
        adb $device_id push ../../../../bin/linux/config.ini /data/local/tmp/
            
        j=0    
        while [ $j -lt $total_file ]; do
            adb $device_id push libs/armeabi/${file_list[j]} /data/local/tmp/    
            echo ${green}${file_list[j]}' pushed /data/local/tmp folder in device '${device_id}${reset}
            let j=j+1
        done    
        
        let i=i+1
    done    
fi
