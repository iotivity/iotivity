#!/bin/bash

#setup
kafka_root=${1}
iotivity_root=${2}

#Build Cloud Server
function build_cloud_server {
    current_dir=`pwd`
    cp -rf $iotivity_root/cloud/certificate/ $iotivity_root/cloud/${1}/
    cd $iotivity_root/cloud/${1}
    mvn install -Dmaven.test.skip=true
    cp $iotivity_root/cloud/certificate/* target/
    cp -rf $iotivity_root/cloud/certificate/ target/
    cd $current_dir
}

#Build All Cloud Servers Source
function build_all_cloud_servers {
    current_dir=`pwd`
    cd $iotivity_root/cloud
    cp messagequeue/server.properties $kafka_root/config/
    build_cloud_server stack
    build_cloud_server resourcedirectory
    build_cloud_server messagequeue
    build_cloud_server account
    build_cloud_server interface
    rm -rf ${iotivity_root}/cloud/account/target/properties
    cp -rf ${iotivity_root}/cloud/account/properties ${iotivity_root}/cloud/account/target/
    cd $current_dir
}

init_current_dir=`pwd`
build_all_cloud_servers
cd $init_current_dir
