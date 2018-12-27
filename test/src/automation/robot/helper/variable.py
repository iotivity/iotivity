'''
/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
'''

import os

ROOT_DIR = os.getcwd()
HOMEPATH = '../../..'
ResultFile = '../../result.txt'
IOTIVITY_DIR = HOMEPATH+'/IotivityOrgSource/target/iotivity'
TC_BUILD_DIR = HOMEPATH+'/IotivityOrgSource/target/iotivity/test'
IOTIVITY_BUILD_COMMAND_LINUX_RC ='scons TARGET_OS=linux'

ORGSOURCEPATH = HOMEPATH + '/IotivityOrgSource'

TARGET = 'target'
x86 = 'x86'

ANDROID_FILTER_LOG_SSM = 'SSMSampleApp'
ANDROID_FILTER_LOG_PPM = 'PPMSampleApp'
ANDROID_FILTER_LOG_NM_HOSTING = 'NMResourceHosting'
ANDROID_FILTER_LOG_NM_CONSUMER = 'NMConsumer'
ANDROID_FILTER_LOG_NM_PROVIDER = 'NMProvider'
ANDROID_FILTER_LOG_TM_CONSERVER = 'CON-SERVER'
ANDROID_FILTER_LOG_TM_SAMPLE = 'TMSample'
ANDROID_FILTER_LOG_CA = 'Sample_Service'

TIZEN_FILTER_LOG_SSM = 'ssmtesterapp'
TIZEN_FILTER_LOG_PPM = 'ppmtesterapp'
TIZEN_FILTER_LOG_NM = 'nmsampleapp'
TIZEN_FILTER_LOG_TM_CONSERVER = 'conserverapp'
TIZEN_FILTER_LOG_TM_SAMPLE = 'tmsampleapp'

GITHUB_USER_KEY = 'github_userid'
GITHUB_PASSWORD_KEY = 'github_password'
CI_SERVER_WIFI_IP_KEY = 'ci_server_wifi_ip'
CI_SERVER_WIFI_IPv6_KEY = 'ci_server_wifi_ipv6'
CI_SERVER_ETHERNET_IP_KEY = 'ci_server_ethernet_ip'

DLM = ';'
INFINITE='infinite'
EQ='='
NE='!='
LT='<'
LE='<='
GT='>'
GE='>='

android='android'
linux='linux'
tizen='tizen'

API='api'
BAT='bat'
SAMPLE_APP='sample_app'
INTEROP_APP='interop_app'

BUILD_IOTIVITY='build_iotivity'
BUILD_TEST='build_test'
TEST_PRE_CONDITION='test_pre_condition'

BUILD_IOTIVITY_FAIL = 2
BUILD_TEST_FAIL = 4
TEST_PRE_CONDITION_PASS = 5
TEST_PRE_CONDITION_FAIL = 6
