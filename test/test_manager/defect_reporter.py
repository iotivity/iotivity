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
import sys
import time
from time import strftime

sys.path.append('./lib')

from ite.reporter.jira_report import JiraDefectReporter
from configuration import *

DEFECT_PREFIX = "Defect_"

module_filter = ''
if len(sys.argv) > 1:
    module_filter = sys.argv[1]

if not os.path.exists(TEST_SPEC_DIR):
    os.makedirs(TEST_SPEC_DIR, exist_ok=True)
    
print("### Start to Make Jira Defect Report")

if not os.path.exists(TEST_JIRA_DEFECT_FILE_PATH):
    print("==> Can't find defect source file: " + TEST_JIRA_DEFECT_FILE_PATH)
    raise SystemExit(1)

reporter = JiraDefectReporter()
reporter.generate_defect_report(TEST_JIRA_DEFECT_FILE_PATH)

timestring = strftime("%Y%m%d_%H%M%S", time.localtime(time.time()))
file_name = DEFECT_PREFIX + timestring + ".xlsx"
file_path = os.path.join(TEST_DEFECT_DIR, file_name)

reporter.report_to_xlsx(file_path)
     
print("### Making Defect Report is Done!!")
