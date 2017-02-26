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
import sys
from time import strftime

from configuration import *
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter

module_filter = ''
if len(sys.argv) > 1:
    module_filter = sys.argv[1]

if not os.path.exists(TEST_SPEC_DIR):
    os.makedirs(TEST_SPEC_DIR, exist_ok=True)

def report(data, prefix):
    reporter = TestSpecReporter()
    reporter.generate_testspec_report(data)

    timestring = strftime("%Y%m%d_%H%M%S", time.localtime(time.time()))
    for report_type in list(SPEC_REPORT_TYPE):
        if SPEC_REPORT_TYPE[report_type] == False:
            continue
        file_name = prefix + timestring + "." + report_type.lower()
        file_path = os.path.join(TEST_SPEC_DIR, file_name)

        reporter.report(report_type, file_path)

print("### Start to Make API Test Spec Report")
container = TestSpecContainer()
container.extract_api_testspec(API_TC_SRC_DIR, module_filter)
report(container.data, "TestSpec_API_")
print("### Making API Test Spec Report is Done!!")

print("### Start to Make Sample App Test Spec Report")
container = TestSpecContainer()
container.extract_sampleapp_testspec(SAMPLEAPP_TC_SRC_DIR, module_filter)
report(container.data, "TestSpec_SampleApp_")
print("### Making Sample App Test Spec Report is Done!!")
