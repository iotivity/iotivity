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

import time
import os
from time import strftime
import shutil

import sys

from configuration import *

from ite.tc.xmlanalyzer import TCXMLAnalyzer
from ite.tc.container import TestSpecContainer
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
if os.path.exists(testspec_path):
    container = TCXMLAnalyzer()
    container.read_spec_xml(testspec_path)
    testspec_data = container.data
elif os.path.exists(API_TC_SRC_DIR):
    container = TestSpecContainer()
    container.extract_api_testspec(API_TC_SRC_DIR, '')
    testspec_data = container.data

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
