#!/usr/bin/python

#******************************************************************
#
# Copyright 2016 Intel Corporation All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

import os
import sys
import textwrap
import argparse
import platform
import subprocess
import multiprocessing
import time
import psutil

def find_avalible_console_port():
    '''
    Find an open port number that will be used for the avd console port. Start searching for the
    at port number 5554 and continue incrementing port number till and open port is found.
    Returns port number as a string
    '''
    # 5554 is the default console port used when starting an avd without specifying a port number
    # since this is the default port number by default we start checking to see if that port is
    # currently in use. If it is being used increase the port to the next even port number.
    # Each instance of the avd uses two ports the console port and the avd port.The avd port is
    # always console port +1 so we will check that that port is also open.
    ret_port = 5554
    nc = psutil.net_connections(kind='inet')
    while True:
        console_port_already_being_used = False
        adb_port_already_being_used = False
        for i in nc:
            if(i.laddr[1] == ret_port):
                console_port_already_being_used = True
            if(i.laddr[1] == (ret_port + 1)):
                adb_port_already_being_used = True
        if((not console_port_already_being_used) and (not adb_port_already_being_used)):
            return str(ret_port)
        ret_port += 2 #for adv the port must be a multiple of 2

def start_avd(avd_name, console_port):
    '''
    Start up the avd specified by the avd_name parameter use the specify the console port that the avd will use
    with the console_port parameter.  The find_avalible_console_port() function should be used to determine an
    open console_port that can be passed into this function.
    note:
        - all data on the avd will be wiped to start with a known starting condition.
        - the avd will be started with the no-window option so there is no visual indication
          that the avd is launched.

    Keyword arguments:
    avd_name -- the name of the created android virtual device
    console_port -- the port number that will attempt to be used
                    by the when starting the avd
    '''
    command = 'emulator -avd ' + avd_name + ' -port ' + console_port + ' -wipe-data -no-boot-anim -no-window'
    subprocess.Popen([command], shell=True)

def wait_for_avd_boot(console_port):
    '''
    After calling start_avd this function is used to wait for the avd to complete booting the console_port
    option must match the console_port option used to start the avd

    Keyword arguments:
    console_port -- the port number that was specified when starting the avd
    '''
    #dev_cmd = 'adb -s emulator-' + console_port + ' shell getprop dev.bootcomplete'
    #sys_cmd = 'adb -s emulator-' + console_port + ' shell getprop sys.boot_completed'
    dev_cmd = ['adb', '-s', 'emulator-' + console_port, 'shell', 'getprop', 'dev.bootcomplete']
    wait_for_boot = True
    while wait_for_boot:
        adb_process = subprocess.Popen(['adb', '-s', 'emulator-' + console_port, 'shell', 'getprop', 'dev.bootcomplete'],
                                       stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = adb_process.communicate()
        #print 'output = ' + str(output) + ' error = ' + str(error) + ' return code = ' + str(adb_process.returncode)
        if(adb_process.returncode == 0):
            if(output.startswith('1')):
                print('property dev.bootcomplete indicates that the avd boot has completed')
                wait_for_boot = False
            else:
                #print('Waiting for emulator to start')
                time.sleep(1);
        else:
            #print('Waiting for emulator to start')
            time.sleep(1)
    wait_for_boot = True
    while wait_for_boot:
        adb_process = subprocess.Popen(['adb', '-s', 'emulator-' + console_port, 'shell', 'getprop', 'sys.boot_completed'],
                                        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = adb_process.communicate()
        #print 'output = ' + str(output) + ' error = ' + str(error) + ' return code = ' + str(adb_process.returncode)
        if(adb_process.returncode == 0):
            if(output.startswith('1')):
                print('property sys.boot_completed indicates that the avd boot has completed')
                wait_for_boot = False
            else:
                #print('Waiting for emulator to start')
                time.sleep(1)
        else:
            #print('Waiting for emulator to start')
            time.sleep(1)

def build_smoketests():
    '''
    Use gradlew to build the android smoke tests
    '''
    os.environ['ANDROID_NDK_HOME'] = os.path.abspath(os.getcwd() + '/../../extlibs/android/ndk/android-ndk-r10e')
    command = './gradlew assembleAndroidTest'
    subprocess.Popen([command], shell=True).wait()

def install_smoketests(console_port):
    '''
    Install the android smoke tests. Must run build_smoketests() before running this function

    Keyword arguments:
    console_port -- the port number that was specified when starting the avd
    '''
    command = 'adb -s emulator-' + console_port + ' install -r ./build/outputs/apk/iotivity-android-debug-androidTest-unaligned.apk'
    subprocess.Popen([command], shell=True).wait()

def run_smoketests(console_port):
    '''
    run the android smoke test

    Keyword arguments:
    console_port -- the port number that was specified when starting the avd
    '''
    command = 'adb -s emulator-' + console_port + ' shell am instrument -w org.iotivity.base.test/android.test.InstrumentationTestRunner'
    print command
    subprocess.Popen([command], shell=True).wait()

def kill_avd(console_port):
    '''
    shut down the avd

    Keyword arguments:
    console_port -- the port number that was specified when starting the avd
    '''
    command = 'adb -s emulator-' + console_port + ' emu kill'
    subprocess.Popen([command], shell=True).wait()

def create_avd(avd_name, target, abi):
    '''
    Create a new android virtual device

    Keyword arguments:
    avd_name -- the name of the created avd
    target -- the target Target ID of the system image to use with the new AVD. example android-21
    abi -- the architecture type for the avd example armeabi, x86, or x86_64

    run command $android list targets to get a list of targets and there Tag/ABIs
    '''
    command = ['android', '-s', 'create', 'avd', '-f', '--name', avd_name, '--target', target, '--abi', abi]
    p = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Just use the default avd no need to specify extra options.
    p.communicate('no')
    p.wait()

def delete_avd(avd_name):
    command = ['android', '-s', 'delete', 'avd', '--name', avd_name]
    subprocess.Popen(command).wait();

def start_android_and_run_tests(target, abi):
    '''
    This function does the following
    1. creates a new avd named smoke_test_avd_####
       where the #### is the port number that is used to talk with the avd
       the port number is assigned automatically.
    2. starts the created avd
    3. waits for the avd to boot
    4. builds android smoke tests
    5. installs the smoke tests on the avd
    6. runs the smoke tests
    7. shuts down the avd
    8. deletes the avd

    Keyword arguments:
    avd_name -- the name of the created avd
    target -- the target Target ID of the system image to use with the new AVD. example android-21
    abi -- the architecture type for the avd example armeabi, x86, or x86_64

    run command $android list targets to get a list of targets and there Tag/ABIs
    '''
    avalible_port = find_avalible_console_port()
    avd_name = 'smoke_test_avd_' + avalible_port

    create_avd(avd_name, target, abi)

    start_avd(avd_name, avalible_port)
    wait_for_avd_boot(avalible_port)

    build_smoketests();
    install_smoketests(avalible_port)
    run_smoketests(avalible_port)

    kill_avd(avalible_port)
    delete_avd(avd_name)

def main(argv):
    target = 'android-21'
    abi = 'x86_64'

    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, epilog=textwrap.dedent('''\
                                      If ABI is not specified it will default to x86_64.
                                      If TARGET is not specified it will default to android-21.'''))
    parser.add_argument('-a', '--abi', help='specify the abi of the android avd example "x86_64"')
    parser.add_argument('-t', '--target', help='the andrdoid target example "android-21"')
    args = parser.parse_args()

    if (args.abi != None):
        abi = args.abi
    if (args.target != None):
        target = args.target

    print '*****************************************************'
    print 'Running andriod smoke test with the following options'
    print '    The android target is -- ', target
    print '    The android abi is -- ', abi
    print '*****************************************************'

    start_android_and_run_tests(target, abi)

if __name__ == "__main__":
   main(sys.argv[1:])
