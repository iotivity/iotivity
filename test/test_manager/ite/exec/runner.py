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

import os
import subprocess
import re
import time
import threading
import signal
from time import strftime
from datetime import datetime
import platform
from ite.result.collector import TestResultCollector
from ite.constants import *
from ite.util import *

class TestRunner:


    def get_gtest_set(self, exe_path):
        command = ["./%s" % os.path.basename(exe_path), '--gtest_list_tests']
        process = subprocess.Popen(command, cwd=os.path.dirname(exe_path), stdout=subprocess.PIPE)

        testset = set()
        testsuite = ''
        for line in process.stdout.readlines():
            line = re.sub(r'(b\'|\')', '', str(line.strip()))

            if line.endswith('.') == True :
                testsuite = line
                continue

            if (testsuite != '' and line != ''):
                testset.add(testsuite + line)

        return testset

    def terminate_process(self, name):
        if platform.system().lower()== 'linux':
            proc = subprocess.Popen(["pgrep", name], stdout=subprocess.PIPE)
            for pid in proc.stdout:
                os.kill(int(pid), signal.SIGTERM)

        if platform.system().lower() == 'windows':
            proc = subprocess.Popen('tasklist', stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell= True)
            teeProcess = [str(x) for x in proc.stdout if name in str(x)]
            for teeProc in teeProcess:
                PID = teeProc.split()[1]
                subprocess.call('taskkill /pid %s /f' % str(PID), shell=True)


    def run_gtest(self, exe_path, is_shuffle, repeat, filter_str, output, log_path):
        shuffle_option = ''
        if is_shuffle:
            shuffle_option = "--gtest_shuffle"

        repeat_option = "--gtest_repeat=" + str(repeat)
        filter_option = "--gtest_filter=" + filter_str
        output_option = "--gtest_output=xml:" + output

        if platform.system().lower() == 'linux':
            command = ["./%s" % os.path.basename(exe_path), shuffle_option, repeat_option, output_option, filter_option]
            proc = subprocess.Popen(command, cwd=os.path.dirname(exe_path), stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        elif platform.system().lower() == 'windows':
            command = ["%s" % os.path.basename(exe_path), shuffle_option, repeat_option, output_option, filter_option]
            proc = subprocess.Popen(command, cwd=os.path.dirname(exe_path), stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

        hangchecking = True
        isHanged = False
        def check_hang():
            nonlocal isHanged
            timeout = 60 * 5
            prev_size = 0
            elapsed_time = 0
            while hangchecking:
                time.sleep(1)

                if os.path.isfile(log_path):
                    statinfo = os.stat(log_path)
                    size = statinfo.st_size

                    if prev_size != size:
                        prev_size = size
                        elapsed_time = 0
                        continue

                elapsed_time += 1

                if elapsed_time > timeout:
                    isHanged = True
                    if platform.system().lower() == 'linux':
                        self.terminate_process("tee")
                    elif platform.system().lower() == 'windows':
                        self.terminate_process("mtee.exe")
                    proc.terminate()
                    break

        t = threading.Thread(target=check_hang)
        t.start()
        if platform.system().lower() == 'linux':
            subprocess.call("tee %s" % log_path, shell=True, stdin=proc.stdout, stderr=proc.stderr)
        elif platform.system().lower() == 'windows':
            subprocess.call("mtee.exe %s" % log_path, shell=True, stdin=proc.stdout, stderr=proc.stderr)

        hangchecking = False
        if t.is_alive():
            t.join(2)

        if isHanged == True:
            print("==> Hang is detected...")
            txt = open_append_file(log_path)
            print(GT_HANG_LOG, file=txt)
            txt.close()


    def create_test_file_info(self, platform, target, transport, network, exe_path, result_dir):
        timestring = datetime.now().strftime("%Y%m%d_%H%M%S.%f")
        file_name = "%s_%s_%s_%s_%s_%s" % (platform, target, transport, network, timestring, os.path.basename(exe_path))
        while os.path.exists(os.path.join(result_dir, "%s.log" % file_name)):
            file_name += "_"

        log_file = os.path.join(result_dir, "%s.log" % file_name)
        xml_file = os.path.join(os.path.abspath(result_dir), "%s.xml" % file_name)

        return log_file, xml_file

    def write_test_progress(self, progress_path, testfilter, testkey, status):
        if progress_path == None or progress_path == '' or testkey == None:
            return

        txt = open_append_file(progress_path)
        if txt == False:
            return

        status_txt = "PASS"
        if status == GT_LOG.FAILED or status == GT_LOG.CRASHED:
            status_txt = "FAIL"

        if testfilter in testkey:
            print("%s|%s" %(testkey[testfilter], status_txt), file=txt)

        txt.close()

    def complete_test(self, platform, target, transport, network, exe_path, result_dir, testset, run_standalone, progress_path, testkey):
        crashedset = set()
        failedset = set()
        analyzer = TestResultCollector()
        #filter_str = '*'
        filter_str = ':'.join(testset)
        while True :
            log_file, xml_file = self.create_test_file_info(platform, target, transport, network, exe_path, result_dir)

            if run_standalone:
                filter_str = testset.pop()

            self.run_gtest(exe_path, True, 1, filter_str, xml_file, log_file)

            results = analyzer.analyze_result_log(log_file)
            for result in results:
                testsuite, testcase, status, startpos, endpos = result
                test = '%s.%s' % (testsuite, testcase)
                try :
                    if not run_standalone:
                        testset.remove(test)
                except KeyError:
                    print("==> Already removed test case: " + test)

                if status == GT_LOG.FAILED:
                    failedset.add(test)
                elif status == GT_LOG.CRASHED:
                    print("==> Found Crashed TC: " + test)
                    crashedset.add(test)
                else:
                    print("TC is success: %s.%s" % (testsuite, testcase))

                self.write_test_progress(progress_path, test, testkey, status)

            if len(testset) == 0:
                break;

            if not run_standalone:
                filter_str = ':'.join(testset)

            print("==> New Filter Option: " + filter_str)

        return failedset, crashedset

    def repeat_test_standalone(self, platform, target, transport, network, exe_path, result_dir, testset):
        for test in testset:
            log_file, xml_file = self.create_test_file_info(platform, target, transport, network, exe_path, result_dir)

            filter_str = test
            print("==> Repeat TC: " + filter_str)

            self.run_gtest(exe_path, True, 1, filter_str, xml_file, log_file)

    def run_test_executable(self, option):
        if (not os.path.isfile(option.exe_path)):
            print("Invalid Path, exe: " + option.exe_path)
            return;

        standalone = option.run_standalone;
        testset = option.testset
        if (testset == None):
            testset = self.get_gtest_set(option.exe_path)

        while option.runtime > 0:
            if not option.run_standalone and option.runtime % 2 == 0:
                standalone = True;
            else:
                standalone = option.run_standalone;

            failedset, crashedset = self.complete_test(option.platform, option.target, option.transport, option.network, option.exe_path, option.result_dir, testset, standalone, option.testprogress_path, option.testkey)

            if option.repeat_failed:
                self.repeat_test_standalone(option.platform, option.target, option.transport, option.network, option.exe_path, option.result_dir, failedset)

            if option.repeat_crashed :
                self.repeat_test_standalone(option.platform, option.target, option.transport, option.network, option.exe_path, option.result_dir, crashedset)

            option.runtime -= 1

class TestRunnerOption:


    def __init__(self):
        self.exe_path = ''
        self.platform = ''
        self.target = ''
        self.result_dir = ''
        self.runtime = 1
        self.repeat_failed = True
        self.repeat_crashed = True
        self.run_standalone = False
        self.testset = None
        self.testkey = None
        self.testprogress_path = ''
        self.transport = ''
        self.network = ''
