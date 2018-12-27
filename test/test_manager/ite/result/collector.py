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
import os.path
from xml.dom.minidom import Document
from xml.etree.ElementTree import ElementTree, ParseError

from ite.config import *
from ite.constants import *
from ite.util import *

class TestResultCollector:

    LOG_START_PATTERN = re.compile(r'\n.*\[\s*RUN\s*\]\s*(?P<testsuite>\w+)\.(?P<testcase>\w+).*')
    LOG_END_PATTERN = re.compile(r'\n.*\[\s*(?P<result>(OK|FAILED))\s*\]\s*(?P<testsuite>\w+)\.(?P<testcase>\w+).*')

    def __init__(self):
        self.result = TestResult()
        self.tcType = "UnKnown"

    def get_platform_from_path(self, path):
        filename = os.path.basename(path)
        for platform in TEST_PLATFORM:
            if platform.lower() in filename.lower():
                return platform

        return 'LINUX'

    def get_target_from_path(self, path):
        filename = os.path.basename(path)
        items = filename.split('_')
        target = items[1]
        if target == '' or target == None:
            target = ' '

        return target

    def get_buildtype_from_path(self, path):
        filename = os.path.basename(path)
        items = filename.split('_')
        build_type = items[2]
        return build_type.upper()

    def get_transport_from_path(self, path):
        filename = os.path.basename(path)
        items = filename.split('_')
        transport = items[3]
        if transport == '' or transport == None:
            transport = NO_TRANSPORT

        return transport.upper()

    def get_network_from_path(self, path):
        filename = os.path.basename(path)
        items = filename.split('_')
        network = items[4]
        if network == '' or network == None:
            network = NO_NETWORK

        return network.upper()

    def get_tctype_from_path(self, path):
        filename = os.path.basename(path)
        for tctype in TESTCASE_TYPES:
            if (tctype.lower() in filename):
                return tctype

        return 'UnKnown'

    def get_module_from_suite(self, testsuite):
        testsuite_array = testsuite.split('.')
        testsuite = testsuite_array[len(testsuite_array) - 1]

        for module in TESTSUITE_MODULES:
            if (testsuite.startswith(module)):
                return module

        return 'UnKnown'

    def get_tctype_from_suite(self, testsuite):
        testsuite_array = testsuite.split('.')
        testsuite = testsuite_array[len(testsuite_array) - 1]

        for tctype in TESTCASE_TYPES:
            if (tctype.lower() in testsuite):
                self.tcType = tctype
                return tctype

        return 'UnKnown'

    def analyze_result_xml(self, path):
        platform = self.get_platform_from_path(path)
        target = self.get_target_from_path(path)
        build_type = self.get_buildtype_from_path(path)
        transport = self.get_transport_from_path(path)
        network = self.get_network_from_path(path)
        runtype = GT_RUN_TYPE.STANDALONE

        tctype = 'UnKnown'
        if platform.upper() == TEST_PLATFORM.ARDUINO or platform.upper() == TEST_PLATFORM.ANDROID:
            tctype = self.get_tctype_from_path(path)

        try :
            doc = ElementTree(file=path)

            if not doc.getroot().tag == GT_ELM.TESTSUITES:
                print("==> Invalid result file path: " + path)
                return;

            for testsuite in doc.findall(GT_ELM.TESTSUITE):
                suite_name = testsuite.get(GT_ATT.NAME)

                module = self.get_module_from_suite(suite_name)
                if tctype == 'UnKnown':
                    tctype = self.get_tctype_from_suite(suite_name)

                for testcase in testsuite.findall(GT_ELM.TESTCASE):
                    tc_name = testcase.get(GT_ATT.NAME)

                    tc_status = testcase.get(GT_ATT.STATUS)
                    tc_time = testcase.get(GT_ATT.TIME)

                    tc_result = self.result.find_testcase(platform, target, build_type, transport, network, tctype, module, suite_name, tc_name)

                    tc_result.add_result(tc_status, tc_time, testcase.findall(GT_ELM.FAILURE))

                    log_path = path.replace('.xml', '.log')
                    if os.path.isfile(log_path):
                        prefix = '\nLog File Name: ' + os.path.basename(path) + '\n\n\nContent:\n'
                        testlog = prefix + read_file(log_path)
                        tc_result.add_test_log(testlog)

                    tc_result.add_test_runtype(runtype)

        except ParseError:
            print("There is a XML Parse Error on " + path)

    def parse_log_pattern(self, path):
        source = read_file(path);
        if source == '':
            return

        search_pos = 0
        while(True):
            start_match = self.LOG_START_PATTERN.search(source, search_pos)
            if (start_match == None):
                break;

            start_data = (start_match.group('testsuite'), start_match.group('testcase'))

            end_match = self.LOG_END_PATTERN.search(source, search_pos)
            if (end_match == None):
                test_result = GT_LOG.CRASHED
                if GT_HANG_LOG in source:
                    test_result = GT_LOG.HANG

                yield start_data + (test_result, start_match.end(), start_match.endpos)
                break;

            search_pos = end_match.end()

            end_data = (end_match.group('testsuite'), end_match.group('testcase'))


            if (start_data != end_data):
                print("Start(%s.%s) and End(%s.%s) Test Name is not matched! - %s" % (start_data[0], start_data[1], end_data[0], end_data[1], path))
                #continue

            yield start_data + (end_match.group('result'), start_match.end(), end_match.start())

    def analyze_result_log(self, path):
        infolist = list()

        for info in self.parse_log_pattern(path):
            infolist.append(info)

        return infolist

    def collect_results(self, path):
        if (not os.path.isdir(path)):
            return

        for dirpath, dirnames, filenames in os.walk(path):
            for filename in filenames:
                filepath = os.path.join(dirpath, filename)

                if filename.endswith('.xml'):
                    if not filename.endswith('TestSpec.xml'):
                        self.analyze_result_xml(filepath)


class TestResult:


    def __init__(self):
        self.data = dict()

    def find_testcase(self, platform, target, build_type, transport, network, tctype, module, suite, name):
        def assign_dict(key, parent_dict):
            if (not key in parent_dict):
                parent_dict[key] = dict()

        assign_dict(platform, self.data)
        assign_dict(target, self.data[platform])
        assign_dict(build_type, self.data[platform][target])
        assign_dict(transport, self.data[platform][target][build_type])
        assign_dict(network, self.data[platform][target][build_type][transport])
        assign_dict(tctype, self.data[platform][target][build_type][transport][network])
        assign_dict(module, self.data[platform][target][build_type][transport][network][tctype])
        assign_dict(suite, self.data[platform][target][build_type][transport][network][tctype][module])

        if (not name in self.data[platform][target][build_type][transport][network][tctype][module][suite]):
            self.data[platform][target][build_type][transport][network][tctype][module][suite][name] = TestCaseResult(name)

        return self.data[platform][target][build_type][transport][network][tctype][module][suite][name]


class TestCaseResult:


    def __init__(self, name):
        self.name = name
        self.success = 0
        self.fail = 0
        self.runresult = list()

    def add_result(self, status, time, failurelist):
        runresult = TestRunResult()
        runresult.fail_msg = ' '
        runresult.test_log = ' '
        runresult.runtype = ''
        if (len(failurelist) > 0 or status == GT_ATT_STATUS[GT_LOG.CRASHED] or status == GT_ATT_STATUS[GT_LOG.HANG]):
            # Fail!
            self.fail += 1
            runresult.result = TC_RESULT.FAIL
            if (status == GT_ATT_STATUS[GT_LOG.CRASHED]):
                runresult.result += '(Crashed)'
            elif (status == GT_ATT_STATUS[GT_LOG.HANG]):
                runresult.result += '(Hang)'

            fail_msg = ''
            for failure in failurelist:
                fail_msg += failure.get(GT_ATT.MESSAGE) + '\n\n'
            runresult.fail_msg = fail_msg.strip()
        else:
            # Success
            self.success += 1
            runresult.result = TC_RESULT.PASS
            runresult.fail_msg = ' '

        runresult.runtime = float(time)
        self.runresult.append(runresult)

    def add_test_log(self, log):
        self.runresult[-1].test_log = log

    def add_test_runtype(self, runtype):
        self.runresult[-1].runtype = runtype


class TestRunResult:

    def __init(self):
        self.result = ''
        self.runtype = ''
        self.runtime = 0.0
        self.fail_msg = ' '
        self.test_log = ' '