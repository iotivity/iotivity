#!/usr/bin/python

'''
/***************************************************************************************************
 * Copyright (c) 2017 Samsung Electronics Co., Ltd. All right reserved.
 *
 * This software is the confidential and proprietary information of Samsung Electronics, Inc.
 * ('Confidential Information'). You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered into with
 * Samsung Electronics.
 ***************************************************************************************************/
'''
import os
import sys
import glob
import shutil
import logging
import optparse
import subprocess
import datetime as dt
from configuration import *
from os.path import basename

try :
    import xlsxwriter
except :
    print('Install xlsxwriter by  sudo easy_install xlsxwriter or by another way')


oparser = optparse.OptionParser()
oparser.add_option('-b', action='store', dest='bit')
oparser.add_option('--bit', action='store', dest='bit')
oparser.add_option('-m', action='store', dest='mode')
oparser.add_option('--mode', action='store', dest='mode')
oparser.add_option('-a', action='store', dest='action_type')
oparser.add_option('--argument', action='store', dest='action_type')
oparser.add_option('--iotivity_root', action='store', dest='iotivity_root')

#./devunittest_runner.py -a nsbuild

#setting default value for command line arguments
oparser.set_defaults(iotivity_root = '../..', platform='linux', bit='64', mode = 'release')
opts, args = oparser.parse_args()

#Assigning command-line options value to global variables
iotivity_root = os.path.abspath(opts.iotivity_root)
build_mode = opts.mode
bit = opts.bit
target_arch = ''
action_type = opts.action_type

#checking user input is 32 or 64
if bit == '32' :
    target_arch = 'x86'
else :
    target_arch = 'x86_64'

print('Runner Target Source : ' + iotivity_root)
print('Runner Build Mode : ' + build_mode)
print('Runner BIT : ' + bit)

#set dev test result file directory
DEV_UNIT_TEST_RESULT_DIRECTORY = iotivity_root + '/out/linux/%s/%s/test_out/'% (target_arch, build_mode)
DEV_UNIT_TEST_RUN_DIRECTORY = iotivity_root
RUNNER_HOME_DIRECTORY = os.getcwd()
print(RUNNER_HOME_DIRECTORY)
log = None

#list to keep catagory(binary) wise not run
not_run_binary_dict = {}

module = {
'resource':['unittests'],
'resource/c_common':['malloctests', 'randomtests', 'stringtests', 'timetests'],
'resource/csdk/connectivity': ['catests'],
'resource/csdk/security':['security_unittest'],
'resource/csdk/security/provisioning':['unittest'],
'resource/csdk/security/provisioning/ck_manager' :['unittest'],
'resource/csdk/stack':['stacktests'],
'resource/provisioning': ['provisiontests']
}

def addCoverageFlag() :
    print ('Adding flag to scons script\n')
    flag_add_command = './add_gcov_flag.sh %s'%iotivity_root
    execute_command(flag_add_command)
    print ('Adding flag to scons script Finished\n')

def generateCoverageReport() :
    print ('Start generating coverage report....\n')
    flag_add_command = './generate_gcov_report_n_clean.sh %s'%iotivity_root
    execute_command(flag_add_command)
    print ('....generating coverage report Finished\n')

def build_non_secured_module() :
    print ('Start Non-secured Modules Build....\n')
    non_secured_run_command = 'scons TARGET_TRANSPORT=ALL TARGET_OS=linux WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k'
    execute_command(non_secured_run_command)
    print ('....Non-secured Modules build Finished\n')

def run_non_secured_module() :
    print ('\nStart Non-secured Notification Modules Execution\n')
    non_secured_run_command = 'scons service/notification/ TARGET_TRANSPORT=ALL TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Non-secured Notification Modules Execution Finished\n###################################################')

    print ('\nStart Non-secured coap-http-proxy Modules Execution\n')
    non_secured_run_command = 'scons service/coap-http-proxy/ TARGET_TRANSPORT=ALL TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Non-secured coap-http-proxy Modules Execution Finished\n###################################################')

    print ('Start Non-secured Resource-Container Modules Execution\n')
    non_secured_run_command = 'scons service/resource-container/ TARGET_TRANSPORT=ALL TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Finish Non-secured Resource-Container Modules Execution\n')

    print ('Start Non-secured easy-setup/mediator/richsdk Modules Execution\n')
    non_secured_run_command = 'scons service/easy-setup/mediator/richsdk/ TARGET_TRANSPORT=ALL TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Finish Non-secured easy-setup/mediator/richsdk Modules Execution\n')

    print ('Start Non-secured easy-setup/enrollee Modules Execution\n')
    non_secured_run_command = 'scons service/easy-setup/enrollee/ TARGET_TRANSPORT=ALL WITH_CLOUD=1 WITH_TCP=1 TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Finish Non-secured easy-setup/easy-setup/enrollee Modules Execution\n')

    print ('Start Non-secured resource-encapsulation Modules Execution\n')
    non_secured_run_command = 'scons service/resource-encapsulation/ TARGET_TRANSPORT=ALL TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Finish Non-secured resource-encapsulation Modules Execution\n')

    print ('Start Non-secured scene-manager Modules Execution\n')
    non_secured_run_command = 'scons service/scene-manager/ TARGET_TRANSPORT=ALL TARGET_OS=linux TEST=1 -i -k'
    execute_command(non_secured_run_command)
    print ('Finish Non-secured scene-manager Modules Execution\n')

def build_secured_module() :
    print ('Start secured Modules Build...\n')
    secured_run_command = 'scons TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k'
    execute_command(secured_run_command)
    print ('...Finished Secured Modules Build\n')

def run_secured_module() :

    print ('Start secured resource/csdk/security/provisioning Modules Execution\n')
    secured_run_command = 'scons resource/csdk/security/provisioning/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/csdk/security/provisioning Modules Execution\n')

    print ('Start secured resource Modules Execution\n')
    secured_run_command = 'scons resource/unittests/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource Modules Execution\n')

#it is building provisioning twice(unittest
    print ('Start secured resource/csdk/security Modules Execution\n')
    secured_run_command = 'scons resource/csdk/security/unittest/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/csdk/security Modules Execution\n')

    print ('Start secured resource/csdk/connectivity Modules Execution\n')
    secured_run_command = 'scons resource/csdk/connectivity/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/csdk/connectivity Modules Execution\n')

    print ('Start secured resource/c_common Modules Execution\n')
    secured_run_command = 'scons resource/c_common/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/c_common Modules Execution\n')

    print ('Start secured resource/csdk/stack Modules Execution\n')
    secured_run_command = 'scons resource/csdk/stack/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/csdk/stack Modules Execution\n')

    print ('Start secured resource/csdk/security/provisioning/ck_manager Modules Execution\n')
    secured_run_command = 'scons resource/csdk/security/provisioning/ck_manager/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/csdk/security/provisioning/ck_manager Modules Execution\n')


    print ('Start secured resource/provisioning Modules Execution\n')
    secured_run_command = 'scons resource/provisioning/ TARGET_TRANSPORT=ALL TARGET_OS=linux SECURED=1 WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k TEST=1'
    execute_command(secured_run_command)
    print ('Finished secured resource/provisioning Modules Execution\n')

def cleanPreviousBuild() :
    print ('Cleaning Previous build...\n')
    clean_command = 'scons TARGET_TRANSPORT=ALL TARGET_OS=linux WITH_CLOUD=1 WITH_MQ=PUB,SUB WITH_TCP=1 RD_CLIENT=1 -i -k -c'
    execute_command(clean_command)
    print ('Finished Cleaning Build\n')

def performNonSecuredQualityMeasure() :
    build_non_secured_module()
    run_non_secured_module()

def performSecuredQualityMeasure() :
    build_secured_module()
    run_secured_module()

def performQualityMeasure() :
    addCoverageFlag()
    os.chdir(DEV_UNIT_TEST_RUN_DIRECTORY)
    downloadtinycbor()
    downloadMbedtls()
    performSecuredQualityMeasure()
    renameResourceAndServiceFolder()
    cleanPreviousBuild()
    performNonSecuredQualityMeasure()
    os.chdir(RUNNER_HOME_DIRECTORY)
    generateCoverageReport()
    make_excel_report()

def downloadMbedtls() :
    if not os.path.exists('extlibs/mbedtls/mbedtls'):
        download_command = 'git clone https://github.com/ARMmbed/mbedtls.git extlibs/mbedtls/mbedtls'
        execute_command(download_command)

def downloadtinycbor() :
    if not os.path.exists('extlibs/tinycbor/tinycbor'):
        download_command = 'git clone https://github.com/01org/tinycbor.git extlibs/tinycbor/tinycbor'
        execute_command(download_command)


def remove_secured_modules_xml() :
    print ('Start removing secured module xml files')
    for module_name in module :
         for file_name in module[module_name] :
          os.remove(DEV_UNIT_TEST_RESULT_DIRECTORY + file_name + '.xml')
          print (file_name+' removed')
    print ('Finished removing secured module xml files')

def write_log_into_file() :
    file = open('unittestlog.txt', 'w')
    file.write(log)
    file.close()

def execute_command(command) :
    global log
    print ('execute_command called')
    p = subprocess.Popen(command, shell=True)
    (output, err) = p.communicate()
    p_status = p.wait()
    print 'Command exit status/return code : ', p_status
    #log += output
    if p_status == 0:
        logging.info('Command Execution Succesful'+'\n#######################################################\n')
    else:
        logging.error('Command Execution Failed')

def find_and_copy_file() :
    directory = DEV_UNIT_TEST_RESULT_DIRECTORY+':xml:'
    print('Directory :'+directory)
    print(os.path.isdir(directory))
    if (os.path.isdir(directory)) :
        for root, dirs ,files in os.walk(directory) :
            for file_name in files :
                if file_name.endswith('.xml') :
                    print (os.path.abspath(file_name))
                    print(file_name)
                    file_full_path = os.path.join(root, file_name)
                    directory_path = os.path.dirname(file_full_path)
                    shutil.copy(directory_path + '/unittest.xml', DEV_UNIT_TEST_RESULT_DIRECTORY + 'security_unittest.xml')

def renameResourceAndServiceFolder() :
    os.chdir(DEV_UNIT_TEST_RUN_DIRECTORY)

    if os.path.exists('out/linux/' + target_arch + '/' + build_mode + '/service'):
        shutil.rmtree ('out/linux/' + target_arch + '/' + build_mode + '/nonsecured_service')
        os.rename('out/linux/' + target_arch + '/' + build_mode + '/service',  'out/linux/' + target_arch + '/' + build_mode + '/nonsecured_service')

    if os.path.exists('out/linux/' + target_arch + '/' + build_mode + '/resource'):
        shutil.rmtree ('out/linux/' + target_arch + '/' + build_mode + '/nonsecured_resource')
        os.rename('out/linux/' + target_arch + '/' + build_mode + '/resource', 'out/linux/' + target_arch + '/' + build_mode + '/nonsecured_resource')

def make_excel_report() :
    excel_report_command = './devunittest_reporter.py --iotivity_root %s --b %s --m %s'% (iotivity_root, bit, build_mode)
    execute_command(excel_report_command)

if __name__ == '__main__':
    print('======================Start Executing Developer Unit TCs===================================')

    if action_type == None :
        performQualityMeasure()

    if action_type == 'nsbuild' :
        addCoverageFlag()
        os.chdir(DEV_UNIT_TEST_RUN_DIRECTORY)
        performNonSecuredQualityMeasure()
        renameResourceAndServiceFolder()
        cleanPreviousBuild()

    if action_type == 'sbuild':
        addCoverageFlag()
        os.chdir(DEV_UNIT_TEST_RUN_DIRECTORY)
        performSecuredQualityMeasure()

    if action_type == 'report' :
        os.chdir(RUNNER_HOME_DIRECTORY)
        make_excel_report()

    if action_type == 'coverage' :
        os.chdir(RUNNER_HOME_DIRECTORY)
        generateCoverageReport()

    if action_type == 'clean' :
        cleanPreviousBuild()

    print("======================Finished Executing Developer Unit TCs===================================\n\n")
