#!/usr/bin/python3
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
import sys
import time
import fnmatch
import optparse
import platform
import fcntl

from os.path import isfile, join

from configuration import *
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter

from ite.util import *
from ite.tc.analyzer import TestSpec
from ite.constants import *
from ite.config import *

from ite.multi_logger import set_file_handler, print_runner_output

from ite.exec.multi_runner import *
from ite.exec.run_options import *
from ite.exec.runner_info_setter import *
from ite.exec.tc_executor import *


TEST_ROOT = '..'
timeout_seconds = 300

oparser = optparse.OptionParser()
oparser.add_option("-p", action="store", dest="platform", default="linux")
oparser.add_option("--platform", action="store", dest="platform", default="linux")
oparser.add_option("-t", action="store", dest="target")
oparser.add_option("--target", action="store", dest="target")
oparser.add_option("-x", action="store", dest="transport", default="")
oparser.add_option("--transport", action="store", dest="transport", default="")
oparser.add_option("-n", action="store", dest="network", default="")
oparser.add_option("--network", action="store", dest="network", default="")
oparser.add_option("-u", action="store", dest="suite")
oparser.add_option("--testsuite", action="store", dest="suite")
oparser.add_option("-c", action="store", dest="testcase")
oparser.add_option("--case", action="store", dest="testcase")
oparser.add_option("-d", action="store", dest="device_name")
oparser.add_option("--device", action="store", dest="device_name")
oparser.add_option("-f", action="store", dest="file_filter")
oparser.add_option("--filter", action="store", dest="file_filter")
oparser.add_option("-m", action="store", dest="module")
oparser.add_option("--module", action="store", dest="module")
oparser.add_option("-w", action="store", dest="tc_framework")
oparser.add_option("--framework", action="store", dest="tc_framework")
oparser.add_option("-i", action="store", dest="tc_scenario")
oparser.add_option("--scenario", action="store", dest="tc_scenario")
oparser.add_option("-g", action="store", dest="package_name")
oparser.add_option("--package", action="store", dest="package_name")
oparser.add_option("-r", action="store", dest="test_result_dir", default="")
oparser.add_option("--result", action="store", dest="test_result_dir", default="")
oparser.add_option("--path", action="store", dest="app_path")
oparser.add_option("--just_print", action="store_true", dest="just_print")
oparser.add_option("--process_id", action="store", dest="process_id")
oparser.add_option("--max_total_count", action="store", dest="max_total_count", default="3")
oparser.add_option("--min_pass_count", action="store", dest="min_pass_count", default="1")
oparser.add_option("--max_timeout_count", action="store", dest="max_timeout_count", default="2")
oparser.add_option("--verdict_path", action="store", dest="verdict_path", default="tc_verdicts.txt")
oparser.add_option("--save_verdict", action = 'store_true', dest="save_verdict", default=False)
oparser.add_option("-b", action="store", dest="build_type", default="")
oparser.add_option("--build_type", action = 'store', dest="build_type", default="")
opts, args = oparser.parse_args()

platform_type = opts.platform
file_filter = opts.file_filter
given_module = opts.module
package_name = opts.package_name
test_result_dir = opts.test_result_dir
tc_verdict_file_path = opts.verdict_path
save_verdict = opts.save_verdict

TestRunOption.max_total_count = int(opts.max_total_count)
TestRunOption.min_pass_count = int(opts.min_pass_count)
TestRunOption.max_timeout_count = int(opts.max_timeout_count)

if save_verdict:
    set_file_handler(given_module, platform_type, tc_verdict_file_path)
#sys.stdout = Multi_Logger(given_module, platform_type, tc_verdict_file_path)

print_runner_output('All parameters have taken')

dynamic_runner = None

if 'linux' == platform_type:
    dynamic_runner = LinuxTestRunner()
elif 'tizen' == platform_type:
    dynamic_runner = TizenTestRunner()
elif 'android' == platform_type:
    tc_framework = opts.tc_framework
    if not tc_framework:
        tc_framework = TESTFW_TYPES.JUNIT
    if TESTFW_TYPES.GTEST == tc_framework:
        dynamic_runner = AndroidGtestRunner()
    elif TESTFW_TYPES.JUNIT == tc_framework:
        dynamic_runner = AndroidJunitRunner()

if not dynamic_runner:
    print_runner_output('No runner has been set')
    exit(0)

device_name = dynamic_runner.set_device_name(opts.device_name)
target = dynamic_runner.set_target(opts.target)
transport = dynamic_runner.set_transport(opts.transport)
network = dynamic_runner.set_network(opts.network)
tc_framework = dynamic_runner.set_tc_framework(opts.tc_framework)
app_path = dynamic_runner.set_app_path(opts.app_path)
given_module = dynamic_runner.set_module(given_module)
build_type = dynamic_runner.set_build_type(opts.build_type, given_module)

devices = dynamic_runner.get_devices()

if devices:
    if opts.device_name and opts.device_name not in devices:
        print_runner_output('Device {} not found'.format(device_name))
        exit(-1)

    if not device_name:
        device_name = dynamic_runner.set_device_name(devices[0])

print_runner_output('dynamic_runner has been set')

if not test_result_dir:
    test_result_dir = TEST_RESULT_RUN_DIR

if test_result_dir.endswith(os.sep):
    test_result_dir = test_result_dir[:-1]

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

if not os.path.exists(test_result_dir):
    os.makedirs(test_result_dir)

test_result_dir = os.path.abspath(test_result_dir)

if not app_path:
    print (Colors.FAIL + "binary file's folder path is not given. Use --path opiotn" + Colors.ENDC)
    exit(0)

if not app_path.endswith(os.sep):
    app_path = app_path + os.sep

if app_path.endswith(os.sep):
    app_path = app_path[:-1]

print_runner_output('Splitting parameters complete')

platform_command_prefix = dynamic_runner.get_platform_command_prefix()

testspec_path = os.path.join(test_result_dir, TEST_SPEC_XML_FOR_RESULT)
if not os.path.exists(testspec_path) and os.path.exists(API_TC_SRC_DIR):
    container = TestSpecContainer()
    container.extract_api_testspec(API_TC_SRC_DIR, '')
    
    reporter = TestSpecReporter()
    reporter.generate_testspec_report(container.data)
    reporter.report('XML', testspec_path)

print_runner_output('Device root on (if applicable)')

dynamic_runner.device_root_on()

print_runner_output('Calling TC executor')

tc_executor = TcExecutor(timeout_seconds, test_result_dir)

print_runner_output('Calling runner info setter')

info_setter = RunnerInfoSetter(TEST_ROOT)

print_runner_output('Calling tc list setter')

if TESTFW_TYPES.GTEST in tc_framework:
    info_setter.set_gtest_tc_list(test_result_dir, given_module, file_filter, given_testsuites, given_testcases, dynamic_runner)

if TESTFW_TYPES.JUNIT in tc_framework:
    info_setter.set_junit_tc_list(test_result_dir, given_module, package_name, given_testsuites, given_testcases, dynamic_runner)

print_runner_output('TC list setting complete')

if opts.just_print:
    tc_executor.print_tc_list('tc_list.txt', dynamic_runner)
else:
    tc_executor.run_selected_testcases(dynamic_runner, tc_verdict_file_path, save_verdict)
