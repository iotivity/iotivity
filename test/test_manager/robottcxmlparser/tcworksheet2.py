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

from .utility import *

class TcWorkSheet2:

    cell_format = None

    def __init__(self, workbook, _transport):
        self.row = 0
        self.column = 0
        self.testcase_work_sheet = workbook.add_worksheet(_transport)
        # Add a bold format to use to highlight cells.
        self.header_format = workbook.add_format({'bold': True, 'align':'center'})
        self.header_format.set_bg_color(Color.LIGHT_BLUE_HEADER)
        self.cell_format = workbook.add_format({'font_size' : 10, 'italic' : True})
        self.cell_format.set_align('top')

        self.pass_format = workbook.add_format({'bold': True, 'align':'center', 'valign': 'vcenter', 'font_color': 'white', 'font_size' : 10})
        self.pass_format.set_bg_color(Color.DARK_BLUE_PASS)

        self.fail_format = workbook.add_format({'bold': True, 'align':'center', 'valign': 'vcenter', 'font_color': 'white', 'font_size' : 10})
        self.fail_format.set_bg_color(Color.DARK_RED_FAIL)

        self.tc_dict = dict()
        self.platform_list = list()

    def make_tc_worksheet_data(self, tc_suite_obj):
        """
        -tcname(dict)----
        -               - description(string)   
        -               -               
        -               - QoS (dict)  ---               
        -                               ---- Platform (dict) ----
        -                                                       - Result   
        """

        for tc in tc_suite_obj.test_list:
            if not tc.name in self.tc_dict.keys():
                self.tc_dict[tc.name] = dict()

            self.tc_dict[tc.name]['description'] = tc.description

            if not "qos_dict" in self.tc_dict[tc.name].keys():
                self.tc_dict[tc.name]["qos_dict"] = dict()

            qos_key = tc.tc_env["qos"]
            if not qos_key in self.tc_dict[tc.name]["qos_dict"].keys():
                self.tc_dict[tc.name]["qos_dict"][qos_key] = dict()

            if not "platform_dict" in self.tc_dict[tc.name]["qos_dict"][qos_key].keys():
                self.tc_dict[tc.name]["qos_dict"][qos_key]["platform_dict"] = dict()

            print("yy")
            print(self.tc_dict[tc.name]["qos_dict"][qos_key]["platform_dict"])

            platform1 = tc_suite_obj.test_env["os1"] + '_' + tc_suite_obj.test_env["os2"]
            sdk1 = tc_suite_obj.test_env["sdk1"] + '_' + tc_suite_obj.test_env["sdk2"]

            platform = platform1 + "\n(Server - Client) \n" + sdk1
            print("Platform :")
            print(platform)


            # #TC QoS as Key and Value
            # self.tc_dict[tc.name]["qos"][tc.tc_env["qos"]] = tc.tc_env["qos"]

            if not platform in self.tc_dict[tc.name]["qos_dict"][qos_key]["platform_dict"].keys():
                self.tc_dict[tc.name]["qos_dict"][qos_key]["platform_dict"][platform] = tc.result

            if not platform in self.platform_list:
                self.platform_list.append(platform)

            # self.tc_dict[tc.name]["qos_dict"][qos_key]["platform_dict"] = tc.result

        self.print_tc_worksheet()

    def print_tc_worksheet(self):
        pass
        # # self.create_header()
        # # self.write_tc_worksheet_data()
        # for key in self.tc_dict:
        #     print("----------------------")
        #     print("TC Name :")
        #     print(key)
        #     # print("Platforms")
        #     # for key in self.tc_dict[key]["platform_dict"]:
        #     #     print(key)

    def create_header(self):
        # No - Suite Name - TC Name - TC Scenario - Platform-1 - Platform-2 .....
        # self.defect_worksheet.set_column(2, 2, 10)
        self.testcase_work_sheet.set_column('C:D', 30)
        header_row = 0
        self.testcase_work_sheet.write(header_row, 0, "No", self.header_format)
        self.testcase_work_sheet.write(header_row, 1, "Suite Name", self.header_format)
        self.testcase_work_sheet.write(header_row, 2, "TC Name", self.header_format)
        self.testcase_work_sheet.write(header_row, 3, "TC Scenario", self.header_format)
        self.testcase_work_sheet.write(header_row, 4, "TC Configuration", self.header_format)

        col = 5
        for platform in self.platform_list:
            self.testcase_work_sheet.write(header_row, col, platform, self.header_format)
            col+=1

    def write_tc_worksheet_data(self):

        for tc_name in self.tc_dict:


            # For Each Platform , Ex : Linux - Linux , Andorid - Linux
            for qos in self.tc_dict[tc_name]["qos_dict"]:
                self.row += 1
                self.testcase_work_sheet.write(self.row, 0, self.row, self.cell_format)
                self.testcase_work_sheet.write(self.row, 2, tc_name, self.cell_format)
                self.testcase_work_sheet.write(self.row, 3, self.tc_dict[tc_name]["description"], self.cell_format)
                self.testcase_work_sheet.write(self.row, 4, qos, self.cell_format)

                for platform in self.tc_dict[tc_name]["qos_dict"][qos]["platform_dict"]:

                    platform_index = 5 + self.platform_list.index(platform)
                    result = "Pass"
                    result_int = self.tc_dict[tc_name]["qos_dict"][qos]["platform_dict"][platform]
                    if result_int== 0:
                        result = "Fail"

                    self.testcase_work_sheet.write(self.row, platform_index, result, self.set_color_format(result_int))

    def set_color_format(self, result):
        if result == 0:
            return self.fail_format
        else:
            return self.pass_format





