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

from ite.config import *
from ite.tc.analyzer import TCAnalyzer
from ite.tc.robot_analyzer import RobotTCAnalyzer

class TestSpecContainer:

    def __init__(self):
        self.data = dict()

        for platform in TEST_PLATFORM:
            self.data[platform] = dict()

            for transport in TEST_TRANSPORT:
                self.data[platform][transport] = dict()

                for network in TEST_NETWORK:
                    self.data[platform][transport][network] = dict()

                    for tctype in TESTCASE_TYPES:
                        self.data[platform][transport][network][tctype] = dict()

                        for module in TESTSUITE_MODULES:
                            self.data[platform][transport][network][tctype][module] = dict()


    def extract_api_testspec_bysuite(self, tctype, module, testfw, path):
        analyzer = TCAnalyzer(testfw)

        rootpath = os.path.normcase(path)

        api_valid_convention = True
        for path, dir, files in os.walk(rootpath):
            for fname in files:
                filepath = os.path.join(path, fname)
                if (os.path.isfile(filepath) == False):
                    continue

                if not (filepath.endswith('.cpp') or filepath.endswith('.c') or filepath.endswith('.java')):
                    return

                testspec_dict, invalid_list = analyzer.analyze_tc_file(filepath)

                for invalid in invalid_list:
                    print("=> Invalid Test Case : %s(%d), %s.%s, message:%s" %
                          (filepath, invalid[0], invalid[1], invalid[2], invalid[3]))
                    api_valid_convention = False

                for platform in testspec_dict.keys():
                    if not platform in self.data.keys():
                        print("=> Invalid Platform: " + platform)
                        continue

                    for transport in testspec_dict[platform].keys():

                        for network, testspec_list in testspec_dict[platform][transport].items():

                            for testspec in testspec_list:
                                if (not testspec.suite in self.data[platform][transport][network][tctype][module]):
                                    self.data[platform][transport][network][tctype][module][testspec.suite] = dict()
                                self.data[platform][transport][network][tctype][module][testspec.suite][testspec.name] = testspec

        return api_valid_convention

    def extract_api_testspec_bytypes(self, module, testfw, path):
        rootpath = os.path.normcase(path)

        api_valid_convention = True
#        rootpath = os.path.join(rootpath, "src")
        for tctype in TESTCASE_TYPES:
            type_dir = os.path.join(rootpath, tctype.lower())
            if (os.path.isdir(type_dir)):
                valid = self.extract_api_testspec_bysuite(tctype, module, testfw, type_dir)
                api_valid_convention = api_valid_convention and valid
        return api_valid_convention

    def extract_api_testspec_bytestfw(self, module, path):
        rootpath = os.path.normcase(path)

        api_valid_convention = True
        for testfw in TESTFW_TYPES:

            fw_dir = os.path.join(rootpath, testfw.lower())
            tmp_dir = fw_dir

            if (os.path.isdir(fw_dir)):
                if (module == "PM" and testfw.lower() == "gtest"):
                    for pmtypes in PM_TYPES:
                        fw_dir = os.path.join(tmp_dir, pmtypes.lower())
                        fw_dir = os.path.join(fw_dir, "src")
                        valid = self.extract_api_testspec_bytypes(module, testfw, fw_dir)
                elif (module == "RI" and testfw.lower() == "gtest"):
                    for ritypes in RI_TYPES:
                        fw_dir = os.path.join(tmp_dir, ritypes.lower())
                        fw_dir = os.path.join(fw_dir, "src")
                        valid = self.extract_api_testspec_bytypes(module, testfw, fw_dir)
                else:
                    fw_dir = os.path.join(fw_dir, "src")
                    if (testfw.lower() == "junit"):
                        fw_dir = os.path.join(fw_dir, "org")
                        fw_dir = os.path.join(fw_dir, "iotivity")
                        fw_dir = os.path.join(fw_dir, "test")
                        fw_dir = os.path.join(fw_dir, module.lower())
                    valid = self.extract_api_testspec_bytypes(module, testfw, fw_dir)
                api_valid_convention = api_valid_convention and valid

        return api_valid_convention

    def extract_api_testspec_bymodule(self, path, module_filter):
        rootpath = os.path.normcase(path)

        api_valid_convention = True
        for module in TESTSUITE_MODULES:
            if not module_filter == '' and module.lower() != module_filter.lower():
                continue

            module_dir = os.path.join(rootpath, module.lower())
            if (os.path.isdir(module_dir)):
                valid = self.extract_api_testspec_bytestfw(module, module_dir)
                api_valid_convention = api_valid_convention and valid

        return api_valid_convention

    def extract_api_testspec(self, path, module_filter):
        rootpath = os.path.normcase(path)

        api_valid_convention = True
        if (os.path.isdir(rootpath)):
            valid = self.extract_api_testspec_bymodule(rootpath, module_filter)
#            valid = self.extract_api_testspec_bymodule(tctype, type_dir, module_filter)
            api_valid_convention = api_valid_convention and valid
#        for tctype in TESTCASE_TYPES:
#            type_dir = os.path.join(rootpath, tctype.lower())
#            if (os.path.isdir(type_dir)):
#                valid = self.extract_api_testspec_bymodule(tctype, type_dir, module_filter)
#                api_valid_convention = api_valid_convention and valid

        return api_valid_convention

    def extract_sampleapp_testspec(self, path, module_filter):
        rootpath = os.path.normcase(path)

        analyzer = RobotTCAnalyzer()
        api_valid_convention = True
        for path, dir, files in os.walk(rootpath):
            for fname in files:
                filepath = os.path.join(path, fname)
                if (os.path.isfile(filepath) == False):
                    continue

                if not 'Iotivity_ComponentSampleApp' in fname:
                    continue

                if not filepath.endswith('.txt'):
                    return

                test_data, invalid_list = analyzer.analyze_tc_file(filepath)

                for invalid in invalid_list:
                    print("=> Invalid Test Case : %s(%d), %s.%s, message:%s" %
                          (filepath, invalid[0], invalid[1], invalid[2], invalid[3]))
                    api_valid_convention = False

                for platform, module, testspec in test_data:
                    if not platform in self.data:
                        print("=> Invalid Platform: " + platform)
                        continue

                    if (not testspec.suite in self.data[platform][NO_TRANSPORT][NO_NETWORK]['STC'][module]):
                        self.data[platform][NO_TRANSPORT][NO_NETWORK]['STC'][module][testspec.suite] = dict()
                    self.data[platform][NO_TRANSPORT][NO_NETWORK]['STC'][module][testspec.suite][testspec.name] = testspec

        return api_valid_convention



