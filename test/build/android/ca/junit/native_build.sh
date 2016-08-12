#!/bin/bash

# Run Command

# ./native_build.sh clean=true source=last type=tc

for i in `seq 1 $#` 
do
	eval arg=\$$i
	arg=${arg// /+}
    args+=$arg" "
done

arg_parts=(${args//=/ })
len=${#arg_parts[@]}

clean="true"
source=""
type="tc"

i=0
while [ $i -lt $len ]; do
    if [[ "${arg_parts[i]}" = "clean" || "${arg_parts[i]}" = "Clean" ]]; then
        clean=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "source" || "${arg_parts[i]}" = "Source" ]]; then
        source=${arg_parts[i+1]}    
    elif [[ "${arg_parts[i]}" = "type" || "${arg_parts[i]}" = "Type"  || "${arg_parts[i]}" = "TYPE" ]]; then
        type=${arg_parts[i+1]}    
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

export SECTEST_PATH=$current_oictest_path
export IOTIVITY_PATH=$current_iotivity_path

export IP_ADAPTER_FLAG=IP_ADAPTER
export EDR_ADAPTER_FLAG=EDR_ADAPTER
export LE_ADAPTER_FLAG=LE_ADAPTER

dst_path=$SECTEST_PATH"/build/android/ca/junit/jni"
tc_path=$SECTEST_PATH"/src/tc/ca/junit/jni"
simulator_path=$SECTEST_PATH"/src/testapp/ca/android/casimulator/caService/src/main/jni"

echo "dst_path: "$dst_path
echo "tc_path: "$tc_path
echo "simulator_path: "$simulator_path

echo 'removing previous file ...'

rm $dst_path/org_iotivity_ca_service_RMInterface.h
rm $dst_path/ResourceModel.c

echo 'removed previous file'
	
if [[ "${type}" = "tc" ]]; then
	echo 'copy tc file'
	cp $tc_path/org_iotivity_ca_service_RMInterface.h $dst_path/org_iotivity_ca_service_RMInterface.h
	cp $tc_path/ResourceModel.c $dst_path/ResourceModel.c
fi

if [[ "${type}" = "simulator" ]]; then
	echo 'copy simulator file'	
	cp $simulator_path/org_iotivity_test_ca_service_RMInterface.h $dst_path/org_iotivity_test_ca_service_RMInterface.h
	cp $simulator_path/ResourceModel.c $dst_path/ResourceModel.c
fi

if [[ "${clean}" = "true" ]]; then
    rm -rf libs
    rm -rf obj
fi

echo '-----------------------Environment Variable-----------------------'
echo $SECTEST_PATH
echo $IOTIVITY_PATH

echo '-----------------------End-----------------------'

${ANDROID_NDK}/ndk-build $binary_name

mkdir -p $SECTEST_PATH/extlibs/android/ca/armeabi
cp -r ./libs/armeabi/* $SECTEST_PATH/extlibs/android/ca/armeabi
