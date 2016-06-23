#!/bin/bash

# Run Command

# ./build.sh deviceId=android_devices_id adapter=ip,edr,le clean=true source=last

deviceId=''
push="false"
clean="true"
source=""
given_adapter=""

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
    if [[ "${arg_parts[i]}" = "deviceId" || "${arg_parts[i]}" = "DeviceId" ]]; then
        deviceId="-s "${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "push" || "${arg_parts[i]}" = "Push" ]]; then
        push=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "clean" || "${arg_parts[i]}" = "Clean" ]]; then
        clean=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "source" || "${arg_parts[i]}" = "Source" ]]; then
        source=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "adapter" || "${arg_parts[i]}" = "Adapter" ]]; then
        given_adapter=${arg_parts[i+1]}
    fi
    let i=i+2
done

if [[ "${ANDROID_NDK}" = "" ]]; then
    echo 'ANDROID_NDK NOT DEFINED'
    echo 'Script Exiting...'
    exit 127
fi

ndk_file=${ANDROID_NDK}"/ndk-build"
if [ ! -f "$ndk_file" ]; then
    echo 'Invalid ANDROID_NDK. No ndk-build found'
    echo 'Script Exiting...'
    exit 127
fi

current_path=`pwd`
cd ../../../../
current_oictest_path=`pwd`
cd $current_path
current_path=`pwd`
echo "pwd: "$current_path
cd ../../../../../
	
if [[ "${source}" = "" ]]; then		
	current_iotivity_path=`pwd`	
else
    path=`pwd`
	current_iotivity_path=${path}"/IotivityOrgSource/"${source}"/iotivity"    
fi

cd $current_path

echo $current_iotivity_path

export SECTEST_PATH=$current_oictest_path
export IOTIVITY_PATH=$current_iotivity_path
    
if [[ "$given_adapter" = "" ]]; then
    export EDR_ADAPTER_FLAG=EDR_ADAPTER
    export IP_ADAPTER_FLAG=IP_ADAPTER
    export LE_ADAPTER_FLAG=LE_ADAPTER    
else
    if [[ "$given_adapter" = *"ip"* || "$given_adapter" = *"IP"* ]]; then
        export IP_ADAPTER_FLAG=IP_ADAPTER
    else
        export IP_ADAPTER_FLAG=NO_IP_ADAPTER
    fi
    if [[ "$given_adapter" = *"edr"* || "$given_adapter" = *"EDR"* ]]; then
        export EDR_ADAPTER_FLAG=EDR_ADAPTER
    else
        export EDR_ADAPTER_FLAG=NO_EDR_ADAPTER
    fi
    if [[ "$given_adapter" = *"le"* || "$given_adapter" = *"LE"* ]]; then
        export LE_ADAPTER_FLAG=LE_ADAPTER
    else
        export LE_ADAPTER_FLAG=NO_LE_ADAPTER
    fi
fi

if [[ "${clean}" = "true" ]]; then
    rm -rf libs
    rm -rf obj
fi

echo '-----------------------Environment Variable-----------------------'
echo $OICTEST_PATH
echo $IOTIVITY_PATH
echo $LE_ADAPTER_FLAG
echo $IP_ADAPTER_FLAG
echo $EDR_ADAPTER_FLAG
echo '-----------------------End-----------------------'

${ANDROID_NDK}/ndk-build

if [[ "${push}" = "true" ]]; then
    adb $deviceId push ../../../../res/ca_resource/runner.sh /data/local/tmp/
    adb $deviceId push libs/armeabi/libCAInterface.so /data/local/tmp/
    adb $deviceId push libs/armeabi/libTinyDtls.so /data/local/tmp/
    adb $deviceId push libs/armeabi/libgnustl_shared.so /data/local/tmp/
    adb $deviceId push libs/armeabi/iotivity_ca_test /data/local/tmp/
    adb $deviceId push libs/armeabi/iotivity_ca_simulator /data/local/tmp/
    adb $deviceId push ../../../../bin/linux/config.ini /data/local/tmp/
fi
