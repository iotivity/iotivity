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

class TestRunOption:

    max_total_count = 3
    min_pass_count = 1
    max_timeout_count = 2
    XML_PASS_CRITERIA = 'xml'
    LOG_PASS_CRITERIA = 'log'

    def __init__(self, binary_name, suite_name, tc_name, package_name):
        self.binary_name = binary_name
        self.suite_name = suite_name
        self.tc_name = tc_name
        self.package_name = package_name
        self.total_count = 0
        self.pass_count = 0
        self.fail_count = 0
        self.timeout_count = 0

    def increase_total_count(self):
        self.total_count += 1

    def increase_pass_count(self):
        self.pass_count += 1

    def increase_fail_count(self):
        self.fail_count += 1

    def increase_timeout_count(self):
        self.timeout_count += 1

    def is_execution_complete(self):

        if self.pass_count >= TestRunOption.min_pass_count or self.timeout_count >= TestRunOption.max_timeout_count or self.total_count >= TestRunOption.max_total_count:
            return True

        return False

