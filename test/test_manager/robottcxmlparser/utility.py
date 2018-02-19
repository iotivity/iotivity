#!/usr/bin/python3
################################################################################################
# Copyright (c) 2010 Samsung Electronics Co., Ltd. All right reserved.
#
# This software is the confidential and proprietary information of Samsung Electronics, Inc.
# ("Confidential Information"). You shall not disclose such Confidential Information and shall
# use it only in accordance with the terms of the license agreement you entered into with
# Samsung Electronics.
################################################################################################


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
