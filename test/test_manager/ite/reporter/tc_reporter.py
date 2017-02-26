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

import operator
from xml.dom.minidom import Document
import sys

from xlsxwriter.workbook import Workbook

from ite.config import *
from ite.reporter.excel_util import *
from ite.reporter.reporter_util import *
from ite.util import *

class TestSpecReporter:

    def __init__(self):
        self.data = None
        self.summary = TCSummary()

    def makeSummary(self):
        for platform in TEST_PLATFORM:

            for transport in TEST_TRANSPORT:
                if not transport in self.data[platform]:
                    continue

                for network in TEST_NETWORK:
                    if not network in self.data[platform][transport]:
                        continue

                    for tctype in TESTCASE_TYPES:
                        if not tctype in self.data[platform][transport][network]:
                            continue

                        for module in TESTSUITE_MODULES:
                            if not module in self.data[platform][transport][network][tctype]:
                                continue

                            for suite in list(self.data[platform][transport][network][tctype][module]):
                                count = len(self.data[platform][transport][network][tctype][module][suite])
                                reviewer_count = 0

                                for spec in self.data[platform][transport][network][tctype][module][suite].values():
                                    reviewer = spec.__dict__['reviewer']
                                    if reviewer != None and reviewer != '':
                                        reviewer_count += 1

                                self.summary.update_tc(platform, transport, network, tctype, module, count, reviewer_count)

        print("### Iterating over contain.data")
        for platform in self.data:
            for transport in self.data[platform]:
                for network in self.data[platform][transport]:
                    for tctype in self.data[platform][transport][network]:
                        for module in self.data[platform][transport][network][tctype]:
                            for suite in list(self.data[platform][transport][network][tctype][module]):
                                count = len(self.data[platform][transport][network][tctype][module][suite])
                                #print (platform + ' - ' + transport + ' - ' + network + ' -- ' + tctype + ' - ' + module + ' - ' + suite + ' = ' + str(count))

    def generate_testspec_report(self, data):
        self.data = data
        self.makeSummary();

    def report(self, report_type, path):
        if report_type == 'TXT':
            self.report_to_txt(path)
        elif report_type == 'CSV':
            self.report_to_csv(path)
        elif report_type == 'XML':
            self.report_to_xml(path)
        elif report_type == 'XLSX':
            self.report_to_xlsx(path)

    def report_to_txt(self, path):
        txt = open_write_file(path)
        if txt == False:
            return

        print("===== [Summary] =====", file=txt)

        total = 0
        for platform in TEST_PLATFORM:
            total += self.summary.platform_count[platform]
            print("[%s]: %d" % (platform, self.summary.platform_count[platform]), file=txt)

            for transport in TEST_TRANSPORT:
                if not transport in self.data[platform]:
                    continue
                print("\t[%s]: %d" % (transport, self.summary.transport_count[platform][transport]), file=txt)

                for tctype in TESTCASE_TYPES:
                    if not tctype in self.summary.tctype_count[platform][transport]:
                        continue
                    print("\t[%s]: %d" % (tctype, self.summary.tctype_count[platform][transport][tctype]), file=txt)

                    for module in TESTSUITE_MODULES:
                        print("\t\t[%s]: %d" %(module, self.summary.module_count[platform][transport][tctype][module]), file=txt)

            print("-------------------------------------------", file=txt)

        print("[Total] : %d" % total, file=txt)

        print("===========================================", file=txt)

        for platform in TEST_PLATFORM:
            print("[%s]" % platform, file=txt)
            print("===========================================", file=txt)

            for transport in TEST_TRANSPORT:
                if not transport in self.data[platform]:
                    continue
                print("[%s]" % transport, file=txt)
                print("-------------------------------------------", file=txt)

                for tctype in TESTCASE_TYPES:
                    if not tctype in self.data[platform][transport]:
                        continue
                    print("[%s]" % tctype, file=txt)
                    print("-------------------------------------------", file=txt)

                    for module in TESTSUITE_MODULES:
                        if not module in self.data[platform][transport][tctype]:
                            continue

                        print("[%s]" % module, file=txt)

                        for suite in list(self.data[platform][transport][tctype][module]):
                            if len(self.data[platform][transport][tctype][module]) <= 0:
                                continue
                            print("[%s]" % suite, file=txt)

                            for spec in sorted(self.data[platform][transport][tctype][module][suite].values(), key=operator.attrgetter('line')):
                                print("%s" % spec.to_string(), file=txt)

                    print("-------------------------------------------", file=txt)

            print("===========================================", file=txt)

        txt.close()

    def report_to_csv(self, path):
        csv = open_write_file(path)
        if csv == False:
            return

        print("<Summary>", file=csv)

        total = 0
        for platform in TEST_PLATFORM:
            total += self.summary.platform_count[platform]
            print("%s, , ,%d" % (platform, self.summary.platform_count[platform]), file=csv)

            for transport in TEST_TRANSPORT:
                if not transport in self.data[platform]:
                    continue
                print(" ,%s, ,%d" % (transport, self.summary.transport_count[platform][transport]), file=csv)

                for tctype in TESTCASE_TYPES:
                    if not tctype in self.summary.tctype_count[platform][transport]:
                        continue
                    print(" ,%s, ,%d" % (tctype, self.summary.tctype_count[platform][transport][tctype]), file=csv)

                    for module in TESTSUITE_MODULES:
                        print(" , ,%s,%d" %(module, self.summary.module_count[platform][transport][tctype][module]), file=csv)

        print("", file=csv)
        print("<Test Spec>", file=csv)

        tag_titles = ''
        for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
            if (title[1] == ''):
                continue
            tag_titles += title[1] +","

        print("Platform,Type,Module,Test Suite,Test Case,%s" % tag_titles, file=csv)
        for platform in TEST_PLATFORM:
            print("%s" % platform, file=csv)

            for transport in TEST_TRANSPORT:
                if not transport in self.data[platform]:
                    continue
                print(" ,%s" % transport, file=csv)

                for tctype in TESTCASE_TYPES:
                    if not tctype in self.data[platform][transport]:
                        continue
                    print(" ,%s" % tctype, file=csv)

                    for module in TESTSUITE_MODULES:
                        if not module in self.data[platform][transport][tctype]:
                            continue
                        print(" , ,%s" % module, file=csv)

                        for suite in list(self.data[platform][transport][tctype][module]):
                            print(" , , ,%s" % suite, file=csv)

                            for spec in sorted(self.data[platform][transport][tctype][module][suite].values(), key=operator.attrgetter('line')):
                                tag_data = spec.name + ','
                                for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
                                    if (title[1] == ''):
                                        continue
                                    tag_data += '''"%s",''' % spec.__dict__[key]
                                print(" , , , ,%s" % tag_data, file=csv)

        csv.close()

    def create_xml_element(self, doc, parent, tagname):
        element = doc.createElement(tagname)
        parent.appendChild(element)

        return element

    def report_to_xml(self, path):
        xml = open_write_file(path)
        if xml == False:
            return

        doc = Document()
        root = self.create_xml_element(doc, doc, SPEC_ELM.TESTSPEC_REPORT)
        summary = self.create_xml_element(doc, root, SPEC_ELM.SUMMARY)

        total = 0
        for platform in TEST_PLATFORM:
            total += self.summary.platform_count[platform]
            platform_elm = self.create_xml_element(doc, summary, SPEC_ELM.PLATFORM)
            platform_elm.setAttribute(SPEC_ATT.NAME, platform)
            platform_elm.setAttribute(SPEC_ATT.NO, str(self.summary.platform_count[platform]))

            for tctype in TESTCASE_TYPES:

                cnt = 0
                for transport in TEST_TRANSPORT:
                    for network in TEST_NETWORK:
                        cnt += self.summary.tctype_count[platform][transport][network][tctype]

                type_elm = self.create_xml_element(doc, platform_elm, SPEC_ELM.TYPE)
                type_elm.setAttribute(SPEC_ATT.NAME, tctype)
                type_elm.setAttribute(SPEC_ATT.NO, str(cnt))

                for module in TESTSUITE_MODULES:

                    module_tc_count = 0
                    module_review_count = 0
                    for transport in TEST_TRANSPORT:
                        for network in TEST_NETWORK:
                            if not module in self.summary.module_count[platform][transport][network][tctype]:
                                continue
                            module_tc_count += self.summary.module_count[platform][transport][network][tctype][module]
                            module_review_count += self.summary.review_count[platform][transport][network][tctype][module]

                    if module_tc_count == 0:
                        continue

                    module_elm = self.create_xml_element(doc, type_elm, SPEC_ELM.MODULE)
                    module_elm.setAttribute(SPEC_ATT.NAME, module)
                    module_elm.setAttribute(SPEC_ATT.NO, str(module_tc_count))
                    module_elm.setAttribute(SPEC_ATT.REVIEW, str(module_review_count))

                    transport_tc_count = 0
                    for transport in TEST_TRANSPORT:
                        for network in TEST_NETWORK:
                            if network != TEST_NETWORK.NONE:
                                transport_tc_count += self.summary.module_count[platform][transport][network][tctype][module]

                    if transport_tc_count == 0:
                        continue

                    for transport in TEST_TRANSPORT:

                        network_tc_count = 0
                        named_network_tc_count = 0
                        for network in TEST_NETWORK:
                            network_tc_count += self.summary.module_count[platform][transport][network][tctype][module]
                            if network != TEST_NETWORK.NONE:
                                named_network_tc_count += self.summary.module_count[platform][transport][network][tctype][module]

                        if network_tc_count == 0:
                            continue

                        transport_elm = self.create_xml_element(doc, module_elm, SPEC_ELM.TRANSPORT)
                        transport_elm.setAttribute(SPEC_ATT.NAME, transport)
                        transport_elm.setAttribute(SPEC_ATT.NO, str(network_tc_count))

                        if named_network_tc_count == 0:
                            continue

                        for network in TEST_NETWORK:
                            if self.summary.module_count[platform][transport][network][tctype][module] == 0:
                                continue

                            network_elm = self.create_xml_element(doc, transport_elm, SPEC_ELM.NETWORK)
                            network_elm.setAttribute(SPEC_ATT.NAME, network)
                            network_elm.setAttribute(SPEC_ATT.NO, str(self.summary.module_count[platform][transport][network][tctype][module]))

        summary.setAttribute(SPEC_ATT.NO, str(total))

        spec_elm = self.create_xml_element(doc, root, SPEC_ELM.TESTSPEC)
        platform_elm = None
        platform = None

        def write_tctype(platform):
            for tctype in TESTCASE_TYPES:
                type_elm = self.create_xml_element(doc, platform_elm, SPEC_ELM.TYPE)
                type_elm.setAttribute(SPEC_ATT.NAME, tctype)
                yield TESTSUITE_MODULES, type_elm, platform, tctype

        def write_module(tctypes_data):
            for tctype_data in tctypes_data:
                module_names, type_elm, platform, tctype = tctype_data
                for module in module_names:
                    module_tc_count = 0
                    for transport in TEST_TRANSPORT:
                        for network in TEST_NETWORK:
                            if not module in self.data[platform][transport][network][tctype]:
                                continue
                            module_tc_count += len(self.data[platform][transport][network][tctype][module])

                    if module_tc_count == 0:
                        continue

                    module_elm = self.create_xml_element(doc, type_elm, SPEC_ELM.MODULE)
                    module_elm.setAttribute(SPEC_ATT.NAME, module)

                    yield module_elm, platform, tctype, module

        def write_transport(modules_data):
            for module_data in modules_data:
                module_elm, platform, tctype, module = module_data

                named_transport_tc_count = 0
                for transport in TEST_TRANSPORT:
                    for network in TEST_NETWORK:
                        if transport != TEST_TRANSPORT.NONE:
                            named_transport_tc_count += len(self.data[platform][transport][network][tctype][module])

                for transport in TEST_TRANSPORT:
                    last_elm = module_elm

                    transport_tc_count = 0
                    for network in TEST_NETWORK:
                        transport_tc_count += len(self.data[platform][transport][network][tctype][module])

                    if transport_tc_count == 0:
                        continue

                    if named_transport_tc_count > 0:
                        transport_elm = self.create_xml_element(doc, last_elm, SPEC_ELM.TRANSPORT)
                        transport_elm.setAttribute(SPEC_ATT.NAME, transport)
                        last_elm = transport_elm

                    yield last_elm, platform, tctype, module, transport

        def write_network(transports_data):
            for transport_data in transports_data:
                transport_elm, platform, tctype, module, transport = transport_data

                named_network_tc_count = 0
                for network in TEST_NETWORK:
                    if network != TEST_NETWORK.NONE:
                        named_network_tc_count += len(self.data[platform][transport][network][tctype][module])

                for network in TEST_NETWORK:
                    last_elm = transport_elm

                    if len(self.data[platform][transport][network][tctype][module]) == 0:
                        continue

                    if named_network_tc_count > 0:
                        network_elm = self.create_xml_element(doc, last_elm, SPEC_ELM.NETWORK)
                        network_elm.setAttribute(SPEC_ATT.NAME, network)
                        last_elm = network_elm

                    yield self.data[platform][transport][network][tctype][module], last_elm


        def write_suite(modules):
            for module in modules:
                suites, module_elm = module
                for suite in list(suites):
                    suite_elm = self.create_xml_element(doc, module_elm, SPEC_ELM.TESTSUITE)
                    suite_elm.setAttribute(SPEC_ATT.NAME, suite)
                    yield suites[suite], suite_elm

        def write_tc(suites):
            for suite in suites:
                testcases, suite_elm = suite
                for spec in sorted(testcases.values(), key=operator.attrgetter('line')):
                    tc_elm = self.create_xml_element(doc, suite_elm, SPEC_ELM.TESTCASE)
                    tc_elm.setAttribute(SPEC_ATT.NAME, spec.name)
                    tc_elm.setAttribute(SPEC_ATT.FILELINE, str(spec.line))

                    for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
                        if (title[1] == ''):
                            continue
                        tag_node = self.create_xml_element(doc, tc_elm, key)
                        tag_node.appendChild(doc.createTextNode(spec.__dict__[key]))

        for platform in TEST_PLATFORM:
            if self.summary.platform_count[platform] == 0:
                continue

            platform_elm = self.create_xml_element(doc, spec_elm, SPEC_ELM.PLATFORM)
            platform_elm.setAttribute(SPEC_ATT.NAME, platform)

            walk_through_testcases_by_module(platform, write_tctype, write_module, write_transport, write_network, write_suite, write_tc)

        doc.writexml(xml, '\t', '\t', '\n', 'UTF-8')

        xml.close()

    def report_to_xlsx(self, path):
        workbook = Workbook(path)
        form = ExcelFormat(workbook)

        summarysheet = workbook.add_worksheet('Summary')

        row = 0
        col = 0

        col += 1

        for platform in TEST_PLATFORM + ('Total',):
            start_platfom_col = col

            for tctype in TESTCASE_TYPES + ('API TC',):
                summarysheet.write(row + 1, col, tctype, form.title)
                col += 1

            merge_cell(summarysheet, row, start_platfom_col, row, col-1, platform, form.title)

        row += 2
        col = 0
        module_start_row = row

        for title in TESTSUITE_MODULES :
            summarysheet.write(row, col, title, form.title)
            row += 1
        summarysheet.write(row, col, 'Total', form.total)

        row = module_start_row
        col = 1

        for platform in TEST_PLATFORM:
            for tctype in TESTCASE_TYPES:
                row = module_start_row
                for module in TESTSUITE_MODULES:
                    cnt = 0
                    for transport in TEST_TRANSPORT:
                        for network in TEST_NETWORK:
                            cnt = cnt + self.summary.module_count[platform][transport][network][tctype][module]
                    summarysheet.write(row, col, cnt, form.cell)
                    row += 1

                total_txt = '=SUM(%s:%s)' % (get_cell_name(col, module_start_row), get_cell_name(col, row-1))
                summarysheet.write(row, col, total_txt, form.total_no)
                col += 1

            row = module_start_row
            for module in TESTSUITE_MODULES:
                total_txt = '=SUM(%s:%s)' % (get_cell_name(col-2, row), get_cell_name(col-1, row))
                summarysheet.write(row, col, total_txt, form.cell)
                row += 1

            total_txt = '=SUM(%s:%s)' % (get_cell_name(col, module_start_row), get_cell_name(col, row-1))
            summarysheet.write(row, col, total_txt, form.total_no)

            col += 1

        start_count_col = 1
        for tctype in TESTCASE_TYPES + ('API TC',):
            row = module_start_row

            for module in TESTSUITE_MODULES + ('Total',):
                count_col = start_count_col
                total_txt = '=SUM('
                for platform in TEST_PLATFORM:
                    total_txt += get_cell_name(count_col, row) + ','
                    count_col += 3

                total_txt += ')'
                summarysheet.write(row, col, total_txt, form.total_no)

                row += 1

            start_count_col += 1
            col += 1


        for platform in TEST_PLATFORM:
            if self.summary.platform_count[platform] == 0:
                continue

            specsheet = workbook.add_worksheet('%s_TCSpec' % platform)

            row = 0
            col = 0
            for title, size in TITLE:
                specsheet.write(row, col, title, form.title)
                specsheet.set_column(col, col, size)
                col += 1

            for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
                if (title[1] == ''):
                    continue
                specsheet.write(row, col, title[1], form.title)
                specsheet.set_column(col, col, title[2])
                col += 1

            row += 1
            col = 0

            def write_transport():
                for transport in self.data[platform]:
                    nonlocal row, col
                    transport_first_row = row
                    col += 1

                    yield transport, self.data[platform][transport]

                    col -= 1
                    merge_cell(specsheet, transport_first_row, col, row-1, col, transport, form.cell)

            def write_network(tuple_data):
                for transport, network_data in tuple_data:
                    for network in network_data:
                        nonlocal row, col
                        network_first_row = row
                        col += 1

                        yield transport, network, network_data[network]

                        col -= 1
                        merge_cell(specsheet, network_first_row, col, row-1, col, network, form.cell)

            def write_tctype(tuple_data):
                for transport, network, type_data in tuple_data:
                    for tctype in type_data:
                        nonlocal row, col
                        type_first_row = row
                        col += 1

                        yield transport, network, tctype, type_data[tctype]

                        col -= 1
                        merge_cell(specsheet, type_first_row, col, row-1, col, tctype, form.cell)

            def write_module(tuple_data):
                for transport, network, tctype, module_data in tuple_data:
                    for module in module_data:
                        if (len(list(module_data[module])) == 0):
                            continue

                        nonlocal row, col
                        module_first_row = row
                        col += 1

                        yield module_data[module]

                        col -= 1
                        merge_cell(specsheet, module_first_row, col, row-1, col, module, form.cell)

            def write_suite(modules):
                for module in modules:
                    for suite in list(module):
                        nonlocal row, col
                        suite_first_row = row
                        col += 1
                        if (len(module[suite]) == 0):
                            specsheet.write(row, col, "", form.cell)
                            row += 1

                        yield module[suite]

                        col -=1
                        merge_cell(specsheet, suite_first_row, col, row-1, col, suite, form.cell)

            def write_tc(suites):
                for suite in suites:
                    for spec in sorted(suite.values(), key=operator.attrgetter('line')):
                        nonlocal row, col
                        specsheet.write(row, col, spec.name, form.cell)
                        specsheet.write(row, col+1, spec.line, form.cell)
                        spec_col = col
                        col += 2

                        for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
                            if (title[1] == ''):
                                continue
                            specsheet.write(row, col, spec.__dict__[key], form.cell_wrap)
                            col += 1

                        row += 1
                        col = spec_col

            walk_through_testcases_by_transport(write_transport, write_network, write_tctype, write_module, write_suite, write_tc)

        workbook.close()


class TCSummary:


    def __init__(self):
        self.platform_count = dict()
        self.transport_count = dict()
        self.network_count = dict()
        self.tctype_count = dict()
        self.module_count = dict()
        self.review_count = dict()

        for platform in TEST_PLATFORM:
            self.platform_count[platform] = 0
            self.transport_count[platform] = dict()
            self.network_count[platform] = dict()
            self.tctype_count[platform] = dict()
            self.module_count[platform] = dict()
            self.review_count[platform] = dict()

            for transport in TEST_TRANSPORT:
                self.transport_count[platform][transport] = 0
                self.network_count[platform][transport] = dict()
                self.tctype_count[platform][transport] = dict()
                self.module_count[platform][transport] = dict()
                self.review_count[platform][transport] = dict()

                for network in TEST_NETWORK:
                    self.network_count[platform][transport][network] = 0
                    self.tctype_count[platform][transport][network] = dict()
                    self.module_count[platform][transport][network] = dict()
                    self.review_count[platform][transport][network] = dict()

                    for tctype in TESTCASE_TYPES:
                        self.tctype_count[platform][transport][network][tctype] = 0
                        self.module_count[platform][transport][network][tctype] = dict()
                        self.review_count[platform][transport][network][tctype] = dict()

                        for module in TESTSUITE_MODULES:
                            self.module_count[platform][transport][network][tctype][module] = 0
                            self.review_count[platform][transport][network][tctype][module] = 0


    def update_tc(self, platform, transport, network, tctype, module, count, reviewer_count):
        #print (platform + ' - ' + transport + ' - ' + tctype + ' - ' + module + ' = ' + str(count))
        self.platform_count[platform] += count
        self.transport_count[platform][transport] += count
        self.network_count[platform][transport][network] += count
        self.tctype_count[platform][transport][network][tctype] += count
        self.module_count[platform][transport][network][tctype][module] += count
        self.review_count[platform][transport][network][tctype][module] += reviewer_count

