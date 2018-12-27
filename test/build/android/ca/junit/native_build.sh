#!/bin/bash

for i in `seq 1 $#`
do
    eval arg=\$$i
    arg=${arg// /+}
    args+=$arg" "
done

arg_parts=(${args//=/ })
len=${#arg_parts[@]}

clean='1'
test='tc'
release_dir='debug'
android_ndk=${ANDROID_NDK}
stand_alone='1'
target_arch='armeabi'
iotivity_root=''
iotivity_test_root=''

i=0
while [ ${i} -lt ${len} ]; do
    arg_parts[i]=${arg_parts[i],,}
    let i=i+2
done

i=0
while [ $i -lt $len ]; do
    if [[ "${arg_parts[i]}" = "clean" ]]; then
        clean=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "iotivity_root" ]]; then
        iotivity_root=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "iotivity_test_root" ]]; then
        iotivity_test_root=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "test" ]]; then
        test=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "target_arch" ]]; then
        target_arch=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "release" ]]; then
        release_dir=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "android_ndk" ]]; then
        android_ndk=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "stand_alone" ]]; then
        stand_alone=${arg_parts[i+1]}
    fi
    let i=i+2
done

current_path=`pwd`

if [[ "${stand_alone}" = "0" ]]; then
    cd build/android/ca/junit
fi

if [[ "${android_ndk}" = "" ]]; then
    echo 'ANDROID_NDK NOT DEFINED'
    echo 'Script Exiting...'
    exit 127
fi

ndk_file=${android_ndk}'/ndk-build'
if [ ! -f "${ndk_file}" ]; then
    echo 'Invalid ANDROID_NDK. No ndk-build found in path: '${android_ndk}
    echo 'Script Exiting...'
    exit 127
fi

export RELEASE_DIR=${release_dir}
export IOTIVITY_TEST_ROOT=${iotivity_test_root}
export IOTIVITY_ROOT=${iotivity_root}
export IOTIVITY_TARGET_ARCH=${target_arch}
export CURRENT_PATH=${current_path}

dst_path=${IOTIVITY_TEST_ROOT}'/build/android/ca/junit/jni'
tc_path=${IOTIVITY_TEST_ROOT}'/src/tc/ca/junit/jni'
simulator_path=${IOTIVITY_TEST_ROOT}'/src/testapp/ca/android/casimulator/src/main/jni'

rm ${dst_path}/org_iotivity_CAJni.h
rm ${dst_path}/CAJni.c

if [[ "${test}" = "tc" ]]; then
    cp ${tc_path}/org_iotivity_CAJni.h ${dst_path}/org_iotivity_CAJni.h
    cp ${tc_path}/CAJni.c ${dst_path}/CAJni.c
fi

if [[ "${test}" = "simulator" ]]; then
    cp ${simulator_path}/org_iotivity_CAJni.h ${dst_path}/org_iotivity_CAJni.h
    cp ${simulator_path}/CAJni.c ${dst_path}/CAJni.c
fi

if [[ "${clean}" = "1" ]]; then
    rm -rf libs
    rm -rf obj
fi

${android_ndk}/ndk-build

mkdir -p ${IOTIVITY_TEST_ROOT}/extlibs/android/ca/${target_arch}
cp -r ./libs/${target_arch}/* ${IOTIVITY_TEST_ROOT}/extlibs/android/ca/${target_arch}

cd ${current_path}
