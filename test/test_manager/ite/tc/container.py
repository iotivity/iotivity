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

from ite.config import *
from ite.tc.analyzer import TCAnalyzer
from ite.tc.robot_analyzer import RobotTCAnalyzer

class TestSpecContainer:

    def __init__(self):
        self.data = dict()

        for platform in TEST_PLATFORM:
            self.data[platform] = dict()

            for build_type in BUILD_TYPE:
                self.data[platform][build_type] = dict()

                for transport in TEST_TRANSPORT:
                    self.data[platform][build_type][transport] = dict()

                    for network in TEST_NETWORK:
                        self.data[platform][build_type][transport][network] = dict()

                        for tctype in TESTCASE_TYPES:
                            self.data[platform][build_type][transport][network][tctype] = dict()

                            for module in TESTSUITE_MODULES:
                                self.data[platform][build_type][transport][network][tctype][module] = dict()


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
                    continue

                testspec_dict, invalid_list = analyzer.analyze_tc_file(filepath, module)

                for invalid in invalid_list:
                    print("=> Invalid Test Case : %s(%d), %s.%s, message:%s" %
                          (filepath, invalid[0], invalid[1], invalid[2], invalid[3]))
                    api_valid_convention = False

                for platform in testspec_dict.keys():
                    if not platform in self.data.keys():
                        print("=> Invalid Platform: " + platform)
                        continue
                    for build_type in testspec_dict[platform]:
                        for transport in testspec_dict[platform][build_type].keys():
                            #print (testspec_dict[platform][build_type][transport].items())
                            for network, testspec_list in testspec_dict[platform][build_type][transport].items():
                                #print('line 77:', len(testspec_list), testspec_list)
                                for testspec in testspec_list:
                                    #print (self.data[platform][build_type][transport][network][tctype][module])
                                    if (not testspec.suite in self.data[platform][build_type][transport][network][tctype][module]):
                                        self.data[platform][build_type][transport][network][tctype][module][testspec.suite] = dict()
                                    self.data[platform][build_type][transport][network][tctype][module][testspec.suite][testspec.name] = testspec

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
                if (testfw.lower() == "gtest"):
                    for sdk_type in SDK_TYPES:
                        sdk_fw_dir = os.path.join(tmp_dir, sdk_type.lower())
                        if (os.path.isdir(sdk_fw_dir)):
                            sdk_fw_dir = os.path.join(sdk_fw_dir, "src")
                            valid = self.extract_api_testspec_bytypes(module, testfw, sdk_fw_dir)
                            api_valid_convention = api_valid_convention and valid
                    fw_dir = os.path.join(tmp_dir, "src")
                    if (os.path.isdir(fw_dir)):
                        valid = self.extract_api_testspec_bytypes(module, testfw, fw_dir)
                        api_valid_convention = api_valid_convention and valid
                else:
                    fw_dir = os.path.join(tmp_dir, "src")
                    if (testfw.lower() == "junit"):
                        fw_dir = os.path.join(fw_dir, "org")
                        fw_dir = os.path.join(fw_dir, "iotivity")
                        fw_dir = os.path.join(fw_dir, "test")
                        fw_dir = os.path.join(fw_dir, module.lower())
                        fw_dir = os.path.join(fw_dir, "tc")
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
                    continue

                test_data, invalid_list = analyzer.analyze_tc_file(filepath)

                for invalid in invalid_list:
                    print("=> Invalid Test Case : %s(%d), %s.%s, message:%s" %
                          (filepath, invalid[0], invalid[1], invalid[2], invalid[3]))
                    api_valid_convention = False

                for platform, module, testspec in test_data:
                    if not platform in self.data:
                        print("=> Invalid Platform: " + platform)
                        continue

                    for build_type in self.data[platform]:
                        if (not testspec.suite in self.data[platform][build_type][NO_TRANSPORT][NO_NETWORK]['STC'][module]):
                            self.data[platform][build_type][NO_TRANSPORT][NO_NETWORK]['STC'][module][testspec.suite] = dict()
                        self.data[platform][build_type][NO_TRANSPORT][NO_NETWORK]['STC'][module][testspec.suite][testspec.name] = testspec

        return api_valid_convention



