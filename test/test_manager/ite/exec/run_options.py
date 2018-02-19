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

