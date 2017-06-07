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
import platform
try:
    import xlsxwriter
except:
    print ('install python3-xlsxwriter. Possible way to install: sudo apt-get install python3-xlsxwriter')
    exit (0)

# Path Configuration
API_TC_SRC_DIR = os.path.join("..","src","tc")
SAMPLEAPP_TC_SRC_DIR = os.path.join ("..","IotivityTest","testsuite","IotivityTC" )
TC_BIN_DIR = os.path.join("..","bin",platform.system().lower())


TEST_REPORT_DIR = ".."+os.sep+"TestReport"
TEST_RESULT_DIR = TEST_REPORT_DIR + ""+os.sep+"Result"
TEST_RESULT_RUN_DIR = TEST_RESULT_DIR + ""+os.sep+"__run__"
TEST_SPEC_DIR = TEST_REPORT_DIR + ""+os.sep+"Spec"
TEST_DEFECT_DIR = TEST_REPORT_DIR + ""+os.sep+"Defect"

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
TEST_JIRA_DEFECT_FILE_PATH = TEST_DEFECT_DIR + ""+os.sep+"SearchRequest.xml"
