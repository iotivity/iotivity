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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
'''

import sys

reload(sys)
sys.setdefaultencoding('utf-8')
import io
import os
import java
import time
import re
import commands
import signal
import glob
import shutil
import string
import random
import datetime
import subprocess
import traceback
import threading
import datetime
from time import gmtime, strftime

sys.path.append("./DeviceLib.jar")

from robot.api import logger
from org.iotivity.test.rfw.common.devicecontroller import *
from org.iotivity.test.rfw.common.devicetestlib import *
from org.iotivity.test.rfw.common.devicecontroller.datamodel import *

from subprocess import Popen, PIPE
from xml.etree.ElementTree import ElementTree, ParseError

class Execution_Status(object):
    INIT = 0
    BUILD_IOTIVITY_FAIL = 2
    BUILD_TEST_FAIL = 4
    TEST_PRE_CONDITION_PASS = 5
    TEST_PRE_CONDITION_FAIL = 6

    BUILD_IOTIVITY = 'build_iotivity'
    BUILD_TEST = 'build_test'
    TEST_PRE_CONDITION = 'test_pre_condition'

    PASS_STATUS_INDEX = 0
    FAIL_STATUS_INDEX = PASS_STATUS_INDEX + 1
    TIMEOUT_STATUS_INDEX = FAIL_STATUS_INDEX + 1
    UNEXPECTED_STATUS_INDEX = TIMEOUT_STATUS_INDEX + 1
    TOTAL_STATUS_INDEX = UNEXPECTED_STATUS_INDEX + 1

class Simulator(object):
    def __init__(self, os_type, device_name, app_name, cmd_dir, run_command, expected_log, app_command):
        self.os_type = os_type
        self.device_name = device_name
        self.app_name = app_name
        self.cmd_dir = cmd_dir
        self.run_command = run_command
        self.expected_log = expected_log
        self.app_command = app_command
        self.log_saved = False
        self.log = ''
        self.status = False

class DeviceControlKeyword(object):

    def __init__(self):
        self._result = ''
        self.multiDeviceManager = MultiDeviceManager.getInstance()
        self.logManager = LogManager.getInstance()
        self.build_commands = {}
        self.log_fp = open('robot_jython_log.txt', 'w')
        self.robot_execution_status = {}
        self.robot_root = '.'
        self.simulator_list = []
        self.current_tc_list = []
        self.tc_verdict_map = {}

    ## Device Manage Keyword ##

    def add_device(self, device_type, device_id):  # Tizen , Android
        return self.multiDeviceManager.addDevice(str(device_type), str(device_id))

    def add_specific_device(self, device_type, device_id, spec):  # Linux, Tizen, Android
        return self.multiDeviceManager.addDevice(str(device_type), str(device_id), str(spec))

    def remove_device(self, device_id):  # Linux, Tizen, Android
        return self.multiDeviceManager.removeDevice(str(device_id))

    def remove_all_devices(self):  # Linux, Tizen , Android
        return self.multiDeviceManager.removeAllDevices()

    def get_deviceID(self, device_id):  # Linux, Tizen , Android
        return self.multiDeviceManager.getDevice(str(device_id)).getDeviceID()

    #### Log ####
    def log_to_console(self, arg):
        logger.info('\n%s' % arg, also_console=True)

    def set_log_file_directory(self, path):
        self.logManager.setLogPath(str(path))

    def start_device_log(self, device_id):  # Tizen , Android
        return self.logManager.registerDeviceLogger(self.multiDeviceManager.getDevice(str(device_id)).getDeviceID(),
                                                    self.multiDeviceManager.getDevice(str(device_id)), True)

    def get_device_log(self, device_id):  # Tizen, Android
        app_logs = self.logManager.getDeviceLogger(self.multiDeviceManager.getDevice(str(device_id)).getDeviceID()).getLogs()
        return self.remove_unwanted_characters(app_logs)

    def clear_device_log(self, device_id):  # Tizen, Android
        return self.logManager.getDeviceLogger(
            self.multiDeviceManager.getDevice(str(device_id)).getDeviceID()).cleanLog()

    def get_filtered_device_log(self, device_id, filters):  # Tizen , Android
        self.multiDeviceManager.getDevice(str(device_id)).getLogManager().getLogs(str(filters))
        return self.multiDeviceManager.getDevice(str(device_id)).getLogManager().getLogs(str(filters))

    def clear_filtered_device_log(self, device_id, filters):  # Tizen, Android
        return self.logManager.getProcessLogger(str(device_id), str(filters)).cleanLog()

    def start_filtered_device_log(self, device_id, filters):  # Tizen, Android
        self.multiDeviceManager.getDevice(str(device_id)).getLogManager().startLogging(str(filters))
        return self.logManager.registerProcessLogger(str(device_id), self.multiDeviceManager.getDevice(str(device_id)),
                                                     str(filters))

    def stop_filtered_device_log(self, device_id, filters):  # Tizen, Android
        self.multiDeviceManager.getDevice(str(device_id)).getLogManager().stopLogging(str(filters))
        return self.logManager.unregisterProcessLogger(str(device_id),
                                                       self.multiDeviceManager.getDevice(str(device_id)), str(filters))

    def start_application_log(self, device_id, app_id):  # Linux
        self.notify_loggers ('start_application_log in')
        ob = None
        try:
            temp_id = self.multiDeviceManager.getDevice(str(device_id))
            self.notify_loggers ('calling registerProcessLogger')
            ob = self.logManager.registerProcessLogger(str(device_id), temp_id, str(app_id), True)
            self.notify_loggers ('registerProcessLogger called')
        except:
            self.notify_loggers ('exception to call registerProcessLogger')
        self.notify_loggers ('start_application_log out')
        return ob

    def clean_application_log(self, device_id, app_id):  # Linux
        return self.logManager.getProcessLogger(str(device_id), str(app_id)).cleanLog()

    def get_application_log(self, device_id, app_id):  # Linux
        app_logs = self.logManager.getProcessLogger(str(device_id), str(app_id)).getLogs()
        return self.remove_unwanted_characters(app_logs)


    def remove_unwanted_characters(self, app_logs):
        logs = ''

        for line in app_logs.splitlines():
            try:
                line.decode('ascii')
                logs = logs + line + '\n'
            except:
                part = ''
                for ch in line:
                    value = ord(ch)
                    if value > 127:
                        part = part + '<Non-Ascii: ' + str(value) + '>'
                    else:
                        part = part + ch
                logs = logs + part + '\n'

        return logs

    def set_application_log_filter(self, device_id, app_id):  # Linux
        return None

    def wait_until_logging_stop(self, device_id, process_name, running_sec, time_out, expected_log=''):
        backup_log = self.get_application_log(str(device_id), str(process_name))
        i = 0
        different = None
        while (int(time_out) >= i):
            time.sleep(int(1))
            i = i + 1
            new_log = self.get_application_log(str(device_id), str(process_name))
            if expected_log and expected_log in new_log:
                print('expected_log found')
                break
            if new_log == backup_log:
                different = None
            else:
                i = 0
                different = True
                backup_log = self.get_application_log(str(device_id), str(process_name))
                if expected_log and expected_log in backup_log:
                    print('expected_log found')
                    break
                time.sleep(int(running_sec))

    def get_test_manager_log_file_info(self, robot_root, os_type, tc_type, module):

        directory = os.path.join(robot_root, 'report', tc_type, os_type)

        prefix = 'test_manager' + '_' + module + '_' + os_type
        suffix = '.log'

        return directory, prefix, suffix


    def wait_until_log_file_complete(self, device_id, process_name, running_sec, time_out, expected_log, folder_path, prefix, suffix):

        i = 0
        running_sec = int(running_sec)
        time_out = int(time_out)
        prev_size = self.get_test_manager_file_size(folder_path, prefix, suffix)

        while (time_out >= i):
            log = self.get_application_log(str(device_id), str(process_name))
            if expected_log in log:
                print('expected log found')
                break

            current_size = self.get_test_manager_file_size(folder_path, prefix, suffix)

            if prev_size == current_size:
                i = i + running_sec
                current_time = strftime("%Y-%m-%d %H:%M:%S", gmtime())
                msg = 'Time: {}, previous size: {}, current size: {}'
                msg = msg.format(current_time, prev_size, current_size)
                print(msg)
            else:
                i = 0
                prev_size = current_size

            time.sleep(running_sec)

    def wait_until_expected_log_found(self, device_id, process_name, expected_log, maximum_waiting_time):
        log = ''
        i = 0
        while (int(maximum_waiting_time) >= i):
            time.sleep(int(1))
            i = i + 1
            log += self.get_application_log(str(device_id), str(process_name))
            if expected_log in log:
                return True
            is_app_alive = self.is_application_running(str(device_id), str(process_name))
            if is_app_alive != True:
                break
        print ('Expected log not found in given time')
        return False

    def wait_until_application_exit(self, device_id, process_name):
        while (True):
            is_app_alive = self.is_application_running(device_id, process_name)
            if is_app_alive != True:
                break
        return True

    def check_keyword(self, keyword, logs):
        if re.search(str(keyword), str(logs)):
            return True
        else:
            return False

    def check_keyword_multiple_callback(self, keyword, logs):
        list1 = re.findall(str(keyword), str(logs))
        lengthList1 = len(list1)
        if lengthList1 > 1:
            return True
        else:
            return False

    def check_keyword_single_callback(self, keyword, logs):
        list1 = re.findall(str(keyword), str(logs))
        lengthList1 = len(list1)
        if lengthList1 == 1:
            return True
        else:
            return False

            ## Screen Control Keyword ##

    def send_virtual_key(self, device_id, key):  # Tizen
        print "******************************"
        print device_id
        print key
        print "*****************************"
        return self.multiDeviceManager.getDevice(str(device_id)).getScreenController().sendString(str(key))

    def press_physical_button(self, device_id, key):  # Tizen
        return self.multiDeviceManager.getDevice(str(device_id)).getScreenController().pressKey(
            KeyEventType.valueOf(str(key)))

    def tap_device_screen(self, device_id, x, y):  # Tizen, Android
        coordinate = Coordinate(int(x), int(y))
        return self.multiDeviceManager.getDevice(str(device_id)).getScreenController().tap(coordinate)

    def drag_device_screen(self, device_id, fromX, fromY, toX, toY):  # Tizen, Android
        fromcoordinate = Coordinate(int(fromX), int(fromY))
        tocoordinate = Coordinate(int(toX), int(toY))
        return self.multiDeviceManager.getDevice(str(device_id)).getScreenController().drag(fromcoordinate,
                                                                                            tocoordinate)

    ## Process/Application Control Keyword ##

    def execute_application(self, device_id, process_name, commands):  # Linux, Tizen , Android
        if type(commands) is list:
            command = ''
            for temp in commands:
                command = command + ' ' + temp
        else:
            command = str(commands)
        return self.multiDeviceManager.getDevice(str(device_id)).getProcessManager().createProcess(str(process_name), command.strip())

    def input_command_to_application(self, device_id, process_name, command):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getProcessManager().inputProcessCommand(
            str(process_name), str(command))

    def terminate_application(self, device_id, process_name):  # Linux, Tizen, Android
        return self.multiDeviceManager.getDevice(str(device_id)).getProcessManager().removeProcess(str(process_name))

    def is_application_running(self, device_id, process_name):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getProcessManager().isProcessRunning(str(process_name))

    ## FileManager Keyword ##

    def is_file_exist(self, device_id, file_path, file_name):  # Linux
        print file_path + os.sep + file_name
        return os.path.exists(file_path + os.sep + file_name)

    def is_file_exist(self, bin_path, prefix, suffix):
        file_list = [f for f in listdir(bin_path) if isfile(join(bin_path, f))]
        for file_name in file_list:
            if file_name.startswith(prefix) and file_name.endswith(suffix):
                return True
        return False

    def push_file(self, device_id, src_path, des_path):  # Tizen, Android
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().push(str(src_path), str(des_path))

    def pull_file(self, device_id, src_path, des_path):  # Tizen, Android
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().pull(str(src_path), str(des_path))

    def change_working_directory(self, device_id, path):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().changeWorkingDir(str(path))

    def get_working_directory_path(self, device_id):  # Linux
        path = self.multiDeviceManager.getDevice(str(device_id)).getFileManager().getWorkingDir()
        pos = path.rfind("/");
        return path[0:pos]

    def copy_file(self, device_id, source_file, destination_file):  # Linux , Tizen, Android
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().copyFile(str(source_file),
                                                                                           str(destination_file))

    def delete_file(self, device_id, file_path):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().deleteFile(str(file_path))

    def move_file(self, device_id, source_file, destination_file):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().moveFile(str(source_file),
                                                                                           str(destination_file))

    def is_exist_word_in_file(self, device_id, word, file_path):
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().isExistWordInFile(word, file_path)

    def make_empty_file(self, device_id, file_path):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().createFile(file_path)

    def make_directory(self, device_id, dir_path):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().mkdir(str(dir_path))

    def copy_directory(self, device_id, source_dir, destination_dir):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().copydir(str(source_dir),
                                                                                          str(destination_dir))

    def move_directory(self, device_id, source_dir, destination_dir):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().movedir(str(source_dir),
                                                                                          str(destination_dir))

    def delete_directory(self, device_id, dir_path):  # Linux
        return self.multiDeviceManager.getDevice(str(device_id)).getFileManager().rmdir(str(dir_path))

    def bluetooth_on(self, device_id):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().blueToothOn()

    def bluetooth_off(self, device_id):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().blueToothOff()

    def wifi_on(self, device_id):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().wifiOn()

    def wifi_off(self, device_id):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().wifiOff()

    def get_ip_address(self, device_id):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().getIPAddress()

    def get_bluetooth_mac_address(self, device_id):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().getBluetoothMacAddress()

    def set_sdk_path(self, device_id, sdk_path):  # Android
        return self.multiDeviceManager.getDevice(str(device_id)).getNetworkManager().setSDKPath(sdk_path)

    ## EOF ##

    ## Android Device Resolution Ratio ##

    def get_android_real_id(self, device_id):
        return self.multiDeviceManager.getDevice(str(device_id)).getDeviceID()

    def get_android_resolution(self, device_id):
        deviceID = self.multiDeviceManager.getDevice(str(device_id)).getDeviceID()
        status, resolution_output = commands.getstatusoutput("adb -s %s shell wm size" % deviceID)
        index = resolution_output.find('Physical size:')
        print (resolution_output)
        resolution_output = resolution_output[index:]
        print (resolution_output)
        resolution_array = resolution_output.split()
        resolution = resolution_array[2]
        return resolution

    def get_android_screen_width(self, device_id):
        resolution = self.get_android_resolution(device_id)
        resolution_array = resolution.split('x')
        screen_width = int(resolution_array[0])
        return screen_width

    def get_android_screen_height(self, device_id):
        resolution = self.get_android_resolution(device_id)
        resolution_array = resolution.split('x')
        screen_height = int(resolution_array[1])
        return screen_height

    def get_android_xcoordinate(self, device_id, screen_width, xcoordinate):
        xcoordinate = int(xcoordinate)
        screen_width = int(screen_width)
        changed_screen_Width = self.get_android_screen_width(device_id)
        xchangedcoordinate = xcoordinate * changed_screen_Width / screen_width
        return xchangedcoordinate

    def get_android_ycoordinate(self, device_id, screen_height, ycoordinate):
        ycoordinate = int(ycoordinate)
        screen_height = int(screen_height)
        changed_screen_height = self.get_android_screen_height(device_id)
        ychangedcoordinate = ycoordinate * changed_screen_height / screen_height
        return ychangedcoordinate

    ## Tizen Device Resolution Ratio ##

    def get_tizen_resolution(self, device_id):
        deviceID = self.multiDeviceManager.getDevice(str(device_id)).getDeviceID()
        status, resolution_output = commands.getstatusoutput("sdb -s %s shell su -c xrandr" % deviceID)
        return resolution_output

    def get_tizen_screen_width(self, device_id):
        resolution = self.get_tizen_resolution(device_id)
        match = re.search("connected (\d+)x(\d+).* (\d+mm) x (\d+mm)", resolution)
        if match:
            screen_width = match.group(1)
        return int(screen_width)

    def get_tizen_screen_height(self, device_id):
        resolution = self.get_tizen_resolution(device_id)
        match = re.search("connected (\d+)x(\d+).* (\d+mm) x (\d+mm)", resolution)
        if match:
            screen_height = match.group(2)
        return int(screen_height)

    def get_tizen_xcoordinate(self, device_id, screen_width, xcoordinate):
        xcoordinate = int(xcoordinate)
        screen_width = int(screen_width)
        changed_screen_Width = self.get_tizen_screen_width(device_id)
        xchangedcoordinate = xcoordinate * changed_screen_Width / screen_width
        return xchangedcoordinate

    def get_tizen_ycoordinate(self, device_id, screen_height, ycoordinate):
        ycoordinate = int(ycoordinate)
        screen_height = int(screen_height)
        changed_screen_height = self.get_tizen_screen_height(device_id)
        ychangedcoordinate = ycoordinate * changed_screen_height / screen_height
        return ychangedcoordinate

    ## Tizen Console ##

    def execute_console_application(self, device_id, process_name, command):  # Tizen
        return self.multiDeviceManager.getDevice(str(device_id)).getConsoleManager().createProcess(str(process_name),
                                                                                                   str(command))

    def terminate_console_application(self, device_id, process_name):  # Tizen
        return self.multiDeviceManager.getDevice(str(device_id)).getConsoleManager().removeProcess(str(process_name))

    def input_command_to_console_application(self, device_id, process_name, command):  # Tizen
        return self.multiDeviceManager.getDevice(str(device_id)).getConsoleManager().inputProcessCommand(
            str(process_name), str(command))

    def start_console_application_log(self, device_id, app_id):  # Tizen
        return self.logManager.registerConsoleProcessLogger(str(device_id),
                                                            self.multiDeviceManager.getDevice(str(device_id)),
                                                            str(app_id), True)

    # Find and Replace #
    def find_and_replace(self, fileName, old_text, new_text):
        with open(fileName, 'rb') as f:
            content = f.read()

        with open(fileName, 'wb') as f:
            temp = content.replace(old_text, new_text)
            f.write(temp)
        return True

    # -----------------------------------------------------------------

    def updatefiletest(self, filename, dico):
        RE = '((' + '|'.join(dico.keys()) + ')\s*=)[^\r\n]*?(\r?\n|\r)'
        pat = re.compile(RE)

        def jojo(mat, dic=dico):
            return dic[mat.group(2)].join(mat.group(1, 3))

        with open(filename, 'rb') as f:
            content = f.read()

        with open(filename, 'wb') as f:
            temp = pat.sub(jojo, content.decode())
            # f.write(bytes(temp, 'UTF-8'))
            f.write(temp)
        return True

    def update_configuration_set_network(self, conf_file, network):
        vars = ['CA_IP', 'CA_LE', 'CA_EDR', 'CA_CLOUD']
        new_values = ['1', '0', '0', '0']
        if network == 'IP':
            new_values = [' 1', ' 0', ' 0', ' 0']
        if network == 'EDR':
            new_values = [' 0', ' 1', ' 0', ' 0']
        if network == 'LE':
            new_values = [' 0', ' 0', ' 1', ' 0']
        # GATT = BLE = 1, RFCOMM = EDR = 2
        what_to_change = dict(zip(vars, new_values))
        self.updatefiletest(conf_file, what_to_change)
        return 'update configuration done'

    def update_configuration_set_address(self, conf_file, address):
        vars = ['IP']
        new_values = [address]
        what_to_change = dict(zip(vars, new_values))
        self.updatefiletest(conf_file, what_to_change)
        return 'update configuration done'

    def update_configuration_set_port(self, conf_file, port):
        vars = ['PORT ']
        new_values = [str(port)]
        what_to_change = dict(zip(vars, new_values))
        self.updatefiletest(conf_file, what_to_change)
        return 'update configuration done'

    def update_configuration_set_secure_port(self, conf_file, secure_port):
        vars = ['SECURE_PORT']
        new_values = [str(secure_port)]
        what_to_change = dict(zip(vars, new_values))
        self.updatefiletest(conf_file, what_to_change)
        return 'update configuration done'

    def update_test_manager_configuration_set_tc_bin(self, conf_file):
        vars = ['TC_BIN_DIR']
        new_values = [' "../IotivitySECTest/bin"']
        what_to_change = dict(zip(vars, new_values))
        self.updatefiletest(conf_file, what_to_change)
        return 'update configuration done'

    # tizen configuration file change #
    def update_test_manager_configuration_set_bin_tizen(self, conf_file, bin_path):
        vars = ['TC_BIN_DIR']
        new_values = [bin_path]
        what_to_change = dict(zip(vars, new_values))
        self.updatefiletest(conf_file, what_to_change)
        return 'update configuration done'

    def update_config(self, fileName, ip, other_ip, port, secure_port, network_flag="1000"):
        out_file = open(fileName, 'w')
        text = '[SIMULATOR]\n'
        text = text + 'IP = ' + str(ip) + '\n'
        text = text + 'OTHER_IP_LIST = ' + str(other_ip) + '\n'
        text = text + 'PORT = ' + str(port) + '\n'
        text = text + 'SECURE_PORT = ' + str(secure_port) + '\n'
        text = text + 'CA_IP = ' + network_flag[0:1] + '\nCA_LE = ' + network_flag[1:2] + '\nCA_EDR = ' + network_flag[
                                                                                                          2:3] + '\nCA_TCP = ' + network_flag[
                                                                                                                                 3:4]
        out_file.write(text)
        out_file.close()

    def update_android_config(self, fileName, ip, port, secure_port):
        out_file = open(fileName, 'w')
        text = 'IP:' + str(ip) + '\n'
        text = text + 'UNSECURED PORT:' + str(port) + '\n'
        text = text + 'SECURED PORT:' + str(secure_port) + '\n'
        out_file.write(text)
        out_file.close()

    def get_executable_name_ca_sim(self, server_type, network):
        if server_type == 'SERVER':
            if network == 'IP':
                return 'iotivity_sim_server'
            else:
                return 'iotivity_sim_server_tizen'
        else:
            if network == 'IP':
                return 'iotivity_sim_client'
            else:
                return 'iotivity_sim_client_tizen'

    def get_port(self, data, secured):
        if secured == '1':
            secured = 'true'
        else:
            secured = 'false'
        findTxt = 'Secured: ' + secured
        print findTxt
        start = data.find('Secured: ' + secured)
        if start == -1:
            return '-1'
        else:
            securedStr = data[start - len('Secured: 0') - 11:start]
            start = securedStr.find('Port: ')
            if start == -1: return '-1'
            endChar = '\n'
            end = securedStr.find(endChar, start)
            start = start + len('Port: ')
            output = securedStr[start:end]
            return output

            # Upload Binary in Arduino Board

    def upload_arduino_binary(self, binary_uploader, config_file, board_name, protocol, arduino_port, speed,
                              binary):  # Linux
        binary_cmd = binary_uploader + ' -C' + config_file + ' -v -v -v -p' + board_name + ' -c' + protocol + ' -P' + arduino_port + ' -b' + speed + ' -D -Uflash:w:' + binary + ':i'
        return os.system(binary_cmd)

    def run_application(self, device_id, path, command):  # Linux
        cd_cmd = 'cd ' + 'path'
        os.system(cd_cmd)
        return os.system(command)

    def get_text(self, keyword, endchar, logs):
        # start = logs.find(keyword)+len(keyword)
        start = logs.find(keyword)
        if (start == -1): return 0
        start = start + len(keyword)
        print start
        end = logs.find(endchar, start)
        print end
        if (end == -1): end = len(logs)
        print end
        ip = logs[start:end]
        ip = ip.strip()
        return ip

    # Utility function
    def get_current_date(self, n):
        a = (datetime.datetime.now())
        b = a + datetime.timedelta(seconds=int(n))
        print (a)
        print (b)
        c = b.strftime('%Y-%m-%d %H:%M:%S')
        print (c)
        return str(c)

    def fetch_from_right(self, string, marker):
        return string.split(marker)[-1].lstrip()

    def check_keyword_count(self, keyword, logs, count):
        list1 = re.findall(str(keyword), str(logs))
        lengthList1 = len(list1)
        if lengthList1 == int(count):
            return True
        else:
            return False

    def get_random_string(self, size):
        return ''.join([random.choice(string.ascii_letters) for n in range(int(size))])

    def write_file(self, path, text):
        fh = open(path, "w")
        fh.write(text)
        fh.close()
        return True

    def read_file(self, path):
        fh = open(path, "r")
        text = ""
        for line in fh:
            text += line.rstrip()
        fh.close()
        return text

    def update_list_item(self, value, index, *old_values):
        new_values = list(old_values);
        new_values[int(index)] = value
        return new_values;

    # ca linux functions

    ca_linux_secure_server_addresses = []
    ca_linux_non_secure_server_addresses = []
    ca_linux_secure_client_addresses = []
    ca_linux_non_secure_client_addresses = []
    received_ca_linux_ip = ''
    received_ca_linux_port = ''

    def is_ip_port_exist_in_ca_linux(self, isClientValue, isSecureValue, ip, port):
        isClient = int(isClientValue)
        isSecure = int(isSecureValue)
        if isClient == 1:
            if isSecure == 1:
                return self.ca_linux_secure_client_addresses.count(
                    ip) > 0 and self.ca_linux_secure_client_addresses.count(port) > 0
            return self.ca_linux_non_secure_client_addresses.count(
                ip) > 0 and self.ca_linux_non_secure_client_addresses.count(port) > 0
        else:
            if isSecure == 1:
                return self.ca_linux_secure_server_addresses.count(
                    ip) > 0 and self.ca_linux_secure_server_addresses.count(port) > 0
            return self.ca_linux_non_secure_server_addresses.count(
                ip) > 0 and self.ca_linux_non_secure_server_addresses.count(port) > 0

    def get_ca_linux_ip_port(self, isClientValue, isSecureValue, isPortValue, indexValue):
        index = int(indexValue)
        isClient = int(isClientValue)
        isPort = int(isPortValue)
        isSecure = int(isSecureValue)
        index = index * 2 + isPort
        if isClient == 1:
            if isSecure == 1:
                item = self.ca_linux_secure_client_addresses[index]
            else:
                item = self.ca_linux_non_secure_client_addresses[index]
        else:
            if isSecure == 1:
                item = self.ca_linux_secure_server_addresses[index]
            else:
                item = self.ca_linux_non_secure_server_addresses[index]
        return item

    def get_ca_linux_network_size(self, isClientValue, isSecureValue):
        isClient = int(isClientValue)
        isSecure = int(isSecureValue)
        if isClient == 1:
            if isSecure == 1:
                length = len(self.ca_linux_secure_client_addresses)
            else:
                length = len(self.ca_linux_non_secure_client_addresses)
        else:
            if isSecure == 1:
                length = len(self.ca_linux_secure_server_addresses)
            else:
                length = len(self.ca_linux_non_secure_server_addresses)
        return length // 2

    def set_ca_linux_network_info(self, isClientValue, msg):
        isClient = int(isClientValue)
        if isClient == 1:
            del self.ca_linux_secure_client_addresses[:]
            del self.ca_linux_non_secure_client_addresses[:]
        else:
            del self.ca_linux_secure_server_addresses[:]
            del self.ca_linux_non_secure_server_addresses[:]
        lines = re.split('\n', msg)
        for line in lines:
            #		print ('line: ' + line)
            i = line.find('Address: ')
            #		print ('Address: ' + str(i))
            if i >= 0:
                k = line.find(':') + 2
                ip = line[k:]
                continue
            i = line.find('Port: ')
            #		print ('Port: ' + str(i))
            if i >= 0:
                k = line.find(':') + 2
                port = line[k:]
                continue
            i = line.find('Secured: true')
            #		print ('Secured: true: ' + str(i))
            if i >= 0:
                if isClient == 1:
                    self.ca_linux_secure_client_addresses.append(ip)
                    self.ca_linux_secure_client_addresses.append(port)
                else:
                    self.ca_linux_secure_server_addresses.append(ip)
                    self.ca_linux_secure_server_addresses.append(port)
                    continue
            i = line.find('Secured: false')
            #		print ('Secured: false: ' + str(i))
            if i >= 0:
                if isClient == 1:
                    self.ca_linux_non_secure_client_addresses.append(ip)
                    self.ca_linux_non_secure_client_addresses.append(port)
                # print ('adding non secure client address')
                else:
                    self.ca_linux_non_secure_server_addresses.append(ip)
                    self.ca_linux_non_secure_server_addresses.append(port)
                    #				print ('adding non secure server address')

    def get_received_ca_linux_ip(self):
        return self.received_ca_linux_ip

    def get_received_ca_linux_port(self):
        return self.received_ca_linux_port

    def set_ca_linux_received_ip_port(self, msg):
        lines = re.split('\n', msg)
        for line in lines:
            i = line.find('Remote Address: ')
            j = line.find('Port: ')
            k = line.find('secured:')
            if i >= 0 and j > i and k > j:
                i = i + len('Remote Address: ')
                self.received_ca_linux_ip = line[i:j - 1]
                j = j + len('Port: ')
                self.received_ca_linux_port = line[j:k - 1]

    def get_ca_android_sampleapp_network_info(self, log):
        ip = ''
        non_secure_port = 0
        secure_port = 0
        mac = ''
        type = 0
        port = 0
        network_ids = []

        line_filter = 'I/JNI_INTERFACE_SAMPLE'
        start_substring = '################## Network Information #######################'
        end_substring = '##############################################################'

        start_index = log.find(start_substring)
        end_index = log.find(end_substring)

        text = log[start_index:end_index]

        print (text)

        lines = text.splitlines()

        for line in lines:
            if line_filter in line:
                if 'Type: ' in line:
                    index = line.find('Type: ') + len('Type: ')
                    type = int(line[index:])
                elif 'Port: ' in line:
                    index = line.find('Port: ') + len('Port: ')
                    port = int(line[index:])
                elif 'Secured: ' in line:
                    index = line.find('Secured: ') + len('Secured: ')
                    secured = int(line[index:])
                elif 'Address: ' in line:
                    index = line.find('Address: ') + len('Address: ')
                    address = line[index:]
                    if type == 1:
                        ip = address
                        if secured == 0:
                            non_secure_port = port
                        else:
                            secure_port = port
                    else:
                        mac = address

        network_ids.append(ip)
        network_ids.append(non_secure_port)
        network_ids.append(secure_port)
        network_ids.append(mac)

        return network_ids

    def read_spec_xml(self, path, searching_platform, searching_tctype, searching_module, searching_testsuite):

        cnt = 0
        data = dict()
        searching_platform = searching_platform.upper()
        searching_tctype = searching_tctype.upper()
        searching_module = searching_module.upper()

        try:
            doc = ElementTree(file=path)
            testspec = doc.find('testspec')

            for platform in testspec.findall('platform'):
                platform_name = platform.get('name')
                print ('platform_name: ' + platform_name)
                if platform_name != searching_platform:
                    continue

                for tctype in platform.findall('type'):
                    type_name = tctype.get('name')
                    print ('type_name: ' + type_name)
                    if type_name != searching_tctype:
                        continue
                    data[type_name] = dict()

                    for module in tctype.findall('module'):
                        module_name = module.get('name')
                        print ('module_name: ' + module_name)
                        if module_name != searching_module:
                            continue

                        for testsuite in module.findall('testsuite'):
                            suite_name = testsuite.get('name')
                            print ('suite_name: ' + suite_name)
                            if suite_name != searching_testsuite:
                                continue
                            list_tc = []
                            for testcase in testsuite.findall('testcase'):
                                print ('testcase: ' + testcase.get('name'))
                                cnt += 1
                                # print( str(cnt) + ': ' + testcase.get(GT_ATT.NAME))
                                list_tc.append(testcase.get('name'))
                            # data[type_name][suite_name] = list_tc
                            return list_tc

        except ParseError:
            print("There is a Parse Error on " + path)

        return []

    # PM Module Keywords
    def create_pm_cbor_files(self, src_dir, dest_dir, json2cbor_tool_dir):
        """ Create CBOR Files from JSON and keep it in required directories
        :param dest_dir: Directory where JSON files are kept
        :param dest_dir: Directory where CBOR files will be  kept as backup
        :param json2cbor_tool_dir: Directory where json2Cbor tool exists
        :return: void
        """

        json_files_src = []
        cbor_files_src = []

        json_file_list = glob.iglob(os.path.join(src_dir, '*.json'))

        for json_file in json_file_list:
            json_files_src.append(json_file)
            cbor_file_temp = json_file[:-5] + '.dat'
            cbor_files_src.append(cbor_file_temp)

        os.chdir(json2cbor_tool_dir)

        i = 0
        for i in range(len(json_files_src)):
            json_file = json_files_src[i]
            cbor_file = cbor_files_src[i]
            cmd = "./json2cbor " + json_file + " " + cbor_file
            os.system(cmd)
            i += 1

        for cbor_file in cbor_files_src:
            if os.path.isfile(cbor_file):
                shutil.copy2(cbor_file, dest_dir)
                shutil.copy2(cbor_file, dest_dir + os.sep + 'linux')

    # CA Module Keywords
    def get_junit_tc_names(self, tc_src_path, tc_type, suite_name):

        import re

        JUNIT_TC_PATTERN = re.compile(r'public\s+void\s+(?P<testcase>(Test|test)\w+)\s*\(\s*\)\s*\{', re.DOTALL)

        filename = tc_src_path + os.sep + tc_type + os.sep + suite_name + ".java"

        fp = open(filename, 'r')
        source = fp.read()

        suites = []

        for tc in JUNIT_TC_PATTERN.findall(source):
            suites.append(tc[0])

        return suites

    def get_split_strings(self, text, delimiter):
        return text.split(delimiter)

    def get_collection_length(self, *items):
        return len(items)

    def get_list(self, *items):
        col = []
        for item in items:
            if type(item) is list:
                for t_item in item:
                    col.append(t_item)
            else:
                col.append(item)
        return col

    def get_concatenated_string(self, dlm, *items):
        s = str(items[0])
        for i in range(1, len(items)):
            s += dlm
            s += str(items[i])
        return s

    def trim_string(self, s):
        return s.strip()

    def remove_extra_space(self, s1):
        s2 = ''
        if type(s1) == list:
            s3 = s1
        elif type(s1) == str or type(s1) == unicode:
            s3 = s1.split()

        print ('type: ' + str(type(s1)))

        for s in s3:
            if s2:
                s2 += ' '
            s2 += s

        return s2

    def get_file_name_from_run_command(self, s):
        s = s.replace('java -jar ', '')
        s = s.replace('stdbuf -oL ', '')
        s = s.replace('./', '')
        end_index = len(s)
        index = s.find(' ')
        if index >= 0:
            end_index = index
        s = s[:end_index]
        sa = s.split(os.sep)
        return sa[len(sa)-1]

    def get_ip_address(self, network_interface):
        f = os.popen('ifconfig %s | grep "inet\ addr" | cut -d: -f2 | cut -d" " -f1' % network_interface)
        return f.read().strip()

    def get_ipv6_address(self, network_interface):
        command = "ifconfig %s | awk '/inet6/{print $3}'" % network_interface
        f = os.popen(command)
        addr = f.read().strip()
        pos = addr.find('/')
        addr = addr[:pos]
        addr += '%' + network_interface
        return addr


    def update_common_config_file(self, file_name, *items):

        index = 0
        text = ''
        keys = []
        while index < len(items):
            text += items[index] + '=' + items[index+1] + '\n'
            keys.append(items[index])
            index += 2

        fp = open(file_name, 'r')

        for line in fp:
            key_value = line.split('=')
            if len(key_value) != 2:
                continue
            if key_value[0] in keys:
                continue
            text += line

        fp.close()

        fp = open(file_name, 'w')
        fp.write(text)
        fp.close()


    def is_build_needed(self, build_path, command):

        os = 'linux'

        if 'gbsbuild' in command:
            os = 'tizen'

        command = re.sub('-j\s+\d', '', command)

        params = command.split(' ')

        for param in params:
            if param.startswith('TARGET_OS='):
                os=param[len('TARGET_OS='):]
                break

        command = re.sub('TARGET_OS=[A-Za-z]+', '', command)

        current_command = {}

        for param in params:
            if not param.startswith('TARGET_OS='):
                split_param = param.split('=')
                param_name = split_param[0]
                param_value = ''
                if len(split_param) > 1:
                    param_value = split_param[1]
                current_command[param_name] = param_value

        if not build_path in self.build_commands.keys():
            self.build_commands[build_path] = {}

        if not os in self.build_commands[build_path].keys():
            self.build_commands[build_path][os] = {}

        build_needed = False

        if len(current_command.keys()) != len(self.build_commands[build_path][os]):
            build_needed = True

        for param_name in current_command.keys():
            param_value = current_command[param_name]
            if not param_name in self.build_commands[build_path][os].keys():
                build_needed = True
            elif self.build_commands[build_path][os][param_name] != param_value:
                build_needed = True

        return build_needed


    def add_build_command(self, build_path, command):

        os = 'linux'

        if 'gbsbuild' in command:
            os = 'tizen'

        command = re.sub('-j\s+\d', '', command)

        params = command.split(' ')

        for param in params:
            if param.startswith('TARGET_OS='):
                os=param[len('TARGET_OS='):]
                break

        command = re.sub('TARGET_OS=[A-Za-z]+', '', command)

        current_command = {}

        for param in params:
            if not param.startswith('TARGET_OS='):
                split_param = param.split('=')
                param_name = split_param[0]
                param_value = ''
                if len(split_param) > 1:
                    param_value = split_param[1]
                current_command[param_name] = param_value

        self.build_commands[build_path][os] = current_command

    def get_os_from_build_command(self, command):
        os = 'linux'
        if 'gbsbuild' in command:
            os = 'tizen'
        elif 'TARGET_OS=android' in command:
            os = 'android'
        elif 'TARGET_OS=tizen' in command:
            os = 'tizen'
        return os

    def is_substring_count_match(self, log, cnt, op, sub):

        cnt_found = log.count(sub)
        cnt_expected = int(cnt)

        print ('found: ' + str(cnt_found) + ' , expected: ' + str(cnt_expected))

        if op == '=':
            return cnt_found == cnt_expected
        if op == '!=':
            return cnt_found != cnt_expected
        if op == '>=':
            return cnt_found >= cnt_expected
        if op == '<=':
            return cnt_found <= cnt_expected
        if op == '>':
            return cnt_found > cnt_expected
        if op == '<':
            return cnt_found < cnt_expected

    def get_java_proxy_flag(self, http_proxy_address, https_proxy_address):

        flag = ''
        
        http_proxy_address = http_proxy_address.split(':')
        if len(http_proxy_address) == 2:
            flag += '-Dhttp.proxyHost=' + http_proxy_address[0] + ' -Dhttp.proxyPort=' + http_proxy_address[1] + ' '
        
        https_proxy_address = https_proxy_address.split(':')
        if len(https_proxy_address) == 2:
            flag += '-Dhttps.proxyHost=' + https_proxy_address[0] + ' -Dhttps.proxyPort=' + https_proxy_address[1]
            
        return flag
        
    def get_tizen_build_success_text(self, build_command):
        s1 = 'done building targets.'
        s2 = 'Build is successful'
        if 'scons ' in build_command:
            return s1
        return s2

    def notify_loggers(self, txt):
        print ('console logger printing the log')
        print (txt)
        self.log_fp.write(str(txt) + '\n')
        self.log_fp.flush()

    def match_any_string(self, txt, *list_of_substring):
        for x in list_of_substring:
            if x in txt:
                return True
        return False


    def write_robot_execution_status(self, execution_part, os_type, module, tc_type, status):
        delimiter = ':'

        print (self.robot_root)

        directory = os.path.join(self.robot_root, 'report')

        if not os.path.exists(directory):
            os.makedirs(directory)

        execution_path = os.path.join(directory, 'robot_execution_status.txt')

        fp = open(execution_path, 'a')

        print (execution_path)

        print (execution_part, os_type, module, tc_type, status)

        if status:
            verdict = 'pass'
        else:
            verdict = 'fail'

        fp.write(verdict + delimiter + module + delimiter + os_type + delimiter + execution_part + delimiter + tc_type + '\n')
        fp.flush()
        fp.close()


    def write_build_log(self, os_type, module, tc_type, build_command, status, log, build_part):

        directory = os.path.join(self.robot_root, 'report', tc_type, os_type)

        if not os.path.exists(directory):
            os.makedirs(directory)

        suffix = ''
        params = build_command.split()
        for param in params:
            pair = param.split('=')
            if len(pair) <= 1:
                continue

            key = pair[0].lower()
            value = pair[1].lower()

            if key == 'secured':
                if value == '1':
                    suffix += '_secured'
                else:
                    suffix += '_non-secured'
            elif key == 'release':
                if value == '1':
                    suffix += '_release'
                else:
                    suffix += '_debug'
            elif key == 'target_transport':
                value = value.replace(',', '-')
                suffix += '_' + value
            elif key == 'framework':
                suffix += '_' + value

        filepath = os.path.join(directory, 'build' + '_' + build_part + '_' + module + suffix + '.txt')

        print (self.robot_root, filepath, os_type, module, tc_type)

        fp = open(filepath, 'w')

        if status:
            verdict = 'pass'
        else:
            verdict = 'fail'

        content = 'Build Command: ' + build_command + '\n\n'
        content += 'Build Status: ' + verdict + '\n\n'
        content += 'Build Log:\n' + log + '\n'

        fp.write(content)
        fp.flush()
        fp.close()


    def write_iotivity_build_log(self, os_type, module, tc_type, build_command, status, log):
        self.write_build_log(os_type, module, tc_type, build_command, status, log, 'iotivity')

    def write_test_build_log(self, os_type, module, tc_type, build_command, status, log):
        self.write_build_log(os_type, module, tc_type, build_command, status, log, 'test')

    def write_precondition_log(self, os_type, module, tc_type, no):

        directory = os.path.join(self.robot_root, 'report', tc_type, os_type)

        if not os.path.exists(directory):
            os.makedirs(directory)

        run_command = self.simulator_list[no].run_command

        if 'tizen' == os_type.lower():
            if 'sdb ' in run_command and ' shell ' in run_command:
                s = '/opt/usr/media/bin '
                index = run_command.find(s)
                if index >= 0:
                    run_command = run_command[index+len(s):].strip()

        binary_filename = self.get_file_name_from_run_command(run_command)

        filepath = os.path.join(directory, 'pre_condition' + '_' + module + '_' + binary_filename  + '.txt')

        print (self.robot_root, filepath, os_type, module, tc_type)

        fp = open(filepath, 'w')

        if self.simulator_list[no].status:
            verdict = 'pass'
        else:
            verdict = 'fail'

        content = 'Execution Directory: ' + self.simulator_list[no].cmd_dir + '\n\n'
        content += 'Execution Command: ' + self.simulator_list[no].run_command + '\n\n'
        content += 'Expected Log: ' + self.simulator_list[no].expected_log + '\n\n'
        content += 'Execution Status: ' + verdict + '\n\n'
        content += 'Execution Procedure:\n' + self.get_simulator_procedure(binary_filename) + '\n\n'
        content += 'Execution Log:\n' + self.get_simulator_log(no) + '\n'

        fp.write(content)
        fp.flush()
        fp.close()

    def get_simulator_procedure(self, binary_name):
        procedure_text = 'No procedure found'

        try:
            procedure_filepath = os.path.join(self.robot_root, '..', '..', '..', 'res', 'procedure', binary_name + '.txt')

            if os.path.exists(procedure_filepath):
                procedure_text = open(procedure_filepath, 'r').read()
            else:
                src_file_path = ''
                file_list = []
                for root, dirs, files in os.walk(os.path.join(self.robot_root, '..', '..', '..', 'src')):
                    for file_name in files:
                        short_name = os.path.basename(os.path.join(root, file_name))
                        if short_name.endswith('.cpp') or short_name.endswith('.c'):
                            if binary_name in short_name:
                                src_file_path = os.path.join(root, file_name)
                            elif binary_name.lower() in short_name.lower():
                                src_file_path = os.path.join(root, file_name)
                            elif binary_name.lower().replace('_', '') in short_name.lower():
                                src_file_path = os.path.join(root, file_name)
                if src_file_path:
                    print(src_file_path)
                    contents = open(src_file_path, 'r').read()
                    p = re.compile(r'/\*\*[\s]+.+\*/\s*int\s*main\s*\(', re.DOTALL)
                    mat = re.search(p, contents)
                    if mat:
                        contents = contents[mat.start() : mat.end()]
                        contents = contents.replace('/**', '').replace('*/', '')
                        contents = re.sub(r'[ ]*\*', '', contents)
                        procedure_text = re.sub(r'int\s*main\s*\(', '', contents).strip()
        except:
            print("Unexpected error:", sys.exc_info()[0])

        return procedure_text

    def add_simulator(self, os_type, device_name, app_name, app_command, dlm):
        app_commands = app_command.split(dlm)
        cmd_dir = app_commands[0]
        run_command = app_commands[1]
        expected_log = app_commands[2]
        print(cmd_dir)
        print(run_command)
        print(expected_log)
        self.simulator_list.append(Simulator(os_type, device_name, app_name, cmd_dir, run_command, expected_log, app_command))

    def clear_simulator_list(self):
        del self.simulator_list[:]

    def total_simulator_count(self):
        return len(self.simulator_list)

    def get_simulator_info(self, no):
        simulator = self.simulator_list[no]
        return simulator.os_type, simulator.device_name, simulator.app_name, simulator.run_command, simulator.expected_log, simulator.status

    def get_simulator_device_name(self, no):
        return self.simulator_list[no].device_name

    def get_simulator_app_name(self, no):
        return self.simulator_list[no].app_name

    def get_simulator_app_command(self, no):
        return self.simulator_list[no].app_command

    def get_simulator_run_command(self, no):
        return self.simulator_list[no].run_command

    def set_simulator_status(self, no, status):
        self.simulator_list[no].status = status

    def set_simulator_log(self, no, log):
        self.simulator_list[no].log_saved = True
        self.simulator_list[no].log = log

    def get_simulator_log(self, no):
        if not self.simulator_list[no].log_saved:
            try:
                self.simulator_list[no].log = self.get_application_log(self.simulator_list[no].device_name, self.simulator_list[no].app_name)
                self.simulator_list[no].log_saved = True
            except:
                print('causing problem when calling get_application_log')
        return self.simulator_list[no].log


    def set_robot_execution_status(self, os_type, module, tc_type, status):

        print (os_type, module, tc_type, status)

        if os_type not in self.robot_execution_status:
            self.robot_execution_status[os_type] = {}
        if module not in self.robot_execution_status[os_type]:
            self.robot_execution_status[os_type][module] = {}

        self.robot_execution_status[os_type][module][tc_type] = int(status)


    def check_robot_execution_status(self, path, os_type, module, tc_type):
        self.set_robot_root(path)

        if os_type not in self.robot_execution_status:
            return
        if module not in self.robot_execution_status[os_type]:
            return
        if tc_type not in self.robot_execution_status[os_type][module]:
            return
        if self.robot_execution_status[os_type][module][tc_type] == Execution_Status.BUILD_IOTIVITY_FAIL:
            self.write_robot_execution_status(Execution_Status.BUILD_IOTIVITY, os_type, module, tc_type, False)
        elif self.robot_execution_status[os_type][module][tc_type] == Execution_Status.BUILD_TEST_FAIL:
            self.write_robot_execution_status(Execution_Status.BUILD_TEST, os_type, module, tc_type, False)
        elif self.robot_execution_status[os_type][module][tc_type] == Execution_Status.TEST_PRE_CONDITION_FAIL:
            self.write_robot_execution_status(Execution_Status.TEST_PRE_CONDITION, os_type, module, tc_type, False)

    def store_binary(self, iotivity_root, release_dir, os_type, tc_type, module):
        backup_path = os.path.join(self.robot_root, 'report', tc_type, os_type, 'bin_' + module)
        bin_path = os.path.abspath(os.path.join(self.robot_root, '..', '..', '..', 'bin'))

        if os.path.exists(backup_path):
            shutil.rmtree(backup_path)

        os.makedirs(backup_path)

        print (os_type.lower())

        os_module_bin_path = bin_path + os.sep + os_type.lower() + os.sep + module

        if 'tizen' in os_type.lower():
            os.system('find ' + os_module_bin_path + ' -name \*.rpm -exec cp {} ' + backup_path + ' \;')
        elif 'android' in os_type.lower():
            os.system('find ' + os_module_bin_path + ' -name \*-debug.apk -exec cp {} ' + backup_path + ' \;')
        
        os.system('cp -r ' + bin_path + os.sep + 'linux' + os.sep + '* ' + backup_path)

        out_dir = os.path.join(iotivity_root, 'out', 'linux')
        os.system('find ' + out_dir + os.sep + '*' + os.sep + release_dir + ' -name \*.so -exec cp {} ' + backup_path + ' \;')

    def set_robot_root(self, path):
        self.robot_root = path
        print (self.robot_root)
        
    def make_tc_list(self, directory, file_name):
        file_path=os.path.join(directory, file_name)
        f = open(file_path,'r')
        contents = f.read()
        print(contents)
        f.close()

        self.current_tc_list[:] = []
        prefix_list = [' -f ', ' -u ', ' -c ', ' -g ']
        param_list = ['binary_name', 'suite_name', 'tc_name', 'package_name']
        contents = contents.strip()
        for row in contents.split('\n'):
            command = ''
            for info in row.split(','):
                for i in range(0, len(param_list)):
                    if info.split(':')[0].strip() == param_list[i] and len(info.split(':')[1].strip()) != 0:
                        command += ' ' + str(prefix_list[i] + info.split(':')[1].strip())
                        break
            command = command.strip()
            command = re.sub(' +', ' ', command)
            self.current_tc_list.append(command)
            self.tc_verdict_map[command] = {}
            self.tc_verdict_map[command][Execution_Status.PASS_STATUS_INDEX] = 0
            self.tc_verdict_map[command][Execution_Status.FAIL_STATUS_INDEX] = 0
            self.tc_verdict_map[command][Execution_Status.TIMEOUT_STATUS_INDEX] = 0
            self.tc_verdict_map[command][Execution_Status.UNEXPECTED_STATUS_INDEX] = 0
            self.tc_verdict_map[command][Execution_Status.TOTAL_STATUS_INDEX] = 0

    def purge_tc_list(self):
        temp_list = []
        for command in self.current_tc_list:
            if self.tc_verdict_map[command][Execution_Status.PASS_STATUS_INDEX] >= 1:
                del self.tc_verdict_map[command]
                continue
            if self.tc_verdict_map[command][Execution_Status.TIMEOUT_STATUS_INDEX] >= 2:
                del self.tc_verdict_map[command]
                continue
            if self.tc_verdict_map[command][Execution_Status.TOTAL_STATUS_INDEX] >= 3:
                del self.tc_verdict_map[command]
                continue
            temp_list.append(command)

        self.current_tc_list[:] = []
        self.current_tc_list = temp_list

    def is_tc_list_exhausted(self):
        return len(self.current_tc_list) == 0

    def get_tc_list(self):
        return self.current_tc_list

    def set_tc_status(self, command, log):
        print(command)
        print(log)
        self.tc_verdict_map[command][Execution_Status.TOTAL_STATUS_INDEX] += 1
        start_text = '[ Result of '
        end_text = ' ]'
        start_pos = log.find(start_text)
        print(start_pos)
        if start_pos == -1:
            return
        log = log[start_pos+len(start_text):]
        end_pos = log.find(end_text)
        print(end_pos)
        line = ''
        if end_pos >= 0:
            line = log[:end_pos]
        print(line)
        if line:
            pos = line.find(':')
            print(pos)
            index = -1
            if pos >= 0:
                verdict = line[pos+1:]
                verdict = verdict.strip().lower()
                print('verdict: ' + verdict)
                if 'pass' == verdict:
                    index = Execution_Status.PASS_STATUS_INDEX
                elif 'fail' == verdict:
                    index = Execution_Status.FAIL_STATUS_INDEX
                elif 'timeout' == verdict:
                    index = Execution_Status.TIMEOUT_STATUS_INDEX
                elif 'unexpected' == verdict:
                    index = Execution_Status.UNEXPECTED_STATUS_INDEX
            if index >= 0:
                self.tc_verdict_map[command][index] += 1

    def remove_param_from_command(self, command, filter_param):
        params = command.split()

        k = -1
        for i in range(0, len(params)):
            if params[i] == filter_param:
                k = i
                break
        if k == -1:
            return command

        command = ''
        for i in range(0, len(params)):
            if i < k or i > k+1:
                command += params[i] + ' '

        return command.strip()

    def get_test_manager_file_size(self, folder_path, prefix, suffix):
        sz = 0
        onlyfiles = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]
        for file_name in onlyfiles:
            if file_name.startswith(prefix) and file_name.endswith(suffix):
                print(file_name)
                sz += os.path.getsize(os.path.join(folder_path, file_name))
        return sz


    def run_test_manager(self, robot_root, tc_type, os_type, module, cmd_dir, command):

        command = str(command)
        print(cmd_dir)
        print(command)

        current_dir = os.getcwd()

        os.chdir(cmd_dir)

        directory = os.path.join(robot_root, 'report', tc_type, os_type)

        if not os.path.exists(directory):
            os.makedirs(directory)

        prefix = 'test_manager' + '_' + module + '_' + os_type
        suffix = '.log'

        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)

        def pull_output():
            while(True):
                ret_code = proc.poll() #returns None while subprocess is running
                if ret_code is not None:
                    print('breaking output pulling')
                    break

        t = threading.Thread(target=pull_output)
        t.start()

        minute = 60
        prev_size = 0
        while t.is_alive():
            t.join(10 * minute)
            print('current time:', strftime("%Y-%m-%d %H:%M:%S", gmtime()))
            current_size = self.get_test_manager_file_size(directory, prefix, suffix)
            print('prev_size:', prev_size)
            print('current_size:', current_size)
            if current_size == prev_size:
                print ('hang detected')
                proc.kill()
                proc.terminate()
                break
            prev_size = current_size

        os.chdir(current_dir)

    def get_verdict_file_suffix(self, secured, command):
        suffix = 'secured'
        if secured == '0':
            suffix = 'non-secure'

        params = command.split()
        for i in range (1, len(params), 2):
            if params[i] == '-x':
                suffix += '_' + params[i+1]
            if params[i] == '-n':
                suffix += '_' + params[i+1]

        print (suffix)

        return suffix


    def execute_shell_command(self, cmd_dir, command, success_text):

        command = str(command)
        print(cmd_dir)
        print(command)

        current_dir = os.getcwd()

        os.chdir(cmd_dir)

        filepath = 'temp_shell_command_19900413.log'

        fp = open(filepath, 'w') 

        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)

        def pull_output():
            while(True):
                ret_code = proc.poll() #returns None while subprocess is running
                if ret_code is not None:
                    break
                line = proc.stdout.readline().strip()
                print(line)
                fp.write(str(line) + '\n')
                fp.flush()

        t = threading.Thread(target=pull_output)
        t.start()

        flag = True
        minute = 60
        prev_size = 0
        while t.is_alive():
            t.join(10 * minute)
            print('current time:', strftime("%Y-%m-%d %H:%M:%S", gmtime()))
            current_size = os.path.getsize(filepath)
            print('prev_size:', prev_size)
            print('current_size:', current_size)
            if current_size == prev_size:
                print ('hang detected')
                break
            prev_size = current_size

        fp.close()

        fp = open(filepath, 'r')
        content = fp.read()
        fp.close()

        if success_text and success_text not in content:
            flag = False

        os.chdir(current_dir)

        return flag, content

    def verify_build_success(self, build_command, build_log):

        if 'scons' in build_command:
            if 'scons: building terminated because of errors.' in build_log:
                return False

        return True


    def get_build_info(self, command, secured):

        i = 0
        build_param = ''
        params = command.split()
        for param in params:
            if '-b' == param:
                build_param = params[i+1]
                break
            i += 1

        if not build_param:
            if True == secured or '1' == secured:
                build_param = 'secured'
            else:
                build_param = 'non-secured'

            build_param = '-b ' + build_param

        return build_param

def main():
    module = 're'
    os_type = 'android'
    prefix = 'test_manager' + '_' + module + '_' + os_type
    suffix = '.log'
    directory = '/home/srbd/Downloads/release/log_android/robot_fw_log'
    a = DeviceControlKeyword()
    print(a.get_test_manager_file_size(directory, prefix, suffix))

if __name__== "__main__":
    main()

