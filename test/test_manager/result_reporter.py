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

import time
import os
from time import strftime
import shutil

import sys

from configuration import *

from ite.tc.xmlanalyzer import TCXMLAnalyzer
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter
from ite.reporter.result_reporter import TestResultReporter

XLSX_FILE_NAME = "TestResult.xlsx"

print("### Start to Make Test Result Report")

folder_prefix = ""

run_dir = TEST_RESULT_RUN_DIR
if len(sys.argv) > 1:
    run_dir = sys.argv[1]

if len(sys.argv) > 2:
    folder_prefix = sys.argv[2].upper() + "_"

timestring = strftime("%Y%m%d_%H%M%S", time.localtime(time.time()))
if os.path.exists(run_dir):
    result_dir = os.path.join(TEST_RESULT_DIR, folder_prefix + "TestResult_" + timestring)
    shutil.move(run_dir, result_dir)
else:
    print("### Can't find Result Source Dir: " + run_dir)
    raise SystemExit(1)

testspec_path = os.path.join(result_dir, TEST_SPEC_XML_FOR_RESULT)
testspec_data = None

if not os.path.exists(testspec_path):
    if os.path.exists(API_TC_SRC_DIR):
        container = TestSpecContainer()
        container.extract_api_testspec(API_TC_SRC_DIR, '')
        reporter = TestSpecReporter()
        reporter.generate_testspec_report(container.data)
        reporter.report('XML', testspec_path)

if os.path.exists(testspec_path):
    container = TCXMLAnalyzer()
    container.read_spec_xml(testspec_path)
    testspec_data = container.data
else:
    print ('Unable to generate report. Problem with Testspec file')
    exit(-1)

report_name = "TestResult_" + timestring + ".xlsx"
reporter = TestResultReporter()
reporter.generate_testresult_report(result_dir, testspec_data)
reporter.report_to_xlsx(os.path.join(result_dir, report_name))

def clear_create_folder(name):
    new_dir = os.path.join(result_dir, name)
    if os.path.exists(new_dir):
        shutil.rmtree(new_dir)
    os.makedirs(new_dir)

    return new_dir

reporter.report_fail_result_to_txt(clear_create_folder("Failed"))
reporter.report_pass_result_to_txt(clear_create_folder("Pass"))

print("Making Test Result Report is Done!!")
