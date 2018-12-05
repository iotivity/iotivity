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
import xml.etree.cElementTree as ET
import time
import platform
import sys
import fnmatch
import subprocess
import optparse
from datetime import datetime
from time import strftime
from time import sleep
import re
import collections
import optparse

from configuration import *
from ite.tc.container import TestSpecContainer
from ite.reporter.tc_reporter import TestSpecReporter
from xml.etree.ElementTree import ElementTree, ParseError

from ite.tc.analyzer import TestSpec
from ite.constants import *
from ite.config import *

try:
    import xlrd
except:
    print ('install xlrd. Possible way to install: sudo pip3 install xlrd')
    exit (0)

current = datetime.now().strftime("%Y-%m-%d_%H:%M")
test_root = '..'

oparser = optparse.OptionParser()
oparser.add_option("-p", action="store", dest="platform", default="linux")
oparser.add_option("--platform", action="store", dest="platform", default="linux")
oparser.add_option("-b", action="store", dest="branch", default="unspecified")
oparser.add_option("--branch", action="store", dest="branch", default="unspecified")
oparser.add_option("-r", action="store", dest="iotivity_root", default="../../")
oparser.add_option("--iotivity_root", action="store", dest="iotivity_root", default="../../")
oparser.add_option("-t", action="store", dest="timestamp", default=current)
oparser.add_option("--timestamp", action="store", dest="timestamp", default=current)
oparser.add_option("-d", action="store", dest="duration", default="")
oparser.add_option("--duration", action="store", dest="duration", default="")
oparser.add_option("--store_log", action="store_true", dest="store_log")
oparser.add_option("--store_path", action="store", dest="store_path", default="../../../../result/")
oparser.add_option("--not_publish", action="store_true", dest="not_publish")
oparser.add_option("--build_number", action="store", dest="build_number")
oparser.add_option("--artifacts_link", action="store", dest="artifacts_link")
opts, args = oparser.parse_args()

platform_name = opts.platform
iotivity_branch = opts.branch
iotivity_root = os.path.abspath(opts.iotivity_root)
total_duration = opts.duration
timestamp = opts.timestamp
store_log = opts.store_log
not_publish = opts.not_publish
store_path = os.path.abspath(opts.store_path)
build_number = opts.build_number
artifacts_link = opts.artifacts_link

print ('store_log: ', store_log)

cwd = os.getcwd()

os.chdir(iotivity_root)
rc = subprocess.check_output('git rev-parse HEAD', shell=True)
log = re.sub(r'(b\'|\')', '', str(rc))
log = log.strip()
log = log.replace('\\n', '')
iotivity_commit_id = log
os.chdir(cwd)

rc = subprocess.check_output('git rev-parse HEAD', shell=True)
log = re.sub(r'(b\'|\')', '', str(rc))
log = log.strip()
log = log.replace('\\n', '')
test_commit_id = log

final_xml_filename = os.path.join(TEST_REPORT_DIR, 'xml_report.xml')
result_dir = TEST_RESULT_DIR
modules = TESTSUITE_MODULES
pass_count_id = 'pass'
fail_count_id = 'fail'
total_count_id = 'total'
na_count_id = 'na'
build_iotivity = 'build_iotivity'
build_test = 'build_test'
test_pre_condition = 'test_pre_condition'
system_failure = 'system_failure'

tc_result_count = {}

for module in modules:
    tc_result_count[module] = {}
    tc_result_count[module][pass_count_id] = 0
    tc_result_count[module][fail_count_id] = 0
    tc_result_count[module][total_count_id] = 0
    tc_result_count[module][na_count_id] = 0

def read_spec_xml(path, searching_platform):

    try :
        doc = ElementTree(file=path)  
        testspec = doc.find(SPEC_ELM.SUMMARY)
        for platform in testspec.findall(SPEC_ELM.PLATFORM):
            if platform.get(SPEC_ATT.NAME) != searching_platform:
                continue
            for build_type in platform.findall(SPEC_ELM.BUILDTYPE):
                for tctype in build_type.findall(SPEC_ELM.TYPE):
                    for module in tctype.findall(SPEC_ELM.MODULE):
                        module_name = module.get(SPEC_ATT.NAME)
                        tc_result_count[module_name][total_count_id] += int(module.get(SPEC_ATT.NO))

    except ParseError:
        print("There is a Parse Error on " + path)

if not os.path.exists(result_dir):
    os.makedirs(result_dir)

testspec_path = os.path.join(result_dir, TEST_SPEC_XML_FOR_RESULT)
if not os.path.exists(testspec_path) and os.path.exists(API_TC_SRC_DIR):
    container = TestSpecContainer()
    container.extract_api_testspec(API_TC_SRC_DIR, '')

    reporter = TestSpecReporter()
    reporter.generate_testspec_report(container.data)
    reporter.report('XML', testspec_path)

read_spec_xml(result_dir + os.sep + 'TestSpec.xml', platform_name.upper())

print ('result_dir: ' + result_dir)

os.system('rm -rf ' + result_dir + os.sep + 'xml_report.xml')
os.system('rm -rf ' + result_dir + os.sep + '__merge_report__')
os.system('rm -rf ' + result_dir + os.sep + 'MERGE_REPORT*')
os.system('mkdir ' + result_dir + os.sep + '__merge_report__')
os.system('cp -u ' + result_dir + os.sep + '*_TestResult_*' + os.sep + platform_name + '_*.xml ' + result_dir + os.sep + '__merge_report__' + os.sep)
os.system('cp -u ' + result_dir + os.sep + '*_TestResult_*' + os.sep + platform_name + '_*.log ' + result_dir + os.sep + '__merge_report__' + os.sep)
os.system('cp -u ' + result_dir + os.sep + '__*_*__' + os.sep + platform_name + '_*.xml ' + result_dir + os.sep + '__merge_report__' + os.sep)
os.system('./result_reporter.py ' + result_dir + os.sep + '__merge_report__ merge_report')

list_of_files = []
for path, subdirs, files in os.walk(result_dir):
    for fname in files:
        if path.startswith(result_dir + os.sep + 'MERGE_REPORT_') and fname.startswith('TestResult_') and fname.endswith('.xlsx'):
            print (path)
            list_of_files.append(os.path.join(path, fname))

sep = ':'
with open(TEST_REPORT_DIR + os.sep + 'test_pre_info.txt') as f:
    lines = f.readlines()
    for line in lines:
        line = line.lower()
        if line.startswith(na_count_id + sep):
            infos = line.split(sep)
            if len(infos) > 1:
                module = infos[1].upper()
                if len(infos) > 2 and platform_name in infos[2]:
                    if len(infos) > 3 and infos[3]:
                        tc_result_count[module][na_count_id] = int(infos[3])
                    else:
                        tc_result_count[module][na_count_id] = tc_result_count[module][total_count_id]

for fname in list_of_files:

    worksheet = xlrd.open_workbook(fname).sheet_by_index(0)

    num_rows = worksheet.nrows - 1

    curr_row = -1
    while curr_row < num_rows:
        curr_row += 1
        row = worksheet.row(curr_row)
        c0 = worksheet.cell_value(curr_row, 0)
      
        for module in modules:
            if c0.upper() == module.upper():
                try:
                    tc_result_count[c0][pass_count_id] += int(worksheet.cell_value(curr_row, 1)) + int(worksheet.cell_value(curr_row, 4))
                    tc_result_count[c0][fail_count_id] += int(worksheet.cell_value(curr_row, 2)) + int(worksheet.cell_value(curr_row, 5))
                except:
                    print ('Problem with Parsing module ' + module)

total_tc = 0
total_pass = 0
total_fail = 0
total_na = 0
total_ne = 0
pass_rate = 100.00

status_set = set()

robot_execution_status_file = '../src/automation/robot/report/robot_execution_status.txt'
if os.path.exists(robot_execution_status_file):
    with open(robot_execution_status_file) as f:
        lines = f.readlines()
        print ('reading robot_execution_status.txt')
        for line in lines:
            line = line.lower().strip()
            status_set.add(line)

for line in status_set:
    if line.startswith(fail_count_id + sep):
        infos = line.split(sep)
        if len(infos) > 1:
            module = infos[1].upper()
            if len(infos) > 2 and platform_name in infos[2]:
                if len(infos) > 3 and infos[3]:
                    print (infos[3])
                    if infos[3] == build_iotivity or infos[3] == test_pre_condition:
                        cnt = tc_result_count[module][total_count_id] - tc_result_count[module][pass_count_id] - tc_result_count[module][fail_count_id] - tc_result_count[module][na_count_id]
                        tc_result_count[module][fail_count_id] += cnt

for module in modules:
    total_tc += tc_result_count[module][total_count_id]
    total_pass += tc_result_count[module][pass_count_id]
    total_fail += tc_result_count[module][fail_count_id]
    total_na += tc_result_count[module][na_count_id]
    total_ne += tc_result_count[module][total_count_id] - tc_result_count[module][pass_count_id] - tc_result_count[module][fail_count_id] - tc_result_count[module][na_count_id]

pass_rate = (total_pass / (total_tc - total_na)) * 100

report = ET.Element("report")

report.append(ET.ProcessingInstruction('xml-stylesheet', 'type="text/xsl" href="style-report.css"'))

label = ET.SubElement(report, "label")
label.text = 'Target Platform: ' + platform_name

if build_number:
    label = ET.SubElement(report, "label")
    label.text = 'Build Number: ' + build_number

if artifacts_link:
    label = ET.SubElement(report, "inline_label")
    label.text = 'Build Artifacts Link:'
    label = ET.SubElement(report, "a")
    label.text = artifacts_link

if total_duration:
    label = ET.SubElement(report, "label")
    label.text = 'Total duration: ' + total_duration

label = ET.SubElement(report, "label")

if iotivity_commit_id != iotivity_branch:
    label.text = 'Iotivity Branch/Tag: ' + iotivity_branch + ' , Commit Id: ' + iotivity_commit_id
else:
    label.text = 'Iotivity Commit Id: ' + iotivity_commit_id

label = ET.SubElement(report, "label")
label.text = 'Test Commit Id: ' + test_commit_id

summary = ET.SubElement(report, "summary")

ET.SubElement(summary, "data-caption").text = 'Summary'
summary_row = ET.SubElement(summary, 'summary-row')
data_row = ET.SubElement(summary, 'data-row')

ET.SubElement(summary_row, 'data-header').text = 'Total TCs'
ET.SubElement(summary_row, 'data-header').text = 'Pass'
ET.SubElement(summary_row, 'data-header').text = 'Fail'
ET.SubElement(summary_row, 'data-header').text = 'NA'
ET.SubElement(summary_row, 'data-header').text = 'NE'
ET.SubElement(summary_row, 'data-header').text = 'Pass Rate'

ET.SubElement(data_row, 'data-cell').text = str(total_tc)
ET.SubElement(data_row, 'data-cell').text = str(total_pass)
ET.SubElement(data_row, 'data-cell').text = str(total_fail)
ET.SubElement(data_row, 'data-cell').text = str(total_na)
ET.SubElement(data_row, 'data-cell').text = str(total_ne)
ET.SubElement(data_row, 'data-cell').text = "{0:.2f}".format(pass_rate)

data = ET.SubElement(report, "data")

ET.SubElement(data, 'data-caption').text = 'Result Details'
data_row1 = ET.SubElement(data, 'data-row')

ET.SubElement(data_row1, 'data-header').text = 'Modules'
ET.SubElement(data_row1, 'data-header').text = 'TCs'
ET.SubElement(data_row1, 'data-header').text = 'Pass'
ET.SubElement(data_row1, 'data-header').text = 'Fail'
ET.SubElement(data_row1, 'data-header').text = 'NA'
ET.SubElement(data_row1, 'data-header').text = 'NE'
ET.SubElement(data_row1, 'data-header').text = 'Pass Rate'

for module in modules:
    if tc_result_count[module][total_count_id] == 0:
        continue
    data_row2 = ET.SubElement(data, 'data-row')
    ET.SubElement(data_row2, 'data-cell').text = module
    ET.SubElement(data_row2, 'data-cell').text = str(tc_result_count[module][total_count_id])
    ET.SubElement(data_row2, 'data-cell').text = str(tc_result_count[module][pass_count_id])
    ET.SubElement(data_row2, 'data-cell').text = str(tc_result_count[module][fail_count_id])
    ET.SubElement(data_row2, 'data-cell').text = str(tc_result_count[module][na_count_id])
    total_ne = tc_result_count[module][total_count_id] - tc_result_count[module][pass_count_id] - tc_result_count[module][fail_count_id] - tc_result_count[module][na_count_id]
    if total_ne < 0:
        total_ne = 0
    ET.SubElement(data_row2, 'data-cell').text = str(total_ne)

    if (tc_result_count[module][pass_count_id] - tc_result_count[module][fail_count_id]) > (tc_result_count[module][total_count_id] - tc_result_count[module][na_count_id]):
        ET.SubElement(data_row2, 'data-cell').text = "Error!!!"
        continue

    if (tc_result_count[module][total_count_id] - tc_result_count[module][na_count_id]) > 0:
        pass_rate = (tc_result_count[module][pass_count_id] / (tc_result_count[module][total_count_id] - tc_result_count[module][na_count_id]))*100
    else:
        pass_rate = 100

    ET.SubElement(data_row2, 'data-cell').text = "{0:.2f}".format(pass_rate)

tree = ET.ElementTree(report)
tree.write(final_xml_filename)

prefix = '<?xml version="1.0" encoding="UTF-8" standalone="no"?><?xml-stylesheet href="style-report.css"?>'

with open(final_xml_filename, 'r+') as f:
        content = f.read()
        f.seek(0, 0)
        f.write(prefix + '\n' + content)

if store_log:

    log_folder = 'log_' + platform_name
    if 'linux' in platform_name.lower():
        log_folder += '_' + platform.architecture()[0]

    log_path = os.path.join(store_path, log_folder)
    robot_log_path = os.path.join(log_path, 'robot_fw_log')
    execution_result_path = os.path.join(log_path, 'execution_result')
    bin_path = os.path.join(log_path, 'bin')
    defect_path = os.path.join(log_path, 'defect')

    print ('store_path: ', store_path)
    print ('log_path: ', log_path)

    os.system('rm -rf ' + store_path + '*')
    os.system('mkdir -p ' + log_path)
    os.system('mkdir -p ' + bin_path)
    os.system('mkdir -p ' + robot_log_path)

    os.system('cp ' + TEST_REPORT_DIR + os.sep + 'test_pre_info.txt' + ' ' + log_path)
    os.system('cp ' + TEST_REPORT_DIR + os.sep + 'style-report.css' + ' ' + store_path)
    os.system('cp ' + TEST_REPORT_DIR + os.sep + 'xml_report.xml' + ' ' + store_path + os.sep + 'report.xml')

    os.system('cp -r ' + result_dir + ' ' + execution_result_path)

    temp_path = os.path.join(test_root, 'src', 'automation', 'robot', 'report')
    os.system('cp ' + temp_path + os.sep + '*.txt ' + log_path)

    robot_report_platform_path = os.path.join(test_root, 'src', 'automation', 'robot', 'report', 'api', platform_name)
    os.system('cp -r ' + robot_report_platform_path + os.sep + '* ' + robot_log_path)

    os.system('mv ' + robot_log_path + os.sep + 'bin_* ' + bin_path)

    os.system('cp ' + execution_result_path + os.sep + 'MERGE_REPORT_TestResult_*' + os.sep + '*.xlsx ' + log_path)

    for module in modules:
        os.system('mkdir -p ' + defect_path + os.sep + module)
        fail_tc_path = execution_result_path
        fail_tc_path += os.sep + 'MERGE_REPORT_TestResult_*'
        fail_tc_path += os.sep + 'Failed'

        if 'ca' == module.lower():
            os.system('cp -r ' + fail_tc_path + os.sep + '*' + os.sep + module.upper() + '* '+ defect_path + os.sep + module)
        else:
            command = 'cp ' + fail_tc_path + os.sep + platform_name.upper() + '_*__' + os.sep + module.upper() + '*.txt ' + defect_path + os.sep + module
            os.system(command)

        file_path_list = []
        search_text_list = []
        if os.path.exists(robot_report_platform_path):
            for each_file in os.listdir(robot_report_platform_path):
                if os.path.isfile(os.path.join(robot_report_platform_path, each_file)):
                    if each_file.endswith('.txt'):
                        if each_file.startswith('pre_condition_' + module.lower() + '_'):
                            file_path_list.append(os.path.join(robot_report_platform_path, each_file))
                            search_text_list.append('Execution Status: pass')
                        elif each_file.startswith('build_iotivity_' + module.lower()):
                            file_path_list.append(os.path.join(robot_report_platform_path, each_file))
                            search_text_list.append('Build Status: pass')

        for i in range(len(file_path_list)):
            with open(file_path_list[i]) as fp:
                status_pass = False
                for line in fp:
                    if search_text_list[i].lower() == line.lower().strip():
                        status_pass = True
                        break
                if not status_pass:
                    os.system('cp ' + file_path_list[i] + ' ' + defect_path + os.sep + module)

    os.system('find ' + defect_path + ' -type d -empty -delete')

    cwd = os.getcwd()
    os.chdir(store_path)
    os.system('zip -rq ' + 'log.zip ' + log_folder)
    os.chdir(cwd)

    #os.system('rm -rf ' + log_path)

    print ('done storing log')
