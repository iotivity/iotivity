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


class Constants:
    interop_report_dir = "../../IotivitySECTest_New/src/automation/robot/report/interop_app/"
    file_name = "Output-SM-INTEROP_APP-Linux-14-64-Linux-14-64-20170725-210647.xml"
    test_env_xpath = '//kw[@name="Get Test Environment Details"]'
    teardown_node_xpath = '//kw[@name="Do Test Teardown"]'
    teardown_firstapp_log_xpath = '//kw[@name="Get Output From First App"]'
    teardown_secondapp_log_xpath = '//kw[@name="Get Output From Second App"]'
    LOG_XPATH = 'kw[@name="Log"]'


class Utility:
    xml_file_path = ""
    interop_report_dir = ""
    module_name = ""
    module_dir = ""
    report_dir_name = ""
    report_name = ""

    file_name = Constants.file_name
    platform = ""


    @staticmethod
    def get_platform_module_info():
        platform_module_info = Constants.file_name.split('-')
        Utility.module_name = platform_module_info[1]
        Utility.platform = platform_module_info[3]
        if "Linux" in Utility.platform:
            Utility.version = platform_module_info[4]
            Utility.bit = platform_module_info[5]


class Color:
    LIGHT_BLUE_HEADER = "#bccadb"
    DARK_BLUE_PASS = "#000cf4"
    DARK_RED_FAIL = "#f40c00"
