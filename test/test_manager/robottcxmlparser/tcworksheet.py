#!/usr/bin/python3
################################################################################################
# Copyright (c) 2017 Samsung Electronics Co., Ltd. All right reserved.
#
# This software is the confidential and proprietary information of Samsung Electronics, Inc.
# ("Confidential Information"). You shall not disclose such Confidential Information and shall
# use it only in accordance with the terms of the license agreement you entered into with
# Samsung Electronics.
################################################################################################


class TcWorkSheet:

    testcase_work_sheet = None
    cell_format = None

    def __init__(self, workbook):
        self.row = 0
        self.column = 0
        self.testcase_work_sheet = workbook.add_worksheet("Test Case")
        # Add a bold format to use to highlight cells.
        self.header_format = workbook.add_format({'bold': True, 'align':'center'})
        self.header_format.set_bg_color('#F5DA81')
        self.cell_format = workbook.add_format({'text_wrap': True})
        self.cell_format.set_align('top')
        self.testcase_work_sheet.set_column(0, 0, 5)
        self.testcase_work_sheet.write('A1', 'No.', self.header_format)
        self.testcase_work_sheet.set_column(0, 0, 2)
        self.testcase_work_sheet.write('B1', 'Test Configuration', self.header_format)
        self.testcase_work_sheet.set_column(0, 0, 10)
        self.testcase_work_sheet.write('C1', 'Test Suite Name', self.header_format)
        self.testcase_work_sheet.set_column(2, 2, 15)
        self.testcase_work_sheet.write('D1', 'Test Case Name', self.header_format)
        self.testcase_work_sheet.write('E1', 'Objective', self.header_format)
        self.testcase_work_sheet.set_column(5, 5, 15)
        self.testcase_work_sheet.write('F1', 'Description', self.header_format)
        self.testcase_work_sheet.set_column(10, 10, 10)
        self.testcase_work_sheet.write('G1', 'Result', self.header_format)
        self.testcase_work_sheet.write('H1', 'Logs', self.header_format)

    def write_tc_desc(self, test_suite_obj):

        platform = test_suite_obj.test_env["os1"] + '_' + test_suite_obj.test_env["os2"]
        sdk = test_suite_obj.test_env["sdk1"] + '_' + test_suite_obj.test_env["sdk2"]
        details = "Security = " + test_suite_obj.test_env["secured"] + "\nTransport = " + test_suite_obj.test_env["transport"] + "\nQoS = " + test_suite_obj.test_env["qos"] + "\nTLS = " + test_suite_obj.test_env["tls"]

        test_configuration = "Platform :\n" + platform + "\n" + "Sdk :\n" + sdk + "\n" + "Details :\n" + details

        for tc_obj in test_suite_obj.test_list:
            self.row += 1
            tc_result = "PASS"
            if tc_obj.result == 0:
                tc_result = "FAIL"

            self.testcase_work_sheet.write(self.row, 0, self.row, self.cell_format)
            self.testcase_work_sheet.write(self.row, 1, test_configuration, self.cell_format)
            self.testcase_work_sheet.write(self.row, 2, tc_obj.suite_name, self.cell_format)
            self.testcase_work_sheet.write(self.row, 3, tc_obj.name, self.cell_format)

            self.testcase_work_sheet.write(self.row, 4, tc_obj.objective, self.cell_format)
            self.testcase_work_sheet.write(self.row, 5, tc_obj.description, self.cell_format)
            self.testcase_work_sheet.write(self.row, 6, tc_result, self.cell_format)

            logs = "First App Log:\n" + tc_obj.log_files[0] + "Second App Log:\n" + tc_obj.log_files[1]
            self.testcase_work_sheet.write(self.row, 7, logs,
                                                  self.cell_format)




