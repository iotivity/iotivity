#!/bin/bash

iotivity_base_dir=${1}
iotivity_download_uri=${2}

if [ "$#" -ne 2 ]; then
    echo 'Two arguments needed. First one is needed for where to download iotivity and second represents iotivity git download uri'
    exit -1
fi

cwd=`pwd`

cd ${iotivity_base_dir}

iotivity_dir_name='iotivity'

rm -rf ${iotivity_dir_name}

git clone ${iotivity_download_uri}
if [ $? -ne 0 ]; then
    echo 'Unable to download iotivity'
    exit -1
fi

cd ${iotivity_dir_name}

cbor_version='v0.4.1'  #setting static verion value here. Need to parse dyanmically from sconscript in future
git clone https://github.com/01org/tinycbor.git extlibs/tinycbor/tinycbor -b ${cbor_version}
if [ $? -ne 0 ]; then
    echo 'Unable to download tinycbor'
    exit -1
fi

mbedtls_version='mbedtls-2.4.0'  #setting static verion value here. Need to parse dyanmically from sconscript in future
git clone https://github.com/ARMmbed/mbedtls.git extlibs/mbedtls/mbedtls -b ${mbedtls_version}
if [ $? -ne 0 ]; then
    echo 'Unable to download mbedtls'
    exit -1
fi

cd ${cwd}

