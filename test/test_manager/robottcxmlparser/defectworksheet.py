#!/usr/bin/python3
import xlsxwriter


class DefectWorkSheet:

    def __init__(self, workbook):
        self.row = 0
        self.column = 0
        self.defect_worksheet = workbook.add_worksheet("Defects")
        self.header_format = workbook.add_format({'bold': True, 'align':'center'})
        self.header_format.set_bg_color('#F5DA81')
        self.normal_format = workbook.add_format()
        self.center_format = workbook.add_format({'align':'center'})
        self.normal_format.set_align('top')

        self.defect_worksheet.set_column(0, 0, 3)
        self.defect_worksheet.write('A1', 'No.', self.header_format)
        self.defect_worksheet.set_column(1, 1, 6)
        self.defect_worksheet.write('B1', 'Module', self.header_format)
        self.defect_worksheet.set_column(2, 2, 10)
        self.defect_worksheet.write('C1', 'Test Suite', self.header_format)
        self.defect_worksheet.set_column(3, 3, 25)
        self.defect_worksheet.write('D1', 'Test Case', self.header_format)
        self.defect_worksheet.write('E1', 'Fail Rate', self.header_format)
        self.defect_worksheet.write('F1', 'Result', self.header_format)
        self.defect_worksheet.write('G1', 'Time', self.header_format)
        self.defect_worksheet.set_column(7, 7, 20)
        self.defect_worksheet.write('H1', 'Defect', self.header_format)
        self.defect_worksheet.write('I1', 'JIRA ID', self.header_format)
        self.defect_worksheet.set_column(9, 9, 20)
        self.defect_worksheet.write('J1', 'Description', self.header_format)
        self.defect_worksheet.write('K1', 'First App Log', self.header_format)
        self.defect_worksheet.write('L1', 'Second App Log', self.header_format)

    def write_defect_desc(self, tc_suite_obj):

        for tc_obj in tc_suite_obj.fail_test_list:
            self.row += 1
            self.column = 0
            tc_result = "PASS"
            if tc_obj.result == 0:
                tc_result = "FAIL"
            self.defect_worksheet.write(self.row, 0, self.row, self.normal_format)
            self.defect_worksheet.write(self.row, 1, tc_suite_obj.module_name, self.center_format)
            self.defect_worksheet.write(self.row, 2, tc_obj.suite_name, self.normal_format)
            self.defect_worksheet.write(self.row, 3, tc_obj.name, self.normal_format)
            self.defect_worksheet.write(self.row, 5, tc_result, self.center_format)
            self.defect_worksheet.write(self.row, 9, tc_obj.description, self.normal_format)
            self.defect_worksheet.write(self.row, 10, tc_obj.log_files[0], self.normal_format)
            self.defect_worksheet.write(self.row, 11, tc_obj.log_files[1], self.normal_format)


