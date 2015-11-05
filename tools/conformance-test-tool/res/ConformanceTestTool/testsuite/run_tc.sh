#!/bin/sh
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

export CLI_ENABLED=true
echo "Executing Testcase: " $1

echo_time() {
    date +"%T*"
}

ts=$(echo_time)

CLASSPATH=$CLASSPATH:null:libs/*:plugins/oic.ctt.ui_1.0.0/libs/* JYTHONPATH=libs/:plugins/oic.ctt.ui_1.0.0/libs/:./:testsuite/ jybot -d "testreport/"$ts -b debugLog.log -C auto $1 
