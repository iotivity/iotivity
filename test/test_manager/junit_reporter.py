#!/usr/bin/python

'''
/***************************************************************************************************
 * Copyright (c) 2015 Samsung Electronics Co., Ltd. All right reserved.
 *
 * This software is the confidential and proprietary information of Samsung Electronics, Inc.
 * ("Confidential Information"). You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered into with
 * Samsung Electronics.
 ***************************************************************************************************/
 author : sk.islam@samsung.com
'''

import os
import sys
from configuration import *
import optparse
import xlsxwriter
import xml.etree.ElementTree as ET
import glob
import datetime as dt

oparser = optparse.OptionParser()
oparser.add_option("-m", action="store", dest="module_name")
oparser.add_option("--module", action="store", dest="module_name")
opts, args = oparser.parse_args()

#Assigning command-line options value to global variables
MODULE_NAME = opts.module_name
MODULE_DIRECTORY= ""
ITC_FILE_PATH = ""
#Initializing global variables
print(MODULE_NAME)
print("GOT Path: " + TEST_RESULT_RUN_DIR)
MODULE_DIRECTORY = TEST_RESULT_DIR + "/android/"+MODULE_NAME

UTC_FILE_PATH= MODULE_DIRECTORY+"/btc"
ITC_FILE_PATH = MODULE_DIRECTORY+"/stc"

TEST_MODULE_COLUMN_INDEX = 0
TEST_TYPE_COLUMN_INDEX = 1
TEST_SUITE_COLUMN_INDEX = 2
TEST_CASE_COLUMN_INDEX = 3
TEST_RESULT_COLUMN_INDEX = 4
TEST_DURATION_COLUMN_INDEX = 5
TEST_MESSAGE_COLUMN_INDEX = 6
TEST_DEFECT_COLUMN_INDEX = 6
TEST_JIRAID_COLUMN_INDEX = 7
TEST_DESCRIPTION_COLUMN_INDEX = 8

total_utc_count = 0
utc_pass_count = 0
utc_fail_count = 0
utc_crash_count = 0
total_itc_count = 0
itc_pass_count = 0
itc_fail_count = 0
itc_crash_count = 0
total_count = 0
module_dict = {}

def set_excel_style() :

    global pass_header_format
    global common_format
    global common_bottom_format
    global pass_header_format
    global testcase_format
    global test_suite_format
    global merge_format
    global description_format

    pass_header_format = module_workbook.add_format({'bold': True,'border':True,'align':'center'})
    pass_header_format.set_border(style=2)
    common_format = module_workbook.add_format({'border':True,'align':'center','valign':'vcenter'})
    common_format.set_border(style=1)
    common_format.set_right(2)
    common_bottom_format = module_workbook.add_format({'border':True,'align':'center','valign':'vcenter'})
    common_bottom_format.set_bottom(2)
    common_bottom_format.set_right(2)
    description_format = module_workbook.add_format({'border':True})
    description_format.set_border(style=2)
    testcase_format = module_workbook.add_format({'border':True, 'align':'center','valign':'vcenter'})
    testcase_format.set_border(style = 1)
    testcase_format.set_right(2)
    test_suite_format = module_workbook.add_format({'bottom':True,'align':'center','valign':'vcenter'})
    test_suite_format.set_border(style = 2)
    merge_format = module_workbook.add_format({'border':True,'align':'center','valign':'vcenter'})
    merge_format.set_border(style=2)


def open_excel_workbook() :
    global module_workbook
    date = dt.datetime.today()
    yy_mm_dd = date.strftime("%Y%m%d")
    wk_no = date.isocalendar()[1]
    module_workbook = xlsxwriter.Workbook(MODULE_DIRECTORY +'/TestResult_%s_Android_%s_W%s.xlsx' % (MODULE_NAME.upper(), yy_mm_dd, str(wk_no) ))
    set_excel_style()
    add_worksheet_to_excel_workbook()

def set_summary_work_sheet_header():
    summary_work_sheet.set_column('A:A', 10)
    summary_work_sheet.set_column('B:B', 10)
    summary_work_sheet.set_column('C:C', 10)
    summary_work_sheet.set_column('D:D', 10)
    summary_work_sheet.set_column('E:E', 10)
    summary_work_sheet.write('A1', 'Module', pass_header_format)
    summary_work_sheet.write('B1', 'Type', pass_header_format)
    summary_work_sheet.write('C1', 'Passed', pass_header_format)
    summary_work_sheet.write('D1', 'Failed', pass_header_format)
    summary_work_sheet.write('E1', 'Total', pass_header_format)

def set_pass_work_sheet_header():
    pass_work_sheet.set_column('A:A', 10)
    pass_work_sheet.set_column('B:B', 10)
    pass_work_sheet.set_column('C:C', 40)
    pass_work_sheet.set_column('D:D', 50)
    pass_work_sheet.set_column('E:E', 12)
    pass_work_sheet.set_column('F:F', 10)
    pass_work_sheet.write('A1', 'Module', pass_header_format)
    pass_work_sheet.write('B1', 'Type', pass_header_format)
    pass_work_sheet.write('C1', 'Test Suite', pass_header_format)
    pass_work_sheet.write('D1', 'Test Case', pass_header_format)
    pass_work_sheet.write('E1', 'Result', pass_header_format)
    pass_work_sheet.write('F1', 'Time(ms)', pass_header_format)


def set_fail_work_sheet_header():
    fail_work_sheet.set_column('A:A', 10)
    fail_work_sheet.set_column('B:B', 10)
    fail_work_sheet.set_column('C:C', 40)
    fail_work_sheet.set_column('D:D', 50)
    fail_work_sheet.set_column('E:E', 12)
    fail_work_sheet.set_column('F:F', 10)
    fail_work_sheet.set_column('G:G', 50)
    fail_work_sheet.write('A1', 'Module', pass_header_format)
    fail_work_sheet.write('B1', 'Type', pass_header_format)
    fail_work_sheet.write('C1', 'Test Suite', pass_header_format)
    fail_work_sheet.write('D1', 'Test Case', pass_header_format)
    fail_work_sheet.write('E1', 'Result', pass_header_format)
    fail_work_sheet.write('F1', 'Time(ms)', pass_header_format)
    fail_work_sheet.write('G1', 'Message', pass_header_format)

def defect_work_sheet_header() :
    defect_work_sheet.set_column('A:A', 10)
    defect_work_sheet.set_column('B:B', 10)
    defect_work_sheet.set_column('C:C', 40)
    defect_work_sheet.set_column('D:D', 50)
    defect_work_sheet.set_column('E:E', 12)
    defect_work_sheet.set_column('F:F', 10)
    defect_work_sheet.set_column('G:G', 50)
    defect_work_sheet.set_column('H:H', 10)
    defect_work_sheet.set_column('I:I', 50)
    defect_work_sheet.write('A1', 'Module', pass_header_format)
    defect_work_sheet.write('B1', 'Type', pass_header_format)
    defect_work_sheet.write('C1', 'Test Suite', pass_header_format)
    defect_work_sheet.write('D1', 'Test Case', pass_header_format)
    defect_work_sheet.write('E1', 'Result', pass_header_format)
    defect_work_sheet.write('F1', 'Time(ms)', pass_header_format)
    defect_work_sheet.write('G1', 'Defect', pass_header_format)
    defect_work_sheet.write('H1', 'Jira ID', pass_header_format)
    defect_work_sheet.write('I1', 'Description', pass_header_format)

def add_worksheet_to_excel_workbook() :
    global summary_work_sheet
    global pass_work_sheet
    global fail_work_sheet
    global defect_work_sheet
    summary_work_sheet = module_workbook.add_worksheet("Summary")
    pass_work_sheet = module_workbook.add_worksheet("PassedTC")
    fail_work_sheet = module_workbook.add_worksheet("FailedTC")
    defect_work_sheet = module_workbook.add_worksheet("DefectReport")
    set_summary_work_sheet_header()
    set_pass_work_sheet_header()
    set_fail_work_sheet_header()
    defect_work_sheet_header()

def parse_xml_file(file_list, tc_type):

    global crash_list
    crash_list = []
    utc_crash_list = []
    result_dict = {}

    for filename in  file_list :
        temp_dict = {}
        if not os.path.getsize(filename) > 0 :
            crash_tc_name = filename.split('-',1)[0]
            crash_list.append(crash_tc_name)
            temp_dict.update({TEST_CASE_COLUMN_INDEX: crash_tc_name})
            temp_dict.update({TEST_RESULT_COLUMN_INDEX:'Fail(Crash)'})
            temp_dict.update({TEST_MESSAGE_COLUMN_INDEX:'Native Crash'})
            temp_dict.update({TEST_DURATION_COLUMN_INDEX:0})
            increment_tc_count(tc_type, 'Crash')
            increment_tc_count(tc_type, 'Fail')
            print crash_tc_name
            continue
        tree = ET.parse(filename)
        root = tree.getroot()
        testsuite = root.find('testsuite')

        if testsuite is None:
            continue

        testcase = testsuite.find('testcase')
        if testcase is None:
            continue

        testcase_class = testcase.get('classname').split('.')
        class_name = ""
        if MODULE_NAME == 'unittest' or MODULE_NAME == 'controller' or MODULE_NAME == 'controllee':
            class_name = testcase_class[0]
        else:
             class_name = testcase_class[6]
        testcase_name = testcase.get('name')
        test_duration = testcase.get('time')
        failure = testcase.find('failure')
        crash = testcase.find('error')
        temp_dict.update({TEST_CASE_COLUMN_INDEX: testcase_name})
        temp_dict.update({TEST_DURATION_COLUMN_INDEX: test_duration})

        if failure is None and crash is None :
            #inserting test name, result and test duration to temp list
            temp_dict.update({TEST_RESULT_COLUMN_INDEX:'Pass'})
            #add test suite as keys and object containing test case,result,duration as value to result dictionary
            increment_tc_count(tc_type, 'Pass')
        else :
            increment_tc_count(tc_type, 'Fail')
            if crash is not None :
                increment_tc_count(tc_type, 'Crash')
                crash_text = crash.text
                temp_dict.update({TEST_RESULT_COLUMN_INDEX:'Fail(Crash)'})
                temp_dict.update({TEST_MESSAGE_COLUMN_INDEX: crash_text})
            elif failure is not None :
                failure_text = testcase.find('failure').text
                temp_dict.update({TEST_RESULT_COLUMN_INDEX:'Fail'})
                temp_dict.update({TEST_MESSAGE_COLUMN_INDEX: failure_text})

            description_text= '[Device: ANDROID 21]\n\n[TC: %s]\n\n[Target: ]\n\n[Procedure]\n\n[Expected]\n\n[Observation]'% testcase_name
            temp_dict.update({TEST_DESCRIPTION_COLUMN_INDEX: description_text})

        result_dict.setdefault(class_name,[]).append(temp_dict)
    module_dict.setdefault(tc_type, []).append(result_dict)

def increment_tc_count(tc_type, result_type) :

    global utc_pass_count
    global itc_pass_count
    global utc_fail_count
    global itc_fail_count
    global total_utc_count
    global total_itc_count
    global total_count
    global utc_crash_count
    global itc_crash_count

    if tc_type == 'utc' :
        if result_type == 'Pass' :
            utc_pass_count += 1
        elif result_type == 'Fail' :
            utc_fail_count += 1
        else :
            utc_crash_count += 1
        total_utc_count +=1
    else :
        if result_type == 'Pass' :
            itc_pass_count += 1
        elif result_type == 'Fail' :
            itc_fail_count += 1
        else :
            itc_crash_count += 1
        total_itc_count +=1

    total_count += 1

def write_value_into_pass_worksheet(pass_work_sheet_row_index,  test_suite, test, common_format) :

    pass_work_sheet.write(pass_work_sheet_row_index, TEST_SUITE_COLUMN_INDEX, test_suite, test_suite_format)
    pass_work_sheet.write(pass_work_sheet_row_index, TEST_CASE_COLUMN_INDEX ,  test[TEST_CASE_COLUMN_INDEX], common_format)
    pass_work_sheet.write(pass_work_sheet_row_index, TEST_RESULT_COLUMN_INDEX, test[TEST_RESULT_COLUMN_INDEX], common_format)
    pass_work_sheet.write(pass_work_sheet_row_index, TEST_DURATION_COLUMN_INDEX, test[TEST_DURATION_COLUMN_INDEX], common_format)

def write_value_into_fail_worksheet(fail_work_sheet_row_index,  test_suite, test, common_format) :

    fail_work_sheet.write(fail_work_sheet_row_index, TEST_SUITE_COLUMN_INDEX, test_suite, test_suite_format)
    fail_work_sheet.write(fail_work_sheet_row_index, TEST_CASE_COLUMN_INDEX ,  test[TEST_CASE_COLUMN_INDEX], common_format)
    fail_work_sheet.write(fail_work_sheet_row_index, TEST_RESULT_COLUMN_INDEX,test[TEST_RESULT_COLUMN_INDEX], common_format)
    fail_work_sheet.write(fail_work_sheet_row_index, TEST_MESSAGE_COLUMN_INDEX,test[TEST_MESSAGE_COLUMN_INDEX], common_format)
    fail_work_sheet.write(fail_work_sheet_row_index, TEST_DURATION_COLUMN_INDEX,test[TEST_DURATION_COLUMN_INDEX],common_format)

def write_value_into_defect_worksheet(fail_work_sheet_row_index,  test_suite, test, common_format) :

    defect_work_sheet.write(fail_work_sheet_row_index, TEST_SUITE_COLUMN_INDEX, test_suite, test_suite_format)
    defect_work_sheet.write(fail_work_sheet_row_index, TEST_CASE_COLUMN_INDEX , test[TEST_CASE_COLUMN_INDEX], common_format)
    defect_work_sheet.write(fail_work_sheet_row_index, TEST_RESULT_COLUMN_INDEX, test[TEST_RESULT_COLUMN_INDEX], common_format)
    defect_work_sheet.write(fail_work_sheet_row_index, TEST_DURATION_COLUMN_INDEX, test[TEST_DURATION_COLUMN_INDEX],common_format)
    defect_work_sheet.write(fail_work_sheet_row_index, TEST_DEFECT_COLUMN_INDEX, '', common_format)
    defect_work_sheet.write(fail_work_sheet_row_index, TEST_JIRAID_COLUMN_INDEX, '', common_format)
    defect_work_sheet.write(fail_work_sheet_row_index,TEST_DESCRIPTION_COLUMN_INDEX,test[TEST_DESCRIPTION_COLUMN_INDEX],description_format)

def write_value_into_worksheet() :

    pass_work_sheet_row_index = 1
    fail_work_sheet_row_index = 1

    for test_type in module_dict :
        for test_suite_dict in module_dict[test_type] :
            for test_suite in test_suite_dict :
                merge_pass_row_suite_start_index = pass_work_sheet_row_index
                merge_fail_row_suite_start_index = fail_work_sheet_row_index
                total_tc_count_under_suite = len(test_suite_dict[test_suite])
                tc_count = 0
                for test in test_suite_dict[test_suite] :
                    tc_count +=1
                    #write into excel pass work sheet
                    if test[TEST_RESULT_COLUMN_INDEX] == 'Pass' :
                        if tc_count == total_tc_count_under_suite :
                             if total_tc_count_under_suite == 1 :
                                 write_value_into_pass_worksheet(pass_work_sheet_row_index, test_suite, test, common_bottom_format)
                             else :
                                 pass_work_sheet.merge_range(merge_pass_row_suite_start_index,2,pass_work_sheet_row_index,2,test_suite, test_suite_format)
                                 write_value_into_pass_worksheet(pass_work_sheet_row_index, test_suite, test, common_bottom_format)
                        else :
                            write_value_into_pass_worksheet(pass_work_sheet_row_index, test_suite, test, common_format)

                        pass_work_sheet_row_index += 1
                    #write into excel fail and defect work sheet
                    else :
                        if tc_count == total_tc_count_under_suite :
                            if total_tc_count_under_suite == 1 :
                                write_value_into_fail_worksheet(fail_work_sheet_row_index, test_suite, test, common_bottom_format)
                                write_value_into_defect_worksheet(fail_work_sheet_row_index, test_suite, test, common_bottom_format)
                            else :
                                fail_work_sheet.merge_range(merge_fail_row_suite_start_index,2,fail_work_sheet_row_index,2,test_suite, test_suite_format)
                                defect_work_sheet.merge_range(merge_fail_row_suite_start_index,2,fail_work_sheet_row_index,2,test_suite, test_suite_format)
                                write_value_into_fail_worksheet(fail_work_sheet_row_index, test_suite, test, common_bottom_format)
                                write_value_into_defect_worksheet(fail_work_sheet_row_index, test_suite, test, common_bottom_format)
                        else :
                            write_value_into_fail_worksheet(fail_work_sheet_row_index, test_suite, test, common_format)
                            write_value_into_defect_worksheet(fail_work_sheet_row_index, test_suite, test, common_format)

                        fail_work_sheet_row_index += 1

def merge_type_column() :

    if utc_pass_count > 1 :
        pass_work_sheet.merge_range(1, 1, utc_pass_count, 1, 'btc', merge_format)
    elif utc_pass_count == 1 :
        pass_work_sheet.write(1, 1, 'btc', merge_format)

    if utc_fail_count > 1:
        fail_work_sheet.merge_range(1, 1, utc_fail_count, 1, 'btc', merge_format)
        defect_work_sheet.merge_range(1, 1, utc_fail_count, 1, 'btc', merge_format)
    elif utc_fail_count == 1 :
        fail_work_sheet.write(1, 1, 'btc', merge_format)
        defect_work_sheet.write(1, 1,'btc', merge_format)

    if itc_pass_count > 1 :
        pass_work_sheet.merge_range(utc_pass_count+1, 1, total_pass_count, 1, 'stc', merge_format)
    elif itc_pass_count == 1 :
        pass_work_sheet.write(1, 1, 'stc', merge_format)

    if itc_fail_count > 1:
        fail_work_sheet.merge_range(utc_fail_count+1, 1, total_fail_count, 1, 'stc', merge_format)
        defect_work_sheet.merge_range(utc_fail_count+1, 1, total_fail_count, 1, 'stc', merge_format)
    elif itc_fail_count == 1 :
        fail_work_sheet.write(utc_fail_count+1, 1, 'stc', merge_format)
        defect_work_sheet.write(utc_fail_count+1, 1,'stc', merge_format)

def merge_module_column() :

    if total_pass_count > 1 :
        pass_work_sheet.merge_range(1, 0, total_pass_count , 0, MODULE_NAME, merge_format)
    elif total_pass_count == 1 :
        pass_work_sheet.write(1, 0, MODULE_NAME,  merge_format)

    if total_fail_count > 1 :
        fail_work_sheet.merge_range(1, 0, total_fail_count , 0, MODULE_NAME, merge_format)
        defect_work_sheet.merge_range(1, 0, total_fail_count , 0, MODULE_NAME, merge_format)
    elif total_fail_count == 1 :
        fail_work_sheet.write(1, 0, MODULE_NAME, merge_format)
        defect_work_sheet.write(1, 0, MODULE_NAME, merge_format)

def write_value_into_summary_worksheet():

    summary_work_sheet.merge_range(1, 0, 2, 0, MODULE_NAME, merge_format)
    summary_work_sheet.write(1 , 1 , 'btc', testcase_format)
    summary_work_sheet.write(1 , 2 , utc_pass_count, testcase_format)
    summary_work_sheet.write(1 , 3 , utc_fail_count, testcase_format)
    summary_work_sheet.write(1 , 4 , total_utc_count-utc_crash_count, testcase_format)
    summary_work_sheet.write(2 , 1 , 'stc', testcase_format)
    summary_work_sheet.write(2 , 2 , itc_pass_count, testcase_format)
    summary_work_sheet.write(2 , 3 , itc_fail_count, testcase_format)
    summary_work_sheet.write(2 , 4 , total_itc_count-itc_crash_count, testcase_format)
    summary_work_sheet.merge_range(3, 0, 3, 1, 'Total', merge_format)
    summary_work_sheet.write(3 , 2, utc_pass_count + itc_pass_count, merge_format)
    summary_work_sheet.write(3 , 3, utc_fail_count + itc_fail_count, merge_format)
    summary_work_sheet.write(3 , 4, total_count-total_crash_count, merge_format)

def close_module_workbook() :

    module_workbook.close()

def create_excel_report() :

    global total_pass_count
    global total_fail_count
    global total_crash_count

    utc_test_result_files_list = glob.glob(UTC_FILE_PATH+"/*.xml")
    itc_test_result_files_list = glob.glob(ITC_FILE_PATH+"/*.xml")

    if len(utc_test_result_files_list) > 0 :
        parse_xml_file(utc_test_result_files_list, "utc")
    if len(itc_test_result_files_list) > 0 :
        parse_xml_file(itc_test_result_files_list, "itc")

    total_pass_count = utc_pass_count + itc_pass_count
    total_fail_count = utc_fail_count + itc_fail_count
    total_crash_count = utc_crash_count + itc_crash_count
    open_excel_workbook()
    write_value_into_worksheet()
    write_value_into_summary_worksheet()
    merge_module_column()
    merge_type_column()
    close_module_workbook()

def print_summary():

    print "\n===========================================================\n"
    print("\nTEST RESULT SUMMARY\n")
    print("\n MODULE : %s\n"%(str(MODULE_NAME.upper())))
    print("\n BTC SUMMARY\n")
    print("    PASS : " +str(utc_pass_count)+"\n")
    print("    FAIL : " +str(utc_fail_count)+"\n")
    print("    CRASH: " +str(utc_crash_count)+"\n")
    print("    TOTAL: " +str(total_utc_count-utc_crash_count)+"\n")
    print("\n STC SUMMARY\n")
    print("    PASS : " +str(itc_pass_count)+"\n")
    print("    FAIL : " +str(itc_fail_count)+"\n")
    print("    CRASH: "+str(itc_crash_count)+"\n")
    print("    TOTAL: "+str(total_itc_count-itc_crash_count)+"\n")
    print("\n TOTAL SUMMARY\n")
    print("   PASS : "+str(total_pass_count)+"\n")
    print("   FAIL : "+str(total_fail_count)+"\n")
    print("   CRASH: "+str(utc_crash_count + itc_crash_count)+"\n")
    print("   TOTAL: "+str(total_count-total_crash_count)+"\n")
    print "\n===========================================================\n"

def print_crash_list() :

    if len(crash_list) > 0 :
        print("Crash Tcs List : ")
        for file_name in crash_list:
            print(file_name)
        print('\n')

if __name__ == '__main__':
    create_excel_report()
    print_summary()

