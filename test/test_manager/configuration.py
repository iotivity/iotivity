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

try:
    import xlsxwriter
except:
    print ('install python3-xlsxwriter. Possible way to install: sudo apt-get install python3-xlsxwriter')
    exit (0)

# Path Configuration
API_TC_SRC_DIR = "../src/tc"
SAMPLEAPP_TC_SRC_DIR = "../src/automation/robot/tc/sample_app"
TC_BIN_DIR = "../bin/linux"

TEST_REPORT_DIR = "../TestReport"
TEST_RESULT_DIR = TEST_REPORT_DIR + "/Result"
TEST_RESULT_RUN_DIR = TEST_RESULT_DIR + "/__run__"
TEST_SPEC_DIR = TEST_REPORT_DIR + "/Spec"
TEST_DEFECT_DIR = TEST_REPORT_DIR + "/Defect"

# Test Spec Configuration
SPEC_REPORT_TYPE = {
                    'TXT' : True,
                    'CSV' : True,
                    'XML' : True,
                    'XLSX' : True
                    }

# Test Runner Configuration
TC_BIN_PREFIX = "iotivity_"
TC_BIN_SUFFIX = "_test"
TEST_REPEAT = 1
TEST_REPEAT_FAILED = True
TEST_REPEAT_CRASHED = True
TEST_STANDALONE = False
TEST_SPEC_XML_FOR_RESULT = "TestSpec.xml"

# Jira Defect Configuration
TEST_JIRA_DEFECT_FILE_PATH = TEST_DEFECT_DIR + "/SearchRequest.xml"

EXECUTION_LOG_FILE = 'excution_log.txt'
EXECUTION_LOG_FP = None
