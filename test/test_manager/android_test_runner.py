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
import os
import re
import sys
import time
import fnmatch
import optparse
import subprocess
import traceback
from time import sleep
from os import listdir
from time import strftime
from datetime import datetime
from os.path import isfile, join
import configuration
from configuration import *
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter
from ite.webreporter.tc_list_reporter import TCListReporter
from subprocess import Popen, PIPE
from xml.etree.ElementTree import ElementTree, ParseError

from ite.tc.analyzer import TestSpec
from ite.constants import *
from ite.config import *

TEST_ROOT = '..'
timeout_seconds = 300

oparser = optparse.OptionParser()
oparser.add_option("-p", action="store", dest="platform")
oparser.add_option("--platform", action="store", dest="platform")
oparser.add_option("-t", action="store", dest="target", default="5.2")
oparser.add_option("--target", action="store", dest="target", default="5.2")
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
oparser.add_option("-r", action="store", dest="testresult", default="")
oparser.add_option("--result", action="store", dest="testresult", default="")
oparser.add_option("-f", action="store", dest="file_filter", default="")
oparser.add_option("--filter", action="store", dest="file_filter", default="")
oparser.add_option("-m", action="store", dest="module", default="")
oparser.add_option("--module", action="store", dest="module", default="")
oparser.add_option("-w", action="store", dest="tc_framework", default=TESTFW_TYPES.JUNIT)
oparser.add_option("--framework", action="store", dest="tc_framework", default=TESTFW_TYPES.JUNIT)
oparser.add_option("-i", action="store", dest="tc_scenario", default="")
oparser.add_option("--scenario", action="store", dest="tc_scenario", default="")
oparser.add_option("-g", action="store", dest="base_package", default="")
oparser.add_option("--package", action="store", dest="base_package", default="")
oparser.add_option("--path", action="store", dest="app_path", default="/data/local/tmp")
opts, args = oparser.parse_args()

platform = 'android'
target = opts.target
transport = opts.transport
network = opts.network
testresult = opts.testresult
device_name = opts.device_name
file_filter = opts.file_filter    #applicable only for android native application, no use in android junit testcase
given_module = opts.module
app_path = opts.app_path
tc_framework = opts.tc_framework
base_package = opts.base_package

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

if opts.tc_scenario:
    tc_scenario = opts.tc_scenario.split(',')
else:
    tc_scenario = []

if opts.testcase:
    given_testcases = opts.testcase.split(',')
else:
    given_testcases = []

if not os.path.exists(testresult):
    os.makedirs(testresult)

if not app_path:
    print (bcolors.FAIL + 'binary file\'s folder path is not given. Use --path opiotn' + bcolors.ENDC)
    exit(0)

if not app_path.endswith(os.sep):
    app_path = app_path + os.sep

if app_path.endswith(os.sep):
    app_path = app_path[:-1]

if device_name:
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


def run_standalone_tc(suite_name, tc_name, command, pass_msg, platform, target, transport, network, file_suffix):

    result = False

    try:
        xml_output = '<testsuites>\n'    
        xml_output = xml_output + '\t<testsuite name="' + suite_name + '">\n'
        xml_output = xml_output + '\t\t<testcase name="' + tc_name + '" status="run" time="0"'
        
        status = 'failed'

        os.system('adb ' + device_name + ' logcat -c')

        log = ''

        try:
            print (command)
            configuration.EXECUTION_LOG_FP.write (str(command) + '\n')
            configuration.EXECUTION_LOG_FP.flush()
            rc = subprocess.check_output(('timeout {} {}').format(timeout_seconds, command), shell=True)
            #rc = subprocess.check_output(command, shell=True, timeout=timeout_seconds)
            log = re.sub(r'(b\'|\')', '', str(rc))  
            log = log.strip()
            log = log.replace('\\x', '')
            log = log.replace('\\r\\n', '\n')
            log = log.replace('\'', '')
            log = log.replace('\"', '')
            log = log.replace('\\t', '    ')
            log = log.replace('1b[0m', '')
            print ('\n' + log)

            if log.find(pass_msg) == -1:
                xml_output = xml_output + '>\n\t\t\t<failure message="'+ 'failed' +'"> </failure>\n</testcase>\n'
                print (bcolors.FAIL + suite_name + '.' + tc_name + ': failed' + bcolors.ENDC)
            else:
                xml_output = xml_output + '/>\n'
                status = 'passed'
                print (bcolors.OKGREEN + suite_name + '.' + tc_name + ': ' + 'passed' + bcolors.ENDC)
                result = True

        except subprocess.TimeoutExpired:
                xml_output = xml_output + '>\n\t\t\t<failure message="'+ ' Timeout occur' +'"> </failure>\n</testcase>\n'
                print (bcolors.FAIL + suite_name + '.' + tc_name + ': timeout' + bcolors.ENDC)

        except subprocess.CalledProcessError:
                xml_output = xml_output + '>\n\t\t\t<failure message="'+ ' Crash occur' +'"> </failure>\n</testcase>\n'
                print (bcolors.FAIL + suite_name + '.' + tc_name + ': timeout' + bcolors.ENDC)


        xml_output = xml_output + '</testsuite>\n'
        xml_output = xml_output + '</testsuites>\n'
        
        timestring = datetime.now().strftime("%Y%m%d_%H%M%S.%f")
            
        file_name = "%s_%s_%s_%s_%s_%s" %(platform, target, transport, network, timestring, file_suffix)

        if os.path.isfile(testresult + os.sep + file_name + '.log') or os.path.isfile(testresult + os.sep + file_name + '.xml'):
                print (bcolors.FAIL + 'File name exist !!!\nRunner Internal Error' + bcolors.ENDC)
                exit(0)

        os.system('adb ' + device_name + ' logcat -d > ' + testresult + os.sep + file_name + '.log')

        log_file = open(testresult + os.sep + file_name + '.log', 'a')
        log_file.write('\n' + log)
                
        xml_file = open(testresult + os.sep + file_name + '.xml', 'w')
        xml_file.write(xml_output)

    except:
        print("Unexpected error:", sys.exc_info()[0])
        traceback.print_exc(file=sys.stdout)

    return result

def run_gtest_testcase(testresult, file_filter, given_testsuites, given_testcases, platform, target, transport, network):

    command = 'adb ' + device_name + ' shell ls ' + app_path + '/' + TC_BIN_PREFIX + '*' + TC_BIN_SUFFIX
    print (command)

    process= subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    log, err = process.communicate(timeout = 30)

    binary_list = str(log).replace('b\'', '').replace('\'', '').replace(app_path + '/', '').replace('\\r\\n', ' ').split()

    sz = 0

    for binary_name in binary_list:

        print (binary_name)

        if file_filter:
            if file_filter not in binary_name:
                continue

        if given_module:
            if len(binary_name.split('_')) < 3:
                continue

            if binary_name.split('_')[1] not in given_module:
                continue

        list_of_testcase = []
        testsuite = ''

        command = 'adb ' + device_name + ' shell LD_LIBRARY_PATH=' + app_path + ' ' + app_path + '/' + binary_name + ' --gtest_list_tests'

        rc = subprocess.check_output(command, shell=True)
        log = re.sub(r'(b\'|\')', '', str(rc))

        for line in log.split('\\r\\n'):
            line = re.sub(r'(b\'|\')', '', str(line.strip()))

            if not line:
                continue

            print (line)

            if line.endswith('.') == True :
                testsuite = line
                continue

            if given_testsuites and testsuite[:-1] not in given_testsuites:
                continue

            if given_testcases and line not in given_testcases:
                continue

            if testsuite != '' and line != '':
                list_of_testcase.append(Testcase(testsuite + line))

        sz = sz + len(list_of_testcase)

        while list_of_testcase:

            print(len(list_of_testcase), ' testcase(s) needed to be run')

            for testcase in list_of_testcase:
                tc_name = testcase.name
                tc_info = tc_name.split('.')
                command = 'adb ' + device_name + ' shell ' + app_path + '/runner.sh ' + binary_name + ' ' + tc_name
                result = run_standalone_tc (tc_info[0], tc_info[1], command, '[  PASSED  ] 1 test.', platform, target, transport, network, binary_name)
                if result:
                    testcase.increase_pass_count()
                else:
                    testcase.increase_fail_count()

                testcase.increase_total_count()

            list_of_testcase[:] = [x for x in list_of_testcase if not x.is_execution_complete()]

    if sz == 0:
        print (bcolors.FAIL + 'No testcase Found !!!\nPlease, Check command parameter(s)' + bcolors.ENDC)
    else:
        print("### Test Is Done!!")



def run_junit_testcase(testresult, base_package, given_testsuites, given_testcases, platform, target, transport, network, tc_scenario):

    list_of_testcase = []

    for testcase_type in TESTCASE_TYPES:

        if tc_scenario:
            found = False
            for temp in tc_scenario:
                if temp.lower() == testcase_type.lower():
                    found = True
                    break
            if not found:
                continue

        for module in TESTSUITE_MODULES:

            if given_testsuites:
                found = False
                for given_suite in given_testsuites:
                    if module in given_suite:
                        found = True
                        break
                if not found:
                    continue

            if given_module:
                if module.lower() not in given_module.lower():
                    continue

            if not base_package:
                base_package = 'org.iotivity.test.' + module.lower() + '.tc'

            cwd = os.getcwd()
            print (cwd)

            build_dir = os.path.join(cwd, TEST_ROOT, 'bin', 'android', module.lower(), 'intermediates', 'classes', 'debug', base_package.replace('.', os.sep), testcase_type.lower())
            print (build_dir)

            os.chdir(build_dir)
            print (os.getcwd())

            file_list = [f for f in listdir(build_dir) if isfile(join(build_dir, f))]

            for suite in file_list :
                if "$" not in suite and suite.endswith('Test.class'):
                    suite_name = suite.split('.',1)[0]
                    
                    if given_testsuites and suite_name not in given_testsuites:
                        continue

                    class_command = "find -name '%s.class' | xargs javap -p" % suite_name
                    rc = subprocess.check_output(class_command, shell=True)
                    log = re.sub(r'(b\'|\')', '', str(rc))

                    for line in log.split('\\n'):
                        line = re.sub(r'(b\'|\')', '', str(line.strip()))

                        if 'public void test' in line:
                            begin_index = line.find('test')
                            end_index = line.find('(')
                            tc_name = line[begin_index:end_index]

                            if given_testcases and tc_name not in given_testcases:
                                continue

                            list_of_testcase.append(Testcase(testcase_type.lower() + '.' + suite_name + '.' + tc_name))

            os.chdir(cwd)

    if not list_of_testcase:
        print (bcolors.FAIL + 'No testcase Found !!!\nPlease, Check command parameter(s)' + bcolors.ENDC)
        return

    while list_of_testcase:

        print(len(list_of_testcase), ' testcase(s) needed to be run')

        for testcase in list_of_testcase:
            tc_info = testcase.name.split('.')

            command = 'adb ' + device_name + ' shell am instrument -w  -e class '
            command = command + base_package + '.' + tc_info[0] + '.' + tc_info[1] + '#' + tc_info[2] + ' ' + base_package
            command = command + '/com.zutubi.android.junitreport.JUnitReportTestRunner'

            result = run_standalone_tc (tc_info[1], tc_info[2], command, 'OK (1 test)', platform, target, transport, network, tc_info[1].lower() + '_' + tc_info[0])

            if result:
                testcase.increase_pass_count()
            else:
                testcase.increase_fail_count()

            testcase.increase_total_count()

        list_of_testcase[:] = [x for x in list_of_testcase if not x.is_execution_complete()]


    print("### Test Is Done!!")

if TESTFW_TYPES.GTEST in tc_framework:
    run_gtest_testcase(testresult, file_filter, given_testsuites, given_testcases, platform, target, transport, network)

if TESTFW_TYPES.JUNIT in tc_framework:
    run_junit_testcase(testresult, base_package, given_testsuites, given_testcases, platform, target, transport, network, tc_scenario)

# ./android_test_runner.py -s CATest_btc -f ca_test
