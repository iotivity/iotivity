#!/bin/bash

#setup
command=${1}
tls=${2}
kafka_root=${3}
iotivity_root=${4}

#command=start
#tls=1
#kafka_root=/home/srbd/Downloads/kafka_2.10-0.9.0.0
#iotivity_root=/home/srbd/git/oictest_repo/IotivityOrgSource/last/iotivity

#Steps:
#1. Free All Required Port
#2. Run Kafka and Zoo Keeper
#3. Run RD, MQ, Acnt and CI server respectively
#4. Free All Required Port

function clean_up {
    ps -ef | grep "java -jar CloudInterface-0.0.1-SNAPSHOT.jar 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686 ${tls}" | grep -v grep | awk '{ print $2 }'  | xargs kill -9
    ps -ef | grep "java -jar CloudAccount-0.0.1-SNAPSHOT.jar 5685" | grep -v grep | awk '{ print $2 }'  | xargs kill -9
    ps -ef | grep "java -jar CloudMessageQueue-0.0.1-SNAPSHOT.jar 5686 127.0.0.1 2181 127.0.0.1 9092 ${tls}" | grep -v grep | awk '{ print $2 }'  | xargs kill -9
    ps -ef | grep "java -jar CloudResourceDirectory-0.0.1-SNAPSHOT.jar 5684 ${tls}" | grep -v grep | awk '{ print $2 }'  | xargs kill -9

    fuser -k -n tcp 9092 #kafa
    fuser -k -n tcp 2181 #zookeeper
    fuser -k -n tcp 5683
    fuser -k -n tcp 5684
    fuser -k -n tcp 5685
    fuser -k -n tcp 5686
    rm -rf /tmp/kafka-logs
}

function run_external_server {
    current_dir=`pwd`
    cd ${kafka_root}
    gnome-terminal -x bash -c "bin/zookeeper-server-start.sh config/zookeeper.properties"
    sleep 10s
    gnome-terminal -x bash -c "bin/kafka-server-start.sh config/server.properties"
    sleep 3s
    cd ${current_dir}
}

function run_cloud_server {
    #xdotool key ctrl+shift+t
    current_dir=`pwd`
    cd $iotivity_root/cloud/$1/target
    gnome-terminal -x bash -c "java -jar $2"
    sleep 5s
    cd ${current_dir}
}

function run_all_cloud_servers {
    current_dir=`pwd`
    run_cloud_server resourcedirectory "CloudResourceDirectory-0.0.1-SNAPSHOT.jar 5684 ${tls}"
    run_cloud_server messagequeue "CloudMessageQueue-0.0.1-SNAPSHOT.jar 5686 127.0.0.1 2181 127.0.0.1 9092 ${tls}"
    run_cloud_server account "CloudAccount-0.0.1-SNAPSHOT.jar 5685 ${tls}"
    run_cloud_server interface "CloudInterface-0.0.1-SNAPSHOT.jar 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686 ${tls}"

    cd ${current_dir}
}

init_current_dir=`pwd`

if [[ "${command}" == "start" ]]; then
    clean_up
    run_external_server
    run_all_cloud_servers
elif [[ "${command}" == "kill" ]]; then
    clean_up
fi

cd ${init_current_dir}
