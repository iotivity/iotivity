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
import io
import re
import sys
import time
import fnmatch
import optparse
import subprocess
import traceback
import platform
import fcntl

import ite.multi_logger as ml
from ite.exec.run_options import *

from ite.config import *
from ite.constants import *
from ite.util import *
import configuration
from configuration import *
from ite.multi_logger import print_runner_output

class GenericTestRunner:

    def __init__(self):
        self.cwd = ''
        self.target = ''
        self.transport = ''
        self.network = ''
        self.device_name = ''
        self.tc_framework = TESTFW_TYPES.GTEST
        self.app_path = ''
        self.platform_type = ''
        self.tc_scenario = ''
        self.list_of_testcase = []
        self.build_type = ''

    def get_platfrom_type():
        return ''

    def set_target(self, target):
        return target

    def set_app_path(self, app_path):
        if app_path:
            self.app_path = app_path
        return self.app_path

    def set_device_name(self, device_name):
        if device_name:
            self.device_name = '-s ' + device_name
        return self.device_name

    def set_tc_framework(self, tc_framework):
        if tc_framework:
            self.tc_framework = tc_framework
        return self.tc_framework

    def get_platform_command_prefix(self):
        return ''

    def get_platform_shell_prefix(self):
        return ''

    def set_module(self, module):
        if module:
            self.module = module
        return self.module

    def set_transport(self, transport):
        if transport:
            self.transport = transport
        return self.transport

    def set_network(self, network):
        if not network:
            if self.transport.upper() == TEST_TRANSPORT.IP or self.transport.upper() == TEST_TRANSPORT.TCP:
                network = TEST_NETWORK.WIFI.lower()
        if network:
            self.network = network
        return self.network

    def get_tc_list_command(self, binary_name):
        return ''

    def set_build_type(self, build_type, module):
        if build_type:
            self.build_type = build_type
        else:
            self.build_type = MODULES_BUILD_TYPES[module.upper()][0]

        self.build_type = self.build_type.lower()

        return self.build_type.lower()

    def get_tc_run_command(self, binary_name, suite_name, tc_name, package_name, output_file):
        return ''

    def get_pass_criteria(self):
        return 'failure message=', False, TestRunOption.XML_PASS_CRITERIA

    def get_crash_text_list(self):
        return ['Segmentation fault', '(core dumped)', 'shortMsg=Process crashed.']

    def change_current_dir_to_app_path(self):
        pass

    def change_back_to_runner_path(self):
        pass

    def clear_device_log(self):
        pass

    def append_device_log(self, file_path):
        pass

    def device_root_on(self):
        pass

    def remove_invalid_character_from_log(self, log):
        log = log.replace('b\'', '')
        log = log.replace('\"','')
        log = log.replace('\'','')
        log = log.replace('<','')
        log = log.replace('>','')
        log = log.replace('\\x', '')
        log = log.replace('\\r\\n', '\n')
        log = log.replace('\\n', '\n')
        log = log.replace('\'', '')
        log = log.replace('\"', '')
        log = log.replace('\\t', '    ')
        log = log.replace('1b[0m', '')
        log = log.replace('1b[0;33m', '\033[0;33m')
        log = log.replace('1b[0;32m', '\033[0;32m')
        log = log.replace('1b[0;31m', '\033[0;31m')
        log = log.replace('1b[0;30m', '\033[0;30m')
        log = log.replace('1b[m', '\033[0m')

        return log

    def get_binary_list(self):

        command = '{}ls {}/{}*{}'.format(self.get_platform_shell_prefix(), self.app_path, TC_BIN_PREFIX, TC_BIN_SUFFIX)
        print_runner_output(command)

        process= subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        log, err = process.communicate(timeout = 30)
        log = str(log)
        log = self.remove_invalid_character_from_log(log)
        log = log.replace(self.app_path + os.sep, '')
        log = log.replace(self.app_path, '')
        log = log.replace('\n', ' ').strip()
        log = re.sub(' +', ' ', log)
        return log.split(' ')

    def add_new_tc(self, binary_name, suite_name, tc_name, package_name):
        self.list_of_testcase.append(TestRunOption(binary_name, suite_name, tc_name, package_name))

    def get_tc_list(self):
        return self.list_of_testcase

    def download_fw_output_file(self, output_file):
        pass

    def remove_fw_output_file(self, output_file):
        pass

    def get_devices(self):
        return []

    def check_connectivity(self):
        return True

    def get_failure_msg(self, log):
        return 'failed'

class LinuxTestRunner(GenericTestRunner):

    def __init__(self):
        super().__init__()
        import lsb_release
        release = lsb_release.get_lsb_information()['RELEASE']
        self.target = release + '.' + platform.architecture()[0]
        self.tc_framework = TESTFW_TYPES.GTEST
        self.app_path = TC_BIN_DIR
        self.platform_type = 'linux'

    def get_tc_list_command(self, binary_name):
        return '{}/{} --gtest_list_tests'.format(self.app_path, binary_name)

    def get_tc_run_command(self, binary_name, suite_name, tc_name, package_name, output_file):
        return './{} --gtest_filter={}.{} --gtest_output=xml:{}'.format(binary_name, suite_name, tc_name, output_file)

    def change_current_dir_to_app_path(self):
        self.cwd = os.getcwd()
        os.chdir(self.app_path)

    def change_back_to_runner_path(self):
        os.chdir(self.cwd)

    def remove_fw_output_file(self, output_file):
        if os.path.exists(output_file):
            os.remove(output_file)


class TizenTestRunner(GenericTestRunner):

    def __init__(self):
        super().__init__()
        self.target = '2.4'
        self.tc_framework = TESTFW_TYPES.GTEST
        self.app_path = '/opt/usr/media/bin'
        self.platform_type = 'tizen'

    def get_platform_command_prefix(self):
        return 'sdb {}'.format(self.device_name)

    def get_platform_shell_prefix(self):
        return self.get_platform_command_prefix() + ' shell '

    def get_tc_list_command(self, binary_name):
        command_suffix = '{}/{} --gtest_list_tests'.format(self.app_path, binary_name)
        return '{}{}'.format(self.get_platform_shell_prefix(), command_suffix)

    def get_tc_run_command(self, binary_name, suite_name, tc_name, package_name, output_file):
        return '{}{}/tizen_runner.sh {} {} --gtest_filter={}.{} --gtest_output=xml:{}'.format(self.get_platform_shell_prefix(), self.app_path, self.app_path, binary_name, suite_name, tc_name, output_file)

    def device_root_on(self):
        command = '{} root on'.format(self.get_platform_command_prefix())
        process= subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        log, err = process.communicate(timeout = 30)

    def clear_device_log(self):
        os.system('{} dlog -c'.format(self.get_platform_command_prefix()))

    def append_device_log(self, file_path):
        os.system('{} dlog -d > {}'.format(self.get_platform_command_prefix(), file_path))

    def download_fw_output_file(self, output_file):
        pull_command = self.get_platform_command_prefix() + ' pull ' + self.app_path + '/' + output_file + ' .'
        print_runner_output(pull_command)
        os.system(pull_command)

    def remove_fw_output_file(self, output_file):
        command = self.get_platform_shell_prefix() + ' rm ' + self.app_path + '/' + output_file
        os.system(output_file)

    def get_devices(self):
        command = "sdb devices | awk '/[a-zA-Z0-9]+[ \t]+device[ \t]+[a-zA-Z0-9-]+$/{print $1}'"
        sdb_devices = subprocess.check_output(command, shell=True).decode('ascii').strip().splitlines()
        print_runner_output(sdb_devices)
        if len(sdb_devices) > 0:
            return sdb_devices
        return []

    def check_connectivity(self):
        command = "{} shell ifconfig wlan0 | grep 'inet ' | cut -d ':' -f 2| cut -d ' ' -f 1".format(self.get_platform_shell_prefix())
        tizen_ip = subprocess.check_output(command, shell=True).decode('ascii').strip()
        command = "ping -c 3 {} | grep 'packet loss'| cut -d ',' -f 3 | cut -d '%' -f 1".format(tizen_ip)
        packet_loss = subprocess.check_output(command, shell=True).decode('ascii').strip()
        print_runner_output("packet_loss : {}%".format(packet_loss))
        if len(packet_loss) > 0 and int(packet_loss) < 50:
            return True
        return False


class AndroidTestRunner(GenericTestRunner):

    def __init__(self):
        super().__init__()
        self.target = '5.2'
        self.platform_type = 'android'
        self.app_path = '/data/local/tmp'

    def get_platform_command_prefix(self):
        return 'adb {}'.format(self.device_name)

    def get_platform_shell_prefix(self):
        return self.get_platform_command_prefix() + ' shell '

    def get_tc_list_command(self, binary_name):
        command_suffix = '{}/{} --gtest_list_tests'.format(self.app_path, binary_name)
        return '{}{}'.format(self.get_platform_shell_prefix(), command_suffix)

    def get_tc_run_command(self, binary_name, suite_name, tc_name, package_name, output_file):
        return '{}{}/runner.sh {} {}'.format(self.get_platform_shell_prefix(), self.app_path, binary_name, tc_name)

    def clear_device_log(self):
        os.system('{} logcat -c'.format(self.get_platform_command_prefix()))

    def append_device_log(self, file_path):
        os.system('{} logcat -d > {}'.format(self.get_platform_command_prefix(), file_path))

    def get_devices(self):
        command = "adb devices | awk '/[a-zA-Z0-9]+[ \t]+device$/{print $1}'"
        adb_devices = subprocess.check_output(command, shell=True).decode('ascii').strip().splitlines()
        if len(adb_devices) > 0:
            return adb_devices
        return []

    def check_connectivity(self):
        command = "{} ip addr show wlan0  | grep 'inet ' | cut -d' ' -f6 | cut -d/ -f1".format(self.get_platform_shell_prefix())
        android_ip = subprocess.check_output(command, shell=True).decode('ascii').strip()
        command = "ping -c 3 {} | grep 'packet loss'| cut -d ',' -f 3 | cut -d '%' -f 1".format(android_ip)
        packet_loss = subprocess.check_output(command, shell=True).decode('ascii').strip()
        print_runner_output ('packet_loss : {}%'.format(packet_loss))
        if len(packet_loss) > 0 and int(packet_loss) < 50:
            return True
        return False


class AndroidGtestRunner(AndroidTestRunner):

    def __init__(self):
        super().__init__()
        self.tc_framework = TESTFW_TYPES.GTEST

    def get_tc_list_command(self, binary_name):
        command_suffix = 'LD_LIBRARY_PATH={} {}/{} --gtest_list_tests'.format(self.app_path, self.app_path, binary_name)
        return '{}{}'.format(self.get_platform_shell_prefix(), command_suffix)

    def get_tc_run_command(self, binary_name, suite_name, tc_name, package_name, output_file):
        command = '{}{}/runner.sh {} {} {} {}'.format(self.get_platform_shell_prefix(), self.app_path, binary_name, suite_name, tc_name, output_file)
        print_runner_output(command)
        return command

    def download_fw_output_file(self, output_file):
        pull_command = self.get_platform_command_prefix() + ' pull ' + self.app_path + '/' + output_file + ' .'
        print_runner_output(pull_command)
        os.system(pull_command)

    def remove_fw_output_file(self, output_file):
        command = self.get_platform_shell_prefix() + ' rm ' + self.app_path + '/' + output_file
        os.system(output_file)


class AndroidJunitRunner(AndroidTestRunner):

    def __init__(self):
        super().__init__()
        self.tc_framework = TESTFW_TYPES.JUNIT

    def get_tc_list_command(self, binary_name):
        command_suffix = '{}/{} --gtest_list_tests'.format(self.app_path, binary_name)
        return '{}{}'.format(self.get_platform_shell_prefix(), command_suffix)

    def get_tc_run_command(self, binary_name, suite_name, tc_name, package_name, output_file):
        tc_type = package_name.split('.')[-1]
        base_package = package_name[:len(package_name)-len(tc_type)-1]
        command = '{}am instrument -w -e class {}.{}.{}'.format(self.get_platform_shell_prefix(), base_package, tc_type, suite_name)
        command += '#{} {}'.format(tc_name, base_package)
        command += '/com.zutubi.android.junitreport.JUnitReportTestRunner'
        return command

    def get_pass_criteria(self):
        return 'OK (1 test)', True, TestRunOption.LOG_PASS_CRITERIA

    def get_failure_msg(self, log):
        s1 = 'junit.framework.AssertionFailedError: '
        pos = log.find(s1)
        if pos >= 0:
            log = log[pos+len(s1):]
            pos = log.find('\n')
            if pos >= 0:
                log = log[:pos]
            return log
        return super().get_failure_msg(log)
