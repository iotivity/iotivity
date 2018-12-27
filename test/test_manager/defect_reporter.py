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
