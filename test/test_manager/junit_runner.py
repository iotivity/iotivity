#!/usr/bin/python
'''
/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

import sys
import os
from os import listdir
from os.path import isfile, join
import re
import optparse
from configuration import *
import datetime as dt
from time import time
import subprocess
import glob

oparser = optparse.OptionParser()
oparser.add_option("-m", action="store", dest="module_name")
oparser.add_option("--module", action="store", dest="module_name")
oparser.add_option("-k", action="store", dest="type_name")
oparser.add_option("--kind", action="store", dest="type_name")
oparser.add_option("-p", action="store", dest="platform")
oparser.add_option("--platform", action="store", dest="platform")
oparser.add_option("-t", action="store", dest="target")
oparser.add_option("--target", action="store", dest="target")
oparser.add_option("-s", action="store", dest="suite")
oparser.add_option("--suite", action="store", dest="suite")
oparser.add_option("-c", action="store", dest="testcase")
oparser.add_option("--case", action="store", dest="testcase")
oparser.add_option("-d", action="store", dest="device_name")
oparser.add_option("--device", action="store", dest="device_name")
oparser.add_option("-r", action="store", dest="is_marge")
oparser.add_option("--merge", action="store", dest="is_marge")
oparser.add_option("-b", action="store", dest="build_target")
oparser.add_option("--buildtarget", action="store", dest="build_target")
oparser.set_defaults(platform='android', target='21', build_target='target')
opts, args = oparser.parse_args()

platform = opts.platform
target = opts.target
module_name = opts.module_name
type_name = opts.type_name
suite_name = opts.suite
testcase_name = opts.testcase
device_name = opts.device_name
build_target_dir = opts.build_target
is_marge = opts.is_marge
EXECUTABLE_DIRECTORY = ""
suites = []
tc_count = 0
if module_name == 'ic' :
    EXECUTABLE_DIRECTORY = 'bin/android/%s/intermediates/classes/debug/org/iotivity/cloud/%s/test/%s' % (module_name,  module_name,
type_name)
    base_package = "org.iotivity.cloud."
else :
    EXECUTABLE_DIRECTORY = 'bin/android/%s/intermediates/classes/debug/org/iotivity/service/%s/test/%s' % (module_name,  module_name,
type_name)
    base_package = "org.iotivity.service."

if build_target_dir == 'sec':
    EXECUTABLE_ROOT_DIR = '../IotivitySECTest_New/'
else :
    EXECUTABLE_ROOT_DIR = "../"

build_dir = EXECUTABLE_ROOT_DIR+EXECUTABLE_DIRECTORY

TEST_RESULT_DIR_ANDROID = TEST_RESULT_DIR + "/android/"+module_name+"/"+type_name
LOG_DIRECTORY = TEST_RESULT_DIR + "/android/"+module_name+"/"+"log"+"/"+type_name
date = dt.datetime.today().strftime("%Y%m%d")
time_stamp = str(time())
result_file_name = platform+"_"+target+"_"+date+"_"+time_stamp+"_iotivity_"+module_name+"_"+type_name+".xml"

package_name = base_package+module_name+".test"

print "Build Directory "+build_dir

full_suite_name= package_name+".%s.%s"%(type_name, suite_name)
savedPath = os.getcwd()

if device_name == None:
    device_name = ""
else:
    device_name = "-s " + device_name
#adb shell pm list instrumentation

def add_suites_to_list() :
    global suites
    suites = []
    if suite_name is not None and testcase_name is None:
        suites = opts.suite.split(',')
    elif suite_name is None and testcase_name is None:
        temp_suites = [f for f in listdir(build_dir) if isfile(join(build_dir, f))]
        for suite in temp_suites :
            if "$" not in suite :
                suite = suite.split('.',1)[0]
                suites.append(suite)
        print "\nSuites List going to execute : \n"
        print suites
    elif suite_name is not None and testcase_name is not None :
        suites.append(suite_name)

def clean_test_result_directory(directory) :
    if not os.path.exists(directory):
        os.makedirs(directory)
    else:
        if is_marge is None:
            files = glob.glob('%s/*'%(directory))
            for xml_file in files:
                os.remove(xml_file)

def write_log_into_file(tc_name) :
    rc = os.system("adb %s logcat -d > %s_log.txt" %(device_name, LOG_DIRECTORY+tc_name))
    os.chdir(savedPath)

def pull_result_file_from_device(result_file_name) :
     os.system("adb %s pull /data/data/%s%s.test/files/junit-report.xml %s" %(device_name, base_package, module_name, TEST_RESULT_DIR_ANDROID))
     os.chdir(TEST_RESULT_DIR_ANDROID)
     os.rename("junit-report.xml", result_file_name)
     os.chdir(savedPath)

def run_module() :
    global tc_count

    #add suites as requested
    add_suites_to_list()

    #getting suites fro suites list
    for suite in  suites:

        #change directory to build directory
        os.chdir(build_dir)
        testcases = []
        full_suite_name= package_name+".%s.%s"%(type_name, suite)
        print("Full suite name : "+full_suite_name)
        #checking if suite exist in build directory
        if os.path.isfile(suite+".class") :
            rc = os.system("find -name '%s.class' | xargs javap -p > %s.txt " % (suite,suite))
            classFile = open("%s.txt" % suite, "r")
            if rc != 0 :
                continue
        else :
             print "Suite : " +suite +" does not exist -----> Moving to next Suite"
             continue
        print "\nStart Running Suite : "+suite

        #Getting testcase list from suite
        for line in classFile:
            words = line.split()
            for word in  words:
                if word.startswith('Test') or word.startswith('test'):
                    testname = word.split('(')
                    testcases.append(testname[0])

        #close test suite class file
        classFile.close()

        #delete temp txt file
        if os.path.isfile(suite+".txt") :
            os.remove(suite+".txt")

        #this if block is for single test execution
        if testcase_name is not None :
            if  testcase_name in testcases :
                testcases = []
                testcases.append(testcase_name)
            else :
                print "TestCase : "+testcase_name +" does not exist------->ending test"
                system.exit(0)

        #getting testcase from testcase list and execute
        for method_name in testcases:
            print  "\nStarted Test : " +method_name
            p = subprocess.Popen(["adb %s shell am instrument -w  -e class  %s#%s %s/com.zutubi.android.junitreport.JUnitReportTestRunner"%(device_name,full_suite_name,method_name,package_name)], stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
            out, err = p.communicate()
            print out
            print err
            tc_count = tc_count+1;
            print  "Completed Test : " +method_name+"\n"
            time_stamp = str(time())
            result_file_name = method_name+"-"+suite +"-"+platform+"-"+target+"-"+date+"-"+time_stamp+"-iotivity-"+module_name+"-"+type_name+".xml"
            os.chdir(savedPath)
            write_log_into_file(method_name)
            os.chdir(savedPath)
            #Pulling testcase xml result file from device
            pull_result_file_from_device(result_file_name)
            print "Completed Running Suite : "+suite

def make_excel_report() :
    subprocess.call("python junit_reporter.py -m %s"%(module_name), shell=True)

if __name__ == '__main__':
    clean_test_result_directory(TEST_RESULT_DIR_ANDROID)
    clean_test_result_directory(LOG_DIRECTORY)
    run_module()
    print "\nTotal TC Count : ",tc_count
    print "\n=============== Test Execution Completed ===============\n"
    make_excel_report()

'''
#adb shell am instrument -w org.iotivity.service.phy.test/com.zutubi.android.junitreport.JUnitReportTestRunner
#Script running procedure
#python ./junit_runner.py -m phy -k stc -p android -t 21 -s PHYWiFiOnBoardingConfigTest -c testSetSharedKeyAndGetSharedKeyByGetConfig_GSRV_P
#./junit_runner.py -m pm -k itc -p android -t 21 -s PMOcProvisioningTest
#./junit_runner.py -m re -k utc -p android -t 21 -s REResourceBrokerTest
#./junit_runner.py -m rh -k utc -p android -t 21 -s ResourceHostingTest
#./junit_runner.py -m tm -k itc -p android -t 21 -s TMManagerTest
#/home/sk/gittest/oictest_repo/IotivityOrgSource/new/iotivity/IotivitySECTest/bin/android/phy -s PHYWiFiOnBoardingConfigTest -c TestSetSharedKeyAndGetSharedKeyByGetConfig_GSRV_P

#./junit_runner.py -m es -k btc -p android -t 21 -b target -s ESDevicePropTest -c testESSetWiFiPropWithInvalidArguments_ESV_NV_ETC_N
'''


