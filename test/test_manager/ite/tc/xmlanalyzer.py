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

from xml.etree.ElementTree import ElementTree, ParseError

from ite.tc.analyzer import TestSpec
from ite.constants import *
from ite.config import *

class TCXMLAnalyzer:


    def __init__(self):
        self.data = dict()
        self.summary = dict()
        self.review_summary = dict();

    def read_spec_xml(self, path):
        try :
            doc = ElementTree(file=path)
            testspec = doc.find(SPEC_ELM.TESTSPEC)

            for platform in testspec.findall(SPEC_ELM.PLATFORM):
                platform_name = platform.get(SPEC_ATT.NAME)
                self.data[platform_name] = dict()
                for build_type_node in platform.findall(SPEC_ELM.BUILDTYPE):
                    build_type = build_type_node.get(SPEC_ATT.NAME)
                    self.data[platform_name][build_type] = dict()
                    for tctype in build_type_node.findall(SPEC_ELM.TYPE):
                        type_name = tctype.get(SPEC_ATT.NAME)
                        self.data[platform_name][build_type][type_name] = dict()
                        for module in tctype.findall(SPEC_ELM.MODULE):
                            module_name = module.get(SPEC_ATT.NAME)
                            self.data[platform_name][build_type][type_name][module_name] = dict()
                            transport_names = []
                            tranport_tag_map = {}

                            for transport in module.findall(SPEC_ELM.TRANSPORT):
                                transport_name = transport.get(SPEC_ATT.NAME)
                                transport_names.append(transport_name)
                                tranport_tag_map[transport_name] = transport

                            if not transport_names:
                                transport_names.append('NONE')
                                tranport_tag_map['NONE'] = module

                            for transport_name in transport_names:
                                self.data[platform_name][build_type][type_name][module_name][transport_name] = dict()

                                network_names = []
                                network_tag_map = {}

                                for network in tranport_tag_map[transport_name].findall(SPEC_ELM.NETWORK):
                                    network_name = network.get(SPEC_ATT.NAME)
                                    network_names.append(network_name)
                                    network_tag_map[network_name] = network

                                if not network_names:
                                    network_names.append('NONE')
                                    network_tag_map['NONE'] = tranport_tag_map[transport_name]

                                for network_name in network_names:
                                    self.data[platform_name][build_type][type_name][module_name][transport_name][network_name] = dict()

                                    for testsuite in network_tag_map[network_name].findall(SPEC_ELM.TESTSUITE):
                                        suite_name = testsuite.get(SPEC_ATT.NAME)
                                        self.data[platform_name][build_type][type_name][module_name][transport_name][network_name][suite_name] = dict()

                                        for testcase in testsuite.findall(SPEC_ELM.TESTCASE):
                                            spec = TestSpec(int(testcase.get(SPEC_ATT.FILELINE)), suite_name, testcase.get(SPEC_ATT.NAME))
                                            self.data[platform_name][build_type][type_name][module_name][transport_name][network_name][suite_name][spec.name] = spec

                                            for tag in list(TAG_DIC):
                                                tag_elm = testcase.find(tag)
                                                if (tag_elm == None):
                                                    spec.__dict__[tag] = ''
                                                else:
                                                    spec.__dict__[tag] = tag_elm.text

        except ParseError:
            print("There is a Parse Error on " + path)

    def read_summary_xml(self, path):
        try :
            doc = ElementTree(file=path)
            #testspec = doc.find(SPEC_ELM.TESTSPEC)

            summary = doc.find(SPEC_ELM.SUMMARY)

            for platform in summary.findall(SPEC_ELM.PLATFORM):
                platform_name = platform.get(SPEC_ATT.NAME)
                #platform_no = platform.get(SPEC_ATT.NO)
                #platform_review_no = platform.get(SPEC_ATT.REVIEW)
                self.summary[platform_name] = dict()
                self.review_summary[platform_name] = dict()

                for transport in summary.findall(SPEC_ELM.TRANSPORT):
                    transport_name = platform.get(SPEC_ATT.NAME)
                    self.summary[platform_name][transport_name] = dict()
                    self.review_summary[platform_name][transport_name] = dict()

                    for network in summary.findall(SPEC_ELM.NETWORK):
                        network_name = platform.get(SPEC_ATT.NAME)
                        self.summary[platform_name][transport_name][network_name] = dict()
                        self.review_summary[platform_name][transport_name][network_name] = dict()

                        for tctype in platform.findall(SPEC_ELM.TYPE):
                            type_name = tctype.get(SPEC_ATT.NAME)
                            #type_no = tctype.get(SPEC_ATT.NO)
                            #type_review_no = tctype.get(SPEC_ATT.REVIEW)

                            self.summary[platform_name][build_type][type_name] = dict()
                            self.review_summary[platform_name][transport_name][network_name][type_name] = dict()

                            for module in tctype.findall(SPEC_ELM.MODULE):
                                module_name = module.get(SPEC_ATT.NAME)
                                module_no = module.get(SPEC_ATT.NO)
                                module_review_no = module.get(SPEC_ATT.REVIEW)
                                self.summary[platform_name][transport_name][network_name][type_name][module_name] = int(module_no);
                                self.review_summary[platform_name][transport_name][network_name][type_name][module_name] = int(module_review_no);


        except ParseError:
            print("There is a Parse Error on " + path)


