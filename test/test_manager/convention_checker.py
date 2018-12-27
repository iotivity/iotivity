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
