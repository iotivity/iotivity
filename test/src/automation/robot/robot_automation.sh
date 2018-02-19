#!/bin/bash

#os=linux type=api module=ca tc="test case name" tag="tag name"
#os=linux type=sample_app module=ca tc="test case name" tag="tag name"
#os=linux type=sample_app module=ca,sm tc="test case name" tag="tag name"

function kill_app() {

    app_name=${1}
    
    #killall ${app_name}
    #kill -9 $(pgrep ${app_name})
    pkill -9 ${app_name}
}

function write_system_failure () {

    os_type=${1}
    tc_type=${2}
    module=${3}

    if ! [[ -d "report" ]]; then
        mkdir report
    fi

    echo "fail:${module}:${os_type}:system_failure:${tc_type}" >> report/robot_execution_status.txt
}

function android_connectivity_test() {

    os_type=${1}
    tc_type=${2}
    module=${3}

    android_devices=($(${ADB} devices | awk '/[a-zA-Z0-9]+[ \t]+device$/{print $1}'))

    if [[ "${module}" == "re" || "${module}" == "pm" || "${module}" == "es" ]]; then
        linux_ssid=$(iwgetid -r)
        if [  "${linux_ssid}" == "" ]; then
            echo "No wifi AP found for Linux"
            return 1
        fi
        echo ${linux_ssid}
    fi

    for adb_device in "${android_devices[@]}"; do
        android_ap=$(${ADB} -s ${adb_device}  shell dumpsys netstats | grep -E 'iface=wlan.*networkId' | tr -d '\r' | cut -d '=' -f 6 | cut -d '"' -f 2)
        android_ap=$(echo ${android_ap})
        android_ap=$(echo ${android_ap} | cut -d ' ' -f 1)

        if [  "${android_ap}" == "" ]; then
            echo "No wifi AP found for Android ${adb_device}"
            return 1
        fi

        if [[ "${module}" == "re" || "${module}" == "pm" || "${module}" == "es" ]]; then
            if [  "${linux_ssid}" == "${android_ap}" ]; then
                echo ${adb_device}" device is connected in same wifi AP"
            else
                echo "Linux and Android device ${adb_device} aren't connected in same wifi AP"
                echo "Android connect to : " ${android_ap}
                echo "Linux connect to : " ${linux_ssid}
                return 1
            fi
        fi
    done

    if [[ "${module}" == "re" || "${module}" == "pm" || "${module}" == "es" ]]; then
        linux_ip=$(ifconfig wlan0  | grep 'inet ' | cut -d ':' -f 2 | cut -d ' ' -f 1)
        echo ${linux_ip}

        if [  "${linux_ip}" == "" ]; then
            echo "No wifi IP found for Linux"
            return 1
        fi

        for adb_device in "${android_devices[@]}"; do
            echo ${adb_device}
            packet_loss=$(${ADB} -s ${adb_device} shell ping -c 10 ${linux_ip} | grep 'packet loss'| cut -d ',' -f 3 | cut -d '%' -f 1 )
            echo "packet_loss : "${packet_loss}"%"
            if [ ${packet_loss} -lt 50 ]; then
                echo "Linux device" ${linux_ip} "responses perfectly from Android ${adb_device}"
            else
                echo "Linux device" ${linux_ip} "is not response perfectly from Android ${adb_device}"
                return 1
            fi
        done

        for adb_device in "${android_devices[@]}"; do
            echo ${adb_device}
            android_ip=$(${ADB} -s ${adb_device} shell ip addr show wlan0  | grep 'inet ' | cut -d' ' -f6|cut -d/ -f1)
            echo ${android_ip}
            packet_loss=$(ping -c 3 ${android_ip} | grep 'packet loss'| cut -d ',' -f 3 | cut -d '%' -f 1 )
            echo "packet_loss : "${packet_loss}"%"
            if [ "${packet_loss}" -lt 50 ]; then
                echo "Android device" ${adb_device} "responses perfectly from linux"
            else
                echo "Android device" ${adb_device} "is not response perfectly from linux"
                return 1
            fi
        done
    fi
}

function tizen_connectivity_test() {

    os_type=${1}
    tc_type=${2}
    module=${3}

    echo "module ${module}"

    tizen_devices=($(${SDB} devices | awk '/[a-zA-Z0-9]+[ \t]+device[ \t]+[a-zA-Z0-9-]+$/{print $1}'))

    for sdb_device in "${tizen_devices[@]}";
    do
        sdb -s ${sdb_device} root on
        tizen_ap=$(sdb -s ${sdb_device} shell dump_systemstate | grep -E '.*/wifi/connected_ap_name.*' | cut -d '=' -f 2 | cut -d ' ' -f 2)

        if [ "${tizen_ap}" == "" ]; then
            echo "Tizen device ${sdb_device} is not connected with any wifi AP"
            return 1
        fi

        tizen_ap=$(echo ${tizen_ap} | tr -d '\r')
        echo "Wifi ap for Tizen device ${sdb_device} is: ${tizen_ap}"
    done

    if [[ "${module}" == "re" || "${module}" == "es" ]]; then
        linux_ssid=$(iwgetid -r)

        if [  "${linux_ssid}" == "" ]; then
            echo "No wifi AP found for Tizen Testhub"
            return 1
        fi

        echo ${linux_ssid}

        linux_ip=$(ifconfig wlan0  | grep 'inet ' | cut -d ':' -f 2 | cut -d ' ' -f 1)

        if [  "${linux_ip}" == "" ]; then
            echo "No wifi IP found for Tizen Testhub"
            return 1
        fi

        echo ${linux_ip}
    fi

    for sdb_device in "${tizen_devices[@]}"; do
        tizen_ap=$(${SDB} -s ${sdb_device} shell dump_systemstate | grep -E '.*/wifi/connected_ap_name.*' | cut -d '=' -f 2 | cut -d ' ' -f 2)
        if [ "${tizen_ap}" == "" ]; then
            echo "No wifi AP found for Tizen Device ${sdb_device}"
            return 1
        fi

        tizen_ap=$(echo ${tizen_ap})
        tizen_ap=$(echo ${tizen_ap} | cut -d ' ' -f 1)

        if [[ "${module}" == "re" || "${module}" == "es" ]]; then
            if [  "${linux_ssid}" == "${tizen_ap}" ]; then
                echo ${sdb_device}" device is connected in same wifi AP"
            else
                echo "Tizen Testhub and Tizen device ${sdb_device} aren't connected in same wifi AP"
                echo "Tizen connect to: "${tizen_ap}
                echo "Linux connect to: "${linux_ssid}
                return 1
            fi
        fi
    done

    if [[ "${module}" == "re" || "${module}" == "es" ]]; then
        for sdb_device in "${tizen_devices[@]}"; do
            echo ${sdb_device}
            tizen_ip=$(${SDB} -s ${sdb_device} shell ifconfig wlan0 | grep 'inet ' | cut -d ':' -f 2| cut -d ' ' -f 1)
            echo ${tizen_ip}

            packet_loss=$(ping -c 10 ${tizen_ip} | grep 'packet loss'| cut -d ',' -f 3 | cut -d '%' -f 1)
            echo "packet_loss: ${packet_loss} %"

            if [ "${packet_loss}" -lt 50 ]; then
                echo "Tizen device ${sdb_device} responses perfectly from Tizen Testhub"
            else
                echo "Tizen device ${sdb_device} is not response perfectly from Tizen Testhub"
                return 1
            fi
        done
    fi
}

function start_server() {

    cwd=$(pwd)

    if [[ ${x86} = "x86" ]]; then
        if [[ ${MONGODB_ROOT} != "" ]]; then
            if [[ -d ${MONGODB_ROOT} ]]; then
                fuser -k -n tcp ${MONGODB_PORT}
                if [[ -d ${MONGODB_ROOT}"/db" ]]; then
                    rm -rf ${MONGODB_ROOT}"/db"
                fi
                mkdir ${MONGODB_ROOT}"/db"
            fi
        fi
    fi

    if [[ "${clean_build}" == "1" ]]; then
        if [[ "${ENABLE_IOTIVITY_BUILD}" == "1" ]]; then
            cd ${IOTIVITY_ROOT}

            store_dir="../extlibs"

            if [[ -d "${store_dir}" ]]; then
                rm -rf ${store_dir}
            fi

            mkdir ${store_dir}

            if [[ -d "extlibs/tinycbor/tinycbor" ]]; then
                echo "creating backup of tinycbor"
                cd extlibs/tinycbor/tinycbor
                git clean -fx -d ""
                cd ../../../
                cp -rf extlibs/tinycbor/tinycbor ${store_dir}
            fi
            if [[ -d "extlibs/mbedtls/mbedtls" ]]; then
                echo "creating backup of mbedtls"
                cd extlibs/mbedtls/mbedtls
                git clean -fx -d ""
                cd ../../../
                cp -rf extlibs/mbedtls/mbedtls ${store_dir}
            fi
            if [[ -d "extlibs/gtest" ]]; then
                echo "creating backup of gtest"
                cp -rf extlibs/gtest ${store_dir}
            fi
            if [[ -d "extlibs/rapidjson" ]]; then
                echo "creating backup of rapidjson"
                cp -rf extlibs/rapidjson ${store_dir}
            fi
            if [[ -d "extlibs/hippomocks" ]]; then
                echo "creating backup of hippomocks"
                cp -rf extlibs/hippomocks ${store_dir}
            fi

            git clean -fx -d ""
            rm -rf tmp
            rm -rf *
            git checkout .

            if [[ -d "${store_dir}/tinycbor" ]]; then
                echo "restoring tinycbor"
                cp -rf ${store_dir}/tinycbor extlibs/tinycbor/
            fi
            if [[ -d "${store_dir}/mbedtls" ]]; then
                echo "restoring mbedtls"
                cp -rf ${store_dir}/mbedtls extlibs/mbedtls/
            fi
            if [[ -d "${store_dir}/gtest" ]]; then
                echo "restoring gtest"
                cp -rf ${store_dir}/gtest/*.zip extlibs/gtest/
            fi
            if [[ -d "${store_dir}/rapidjson" ]]; then
                echo "restoring rapidjson"
                cp -rf ${store_dir}/rapidjson/*.zip extlibs/rapidjson/
            fi
            if [[ -d "${store_dir}/hippomocks" ]]; then
                echo "restoring hippomocks"
                cp -rf ${store_dir}/hippomocks/*.zip extlibs/hippomocks/
            fi

            rm -rf ${store_dir}

            cd ${cwd}
        fi
    fi
    
    if [[ "${complete_run}" == "1" ]]; then
        rm -rf report/*
    fi

    cd ${IOTIVITY_ROOT}/build_common/linux
    git checkout SConscript

    if [[ "${COVERAGE}" != "0" ]]; then
        echo "intentionally block coverage flag as it cause boost_thread not to be found( (cause unknown)"
        #sed -i "/Import('env')/a env.AppendUnique(CCFLAGS = ['-ftest-coverage' , '-fprofile-arcs'])\nenv.AppendUnique(LIBS = ['gcov'])" ${IOTIVITY_ROOT}/build_common/linux/SConscript
    fi

    cd ${cwd}

    cd device_lib
    gradle install
    ret=${?}
    cd ${cwd}

    if [[ ${ret} != 0 ]]; then
        echo 'DeviceLib Build Falied'
        exit 127
    fi

    pwd

    if [[ "${start}" == *"server"* ]]; then
        fuser -k -n tcp ${REMOTE_PORT}
        cd helper
        if [[ "${start}" = *"client"* ]]; then
            gnome-terminal --tab -e "bash -c \"jython server_runner.py -i ${REMOTE_IP} -p ${REMOTE_PORT}\""
            sleep 10s
        else
            jython server_runner.py -i ${REMOTE_IP} -p ${REMOTE_PORT}
        fi
        cd ${cwd}
    fi
}

function start_client() {

    if [[ "${is_parallel}" = "false" ]]; then
        parallel_prefix=""
        parallel_suffix=""
    fi

    if ! [[ -d "${TEST_ROOT}" ]]; then
        echo 'TEST_ROOT Path does not Exsit'
        exit 127
    fi

    cwd=`pwd`
    cd ${TEST_ROOT}
    TEST_ROOT=`pwd`

    input="bin/linux/pid_list.txt"

    if [ -f $input ]; then
        while IFS= read -r var
        do
            var=`echo $var`
            echo "$var"
        done < "$input"
        rm $input
    fi

    if [[ "${clean_build}" == "1" ]]; then
        if [[ "${ENABLE_TC_BUILD}" == "1" ]]; then
            git checkout -- bin
            rm .sconsign.dblite
            rm -rf bin
        fi
    fi

    cd ${cwd}

    if ! [[ -d "${TEST_MANAGER_ROOT}" ]]; then
        echo 'TEST_MANAGER_ROOT Path does not Exsit'
        exit 127
    fi

    cwd=`pwd`
    cd ${TEST_MANAGER_ROOT}
    TEST_MANAGER_ROOT=`pwd`
    cd ${cwd}

    filter=""

    v=' --variable '

    tctype_list=('api' 'bat' 'interop_app' 'sample_app')
    os_list=('linux' 'android' 'tizen')

    for tctype in ${tctype_list[@]};
    do
        if [[ "${given_tc_type}" != "all" && "${given_tc_type}" != *"${tctype}"* ]]; then
            continue
        fi

        echo 'tctype: '${tctype}

        module_list=('ri' 're' 'rc' 'sm' 'ns' 'rd' 'ic' 'ca' 'es' 'cs' 'pm' 'chp')

        if [[ "${tctype}" == "bat" ]]; then
            module_list=('build' 'coverage' 'unittest')
        fi

        for os_type in ${os_list[@]};
        do
            if [[ "${given_os_type}" != "all" && "${given_os_type}" != *"${os_type}"* ]]; then
                continue
            fi

            echo 'os: '${os_type}

            for os2_type in ${os_list[@]};
            do

                if [[ "${tctype}" == "interop_app" ]]; then
                    if [[ "${given_os2_type}" != *"${os2_type}"* ]]; then
                        continue
                    fi
                    echo ${os_type}'->'${os2_type}
                fi

                echo 'os2: '${os2_type} 

                for module_type in ${module_list[@]};
                do
                    if [[ "${given_module_type}" != "all" && "${given_module_type}" != *"${module_type}"* ]]; then
                        continue
                    fi

                    echo 'module_type: '${module_type}

                    secured_list=('0')

                    if [[ "${module_type}" == "pm" || "${module_type}" == "cs" ]]; then
                        secured_list=('1')
                    fi

                    if [[ "${module_type}" == "es" ]]; then
                        #secured_list=('0' '1')
                        secured_list=('0')
                    fi

                    if [[ "${module_type}" == "chp" ]]; then
                        secured_list=('0' '1')
                    fi

                    sdk_list=('undefined')
                    sdk2_list=('undefined')

                    if [[ "${tctype}" == "interop_app" ]]; then
                        if [[ "${os_type}" == "linux" || "${os_type}" == "tizen" ]]; then
                            if [[ "${module_type}" == "ns" || "${module_type}" == "cs" ]]; then
                                sdk_list=('csdk' 'cpp')
                            fi
                            if [[ "${module_type}" == "pm" ]]; then
                                if [[ "${tctype}" != "api" ]]; then
                                    sdk_list=('csdk')
                                fi
                            fi
                        fi
                        if [[ "${os2_type}" == "linux" || "${os2_type}" == "tizen" ]]; then
                            if [[ "${module_type}" == "ns" || "${module_type}" == "cs" ]]; then
                                sdk2_list=('csdk' 'cpp')
                            fi
                            if [[ "${module_type}" == "pm" ]]; then
                                if [[ "${tctype}" != "api" ]]; then
                                    sdk2_list=('csdk')
                                fi
                            fi
                        fi
                    fi

                    for secured_type in ${secured_list[@]};
                    do
                        echo 'secured_type: '${secured_type}

                        for sdk_type in ${sdk_list[@]};
                        do
                            if [[ "${given_sdk}" != *"${sdk_type}"* ]]; then
                                continue
                            fi

                            for sdk2_type in ${sdk2_list[@]};
                            do
                                if [[ "${given_sdk2}" != *"${sdk2_type}"* ]]; then
                                    continue
                                fi

                                tc_file_filter="tc/${tctype}/Iotivity_Component*_*${os_type^}Test.txt"
                                if [[ "${tctype}" == "interop_app" ]]; then
                                    tc_file_filter="tc/${tctype}/Iotivity_Component*_*Test.txt"
                                fi

                                for full_name in `ls -a ${tc_file_filter}`;
                                do
                                    filter=''
                                    common_filter=''

                                    if [[ "${tctype}" == "api" || "${tctype}" == "bat" ]]; then
                                        filter+=" --include=\""${module_type}"\""
                                    fi

                                    if [[ "${tctype}" == "sample_app" || "${tctype}" == "interop_app" ]]; then
                                        if [[ "${full_name}" != *"_${module_type^^}_"* ]]; then
                                            continue
                                        fi
                                    fi

                                    if [[ "${tctype}" == "interop_app" ]]; then
                                        filter+=" --include=all"
                                        filter+=" --include=\""${os_type}-${os2_type}"\""
                                    fi

                                    echo 'file_name: '${full_name}

                                    transport_list=('nstt') #nsmt for not specific transport type
                                    qos_list=('nsmt') #nsmt for not specific message type

                                    if [[ "${tctype}" == "interop_app" ]]; then
                                        if [[ "${module_type}" == "ri" ]]; then
                                            transport_list=('ip' 'tcp')
                                            qos_list=('nsmt' 'non' 'con')
                                        fi
                                    fi

                                    for transport_type in ${transport_list[@]};
                                    do
                                        if [[ "${given_transport_type}" != *"${transport_type}"* ]]; then
                                            continue
                                        fi

                                        for qos_type in ${qos_list[@]};
                                        do
                                            if [[ "${given_msg_type}" != *"${qos_type}"* ]]; then
                                                continue
                                            fi

                                            temp_common_filter=${common_filter}
                                            temp_filter=${filter}
                                            interop_filter=''
                                            if [[ "${tctype}" == "interop_app" ]]; then
                                                if [[ "${module_type}" == "ri" ]]; then
                                                    interop_filter+=" --include=${qos_type}"
                                                fi
                                            fi

                                            common_filter+=${v}"MODULE:"${module_type}
                                            common_filter+=${v}"TC_TYPE:"${tctype}
                                            common_filter+=${v}"ENABLE_IOTIVITY_BUILD:"${ENABLE_IOTIVITY_BUILD}
                                            common_filter+=${v}"ENABLE_TC_BUILD:"${ENABLE_TC_BUILD}
                                            common_filter+=${v}"ENABLE_TC_RUN:"${ENABLE_TC_RUN}
                                            common_filter+=${v}"ENABLE_SAMPLE_APP_RUN:"${ENABLE_SAMPLE_APP_RUN}

                                            common_filter+=${v}"OS:"${os_type}
                                            if [[ "${tctype}" == "interop_app" ]]; then
                                                common_filter+=${v}"OS2:"${os2_type}
                                            fi

                                            common_filter+=${v}"SECURED:"${secured_type}
                                            common_filter+=${v}"SDK:"${sdk_type}
                                            common_filter+=${v}"SDK2:"${sdk2_type}
                                            common_filter+=${v}"TRANSPORT:"${transport_type}
                                            common_filter+=${v}"QOS:"${qos_type}
                                            common_filter+=${v}"RELEASE:"${RELEASE}
                                            common_filter+=${v}"RELEASE_DIR:"${RELEASE_DIR}
                                            common_filter+=${v}"CLEAN_BUILD:"${clean_build}
                                            common_filter+=${v}"bit:"${bit}
                                            common_filter+=${v}"J:"${J}

                                            if [[ "${module_type}" == "cs" || "${module_type}" == "ic" || "${tctype}" == "bat" ]]; then
                                                common_filter+=${v}"MONGODB_ROOT:"${MONGODB_ROOT}
                                                common_filter+=${v}"MONGODB_FOLDER:"${MONGODB_FOLDER}
                                                common_filter+=${v}"KAFKA_ROOT:"${KAFKA_ROOT}
                                                common_filter+=${v}"AUTH_PROVIDER:"${AUTH_PROVIDER}
                                                common_filter+=${v}"AUTH_USERNAME:"${AUTH_USERNAME}
                                                common_filter+=${v}"AUTH_PASSWORD:"${AUTH_PASSWORD}
                                            fi

                                            if [[ "${os_type}" == "android" || ("${tctype}" == "interop_app" && "${os2_type}" == "android") ]]; then
                                                common_filter+=${v}"ANDROID_HOME:"${ANDROID_HOME}
                                                common_filter+=${v}"ADB:"${ANDROID_HOME}/platform-tools/adb
                                                common_filter+=${v}"PARAM_ANDROID_HOME:ANDROID_HOME="${ANDROID_HOME}
                                                common_filter+=${v}"PARAM_ANDROID_NDK:ANDROID_NDK="${ANDROID_NDK}
                                                common_filter+=${v}"PARAM_ANDROID_GRADLE:ANDROID_GRADLE="${ANDROID_GRADLE}

                                                if [[ "${arch}" == "" ]]; then
                                                    common_filter+=${v}'ANDROID_TARGET_ARCH:armeabi'
                                                else
                                                    common_filter+=${v}'ANDROID_TARGET_ARCH:'${arch}
                                                fi

                                                adb_devices=($(${ANDROID_HOME}/platform-tools/adb devices | awk '/[a-zA-Z0-9]+[ \t]+device$/{print $1}'))

                                                if [[ ${given_android_device_ids} != "" ]]; then
                                                    given_adb_devices=(${given_android_device_ids//,/ })

                                                    for given_adb_device in "${given_adb_devices[@]}";
                                                    do
                                                        found="0"
                                                        for temp in "${adb_devices[@]}";
                                                        do
                                                            if [[ "${temp}" == "${given_adb_device}" ]]; then
                                                                found="1"
                                                            fi
                                                        done
                                                        if [[ "${found}" == "0" ]]; then
                                                            echo ${given_adb_device}' Android Device Id Not Found'
                                                            write_system_failure ${os_type} ${tctype} ${module_type}
                                                            exit 127
                                                        fi
                                                    done
                                                    adb_devices=("${given_adb_devices[@]}")
                                                fi

                                                for adb_device in "${adb_devices[@]}";
                                                do
                                                    echo ${adb_device}
                                                done

                                                if [[ "${adb_devices}" == "" ]]; then
                                                    echo 'No android device found'
                                                    write_system_failure ${os_type} ${tctype} ${module_type}
                                                    exit 127
                                                else
                                                    sz=${#adb_devices[@]}
                                                    common_filter+=${v}"ANDROID_DEVICE_ID:"${adb_devices[0]}
                                                    if [[ "${sz}" -gt 1 ]]; then
                                                        common_filter+=${v}"PEER_ANDROID_DEVICE_ID:"${adb_devices[1]}
                                                    elif [[ "${module_type}" == "ca" || ("${tctype}" == "interop_app" && "${os_type}" == "android" && "${os2_type}" == "android") || ("${tctype}" == "sample_app" && "${os_type}" == "android") ]]; then
                                                        echo 'Two Devices Needed to run CA or interop_app or sample_app android TC'
                                                        #common_filter+=${v}"PEER_ANDROID_DEVICE_ID:"${adb_devices[0]}
                                                        write_system_failure ${os_type} ${tctype} ${module_type}
                                                        exit 127
                                                    fi
                                                fi

                                                android_connectivity_test ${os_type} ${tctype} ${module_type}
                                                ret=$?
                                                if [ ${ret} -ne 0 ]; then
                                                    write_system_failure ${os_type} ${tctype} ${module_type}
                                                    exit 127
                                                fi
                                            fi
                                            if [[ "${os_type}" == "tizen" || ("${tctype}" == "interop_app" && "${os2_type}" == "tizen") ]]; then
                                                common_filter+=${v}"TIZEN_HOME:"${TIZEN_HOME}
                                                common_filter+=${v}"SDB:"${SDB}
                                                common_filter+=${v}"GBS_ROOT_PARENT:"${HOME}
                                                common_filter+=${v}"RPM_VERSION:"${RPM_VERSION}
                                                if [[ "${arch}" == "" ]]; then
                                                    common_filter+=${v}'TIZEN_TARGET_ARCH:armv7l'
                                                else
                                                    common_filter+=${v}'TIZEN_TARGET_ARCH:'${arch}
                                                fi

                                                sdb_devices=($(${SDB} devices | awk '/[a-zA-Z0-9]+[ \t]+device[ \t]+[a-zA-Z0-9-]+$/{print $1}'))

                                                if [[ ${given_tizen_device_ids} != "" ]]; then
                                                    given_sdb_devices=(${given_tizen_device_ids//,/ })

                                                    for given_sdb_device in "${given_sdb_devices[@]}";
                                                    do
                                                        found="0"
                                                        for temp in "${sdb_devices[@]}";
                                                        do
                                                            if [[ "${temp}" == "${given_sdb_device}" ]]; then
                                                                found="1"
                                                            fi
                                                        done
                                                        if [[ "${found}" == "0" ]]; then
                                                            echo ${given_sdb_device}' Tizen Device Id Not Found'
                                                            write_system_failure ${os_type} ${tctype} ${module_type}
                                                            exit 127
                                                        fi
                                                    done
                                                    sdb_devices=("${given_sdb_devices[@]}")
                                                fi

                                                for sdb_device in "${sdb_devices[@]}";
                                                do
                                                    echo ${sdb_device}
                                                    ${SDB} -s ${sdb_device} root on

                                                    output=$(${SDB} -s ${sdb_device} shell ls /opt/usr/media/bin/pid_list.txt)

                                                    if [[ ${output} != *"No such file or directory"* ]]; then
                                                        input=$(${SDB} -s ${sdb_device} shell cat /opt/usr/media/bin/pid_list.txt)

                                                        while read -r var; do
                                                            var=`echo $var`
                                                            echo "$var"
                                                            ${SDB} -s ${sdb_device} shell kill -9 $var
                                                        done <<< "$input"

                                                        ${SDB} -s ${sdb_device} shell rm /opt/usr/media/bin/pid_list.txt
                                                    fi

                                                done

                                                if [[ "${sdb_devices}" == "" ]]; then
                                                    echo 'No tizen device found'
                                                    write_system_failure ${os_type} ${tctype} ${module_type}
                                                    exit 127
                                                else
                                                    sz=${#sdb_devices[@]}
                                                    common_filter+=${v}"TIZEN_DEVICE_ID:"${sdb_devices[0]}
                                                    if [[ "${sz}" -gt 1 ]]; then
                                                        common_filter+=${v}"PEER_TIZEN_DEVICE_ID:"${sdb_devices[1]}
                                                    elif [[ "${module_type}" == "ca" || ("${tctype}" == "interop_app" && "${os_type}" == "tizen" && "${os2_type}" == "tizen") ]]; then
                                                    #elif [[ "${module_type}" == "ca" ]]; then
                                                        echo 'Two Devices Needed to run CA, interop_app and sample_app tizen TC'
                                                        #common_filter+=${v}"PEER_TIZEN_DEVICE_ID:"${sdb_devices[0]}
                                                        write_system_failure ${os_type} ${tctype} ${module_type}
                                                        exit 127
                                                    fi
                                                fi

                                                tizen_connectivity_test ${os_type} ${tctype} ${module_type}
                                                ret=$?
                                                if [ ${ret} -ne 0 ]; then
                                                    write_system_failure ${os_type} ${tctype} ${module_type}
                                                    exit 127
                                                fi
                                            fi
                                            if [[ "${os_type}" == "linux" || "${os2_type}" == "linux" ]]; then
                                                common_filter+=${v}'LINUX_TARGET_ARCH:'${x86}
                                            fi

                                            filter+=${v}'LINUX_LIB_PATH:'${IOTIVITY_ROOT}/out/linux/${x86}/${RELEASE_DIR}

                                            file_suffix=${module_type^^}'-'${tctype^^}'-'${os_type^}

                                            if [[ "${os_type}" = "linux" ]]; then
                                                file_suffix+='-'${version}'-'${bit}
                                            fi

                                            if [[ "${sdk_type}" != "undefined" ]]; then
                                                file_suffix+='-'${sdk_type^^}
                                            fi

                                            if [[ "${tctype}" == "interop_app" ]]; then
                                                file_suffix+='-'${os2_type^}
                                                if [[ "${os2_type}" = "linux" ]]; then
                                                    file_suffix+='-'${version}'-'${bit}
                                                fi
                                            fi

                                            if [[ "${sdk2_type}" != "undefined" ]]; then
                                                file_suffix+='-'${sdk2_type^^}
                                            fi

                                            if [[ "${transport_type}" != "nstt" ]]; then
                                                file_suffix+='-'${transport_type^^}
                                            fi

                                            if [[ "${qos_type}" != "nsmt" ]]; then
                                                file_suffix+='-'${qos_type^^}
                                            fi

                                            #file_suffix+='.html'

                                            fullReport=' --report Report-'${file_suffix}'.html'
                                            fullReport+=' --log Log-'${file_suffix}'.html'
                                            fullReport+=' --output Output-'${file_suffix}'.xml'

                                            if [[ "${given_version}" != *"${version}"* ]]; then
                                                continue
                                            fi

                                            if [[ "${given_bit}" != *"${bit}"* ]]; then
                                                continue
                                            fi

                                            if [[ "${start}" == "local" ]]; then
                                                filter+=${v}'LIB_MODE:file'
                                            else
                                                filter+=${v}'LIB_MODE:client-server'
                                            fi

                                            filter+=${v}'REMOTE_IP:'${REMOTE_IP}
                                            filter+=${v}'REMOTE_PORT:'${REMOTE_PORT}

                                            filter+=${v}'ROBOT_ROOT:'${ROBOT_ROOT}

                                            filter+=${v}'VERSION:'${version}
                                            filter+=${v}'BIT:'${bit}
                                            filter+=${v}'x86:'${x86}
                                            filter+=${v}'IOTIVITY_ROOT:'${IOTIVITY_ROOT}
                                            filter+=${v}'IOTIVITY_COMMIT_ID:'${IOTIVITY_COMMIT_ID}
                                            filter+=${v}'TEST_ROOT:'${TEST_ROOT}
                                            filter+=${v}'TEST_MANAGER_ROOT:'${TEST_MANAGER_ROOT}
                                            filter+=${v}'COVERAGE:'${COVERAGE}
                                            filter+=${v}'HTTP_PROXY_ADDRESS:'${HTTP_PROXY_ADDRESS}
                                            filter+=${v}'HTTPS_PROXY_ADDRESS:'${HTTPS_PROXY_ADDRESS}
                                            filter+=${v}'MONGODB_PORT:'${MONGODB_PORT}

                                            if [[ "${module_type}" = "ca" ]]; then

                                                filter+=${v}"PEER_IP:"${PEER_IP}

                                                if [[ "${PEER_BIT}" = "64" ]]; then
                                                    peer_x86='x86_64'
                                                else
                                                    peer_x86='x86'
                                                fi

                                                filter+=${v}'PEER_x86:'${peer_x86}
                                                filter+=${v}'PEER_IOTIVITY_ROOT:'${PEER_IOTIVITY_ROOT}
                                                filter+=${v}'PEER_TEST_ROOT:'${PEER_TEST_ROOT}
                                            fi

                                            if [[ "${given_tc}" != "all" ]]; then
                                                filter+=" --test=\""${given_tc}"\""
                                            fi

                                            if [[ "${given_tag}" != "" ]]; then
                                                for each_tag in $(echo ${given_tag} | sed "s/,/ /g")
                                                do
                                                    filter+=" --include=\""${each_tag}"\""
                                                done
                                            fi

                                            if [[ "${tctype}" == "interop_app" ]]; then
                                                command="jybot -T -d ./report/"${tctype}/${module_type}" "${fullReport}${common_filter}${filter}${interop_filter}" "${full_name}
                                            else
                                                command="jybot -T -d ./report/"${tctype}/${os_type}" "${fullReport}${common_filter}${filter}" "${full_name}
                                            fi

                                            echo ${command}

                                            if eval ${command}; then
                                                echo 'Successfully Run above command'
                                            else
                                                echo 'Something wrong with above command'
                                            fi
                                            common_filter=${temp_common_filter}
                                            filter=${temp_filter}
                                        done
                                    done
                                done
                            done #done sdk2_type
                        done
                    done
                done

                if [[ "${tctype}" != "interop_app" ]]; then
                    echo 'breaking loop for '${tctype}' '
                    break
                fi
            done
        done
    done

    wait
}

ROBOT_ROOT=`pwd`

source "config.txt"

REMOTE_IP=($(ifconfig -a | grep "inet " | grep "Bcast:" | awk '{print $2}' | awk -F: '{print $2}'))
echo ${REMOTE_IP}

bit=$(getconf LONG_BIT)
echo 'bit: '${bit}
if [ "${bit}" == "64" ]; then
    x86="x86_64"
else
    x86="x86"
fi

version=$(echo $(lsb_release -r) | awk -F '[ .]+' '{print $2}')
echo 'version: '${version}

for i in `seq 1 $#`
do
    eval arg=\${${i}}
    arg=${arg// /+}
    args+=${arg}" "
done

arg_parts=(${args//=/ })

len=${#arg_parts[@]}

given_os_type="android,linux,tizen"
given_os2_type="android,linux,tizen"
given_tc_type="api,bat,sample_app,interop_app"
given_module_type="all"
given_bit="32,64"
given_version="12,14, 16"
given_tc=all
given_tag=''
J='3'
start='server,client'
given_sdk='undefined,csdk,cpp,java'
given_sdk2='undefined,csdk,cpp,java'
given_msg_type='con,non,nsmt'
given_transport_type='ipv4,ipv6,tcp,nstt'
given_android_device_ids=''
given_tizen_device_ids=''

PEER_IP=${REMOTE_IP}
PEER_BIT=${bit}
PEER_VERSION=${version}
PEER_IOTIVITY_ROOT=${IOTIVITY_ROOT}
PEER_TEST_ROOT=${TEST_ROOT}
AUTH_PROVIDER='github'
MONGODB_FOLDER='mongodb-linux-i686-3.0.6'
MONGODB_PORT='27017'
HTTP_PROXY_ADDRESS=''
HTTPS_PROXY_ADDRESS=''
MONGODB_ROOT=''
JYTHON_PATH=''

clean_build='1'

i=0
while [ ${i} -lt ${len} ]; do
    arg_parts[i]=${arg_parts[i],,}
    let i=i+2
done

i=0
while [ ${i} -lt ${len} ]; do
    if [[ "${arg_parts[i]}" = "os" ]]; then
        given_os_type=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "os2" ]]; then
        given_os2_type=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "type" ]]; then
        given_tc_type=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "module" ]]; then
        given_module_type=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "sdk" ]]; then
        given_sdk=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "sdk2" ]]; then
        given_sdk2=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "qos" ]]; then
        given_msg_type=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "transport" ]]; then
        given_transport_type=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "arch" ]]; then
        arch=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "j" ]]; then
        J=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "tc" ]]; then
        given_tc=${arg_parts[i+1]}
        given_tc=${given_tc//+/ }
    elif [[ "${arg_parts[i]}" = "tag" ]]; then
        given_tag=${arg_parts[i+1]}
        given_tag=${given_tag//+/ }
    elif [[ "${arg_parts[i]}" = "start" ]]; then
        start=${arg_parts[i+1],,}
    elif [[ "${arg_parts[i]}" = "port" ]]; then
        REMOTE_PORT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "iotivity_root" ]]; then
        IOTIVITY_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "iotivity_commit_id" ]]; then
        IOTIVITY_COMMIT_ID=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "test_root" ]]; then
        TEST_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "test_manager_root" ]]; then
        TEST_MANAGER_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "peer_ip" ]]; then
        PEER_IP=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "peer_bit" ]]; then
        PEER_BIT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "peer_iotivity_root" ]]; then
        PEER_IOTIVITY_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "peer_test_root" ]]; then
        PEER_TEST_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "release" ]]; then
        RELEASE=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "coverage" ]]; then
        COVERAGE=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "android_home" ]]; then
        ANDROID_HOME=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "android_ndk" ]]; then
        ANDROID_NDK=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "android_gradle" ]]; then
        ANDROID_GRADLE=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "tizen_home" ]]; then
        TIZEN_HOME=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "gbs_root_parent" ]]; then
        GBS_ROOT_PARENT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "http_proxy_address" ]]; then
        HTTP_PROXY_ADDRESS=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "https_proxy_address" ]]; then
        HTTPS_PROXY_ADDRESS=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "kafka_root" ]]; then
        KAFKA_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "mongodb_root" ]]; then
        MONGODB_ROOT=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "auth_provider" ]]; then
        AUTH_PROVIDER=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "auth_username" ]]; then
        AUTH_USERNAME=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "auth_password" ]]; then
        AUTH_PASSWORD=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "android_device_ids" ]]; then
        given_android_device_ids=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "tizen_device_ids" ]]; then
        given_tizen_device_ids=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "jython_path" ]]; then
        JYTHON_PATH=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "clean_build" ]]; then
        clean_build=${arg_parts[i+1]}
    elif [[ "${arg_parts[i]}" = "complete_run" ]]; then
        complete_run=${arg_parts[i+1]}
    fi
    let i=i+2
done

if [[ "${RELEASE}" == "0" ]]; then
    RELEASE_DIR='debug'
else
    RELEASE_DIR='release'
fi

echo 'Release: '${RELEASE}
echo 'Release_dir: '${RELEASE_DIR}
echo 'Robot Root: '${ROBOT_ROOT}

JYTHON_BIN_PATH=${JYTHON_PATH}'/bin'
if [[ "${JYTHON_PATH}" == "" ]]; then
    echo 'jython_path not given, looking into system path'
elif [[ -d "${JYTHON_BIN_PATH}" ]]; then
    if [[ "${PATH}" != *"${JYTHON_BIN_PATH}"* ]]; then
        export PATH=${PATH}:${JYTHON_BIN_PATH}
    fi
    echo 'jython_path: '${JYTHON_PATH}
else
    echo 'invalid jython_path: '${JYTHON_PATH}
fi

if [[ -d "${ANDROID_HOME}/platform-tools" ]]; then
    if [[ "${PATH}" != *"${ANDROID_HOME}/platform-tools"* ]]; then
        export PATH=${PATH}:${ANDROID_HOME}/platform-tools
    fi
fi

if [[ -d "${TIZEN_HOME}/tools" ]]; then
    if [[ "${PATH}" != *"${TIZEN_HOME}/tools"* ]]; then
        export PATH=${PATH}:${TIZEN_HOME}/tools
    fi
fi

required_program_list=('gcc' 'g++' 'scons' 'java' 'jybot' 'git' 'python' 'python3' 'jython' 'gradle' 'unzip')

if ! [[ -d "${IOTIVITY_ROOT}" ]]; then
    echo 'Iotivity_ROOT path does not exist'
    exit 127
fi

cwd=`pwd`
cd ${IOTIVITY_ROOT}
IOTIVITY_ROOT=`pwd`
cd ${cwd}

if [[ "${start}" == *"client"* ]]; then
    if [[ "${given_tc_type}" == *"bat"* || "${given_module_type}" == *"cs"* || "${given_module_type}" == *"ic"* ]]; then
        if ! [[ -d "${KAFKA_ROOT}" ]]; then
            echo 'KAFKA_ROOT path does not exist'
            exit 127
        fi
        required_program_list+=('mvn')
    fi
fi

if [[ "${start}" == *"client"* ]]; then
    if [[ "${given_os_type}" == *"android"* || ("${given_tc_type}" == *"interop_app"* && "${given_os2_type}" == *"android"*) ]]; then
        if [[ "${ANDROID_HOME}" != "" ]]; then
            if ! [[ -d "${ANDROID_HOME}" ]]; then
                echo 'Invalid ANDROID_HOME path'
                exit 127
            fi
        else
            echo 'ANDROID_HOME Empty'
        fi

        if [[ "${ANDROID_NDK}" != "" ]]; then
            if ! [[ -d "${ANDROID_NDK}" ]]; then
                echo 'Invalid ANDROID_NDK path'
                exit 127
            fi
        fi
        if [[ "${ANDROID_GRADLE}" != "" ]]; then
            if ! [[ -f "${ANDROID_GRADLE}" ]]; then
                echo 'Invalid ANDROID_GRADLE binary'
                exit 127
            fi
        fi
        required_program_list+=('adb')
    fi

    if [[ "${given_os_type}" == *"tizen"* || ("${given_tc_type}" == *"interop_app"* && "${given_os2_type}" == *"tizen"*) ]]; then
        if [[ "${TIZEN_HOME}" != "" ]]; then
            if ! [[ -d "${TIZEN_HOME}" ]]; then
                echo 'Invalid TIZEN_HOME path'
                exit 127
            fi
        else
            echo 'TIZEN_HOME Empty'
        fi
        required_program_list+=('sdb')
        required_program_list+=('gbs')
    fi
fi

for required_program in ${required_program_list[@]};
do
    if ! type "${required_program}" > /dev/null; then
        echo "${required_program} not exist"
        exit 127
    fi
done

cwd=`pwd`
cd ${TEST_ROOT}
TEST_ROOT=`pwd`
cd ${cwd}

export CLASSPATH=$CLASSPATH:${TEST_ROOT}/src/automation/robot/device_lib/build/lib/*:${TEST_ROOT}/src/automation/robot/device_lib/build/libs/*
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${IOTIVITY_ROOT}/out/linux/${x86}/${RELEASE_DIR}:/usr/local/lib

ADB=${ANDROID_HOME}/platform-tools/adb
SDB=${TIZEN_HOME}/tools/sdb

if [[ "${start}" != *"server"* ]]; then
    export CLASSPATH=$CLASSPATH:${TEST_ROOT}/src/automation/robot/helper/*
fi

if [[ "${start}" == *"server"* || "${start}" == "local" ]]; then
    echo ClassPath = $CLASSPATH
    start_server
fi

if [[ "${start}" == *"client"* || "${start}" == "local" ]]; then
    start_client
fi

if [[ "${start}" == *"kill"* ]]; then
    fuser -k -n tcp ${REMOTE_PORT}
    kill_app iotivity_*
    if [[ -f "${TEST_ROOT}/bin/linux/SerialKiller" ]]; then
        cwd=`pwd`
        cd ${TEST_ROOT}/bin/linux/
        ./SerialKiller
        cd ${cwd}
    fi
fi

echo "All Process is completed"
