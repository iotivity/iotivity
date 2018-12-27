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

# This script parses the the Report XML Files from <..oictest_repo>/IotivityTest/testsuite/IotivityTC/report/ Direcotry
# and put the parsed info to .xlsx files in same directory

# TODO Implement Python Logger

import argparse
import sys

from robottcxmlparser.utility import *
from robottcxmlparser.xmlparser import *
from robottcxmlparser.testsuite import *
from robottcxmlparser.reporter import *


def get_test_suite_obj(xml_file_path):
    xml_obj = XmlParser(xml_file_path)

    testsuite_obj = TestSuite()
    testsuite_obj.set_test_env(xml_obj.xml_content)
    Utility.report_dir_name = TestSuite.set_dir_name(Utility.xml_file_path)
    testsuite_obj.test_case_parse(xml_obj.testcase_lists)
    return testsuite_obj


if __name__ == "__main__":
    cmd_parser = argparse.ArgumentParser()
    cmd_parser.add_argument('-m', dest='module_name', help='Module Name')
    cmd_parser.add_argument('-f', dest='file_path', default="all", help='XML File Name')

    Utility.get_platform_module_info()
    Utility.interop_report_dir = os.path.abspath(Constants.interop_report_dir)
    Utility.module_name = sys.argv[2]
    Utility.module_dir = Utility.interop_report_dir + os.sep + Utility.module_name.lower()
    print("Working DIR : " + Utility.module_dir + '\n')

    if len(sys.argv)==1:
        cmd_parser.print_help()
        sys.exit(1)
    arg = cmd_parser.parse_args()

    Utility.module_name = arg.module_name

    report_obj = Reporter()
    report_obj.create_report(Utility.module_name)
    #
    # if report_obj.summary_work_sheet == None:
    #     print("Report WorkBook is NULL")
    if arg.file_path == 'all' :
        for file in os.listdir(Utility.module_dir):
            if file.endswith(".xml"):
                Utility.file_name = file
                Utility.xml_file_path = Utility.module_dir + '/' + Utility.file_name
                report_obj.tc_suite_obj_list.append(get_test_suite_obj(Utility.xml_file_path))

    # for tc in testsuite_obj.test_list:
    #     tc.print_tc_details()

    report_obj.merge_report()
    report_obj.close_report()
    print(Utility.module_name)
