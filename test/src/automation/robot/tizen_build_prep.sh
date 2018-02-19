#!/bin/bash

iotivity_root=${1}
mode=${2}
http_proxy_address=${3}
https_proxy_address=${4}

iotivity_temp='../iotivity_temp'
iotivity_tizen='../iotivity_tizen'

if [[ ${iotivity_root} == "" ]]; then
    echo 'Empty iotivity root'
    exit 127
fi

if ! [[ -d "${iotivity_root}/.git" ]]; then
    echo 'invalid iotivity root'
    exit 127
fi

cwd=`pwd`
echo `pwd`

cd ${iotivity_root}
echo `pwd`

if [[ ${mode} == "move" ]]; then
    #echo 'moving iotivity to temp directory'
    #mkdir ${iotivity_temp}
    #mv ./* ${iotivity_temp}

    #if ! [[ -d "${iotivity_tizen}" ]]; then
    #    echo 'no iotivity_tizen directory found'
        #git checkout .

        #temporary added
        ##git clean -fx -d ""
        ##rm -rf *
        ##git checkout .
        rm -rf tmp
        git checkout service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample/build/tizen/SConscript
        #EXEC_MODE=true ./prep.sh
        sed -i "s/sh service/export http_proxy=http:\/\/${http_proxy_address}; export https_proxy=https:\/\/${https_proxy_address}; sh service/g" service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample/build/tizen/SConscript
    #else
    #    echo 'iotivity_tizen directory exist'
        #mv ${iotivity_tizen}/* .
    #fi
fi

if [[ ${mode} == "restore" ]]; then
    echo 'restoring iotivity from temp directory'
    #if ! [[ -d "${iotivity_tizen}" ]]; then
    #    mkdir ${iotivity_tizen}
    #fi
    #mv ./* ${iotivity_tizen}
    #mv ${iotivity_temp}/* .
    #rm -rf ${iotivity_temp}
fi

cd ${cwd}
echo `pwd`
