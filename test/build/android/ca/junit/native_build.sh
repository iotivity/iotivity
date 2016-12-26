#!/bin/bash

for i in `seq 1 $#`
do
    eval arg=\$$i
    arg=${arg// /+}
    args+=${arg}" "
done

arg_parts=(${args//=/ })
len=${#arg_parts[@]}

clean='1'
type='tc'
release='debug'
android_ndk=${ANDROID_NDK}
stand_alone='1'
target_arch=''

i=0
while [ ${i} -lt ${len} ]; do
    arg_parts[i]=${arg_parts[i],,}
    let i=i+2
done

i=0
while [ ${i} -lt ${len} ]; do
    if [[ "${arg_parts[i]}" = "clean" ]]; then
        clean=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "type" ]]; then
        type=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "release" ]]; then
        release=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "android_ndk" ]]; then
        android_ndk=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "stand_alone" ]]; then
        stand_alone=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "target_arch" ]]; then
        target_arch=${arg_parts[i+1]}
    fi
    let i=i+2
done

if [[ "${stand_alone}" = "0" ]]; then
    cd build/android/ca/junit
fi

if [[ "${android_ndk}" = "" ]]; then
    echo 'ANDROID_NDK NOT DEFINED'
    echo 'Script Exiting...'
    exit 127
fi

ndk_file=${android_ndk}"/ndk-build"
if [ ! -f "${ndk_file}" ]; then
    echo 'Invalid ANDROID_NDK. No ndk-build found in path: '${android_ndk}
    echo 'Script Exiting...'
    exit 127
fi

current_path=`pwd`
cd ../../../../
current_oictest_path=`pwd`
cd ${current_path}
current_path=`pwd`
echo "pwd: "${current_path}
cd ../../../../../

current_iotivity_path=`pwd`

cd ${current_path}

export RELEASE_DIR=${release}
export SECTEST_PATH=${current_oictest_path}
export IOTIVITY_PATH=${current_iotivity_path}
export DEVICE_ARCH=${target_arch}

dst_path=${SECTEST_PATH}"/build/android/ca/junit/jni"
tc_path=${SECTEST_PATH}"/src/tc/ca/junit/jni"
simulator_path=${SECTEST_PATH}"/src/testapp/ca/android/casimulator/src/main/jni"

echo "dst_path: "${dst_path}
echo "tc_path: "${tc_path}
echo "simulator_path: "${simulator_path}

echo 'removing previous file ...'

rm ${dst_path}/org_iotivity_CAJni.h
rm ${dst_path}/CAJni.c

echo 'removed previous file'

if [[ "${type}" = "tc" ]]; then
    echo 'copy tc file'
    cp ${tc_path}/org_iotivity_CAJni.h ${dst_path}/org_iotivity_CAJni.h
    cp ${tc_path}/CAJni.c ${dst_path}/CAJni.c
fi

if [[ "${type}" = "simulator" ]]; then
    echo 'copy simulator file'
    cp ${simulator_path}/org_iotivity_CAJni.h ${dst_path}/org_iotivity_CAJni.h
    cp ${simulator_path}/CAJni.c ${dst_path}/CAJni.c
fi

if [[ "${clean}" = "1" ]]; then
    rm -rf libs
    rm -rf obj
fi

echo '-----------------------Environment Variable-----------------------'
echo ${SECTEST_PATH}
echo ${IOTIVITY_PATH}
echo ${RELEASE_DIR}
echo ${DEVICE_ARCH}
echo '-----------------------End-----------------------'

${android_ndk}/ndk-build ${binary_name}

mkdir -p ${SECTEST_PATH}/extlibs/android/ca/${target_arch}
cp -r ./libs/${target_arch}/* ${SECTEST_PATH}/extlibs/android/ca/${target_arch}

if [[ "${stand_alone}" = "0" ]]; then
    cd ../../../..
fi
