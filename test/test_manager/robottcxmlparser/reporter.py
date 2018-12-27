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
import traceback
import xlsxwriter
from .utility import *
from .summaryworksheet import *
from .tcworksheet import *
from .tcworksheet2 import *
from .defectworksheet import *

class Reporter:

    def __init__(self):
        self.module_name = ""
        self.tc_suite_obj_list = list()
        self.report_path = ""
        self.result_file_name = ""
        self.report_dir = ""
        self.total_pass = 0
        self.total_fail = 0
        self.total_not_run = 0
        self.workbook = None
        self.summary_work_sheet = None
        self.tc_work_sheet = None
        self.tc_work_sheet2 = None
        self.tc_worksheet_dict = dict()
        self.defect_work_sheet = None

    def create_report(self, module_name):
        try:
            self.module_name = module_name.upper()
            self.result_file_name = "InterOp_Test_Result" + module_name.upper() + ".xlsx"

            self.report_dir = Utility.interop_report_dir + os.sep + module_name.lower() + os.sep + 'report'

            if not os.path.isdir(self.report_dir):
                os.makedirs(self.report_dir)
            os.chdir(self.report_dir)
            print("REPORT DIR : " + self.report_dir)
            print(self.result_file_name)
            self.workbook = xlsxwriter.Workbook(self.result_file_name)
            self.summary_work_sheet = SummaryWorkSheet(self.workbook)
            self.defect_work_sheet = DefectWorkSheet(self.workbook)
            # self.tc_work_sheet = TcWorkSheet(self.workbook)
            # self.tc_work_sheet = TcWorkSheet2(self.workbook)


        except Exception:
            traceback.print_exc()

    def merge_report(self):
        for test_suite_obj in self.tc_suite_obj_list:
            # self.tc_work_sheet.write_tc_desc(test_suite_obj)
            self.defect_work_sheet.write_defect_desc(test_suite_obj)
            self.summary_work_sheet.write_suite_summary(test_suite_obj, self.module_name, test_suite_obj.total_pass, test_suite_obj.total_fail, test_suite_obj.total_not_run)
            self.total_pass += test_suite_obj.total_pass
            self.total_fail += test_suite_obj.total_fail

        for test_suite_obj in self.tc_suite_obj_list:
            # Create WorkSheet for Unique Transport
            transport = test_suite_obj.test_env["transport"]
            if not transport in self.tc_worksheet_dict.keys():
                print("transport doesn't exist")
                self.tc_worksheet_dict[transport] = TcWorkSheet2(self.workbook, transport)

            self.tc_worksheet_dict[transport].make_tc_worksheet_data(test_suite_obj)

        for key in self.tc_worksheet_dict:
            # self.tc_worksheet_dict[key].print_tc_worksheet()
            self.tc_worksheet_dict[key].create_header()
            self.tc_worksheet_dict[key].write_tc_worksheet_data()

        # sortTC(self.tc_suite_obj_list)
        self.summary_work_sheet.write_total_summary(self.total_pass, self.total_fail, self.total_not_run)



    def close_report(self):
        self.workbook.close()
