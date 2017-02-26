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

import sys

from configuration import *
from ite.tc.container import TestSpecContainer

module_filter = ''
if len(sys.argv) > 1:
    module_filter = sys.argv[1]

print("### Start to Check TC Name & Comment Rule Conventions for Iotivity SEC Test Codes\n")
checker = TestSpecContainer()
api_valid_convention = checker.extract_api_testspec(API_TC_SRC_DIR, module_filter)
sampleapp_valid_convention = checker.extract_sampleapp_testspec(SAMPLEAPP_TC_SRC_DIR, module_filter)
print("\n### Checking TC Name & Comment Rule Conventions for Iotivity SEC Test Codes is Done!!")

if api_valid_convention == False or sampleapp_valid_convention == False:
    print ("\n==> There is a violation of API TC or Sample App TC naming convention")
    exit(1)
