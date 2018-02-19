import re
import os
import io
import sys
import time
from time import strftime
from datetime import datetime
import subprocess
import traceback
import threading

from ite.constants import *
from ite.util import *
from ite.multi_logger import print_runner_output, show_exeception, print_to_file
from ite.exec.run_options import TestRunOption
from ite.exec.tc_utility_action import TcUtilityAction

class TcExecutor:

    def __init__(self, timeout_seconds, test_result_dir):
        self.timeout_seconds = timeout_seconds
        self.test_result_dir = test_result_dir

    def print_tc_list(self, file_path, dynamic_runner):
        fp = open(file_path, "w")
        list_of_testcase = dynamic_runner.get_tc_list()
        for testcase in list_of_testcase:
            command = 'binary_name:{}, suite_name:{}, tc_name:{}, package_name:{}'.format(testcase.binary_name, testcase.suite_name, testcase.tc_name, testcase.package_name)
            fp.write (command + '\n')
        fp.close()
        print_runner_output('TC List Generated!!')

    def run_selected_testcase(self, binary_name, suite_name, tc_name, package_name, dynamic_runner, tc_utility_action):

        fw_output_file = 'temporary_test_output'
        platform_type = dynamic_runner.platform_type
        target = dynamic_runner.target
        build_type = dynamic_runner.build_type
        transport = dynamic_runner.transport
        network = dynamic_runner.network
        module = dynamic_runner.module
        search_text, expected, search_file_type = dynamic_runner.get_pass_criteria()
        crash_texts = dynamic_runner.get_crash_text_list()
        command = dynamic_runner.get_tc_run_command(binary_name, suite_name, tc_name, package_name, fw_output_file)
        print_runner_output(command)

        if os.path.exists(fw_output_file):
            os.remove(fw_output_file)

        dynamic_runner.remove_fw_output_file(fw_output_file)

        failure_msg_list = []

        for attempt in range(0, 2):

            log = ''
            result = 0
            delta = 0
            status = GT_ATT_STATUS[GT_LOG.OK]
            is_execution_finshed = False
            failure_msg_list = []

            dynamic_runner.clear_device_log()
            print_runner_output('current_path: {}'.format(os.getcwd()))
            dynamic_runner.change_current_dir_to_app_path()

            #function_name = '{}_{}_tc_pre_action'.format(module, platform_type)
            #if hasattr(TcUtilityAction, function_name) and callable(getattr(TcUtilityAction, function_name)):
            #    getattr(tc_utility_action, function_name)()
            #else:
            #    print_runner_output('no {} function found'.format(function_name))

            start_time = datetime.now()

            try:

                def start_tc_execution():
                    nonlocal log
                    nonlocal is_execution_finshed

                    rc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True, bufsize=1)
                    #encoding utf-8 wasn't working, ISO-8859-1 was worked for a testcase
                    for line in io.TextIOWrapper(rc.stdout, encoding="ISO-8859-1"):
                        print_to_file(line.strip())
                        log += line
                        for crash_text in crash_texts:
                            if crash_text in log:
                                break
                    is_execution_finshed = True

                t = threading.Thread(target=start_tc_execution)
                t.start()
                cnt = 0
                timeout = self.timeout_seconds / 10
                while t.is_alive() and cnt < timeout:
                    print_runner_output('Observe Session No: {}'.format(cnt))
                    t.join(10)
                    cnt += 1
                    if is_execution_finshed:
                        break

                for crash_text in crash_texts:
                    if crash_text in log:
                        failure_msg_list.append("crashed")
                        status = GT_ATT_STATUS[GT_LOG.CRASHED]
                        result = 3
                        break

                if result == 0 and not is_execution_finshed:
                    failure_msg_list.append("hang")
                    status = GT_ATT_STATUS[GT_LOG.HANG]
                    result = 2

                if result == 0:
                    text_found = False
                    if search_file_type == TestRunOption.XML_PASS_CRITERIA:
                        dynamic_runner.download_fw_output_file(fw_output_file)
                        if os.path.exists(fw_output_file):
                            contents = open(fw_output_file, 'r').read()
                            text_found = True
                        else:
                            print_runner_output('No output file found')
                    else:
                        contents = log
                        text_found = True

                    if text_found and ((expected and contents.find(search_text) == -1) or (not expected and contents.find(search_text) >= 0)):
                        failure_msg_list.append(dynamic_runner.get_failure_msg(log))
                        result = 1
                    if not text_found:
                        failure_msg_list.append("unexpected")
                        result = 4

            except subprocess.CalledProcessError:
                    show_exeception()
                    failure_msg_list.append("crashed")
                    result = 3
                    status = GT_ATT_STATUS[GT_LOG.CRASHED]

            end_time = datetime.now()

            delta = end_time - start_time

            #function_name = '{}_{}_tc_post_action'.format(module, platform_type)
            #if hasattr(TcUtilityAction, function_name) and callable(getattr(TcUtilityAction, function_name)):
            #    getattr(tc_utility_action, function_name)(log_file)
            #else:
            #    print_runner_output('no {} function found'.format(function_name))

            print_runner_output('executing {} complete'.format(command))
            print_runner_output('current_path {}'.format(os.getcwd()))
            dynamic_runner.change_back_to_runner_path()

            if result == 0:
                break

            if dynamic_runner.check_connectivity():
                break
            else:
                failure_msg_list.append("low network quality")

        xml_output = '<testsuites>\n'
        xml_output += '\t<testsuite name="{}">\n'.format(suite_name)
        xml_output += '\t\t<testcase name="{}" status="{}" time="{}">\n'.format(tc_name, status, int(delta.total_seconds() * 1000))

        for failure_msg in failure_msg_list:
            xml_output += ('\t\t\t<failure message="{}"> </failure>\n'.format(failure_msg))

        xml_output += '\t\t</testcase>\n'
        xml_output += '\t</testsuite>\n'
        xml_output += '</testsuites>\n'
        
        timestring = datetime.now().strftime("%Y%m%d_%H%M%S.%f")

        process_id = binary_name
        if not process_id:
            process_id = package_name

        file_name = "{}_{}_{}_{}_{}_{}_{}" .format(platform_type, target, build_type, transport, network, timestring, process_id.replace('.', '_'))
        log_file_path = os.path.join(self.test_result_dir, file_name + '.log')
        xml_file_path = os.path.join(self.test_result_dir, file_name + '.xml')

        if os.path.isfile(log_file_path) or os.path.isfile(xml_file_path):
            print_runner_output('', Colors.FAIL, 'File name exist !!!\nRunner Internal Error', Colors.ENDC)
            return command, 4

        dynamic_runner.append_device_log(log_file_path)

        if not os.path.exists(self.test_result_dir):
            print_runner_output('{} path not found'.format(self.test_result_dir))
            result_path_parent = self.test_result_dir

            result_path_count = 0
            while result_path_parent:
                pos = result_path_parent.rfind(os.sep)
                if pos == -1:
                    break
                result_path_parent = result_path_parent[:pos]
                if os.path.exists(result_path_parent):
                    print_runner_output('{} path found'.format(result_path_parent))
                    file_list = os.listdir(result_path_parent)
                    print_runner_output('printing file list ...')
                    for f in file_list:
                        print_runner_output(f)
                    print_runner_output('file list printed')
                    break
                else:
                    print_runner_output('{} path not found'.format(result_path_parent))
                result_path_count += 1
                if result_path_count > 20:
                    break

            return command, 4

        if not os.path.isdir(self.test_result_dir):
            print_runner_output('{} directory not found'.format(self.test_result_dir))
            return command, 4

        if not os.path.isfile(log_file_path):
            print_runner_output('No log file {} found'.format(log_file_path))

        log_file = open(log_file_path, 'a')
        log_file.write('\n' + log)

        xml_file = open(xml_file_path, 'w')
        xml_file.write(xml_output)

        return command, result

    def remove_pass_tc(self, list_of_testcase, verdict_file_path):

        if not os.path.exists(verdict_file_path):
            return list_of_testcase

        f = open(verdict_file_path,'r')
        contents = f.readlines()
        f.close()

        mydi={}
        for row in contents:
            d = row.split('|')
            d = [x.strip() for x in d]

            key1 = d[1]
            key2 = d[4]
            key3 = d[2]
            key4 = d[3]
            value = d[5]

            if key1 not in mydi.keys():
                mydi[key1] = {}
            if key2 not in mydi[key1].keys():
                mydi[key1][key2] = {}
            if key3 not in mydi[key1][key2].keys():
                mydi[key1][key2][key3] = {}
            if key4 not in mydi[key1][key2][key3].keys():
                mydi[key1][key2][key3][key4] = value
            else:
                if value == 'pass':
                    mydi[key1][key2][key3][key4] = value

        new_tc_list = []

        for testcase in list_of_testcase:
            if testcase.binary_name in mydi.keys():
                if testcase.package_name in mydi[testcase.binary_name].keys():
                    if testcase.suite_name in mydi[testcase.binary_name][testcase.package_name].keys():
                        if testcase.tc_name in mydi[testcase.binary_name][testcase.package_name][testcase.suite_name].keys():
                            if 'pass' == mydi[testcase.binary_name][testcase.package_name][testcase.suite_name][testcase.tc_name]:
                                continue
            new_tc_list.append(testcase)
        return new_tc_list

    def run_selected_testcases(self, dynamic_runner, verdict_file_path, save_verdict):

        verdict_types = ['pass', 'fail', 'timeout', 'crash', 'unexpected']
        color_types = [Colors.OKGREEN, Colors.FAIL, Colors.FAIL, Colors.FAIL, Colors.FAIL]

        list_of_testcase = dynamic_runner.get_tc_list()

        if not list_of_testcase:
            print_runner_output(Colors.FAIL + 'No testcase Found !!!\nPlease, Check command parameter(s)' + Colors.ENDC)
            return

        if save_verdict:
            list_of_testcase = self.remove_pass_tc(list_of_testcase, verdict_file_path)

        fp = None
        if save_verdict:
            fp = open(verdict_file_path, 'a')

        for testcase in list_of_testcase:
            if fp:
                text = 'verdict | '
                text += testcase.binary_name + ' |'
                text += testcase.suite_name + ' |'
                text += testcase.tc_name + ' |'
                text += testcase.package_name + ' |'
                text += 'in_queue'

                fp.write(text + '\n')
                fp.flush()

        tc_utility_action = TcUtilityAction(dynamic_runner)

        while list_of_testcase:

            print_runner_output('{} testcase(s) needed to be run'.format(len(list_of_testcase)))

            for testcase in list_of_testcase:
                result = 1
                print_runner_output('Executing ' + testcase.binary_name + ' ' + testcase.package_name + '.' + testcase.suite_name + '.' + testcase.tc_name)

                try:
                    command, result = self.run_selected_testcase (testcase.binary_name, testcase.suite_name, testcase.tc_name, testcase.package_name, dynamic_runner, tc_utility_action)
                    print_runner_output('result: {}'.format(result))

                    if result == 0:
                        testcase.increase_pass_count()
                    elif result == 2:
                        testcase.increase_timeout_count()
                        testcase.increase_fail_count()
                    else:
                        testcase.increase_fail_count()

                    testcase.increase_total_count()

                    print_runner_output('increase tc verdict count')
                except:
                    show_exeception()
                    break

                print_runner_output(color_types[result] + '[ ' + 'Result of ' + testcase.suite_name + '.' + testcase.tc_name + ': ' + verdict_types[result] + ' ]' + Colors.ENDC)

                if fp:
                    text = 'verdict | '
                    text += testcase.binary_name + ' |'
                    text += testcase.suite_name + ' |'
                    text += testcase.tc_name + ' |'
                    text += testcase.package_name + ' |'
                    text += verdict_types[result]

                    fp.write(text + '\n')
                    fp.flush()

                if result == 4:
                    return

            list_of_testcase[:] = [x for x in list_of_testcase if not x.is_execution_complete()]

        if fp:
            fp.close()

        print_runner_output("### Test Is Done!!")

