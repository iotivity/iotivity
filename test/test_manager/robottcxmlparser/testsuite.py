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

from .testcase import *
from lxml import etree


class TestSuite:

    def __init__(self):
        self.test_env = dict()
        self.test_list = list()
        self.fail_test_list = list()
        self.platform = Utility.platform.replace("_", "")
        self.network = ""
        self.total_pass = 0
        self.total_fail = 0
        self.total_not_run = 0
        self.version = ""
        self.bit = ""
        self.module_name = ""

    def test_case_parse(self, test_case_list):
        for tc in test_case_list:
            tc_obj = TestCase(tc, self.module_name, self.test_env)
            tc_obj.get_tc_content()
            self.test_list.append(tc_obj)
            if tc_obj.result == 1:
                self.total_pass += 1
            else:
                self.total_fail += 1
                self.fail_test_list.append(tc_obj)

    def set_test_env(self, suite_setup_content):
        test_env_node = suite_setup_content.xpath(Constants.test_env_xpath)
        logs = test_env_node[0].find("kw")
        msg = logs[2].text
        tc_env_list = msg.split()
        print(tc_env_list)
        self.test_env["os1"] = tc_env_list[0]
        self.test_env["os2"] = tc_env_list[1]
        self.test_env["sdk1"] = tc_env_list[2]
        self.test_env["sdk2"] = tc_env_list[3]
        self.test_env["transport"] = tc_env_list[4]
        self.test_env["secured"] = tc_env_list[5]
        self.test_env["qos"] = tc_env_list[6]
        self.test_env["tls"] = ""

    @staticmethod
    def set_dir_name(xml_file_name):
        xml_file_name_split = xml_file_name.split('-')
        temp_dir_name = xml_file_name_split[-2] + '-' + xml_file_name_split[-1]
        return temp_dir_name.replace(".xml", "")

