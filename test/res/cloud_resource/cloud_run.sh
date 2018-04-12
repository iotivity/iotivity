#!/bin/bash

#setup
command=${1}
kafka_root=${2}
tls=${3}
iotivity_root=${4}

#example
#command=start
#tls=1
#kafka_root=/home/srbd/Downloads/kafka_2.10-0.9.0.0
#iotivity_root=/home/srbd/git/oictest_repo/IotivityOrgSource/rc3/iotivity

#Steps:
#1. Free All Required Port
#2. Run Kafka and Zoo Keeper
#3. Run RD, MQ, Acnt and CI server respectively
#4. Free All Required Port

function clean_up {
    ps -ef | grep "java -jar CloudInterface-0.0.1-SNAPSHOT.jar 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686" | grep -v grep | awk '{ print $2 }'  | xargs kill -9
    ps -ef | grep "java -jar CloudAccount-0.0.1-SNAPSHOT.jar 5685" | grep -v grep | awk '{ print $2 }'  | xargs kill -9
    ps -ef | grep "java -jar CloudMessageQueue-0.0.1-SNAPSHOT.jar 5686 127.0.0.1 2181 127.0.0.1 9092" | grep -v grep | awk '{ print $2 }'  | xargs kill -9
    ps -ef | grep "java -jar CloudResourceDirectory-0.0.1-SNAPSHOT.jar 5684 127.0.0.1 27017" | grep -v grep | awk '{ print $2 }'  | xargs kill -9

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
    gnome-terminal --title=${1} -x bash -c "java -jar $2"
    sleep 10s
    cd ${current_dir}
}

function run_all_cloud_servers {
    current_dir=`pwd`
    run_cloud_server resourcedirectory "CloudResourceDirectory-0.0.1-SNAPSHOT.jar 5684 127.0.0.1 27017 ${tls}"
    run_cloud_server messagequeue "CloudMessageQueue-0.0.1-SNAPSHOT.jar 5686 127.0.0.1 2181 127.0.0.1 9092 ${tls}"
    run_cloud_server account "CloudAccount-0.0.1-SNAPSHOT.jar 5685 127.0.0.1 27017 ${tls}"
    run_cloud_server interface "CloudInterface-0.0.1-SNAPSHOT.jar 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686 0 0 ${tls}"

    cd ${current_dir}
}

function clean_tc_generated_junk {
    current_dir=`pwd`
    if [[ "${kafka_root}" == "" ]]; then
        return 127
    fi
    cd ${kafka_root}
    zookeeper_port=2181
    output="$(lsof -i :${zookeeper_port})"
    if [[ "${output}" != *"${zookeeper_port} (LISTEN)"* ]]; then
        bin/zookeeper-server-start.sh config/zookeeper.properties &
        sleep 10s
    fi

    kafka_port=9092
    output="$(lsof -i :${kafka_port})"
    if [[ "${output}" != *"${kafka_port} (LISTEN)"* ]]; then
        bin/kafka-server-start.sh config/server.properties &
        sleep 3s
    fi
    bin/kafka-topics.sh --zookeeper localhost:2181 --delete --topic .oic.ps.light313 #this action removes MQ topic, created by IC MQ create TC 
    bin/kafka-topics.sh --zookeeper localhost:2181 --delete --topic .oic.ps.light55  #update require if MQ uri changed in TC
    bin/kafka-topics.sh --zookeeper localhost:2181 --delete --topic .oic.ps.light7
    bin/kafka-topics.sh --zookeeper localhost:2181 --delete --topic .oic.ps.light13
    cd ${current_dir}
}

init_current_dir=`pwd`

if [[ "${command}" == "start" ]]; then
    clean_up
    run_external_server
    run_all_cloud_servers
elif [[ "${command}" == "kill" ]]; then
    clean_tc_generated_junk
    clean_up
fi

cd ${init_current_dir}
