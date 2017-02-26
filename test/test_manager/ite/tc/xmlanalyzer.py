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

                for transport in testspec.findall(SPEC_ELM.TRANSPORT):
                    transport_name = transport.get(SPEC_ATT.NAME)
                    self.data[platform_name][transport_name] = dict()

                    for network in testspec.findall(SPEC_ELM.NETWORK):
                        network_name = network.get(SPEC_ATT.NAME)
                        self.data[platform_name][transport_name][network_name] = dict()

                        for tctype in platform.findall(SPEC_ELM.TYPE):
                            type_name = tctype.get(SPEC_ATT.NAME)
                            self.data[platform_name][transport_name][network_name][type_name] = dict()

                            for module in tctype.findall(SPEC_ELM.MODULE):
                                module_name = module.get(SPEC_ATT.NAME)
                                self.data[platform_name][transport_name][network_name][type_name][module_name] = dict()

                                for testsuite in module.findall(SPEC_ELM.TESTSUITE):
                                    suite_name = testsuite.get(SPEC_ATT.NAME)
                                    self.data[platform_name][transport_name][network_name][type_name][module_name][suite_name] = dict()

                                    for testcase in testsuite.findall(SPEC_ELM.TESTCASE):
                                        spec = TestSpec(int(testcase.get(SPEC_ATT.FILELINE)), suite_name, testcase.get(SPEC_ATT.NAME))
                                        self.data[platform_name][transport_name][network_name][type_name][module_name][suite_name][spec.name] = spec

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

                            self.summary[platform_name][type_name] = dict()
                            self.review_summary[platform_name][transport_name][network_name][type_name] = dict()

                            for module in tctype.findall(SPEC_ELM.MODULE):
                                module_name = module.get(SPEC_ATT.NAME)
                                module_no = module.get(SPEC_ATT.NO)
                                module_review_no = module.get(SPEC_ATT.REVIEW)
                                self.summary[platform_name][transport_name][network_name][type_name][module_name] = int(module_no);
                                self.review_summary[platform_name][transport_name][network_name][type_name][module_name] = int(module_review_no);


        except ParseError:
            print("There is a Parse Error on " + path)


