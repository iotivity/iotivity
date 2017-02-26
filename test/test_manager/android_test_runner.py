#!/usr/bin/python3
'''
/***************************************************************************************************
 * Copyright (c) 2010 Samsung Electronics Co., Ltd. All right reserved.
 *
 * This software is the confidential and proprietary information of Samsung Electronics, Inc.
 * ("Confidential Information"). You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered into with
 * Samsung Electronics.
 ***************************************************************************************************/
'''
import os
import re
import sys
import time
import fnmatch
import optparse
import subprocess
from time import sleep
from os import listdir
from time import strftime
from datetime import datetime
from os.path import isfile, join

from configuration import *
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter
from ite.webreporter.tc_list_reporter import TCListReporter
from subprocess import Popen, PIPE
from xml.etree.ElementTree import ElementTree, ParseError

from ite.tc.analyzer import TestSpec
from ite.constants import *
from ite.config import *

TEST_ROOT = '../'
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
binary_name = opts.file_filter
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

if transport.upper() == TEST_TRANSPORT.IP:
    network = TEST_NETWORK.WIFI.lower()

testspec_path = os.path.join(testresult, TEST_SPEC_XML_FOR_RESULT)
if not os.path.exists(testspec_path) and os.path.exists(API_TC_SRC_DIR):
    container = TestSpecContainer()
    container.extract_api_testspec(API_TC_SRC_DIR, '')

    reporter = TestSpecReporter()
    reporter.generate_testspec_report(container.data)
    reporter.report('XML', testspec_path)

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

    xml_output = '<testsuites>\n'
    xml_output = xml_output + '\t<testsuite name="' + suite_name + '">\n'
    xml_output = xml_output + '\t\t<testcase name="' + tc_name + '" status="run" time="0"'

    status = 'failed'

    try:
        rc = subprocess.check_output(command, shell=True, timeout=timeout_seconds)
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

    except subprocess.TimeoutExpired:
            xml_output = xml_output + '>\n\t\t\t<failure message="'+ ' Timeout occur' +'"> </failure>\n</testcase>\n'
            print (bcolors.FAIL + suite_name + '.' + tc_name + ': timeout' + bcolors.ENDC)

    xml_output = xml_output + '</testsuite>\n'
    xml_output = xml_output + '</testsuites>\n'

    timestring = datetime.now().strftime("%Y%m%d_%H%M%S.%f")

    file_name = "%s_%s_%s_%s_%s_%s" %(platform, target, transport, network, timestring, file_suffix)

    if os.path.isfile(testresult + os.sep + file_name + '.log') or os.path.isfile(testresult + os.sep + file_name + '.xml'):
            print (bcolors.FAIL + 'File name exist !!!\nRunner Internal Error' + bcolors.ENDC)
            exit(0)

    log_file = open(testresult + os.sep + file_name + '.log', 'w')
    log_file.write('\n' + log)

    xml_file = open(testresult + os.sep + file_name + '.xml', 'w')
    xml_file.write(xml_output)


def run_gtest_testcase(testresult, binary_name, given_testsuites, given_testcases, platform, target, transport, network):

    if not binary_name:
        print (bcolors.FAIL + 'binary file name is not given. Use -f opiotn' + bcolors.ENDC)
        exit(0)

    if not binary_name.startswith(TC_BIN_PREFIX):
        binary_name = TC_BIN_PREFIX + binary_name

    testset = set()
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

        testset.add(testsuite + line)

    if not testset:
        print (bcolors.FAIL + 'No testcase Found !!!\nPlease, Check command parameter(s)' + bcolors.ENDC)

    for tc_name in testset:
        tc_info = tc_name.split('.')
        command = 'adb ' + device_name + ' shell ' + app_path + '/runner.sh ' + binary_name + ' ' + tc_name
        run_standalone_tc (tc_info[0], tc_info[1], command, '[  PASSED  ] 1 test.', platform, target, transport, network, binary_name)


def run_junit_testcase(testresult, base_package, given_testsuites, given_testcases, platform, target, transport, network, tc_scenario):

    testset = set()

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

            if not base_package:
                base_package = 'org.iotivity.service.' + module.lower() + '.test'

            cwd = os.getcwd()
            build_dir = cwd + '/../IotivityOrgSource/1.2.0-RC3/iotivity/test/bin/android/' + module.lower() + '/intermediates/classes/debug/'
            build_dir = build_dir + base_package.replace('.', os.sep) + os.sep + testcase_type.lower()
            os.chdir(build_dir)

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

                            testset.add(testcase_type.lower() + '.' + suite_name + '.' + tc_name)

            os.chdir(cwd)

    if not testset:
        print (bcolors.FAIL + 'No testcase Found !!!\nPlease, Check command parameter(s)' + bcolors.ENDC)

    for tc in testset:
        tc_info = tc.split('.')

        command = 'adb ' + device_name + ' shell am instrument -w  -e class '
        command = command + base_package + '.' + tc_info[0] + '.' + tc_info[1] + '#' + tc_info[2] + ' ' + base_package
        command = command + '/com.zutubi.android.junitreport.JUnitReportTestRunner'

        run_standalone_tc (tc_info[1], tc_info[2], command, 'OK (1 test)', platform, target, transport, network, tc_info[1].lower() + '_' + tc_info[0])

if TESTFW_TYPES.GTEST in tc_framework:
    run_gtest_testcase(testresult, binary_name, given_testsuites, given_testcases, platform, target, transport, network)

if TESTFW_TYPES.JUNIT in tc_framework:
    run_junit_testcase(testresult, base_package, given_testsuites, given_testcases, platform, target, transport, network, tc_scenario)

# ./android_test_runner.py -s CATest_btc -f ca_test
