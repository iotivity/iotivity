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

from xml.dom.minidom import Document
from xml.etree.ElementTree import ElementTree, ParseError

from ite.tc.xmlanalyzer import TCXMLAnalyzer
from ite.webreporter.webreporter_util import *
from ite.constants import *
from ite.config import *
from ite.util import *

class TCListReporter:

    def report(self, src_path, dest_path):
        analyzer = TCXMLAnalyzer();

        analyzer.read_spec_xml(src_path);

        xml = open_write_file(dest_path)
        if xml == False:
            return

        doc = Document()
        package = create_xml_element(doc, doc, WEB_REPORT.PACKAGE)

        for platform in TEST_PLATFORM:
            if not platform in analyzer.data:
                continue;
            platform_elm = create_xml_element(doc, package, WEB_REPORT.TC)
            platform_elm.setAttribute(WEB_MODEL_ATT.NAME, platform)

            for tctype in TESTCASE_TYPES:
                if not tctype in analyzer.data[platform]:
                    continue;
                tctype_elm = create_xml_element(doc, platform_elm, WEB_REPORT.TC)
                tctype_elm.setAttribute(WEB_MODEL_ATT.NAME, tctype)

                for module in TESTSUITE_MODULES:
                    if not module in analyzer.data[platform][tctype]:
                        continue;
                    module_elm = create_xml_element(doc, tctype_elm, WEB_REPORT.TC)
                    module_elm.setAttribute(WEB_MODEL_ATT.NAME, module)

                    for testsuite in analyzer.data[platform][tctype][module]:
                        testsuite_elm = create_xml_element(doc, module_elm, WEB_REPORT.TC)
                        testsuite_elm.setAttribute(WEB_MODEL_ATT.NAME, testsuite)

                        for testcase in analyzer.data[platform][tctype][module][testsuite]:
                            testcase_elm = create_xml_element(doc, testsuite_elm, WEB_REPORT.TC)
                            testcase_elm.setAttribute(WEB_MODEL_ATT.NAME, testcase)
                            testcase_elm.setAttribute(WEB_REPORT.KEY, "%s_%s_%s_%s_%s" %(platform, tctype, module, testsuite, testcase))


        doc.writexml(xml, '\t', '\t', '\n', 'UTF-8')

        xml.close()

    def analyze(self, src_path):
        try :
            testgroup = dict()

            doc = ElementTree(file=src_path)

            for platform in doc.findall(WEB_REPORT.TC):
                #platform_name = platform.get(SPEC_ATT.NAME)

                for tctype in platform.findall(WEB_REPORT.TC):
                    type_name = tctype.get(SPEC_ATT.NAME)

                    for module in tctype.findall(WEB_REPORT.TC):
                        module_name = module.get(SPEC_ATT.NAME)

                        tc_filter = "%s_%s" % (module_name, type_name)
                        tc_filter = tc_filter.lower()
                        testgroup[tc_filter] = dict()
                        print("### Filter : " + tc_filter)

                        for testsuite in module.findall(WEB_REPORT.TC):
                            suite_name = testsuite.get(SPEC_ATT.NAME)

                            for testcase in testsuite.findall(WEB_REPORT.TC):
                                tc_name = testcase.get(SPEC_ATT.NAME)
                                tc_key = testcase.get(WEB_REPORT.KEY)

                                testcase_filter = "%s.%s" % (suite_name, tc_name)
                                testgroup[tc_filter][testcase_filter] = tc_key

            return testgroup

        except ParseError:
            print("There is a Parse Error on " + src_path)


