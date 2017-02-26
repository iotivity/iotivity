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
#import ntpath
from os.path import basename
from iotivitybuildtool import *

try :
    import xlsxwriter
except :
    print("Install xlsxwriter by  sudo easy_install xlsxwriter or by another way")


oparser = optparse.OptionParser()
oparser.add_option("-t", action="store", dest="target_source")
oparser.add_option("--target", action="store", dest="target_source")
oparser.add_option("-b", action="store", dest="bit")
oparser.add_option("--bit", action="store", dest="bit")
oparser.add_option("-m", action="store", dest="mode")
oparser.add_option("--mode", action="store", dest="mode")


#setting default value for command line arguments
oparser.set_defaults(target_source = 'target', bit='64', mode= 'release')
opts, args = oparser.parse_args()

#Assigning command-line options value to global variables
TARGET_SOURCE = opts.target_source
BUILD_MODE = opts.mode
BIT = opts.bit

#checking user input is 32 or 64
if BIT == "32" :
    BIT = "x86"
else :
    BIT = "x86_64"
print ("Reporter Target : "+ TARGET_SOURCE)
print ("Reporter Bit : "+ BIT)
print ("Reporter Build Mode : "+ BUILD_MODE)

#set dev test result file directory
DEV_UNIT_TEST_RESULT_DIRECTORY = "../IotivityOrgSource/%s/iotivity/out/linux/%s/%s/test_out/"% (TARGET_SOURCE, BIT, BUILD_MODE)

#constant varibales for execel column management
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

#global count variables
total_count = 0
fail_count = 0
not_run_count = 0
pass_count = 0
error_count = 0

#global variable for
linux_work_sheet_row_index = 1

#list to keep catagory(binary) wise not run
not_run_binary_dict = {}

module = {
'resource':["unittests"],
'resource/c_common':["malloctests", "randomtests", "stringtests", "timetests"],
'resource/csdk/connectivity': ["catests"],
'resource/csdk/security':["unittest"],
'resource/csdk/security/provisioning':["security_unittest"],
'resource/csdk/security/provisioning/ck_manager' :["unittest_1"],
'resource/csdk/stack':["stacktests"],
'resource/provisioning': ["provisiontests"],
'service/scene-manager': ["remote_scene_action_test","remote_scene_col_test","remote_scene_list_test","remote_scene_test", "scene_action_test","scene_collection_test", "scene_list_test", "scene_test"],
'service/easy-setup/enrollee':["easysetup_enrollee_test"],
'service/easy-setup/mediator/richsdk':["easysetup_mediator_test"],
'service/resource-container':["container_test"],
'service/notification':["notification_consumer_test", "notification_provider_test"],
'service/resource-encapsulation':["broker_test", "cache_test", "rcs_client_test", "rcs_server_test", "rcs_common_test"]
}

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
    MODULE_NAME ="testtsts"
    module_workbook = xlsxwriter.Workbook(DEV_UNIT_TEST_RESULT_DIRECTORY +'TestResult_UnitTest_%s_W%s.xlsx' % (yy_mm_dd, str(wk_no)))
    set_excel_style()
    add_worksheet_to_excel_workbook()

def set_summary_work_sheet_header():

    #set column
    summary_work_sheet.set_column('A:A', 10)
    summary_work_sheet.set_column('B:B', 40)
    summary_work_sheet.set_column('C:C', 10)
    summary_work_sheet.set_column('D:D', 10)
    summary_work_sheet.set_column('E:E', 10)
    summary_work_sheet.set_column('F:F', 10)
    summary_work_sheet.set_column('G:G', 10)

    #write header value in cloulmns
    summary_work_sheet.write('A1', 'Platform', pass_header_format)
    summary_work_sheet.write('B1', 'Module', pass_header_format)
    summary_work_sheet.write('C1', 'Passed', pass_header_format)
    summary_work_sheet.write('D1', 'Failed', pass_header_format)
    summary_work_sheet.write('E1', 'Error', pass_header_format)
    summary_work_sheet.write('F1', 'Not Run', pass_header_format)
    summary_work_sheet.write('G1', 'Total', pass_header_format)

def set_pass_work_sheet_header():
    pass_work_sheet.set_column('A:A', 30)
    pass_work_sheet.set_column('B:B', 30)
    pass_work_sheet.set_column('C:C', 30)
    pass_work_sheet.set_column('D:D', 50)
    pass_work_sheet.set_column('E:E', 12)
    pass_work_sheet.set_column('F:F', 10)
    pass_work_sheet.set_column('G:G', 60)
    pass_work_sheet.write('A1', 'Module', pass_header_format)
    pass_work_sheet.write('B1', 'Catagory', pass_header_format)
    pass_work_sheet.write('C1', 'Test Suite', pass_header_format)
    pass_work_sheet.write('D1', 'Test Case', pass_header_format)
    pass_work_sheet.write('E1', 'Result', pass_header_format)
    pass_work_sheet.write('F1', 'Time(ms)', pass_header_format)
    pass_work_sheet.write('G1', 'Reason', pass_header_format)
    #pass_work_sheet.autofilter('A1:D11')


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
    pass_work_sheet = module_workbook.add_worksheet("Linux")
    fail_work_sheet = module_workbook.add_worksheet("Java")
    defect_work_sheet = module_workbook.add_worksheet("DefectReport")
    set_summary_work_sheet_header()
    set_pass_work_sheet_header()
    set_fail_work_sheet_header()
    defect_work_sheet_header()

summary_work_sheet_row_index = 1

def parse_xml_file(file_list, module_name):

    global summary_work_sheet_row_index
    global linux_work_sheet_row_index
    global total_count
    global fail_count
    global not_run_count
    global pass_count
    global error_count

    module_wise_total_count = 0
    module_wise_fail_count = 0
    module_wise_not_run_count = 0
    module_wise_pass_count = 0
    module_wise_error_count = 0

    summary_module_row_start_index = summary_work_sheet_row_index
    module_name_column_start_index = linux_work_sheet_row_index
    missing_binary_list = []
    type_wise_total_count = 0
    type_wise_pass_count = 0
    type_wise_fail_count = 0
    type_wise_not_run_count = 0
    type_wise_error_count = 0

    for file_name in  file_list :


        full_file_name = DEV_UNIT_TEST_RESULT_DIRECTORY+file_name+".xml"
        print("File Name :"+ file_name)
        temp_dict = {}
        pass_work_sheet_module_row_start_index = linux_work_sheet_row_index
        #file_base_name = basename(filename).split(".")[0]
        pass_work_sheet.write(linux_work_sheet_row_index, TEST_MODULE_COLUMN_INDEX, file_name, test_suite_format)
        if not os.path.isfile(full_file_name) :
            missing_binary_list.append(file_name)
            continue
        if not os.path.getsize(full_file_name) > 0 :
            continue
        tree = ET.parse(full_file_name)
        #Getting root that is testsuites
        root = tree.getroot()
        test_count = root.get('tests')
        fail_cnt = root.get('failures')
        not_run_cnt = root.get('disabled')
        error_cnt = root.get('errors')
        pass_cnt = int(test_count) - int(fail_cnt)-int(not_run_cnt)-int(error_cnt )

        #module wise count operation
        type_wise_total_count += int(test_count)
        type_wise_pass_count += pass_cnt
        type_wise_fail_count += int(fail_cnt)
        type_wise_error_count += int(error_cnt)
        type_wise_not_run_count += int(not_run_cnt)


        print("Total Test Cases count : "+test_count)

        print("Total TEst Cases count : "+test_count)
        #Find all tags named <testsuite> and store to testsuites list
        testsuites = root.findall('testsuite')

        #Iterating all test suite from testsuites list
        for test_suite in testsuites :
            merge_pass_row_suite_start_index = linux_work_sheet_row_index
            test_suite_name = test_suite.get('name')
            print ("Suite Name : "+test_suite_name+"\n")
            pass_work_sheet.write(linux_work_sheet_row_index, TEST_SUITE_COLUMN_INDEX, test_suite_name, test_suite_format)
            testcases = test_suite.findall('testcase')
            failure_message = "N/A"
            for test_case in testcases :
                test_result = "Pass"
                failure_message = "N/A"
                test_name = test_case.get('name')
                test_status = test_case.get('status')
                test_time = test_case.get('time')
                print("Test Name : " +test_name)
                if test_status == "notrun" :
                    test_result = "Not Run"
                    failure_message = "Disable by code"
                fail = test_case.find('failure')
                error = test_case.find('error')
                if error is not None :
                    test_result = "Error"
                    failure_message = error.get('message')
                if fail is not None :
                    test_result = "Fail"
                    failure_message = fail.get('message')
                    print("Result : Fail "+ failure_message )
                    print("Result : "+test_result +" Message : "+failure_message+" Time : "+test_time)
                else :
                    print("Result : "+test_result+ " Time : "+test_time)
               #writing value into pass worksheet
                pass_work_sheet.write(linux_work_sheet_row_index, TEST_CASE_COLUMN_INDEX, test_name, common_format)
                pass_work_sheet.write(linux_work_sheet_row_index, TEST_RESULT_COLUMN_INDEX, test_result, common_format)
                pass_work_sheet.write(linux_work_sheet_row_index, TEST_DURATION_COLUMN_INDEX, test_time, common_format)
                pass_work_sheet.write(linux_work_sheet_row_index, TEST_MESSAGE_COLUMN_INDEX, failure_message, common_format)
                linux_work_sheet_row_index += 1

            #suite wise merging in pass worksheet
            pass_work_sheet.merge_range(merge_pass_row_suite_start_index,2,linux_work_sheet_row_index - 1,2,test_suite_name,test_suite_format)
            print ("\n")

        #Catagory wise merging in pass worksheet
        pass_work_sheet.merge_range(pass_work_sheet_module_row_start_index,1,linux_work_sheet_row_index - 1,1,file_name,test_suite_format)
    #Module wise merging in pass worksheet
    pass_work_sheet.merge_range(module_name_column_start_index, 0,linux_work_sheet_row_index - 1,0, module_name ,test_suite_format)

    #adding module wise missing binary file
    not_run_binary_dict.update({module_name : missing_binary_list})

    #module wise  total count
    module_wise_total_count += type_wise_total_count
    module_wise_fail_count += type_wise_fail_count
    module_wise_not_run_count+= type_wise_not_run_count
    module_wise_pass_count += type_wise_pass_count
    module_wise_error_count += type_wise_error_count

    #grand total count
    total_count += module_wise_total_count
    fail_count += module_wise_fail_count
    not_run_count += module_wise_not_run_count
    pass_count += module_wise_pass_count
    error_count += module_wise_error_count


    #write catagory wise count in excel
    summary_work_sheet.write(summary_work_sheet_row_index, 1, module_name, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 2, module_wise_pass_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 3, module_wise_fail_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 4, module_wise_error_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 5, module_wise_not_run_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 6, module_wise_total_count , common_format)
    summary_work_sheet_row_index += 1

#merging summary worksheet Platform column


def print_missing_binary_list_type_wise() :
    print("===============================Not Run Binary==================================")
    for module_name in not_run_binary_dict :
        if len(not_run_binary_dict [module_name]) is not 0 :
            print("Module : "+ module_name)
            for binary_name in not_run_binary_dict [module_name] :
                print(" "+binary_name+" ")
            print("\n")
    print("================================================================================")

def close_module_workbook() :
    module_workbook.close()

def create_excel_report() :
    dev_test_result_files_list = []
    open_excel_workbook()
    for module_name in module :
        #print(module_name)
        dev_test_result_files_list = module[module_name]
        print (module[module_name])
        if len(dev_test_result_files_list) > 0 :
            parse_xml_file(dev_test_result_files_list, module_name)

    summary_work_sheet.write(summary_work_sheet_row_index, 1, "All", common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 2, pass_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 3, fail_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 4, error_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 5, not_run_count, common_format)
    summary_work_sheet.write(summary_work_sheet_row_index, 6, total_count , common_format)

    summary_work_sheet.merge_range(1, 0,summary_work_sheet_row_index, 0 ,"Linux", common_format)
    close_module_workbook()

def find_and_copy_file() :
    directory = DEV_UNIT_TEST_RESULT_DIRECTORY+":xml:"
    print("Directory :"+directory)
    print(Utility.is_directory_exist(directory))
    if (Utility.is_directory_exist(directory)) :
        for root, dirs ,files in os.walk(directory) :
            for file_name in files :
                if file_name.endswith('.xml') :
                    print (os.path.abspath(file_name))
                    print(file_name)
                    file_full_path = os.path.join(root, file_name)
                    directory_path = os.path.dirname(file_full_path)
                    Utility.copy_file(directory_path+"/unittest.xml", DEV_UNIT_TEST_RESULT_DIRECTORY+"security_unittest.xml")


def print_summary():
    print "\n========================Dev Team Unit TCs===================================\n"
    print("\nTEST RESULT SUMMARY \n")
    print("\n PLATFORM : %s\n"%("LINUX"))
    print("\n MODULE : %s\n"%("ALL"))
    print("     PASS    : " +str(pass_count)+"\n")
    print("     FAIL    : " +str(fail_count)+"\n")
    print("     ERROR   : " +str(error_count)+"\n")
    print("     NOT RUN : " +str(not_run_count)+"\n")
    print("     TOTAL   : " +str(total_count)+"\n")
    print "\n==============================================================================\n"



if __name__ == '__main__':
    find_and_copy_file()
    create_excel_report()
    print_summary()
    print_missing_binary_list_type_wise()


