#!/usr/bin/python3.4
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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/
'''
import re
import time
import os
import sys
import fnmatch
import subprocess
import optparse
import traceback

from time import strftime
from time import sleep
from datetime import datetime
import configuration
from configuration import *
from ite.config import *
from ite.constants import *
from ite.tc.analyzer import TestSpec
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter
from ite.webreporter.tc_list_reporter import TCListReporter

from xml.etree.ElementTree import ElementTree, ParseError

timeout_seconds = 60
passStr='\033[0;32m[  PASSED  ] \033[0m1 test.'

oparser = optparse.OptionParser()
oparser.add_option("-p", action="store", dest="platform")
oparser.add_option("--platform", action="store", dest="platform")
oparser.add_option("-t", action="store", dest="target", default="2.4")
oparser.add_option("--target", action="store", dest="target", default="2.4")
oparser.add_option("-x", action="store", dest="transport", default="")
oparser.add_option("--transport", action="store", dest="transport", default="")
oparser.add_option("-n", action="store", dest="network", default="")
oparser.add_option("--network", action="store", dest="network", default="")
oparser.add_option("-u", action="store", dest="suite", default="")
oparser.add_option("--testsuite", action="store", dest="suite", default="")
oparser.add_option("-c", action="store", dest="testcase", default="")
oparser.add_option("--case", action="store", dest="testcase", default="")
oparser.add_option("-d", action="store", dest="device_name", default="")
oparser.add_option("--device", action="store", dest="device_name", default="")
oparser.add_option("-m", action="store", dest="module", default="")
oparser.add_option("--module", action="store", dest="module", default="")
oparser.add_option("-f", action="store", dest="file_filter", default="")
oparser.add_option("--filter", action="store", dest="file_filter", default="")
oparser.add_option("-r", action="store", dest="testresult", default="")
oparser.add_option("--result", action="store", dest="testresult", default="")
oparser.add_option("--path", action="store", dest="app_path", default="/opt/usr/media/bin")
opts, args = oparser.parse_args()

platform = 'tizen'
target = opts.target
transport = opts.transport
network = opts.network
testresult = opts.testresult
device_name = opts.device_name
file_filter = opts.file_filter
app_path = opts.app_path
module = opts.module

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class Testcase:

    max_total_count = 3
    min_pass_count = 1
    

    def __init__(self, name):
        self.name = name
        self.total_count = 0
        self.pass_count = 0
        self.fail_count = 0

    def increase_total_count(self):
        self.total_count += 1

    def increase_pass_count(self):
        self.pass_count += 1

    def increase_fail_count(self):
        self.fail_count += 1

    def is_execution_complete(self):

        if self.pass_count > 0 or self.total_count > self.max_total_count:
            return True

        return False


if not testresult:
    testresult = TEST_RESULT_RUN_DIR

if testresult.endswith(os.sep):
    testresult = testresult[:-1]

if opts.suite:
    given_testsuites = opts.suite.split(',')
else:
    given_testsuites = []

if opts.testcase:
    given_testcases = opts.testcase.split(',')
else:
    given_testcases = []

if not os.path.exists(testresult):
    os.makedirs(testresult)

if not app_path:
    print (bcolors.FAIL + 'binary file\'s folder path is not given. Use --path opiotn' + bcolors.ENDC)
    exit(0)

if app_path.endswith(os.sep):
    app_path = app_path[:-1]

if not device_name:
    device_name = ''
else:
    device_name = '-s ' + device_name

if transport.upper() == TEST_TRANSPORT.IP or transport.upper() == TEST_TRANSPORT.TCP:
    network = TEST_NETWORK.WIFI.lower()

testspec_path = os.path.join(testresult, TEST_SPEC_XML_FOR_RESULT)
if not os.path.exists(testspec_path) and os.path.exists(API_TC_SRC_DIR):
    container = TestSpecContainer()
    container.extract_api_testspec(API_TC_SRC_DIR, '')
    
    reporter = TestSpecReporter()
    reporter.generate_testspec_report(container.data)
    reporter.report('XML', testspec_path)

configuration.EXECUTION_LOG_FP  = open(testresult + os.sep + EXECUTION_LOG_FILE, "w")

def remove_invalid_character_from_log(log) :
    if "\"" in log :
        log = log.replace('\"','')
    if "\'" in log :
       log = log.replace('\'','')
    if "<" in log :
       log = log.replace('<','')
    if ">" in log :
       log = log.replace('>','')
    return log

def execute_testcase(testcase):

    binary_name = testcase.name.split('.')[0]
    suite_name = testcase.name.split('.')[1]
    tc_name = testcase.name.split('.')[2]
    full_tc_name = suite_name + '.' + tc_name

    logs = ''
    outXML = ''
    outXML += '<testsuites>\n'
    outXML += '<testsuite name="' + suite_name + '">\n'
    outXML = outXML + '<testcase name="' + tc_name + '" status="run" time="0"'
    result = False

    os.system('sdb ' + device_name + ' dlog -c')
    
    try:
        #command = 'sdb %s shell %s/%s --gtest_filter=%s'%(device_name, app_path, binary_name, full_tc_name)
        command = 'sdb %s shell %s/tizen_runner.sh %s %s --gtest_filter=%s.%s'%(device_name, app_path, app_path, binary_name, suite_name, tc_name)
        print("Start Execution TC: " , command+'\n')
        #rc = subprocess.check_output(command, shell=True)
        rc = subprocess.check_output(('timeout {} {}').format(timeout_seconds, command), shell=True)
        #rc= subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        #log, err = proc.communicate(timeout = timeout_seconds) 
        log = re.sub(r'(b\'|\')', '', str(rc))
        #log = log.strip()
        #log = str(log)
        log = remove_invalid_character_from_log(log)
        log = log.replace('\\x', '')
        log = log.replace('1b[0;33m', '\033[0;33m')
        log = log.replace('1b[0;32m', '\033[0;32m')
        log = log.replace('1b[0;31m', '\033[0;31m')
        log = log.replace('1b[0;30m', '\033[0;30m')
        log = log.replace('1b[m', '\033[0m')

        log_part = log.split('\\r\\n')

        for log in log_part:
            print (log)
            logs += log + '\n'

        if logs.find(passStr) != -1:
            outXML = outXML + "/>\n"
            print (bcolors.OKGREEN + full_tc_name + ': ' + 'passed' + bcolors.ENDC)
            result = True
        else:
            outXML = outXML + '>\n<failure message="'+ 'fail' +'"> </failure>\n</testcase>\n'
            print (bcolors.FAIL + full_tc_name + ': failed' + bcolors.ENDC)

    except subprocess.TimeoutExpired:
        print ('Timeout exception')
        outXML = outXML + '>\n<failure message="'+ ' Timeout occur' +'"> </failure>\n</testcase>\n'
        print (bcolors.FAIL + full_tc_name + ': timeout' + bcolors.ENDC)

    except subprocess.CalledProcessError:
        outXML = outXML + '>\n\t\t\t<failure message="'+ ' Crash occur' +'"> </failure>\n</testcase>\n'
        print (bcolors.FAIL + suite_name + '.' + tc_name + ': timeout' + bcolors.ENDC)

    print('Finised Execution TC :' + full_tc_name + '\n')

    outXML = outXML + "</testsuite>\n"
    outXML = outXML + '</testsuites>\n'

    timestring = datetime.now().strftime("%Y%m%d_%H%M%S.%f")

    file_name = platform + '_' + target + '_' + transport + '_' + network + '_' + timestring + '_' + binary_name

    if os.path.isfile(testresult + os.sep + file_name + '.log') or os.path.isfile(testresult + os.sep + file_name + '.xml'):
            print (bcolors.FAIL + 'File name exist !!!\nRunner Internal Error' + bcolors.ENDC)
            exit(0)

    os.system('sdb ' + device_name + ' dlog -d > ' + testresult + os.sep + file_name + '.log')

    logFile = open(testresult + os.sep + file_name + '.log', 'a')
    logFile.write(logs)

    xmlFile = open(testresult + os.sep + file_name + '.xml', 'w')
    xmlFile.write(outXML)

    return result


command = 'sdb %s root on'%(device_name)
process= subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
log, err = process.communicate(timeout = 30)

command = 'sdb %s shell ls %s/iotivity_*_test'%(device_name, app_path)
process= subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
log, err = process.communicate(timeout = 30)

binary_list = str(log).replace('b\'', '').replace('\'', '').replace(app_path + '/', '').replace('\\r\\n', ' ').split()

list_of_testcase = []

for binary_name in binary_list:
    print (binary_name)

    if file_filter and file_filter not in binary_name:
        continue
    
    if not binary_name.startswith(TC_BIN_PREFIX):
        continue

    if not binary_name.endswith(TC_BIN_SUFFIX):
        continue

    if module and module != binary_name.split('_')[1]:
        continue

    command = 'sdb %s shell %s/%s --gtest_list_tests'%(device_name, app_path, binary_name)

    print ('command: ' + command)
    configuration.EXECUTION_LOG_FP.write (str(command) + '\n')
    configuration.EXECUTION_LOG_FP.flush()

    process = subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    testsuite = ''

    for line in process.stdout.readlines():
        line = re.sub(r'(b\'|\')', '', str(line.strip()))

        if not line:
            continue

        if line.endswith('.') == True :
            testsuite = line
            continue

        if given_testsuites and testsuite[:-1] not in given_testsuites:
            continue

        if given_testcases and line not in given_testcases:
            continue

        if testsuite != '' and line != '':
            testcase = Testcase(binary_name + '.' + testsuite + line)
            list_of_testcase.append(testcase)

if not list_of_testcase:
    print (bcolors.FAIL + 'no testcase is found with given parameter' + bcolors.ENDC)
    exit(0)

while list_of_testcase:

    print(len(list_of_testcase))

    for testcase in list_of_testcase:

        result = execute_testcase(testcase)

        print (testcase.name, testcase.total_count, testcase.pass_count, testcase.fail_count, result)

        if result:
            testcase.increase_pass_count()
        else:
            testcase.increase_fail_count()

        testcase.increase_total_count()

    list_of_testcase[:] = [x for x in list_of_testcase if not x.is_execution_complete()]

print("### Test Is Done!!")

#./tizen_test_runner -f ca_test -u CATest_btc
