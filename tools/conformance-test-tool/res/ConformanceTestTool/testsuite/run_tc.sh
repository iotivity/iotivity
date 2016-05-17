#!/bin/bash
#******************************************************************
#
# Copyright 2014 Samsung Electronics All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

# ./run_tc.sh testsuite/CT1_1_1_CoAP_Based_Endpoint_Resource_Discovery.txt  stack=IOTIVITY LD_LIBRARY_PATH=../../../../../out/linux/x86_64/release tc="1_Multicast_Discovery_Without_Query" capture=false
# stack, LD_LIBRARY_PATH, tc, capture are optional parameter
# possible stack value IOTIVITY, OIC, default stack value OIC. OIC means when jcoap library is used.

export CLI_ENABLED=true
echo "Executing Testcase: " $1

echo_time() {
    date +"_%Y-%m-%d_%H:%M:%S"
}

interim=$(echo "$1" | cut -d. -f1)
tc_name=$(echo "$interim" | rev | cut -d/ -f1 | rev)
folder_name=$tc_name$(echo_time)

for i in `seq 2 $#` 
do
	eval arg=\$$i
	echo $arg
	arg=${arg// /+}
    args+=$arg" "
done

arg_parts=(${args//=/ })

len=${#arg_parts[@]}

MACHINE_TYPE=`uname -m`

given_capture="true"
given_stack="OIC"
tc_filter=""
given_library_path='../../../../../out/linux/'$MACHINE_TYPE'/release'

i=0
while [ $i -lt $len ]; do
    if [[ "${arg_parts[i]}" = "stack" || "${arg_parts[i]}" == "Stack"  || "${arg_parts[i]}" == "STACK" ]]; then
        given_stack=${arg_parts[i+1]}
    fi
    if [[ "${arg_parts[i]}" = "tc" || "${arg_parts[i]}" == "TC" || "${arg_parts[i]}" == "Tc" ]]; then
        tc_filter="--test "${arg_parts[i+1]}
    fi
    if [[ "${arg_parts[i]}" = "LD_LIBRARY_PATH" || "${arg_parts[i]}" == "Ld_Library_Path" || "${arg_parts[i]}" == "ld_library_path" ]]; then
        given_library_path=${arg_parts[i+1]}
    fi    
    if [[ "${arg_parts[i]}" = "CAPTURE" || "${arg_parts[i]}" == "Capture" || "${arg_parts[i]}" == "capture" ]]; then
        given_capture="--test "${arg_parts[i+1]}
    fi    
    let i=i+2
done

export LD_LIBRARY_PATH=$given_library_path
export CTT_CLIENT_STACK=$given_stack
export CTT_PACKET_CAPTURE=$given_capture

#tc_filter="--test 1_Multicast_Discovery_Without_Query"
#tc_filter="--test 2_NON_RETRIEVE_Message_with_Observe_Indication_Observable_Resource"

CLASSPATH=$CLASSPATH:null:libs/*:plugins/oic.ctt.ui_1.0.0/libs/* JYTHONPATH=libs/:plugins/oic.ctt.ui_1.0.0/libs/:./:testsuite/ jybot -d "testreport/"$folder_name $tc_filter -b debugLog.log -C auto $1
