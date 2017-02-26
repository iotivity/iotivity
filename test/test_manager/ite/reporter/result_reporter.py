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

import sys
import operator
import os

from xlsxwriter.workbook import Workbook

from ite.config import *
from ite.constants import *
from ite.result.collector import TestResultCollector
from ite.tc.container import TestSpecContainer
from ite.reporter.excel_util import *
from ite.reporter.reporter_util import *
from ite.util import *

class TestResultReporter:


    def __init__(self):
        self.summary = dict()
        self.failtc = dict()
        self.passtc = dict()
        self.testspec = None

    def make_summary(self, data):
        for platform in TEST_PLATFORM:
            if not platform in data:
                continue

            self.summary[platform] = dict()
            self.failtc[platform] = dict()
            self.passtc[platform] = dict()

            for target in data[platform]:
                self.summary[platform][target] = dict()
                self.failtc[platform][target] = dict()
                self.passtc[platform][target] = dict()

                for transport in data[platform][target]:
                    self.summary[platform][target][transport] = dict()
                    self.failtc[platform][target][transport] = dict()
                    self.passtc[platform][target][transport] = dict()

                    for network in data[platform][target][transport]:
                        self.summary[platform][target][transport][network] = dict()
                        self.failtc[platform][target][transport][network] = dict()
                        self.passtc[platform][target][transport][network] = dict()

                        for tctype in TESTCASE_TYPES:
                            self.summary[platform][target][transport][network][tctype] = dict()
                            self.failtc[platform][target][transport][network][tctype] = dict()
                            self.passtc[platform][target][transport][network][tctype] = dict()

                            for module in TESTSUITE_MODULES:
                                self.summary[platform][target][transport][network][tctype][module] = dict()
                                self.failtc[platform][target][transport][network][tctype][module] = dict()
                                self.passtc[platform][target][transport][network][tctype][module] = dict()

                                for result in TC_RESULT:
                                    self.summary[platform][target][transport][network][tctype][module][result] = 0

                                if ((not tctype in list(data[platform][target][transport][network])) or (not module in list(data[platform][target][transport][network][tctype]))):
                                    continue

                                for suite in list(data[platform][target][transport][network][tctype][module]):
                                    self.failtc[platform][target][transport][network][tctype][module][suite] = list()
                                    self.passtc[platform][target][transport][network][tctype][module][suite] = list()

                                    for tc_name in list(data[platform][target][transport][network][tctype][module][suite]):
                                        testcase = data[platform][target][transport][network][tctype][module][suite][tc_name]
                                        if (testcase.fail > 0):
                                            self.summary[platform][target][transport][network][tctype][module][TC_RESULT.FAIL] += 1
                                            self.failtc[platform][target][transport][network][tctype][module][suite].append(testcase)
                                        else:
                                            self.summary[platform][target][transport][network][tctype][module][TC_RESULT.PASS] += 1
                                            self.passtc[platform][target][transport][network][tctype][module][suite].append(testcase)


    def generate_testresult_report(self, path, spec_data):
        collector = TestResultCollector()
        collector.collect_results(path)

        self.testspec = spec_data

        self.make_summary(collector.result.data)

    def report_fail_result_to_txt(self, dir_path):
        self.report_test_result_to_txt(dir_path, self.failtc)

    def report_pass_result_to_txt(self, dir_path):
        self.report_test_result_to_txt(dir_path, self.passtc)

    def report_test_result_to_txt(self, dir_path, result_suite):
        for platform in TEST_PLATFORM:
            if not platform in result_suite:
                continue

            for target in result_suite[platform]:
                for transport in result_suite[platform][target]:
                    transport_name = transport
                    if transport_name == NO_TRANSPORT:
                        transport_name = ''
                    for network in result_suite[platform][target][transport]:
                        network_name = network
                        if network_name == NO_NETWORK:
                            network_name = ''
                        new_dir = os.path.join(dir_path, '%s_%s_%s_%s' %(platform, target.strip(), transport_name, network_name))
                        os.makedirs(new_dir)
                        for tctype in result_suite[platform][target][transport][network]:
                            for module in result_suite[platform][target][transport][network][tctype]:
                                for suite in list(result_suite[platform][target][transport][network][tctype][module]):
                                    for testcase in list(result_suite[platform][target][transport][network][tctype][module][suite]):
                                        file_path = os.path.join(new_dir, testcase.name + ".txt")
                                        txt = open_write_file(file_path)
                                        if txt == False:
                                            continue
                                        print("[Defect Info]", file=txt)
                                        print(" Title: ", file=txt)
                                        print(" Description: \n", file=txt)

                                        if platform in self.testspec:
                                            if transport in self.testspec[platform]:
                                                if network in self.testspec[platform][transport]:
                                                    if tctype in self.testspec[platform][transport][network]:
                                                        if module in self.testspec[platform][transport][network][tctype]:
                                                            if suite in self.testspec[platform][transport][network][tctype][module]:
                                                                if testcase.name in self.testspec[platform][transport][network][tctype][module][suite]:
                                                                    spec = self.testspec[platform][transport][network][tctype][module][suite][testcase.name]
                                                                    print("[Test Case Info]", file=txt)
                                                                    print(spec.to_string(), file=txt)

                                        print("\n[Test Result]", file=txt)
                                        print("Fail Rate: %d/%d\n" % (testcase.fail, testcase.fail+testcase.success), file=txt)

                                        index = 1
                                        for result in testcase.runresult:
                                            print("[%d Try]" % index, file=txt)
                                            print("--------------------------------------------------------------------", file=txt)
                                            print("Result: " + result.result, file=txt)
                                            print("Run Type: " + result.runtype, file=txt)
                                            print("Run Time: " + str(result.runtime), file=txt)
                                            print("\n<<<Fail Message>>> \n" + result.fail_msg, file=txt)
                                            print("\n<<<Test Log>>> \n" + result.test_log, file=txt)
                                            print("--------------------------------------------------------------------\n\n", file=txt)
                                            index += 1

    def report_result(self, sheet, form, report_title, result_writer, result_suite):
        row = 0
        col = 0
        for title, size in RESULT_CAT_TITLE:
            if (title == 'Line'):
                continue
            sheet.write(row, col, title, form.title)
            sheet.set_column(col, col, size)
            col += 1

        for title, size in report_title:
            sheet.write(row, col, title, form.title)
            sheet.set_column(col, col, size)
            col += 1

        row += 1
        col = 0

        def write_platform(platforms):
            for platform in platforms:
                nonlocal row, col
                platform_first_row = row

                if not platform in result_suite:
                    continue

                col += 1

                yield_testspec = dict()
                if not self.testspec == None and platform in self.testspec:
                    yield_testspec = self.testspec[platform]

                yield result_suite[platform], yield_testspec, platform

                col -= 1
                merge_cell(sheet, platform_first_row, col, row-1, col, platform, form.cell)

        def write_target(platforms):
            for platform in platforms:
                targets, testspec, platform_name = platform
                for target in list(targets):
                    nonlocal row, col
                    target_first_row = row

                    col += 1

                    yield targets[target], testspec, platform_name, target

                    col -= 1
                    merge_cell(sheet, target_first_row, col, row-1, col, target, form.cell)

        def write_transport(targets):
            for target in targets:
                transports, testspec, platform, target = target
                for transport in list(transports):
                    nonlocal row, col
                    transport_first_row = row

                    col += 1

                    yield transports[transport], testspec, platform, target, transport

                    col -= 1
                    merge_cell(sheet, transport_first_row, col, row-1, col, transport, form.cell)


        def write_network(transports):
            for transport in transports:
                networks, testspec, platform, target, transport = transport
                for network in list(networks):
                    nonlocal row, col
                    network_first_row = row

                    col += 1

                    yield networks[network], testspec, platform, target, transport

                    col -= 1
                    merge_cell(sheet, network_first_row, col, row-1, col, network, form.cell)


        def write_tctype(networks):
            for network in networks:
                types, testspec, platform, target, transport = network
                for tctype in list(types):
                    nonlocal row, col
                    type_first_row = row

                    module_count = 0
                    for module in TESTSUITE_MODULES:
                        module_count += len(types[tctype][module])
                    if module_count == 0:
                        continue

                    col += 1

                    yield_testspec = dict()
                    if tctype in testspec:
                        yield_testspec = testspec[tctype]

                    yield TESTSUITE_MODULES, types[tctype], yield_testspec, platform, target, transport, network

                    col -= 1
                    merge_cell(sheet, type_first_row, col, row-1, col, tctype, form.cell)

        def write_module(tctypes):
            for tctype in tctypes:
                module_names, modules, testspec, platform, target, transport, network = tctype
                for module in module_names:
                    nonlocal row, col
                    module_first_row = row

                    if (len(list(modules[module])) == 0):
                        continue

                    col += 1

                    yield_testspec = dict()
                    if module in testspec:
                        yield_testspec = testspec[module]

                    yield modules[module], yield_testspec, platform, target, transport, network

                    col -= 1
                    merge_cell(sheet, module_first_row, col, row-1, col, module, form.cell)

        def write_suite(modules):
            for module in modules:
                suites, testpsec, platform, target, transport, network = module
                for suite in list(suites):
                    nonlocal row, col
                    suite_first_row = row

                    if (len(suites[suite]) == 0):
                        continue

                    if not suite in suites and not suite in testpsec:
                        continue

                    col += 1

                    yield_testspec = dict()
                    if suite in testpsec:
                        yield_testspec = testpsec[suite]

                    yield suites[suite], yield_testspec, platform, target, transport, network

                    col -=1
                    merge_cell(sheet, suite_first_row, col, row-1, col, suite, form.cell)

        def write_tc(suites):
            for suite in suites:
                testcases, testspec, platform, target, transport, network = suite
                for testcase in testcases:
                    nonlocal row, col
                    row, col = result_writer(row, col, testcase, testspec, sheet, platform, target, transport, network)


        walk_through_results(write_platform, write_target, write_transport, write_network, write_tctype, write_module, write_suite, write_tc)


    def report_to_xlsx(self, path):
        workbook = Workbook(path)
        form = ExcelFormat(workbook)

        summarysheet = workbook.add_worksheet('Summary')

        row = 0
        col = 0

        summarysheet.merge_range(row, col, row+3, col, '', form.cell)

        col += 1
        for platform in TEST_PLATFORM:
            if not platform in self.summary:
                continue

            row += 1
            platform_col = col

            for target in self.summary[platform]:
                row += 1
                target_col = col

                for tc_type in TESTCASE_TYPES + ('Total',):
                    row += 1
                    tc_type_col = col

                    for result in TC_RESULT + ('Total',):
                        summarysheet.write(row, col, result, form.result_title[result])
                        col += 1

                    row -= 1
                    summarysheet.merge_range(row, tc_type_col, row, col-1, tc_type, form.title)

                row -= 1
                summarysheet.merge_range(row, target_col, row, col-1, target, form.title)

            row -= 1
            summarysheet.merge_range(row, platform_col, row, col-1, platform, form.title)

        total_col = col
        for result in TC_RESULT + ('Total','Pass Rate',):
            summarysheet.write(row+3, col, result, form.result_title[result])
            col += 1
        summarysheet.merge_range(row, total_col, row+2, col-1, "Total", form.total)

        row += 4
        col = 0

        for module in TESTSUITE_MODULES :
            col = 0
            summarysheet.write(row, col, module, form.title)
            col += 1

            module_total_txt = dict()
            for result in TC_RESULT + ('Total',):
                module_total_txt[result] = '=SUM('

            for platform in TEST_PLATFORM:
                if not platform in self.summary:
                    continue

                for target in self.summary[platform]:
                    for tc_type in TESTCASE_TYPES:
                        for result in TC_RESULT:
                            result_sum = 0
                            for transport in self.summary[platform][target]:
                                for network in self.summary[platform][target][transport]:
                                    result_sum += self.summary[platform][target][transport][network][tc_type][module][result]
                            summarysheet.write(row, col, result_sum, form.cell)
                            col += 1

                        total_txt = '=SUM(%s:%s)' % (get_cell_name(col - len(TC_RESULT), row), get_cell_name(col - 1, row))
                        summarysheet.write(row, col, total_txt, form.cell)
                        col += 1

                    for result in TC_RESULT + ('Total',):
                                total_txt = '=SUM(%s,%s)' % (get_cell_name(col - (len(TC_RESULT) + 1)*2, row), get_cell_name(col - (len(TC_RESULT) + 1), row))
                                summarysheet.write(row, col, total_txt, form.cell)
                                module_total_txt[result] += '%s,' % get_cell_name(col, row)
                                col += 1

            for result in TC_RESULT + ('Total',):
                module_total_txt[result] += ')'
                summarysheet.write(row, col, module_total_txt[result], form.cell)
                col += 1

            total_txt = '=IF(%s=0,"",ROUND(%s/%s*100, 2))' % (get_cell_name(col - 1, row), get_cell_name(col - 3, row), get_cell_name(col - 1, row))
            summarysheet.write(row, col, total_txt, form.cell)

            row += 1

        col = 0
        summarysheet.write(row, col, 'Total', form.total)
        col += 1

        for platform in TEST_PLATFORM:
                if not platform in self.summary:
                    continue

                for target in self.summary[platform]:
                    for tc_type in TESTCASE_TYPES + ('Total',):
                        for result in TC_RESULT + ('Total',):
                            total_txt = '=SUM(%s:%s)' % (get_cell_name(col, row - len(TESTSUITE_MODULES)), get_cell_name(col, row - 1))
                            summarysheet.write(row, col, total_txt, form.total_no)
                            col += 1

        for result in TC_RESULT + ('Total',):
            total_txt = '=SUM(%s:%s)' % (get_cell_name(col, row - len(TESTSUITE_MODULES)), get_cell_name(col, row - 1))
            summarysheet.write(row, col, total_txt, form.total_no)
            col += 1
        total_txt = '=IF(%s=0,"",ROUND(%s/%s*100, 2))' % (get_cell_name(col - 1, row), get_cell_name(col - 3, row), get_cell_name(col - 1, row))
        summarysheet.write(row, col, total_txt, form.total_no)

        def write_pass_result(row, col, testcase, testspec, sheet, platform, target, transport, network):
            tc_col = col
            tc_row = row
            col += 2

            index = 0
            while(index < len(testcase.runresult)):
                sheet.write(row, col, testcase.runresult[index].result, form.cell)
                sheet.write(row, col + 1, testcase.runresult[index].runtype, form.cell)
                sheet.write(row, col + 2, testcase.runresult[index].runtime, form.cell)
                sheet.write(row, col + 3, testcase.runresult[index].fail_msg, form.cell_wrap)
                sheet.write(row, col + 4, testcase.runresult[index].test_log, form.cell_wrap)
                index += 1
                row +=1

            col = tc_col
            merge_cell(sheet, tc_row, col, row - 1, col, testcase.name, form.cell)
            col += 1
            merge_cell(sheet, tc_row, col, row - 1, col,
                            "%d/%d" % (testcase.fail, testcase.fail + testcase.success), form.cell)
            col += 6

            if testcase.name in testspec:
                spec = testspec[testcase.name]
                for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
                    if (title[0] < 5):
                        continue;
                    merge_cell(sheet, tc_row, col, row -1, col, spec.__dict__[key], form.cell_wrap)
                    col += 1

            col = tc_col

            return row, col

        def write_fail_result(row, col, testcase, testspec, sheet, platform, target, transport, network):
            tc_col = col
            tc_row = row
            col += 2

            index = 0
            while(index < len(testcase.runresult)):
                sheet.write(row, col, testcase.runresult[index].result, form.cell)
                sheet.write(row, col + 1, testcase.runresult[index].runtype, form.cell)
                sheet.write(row, col + 2, testcase.runresult[index].runtime, form.cell)
                sheet.write(row, col + 3, testcase.runresult[index].fail_msg, form.cell_wrap)
                sheet.write(row, col + 4, testcase.runresult[index].test_log, form.cell_wrap)
                index += 1
                row +=1

            col = tc_col
            merge_cell(sheet, tc_row, col, row - 1, col, testcase.name, form.cell)
            col += 1
            merge_cell(sheet, tc_row, col, row - 1, col,
                            "%d/%d" % (testcase.fail, testcase.fail + testcase.success), form.cell)
            col += 6

            if testcase.name in testspec:
                spec = testspec[testcase.name]
                for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
                    if (title[0] < 5):
                        continue;
                    merge_cell(sheet, tc_row, col, row -1, col, spec.__dict__[key], form.cell_wrap)
                    col += 1

            col = tc_col

            return row, col

        def write_defect_result(row, col, testcase, testspec, sheet, platform, target, transport, network):
            tc_col = col
            tc_row = row
            col += 2

            index = 0
            while(index < len(testcase.runresult)):
                sheet.write(row, col, testcase.runresult[index].result, form.cell)
                sheet.write(row, col + 1, testcase.runresult[index].runtype, form.cell)
                sheet.write(row, col + 2, testcase.runresult[index].runtime, form.cell)
                index += 1
                row +=1

            col = tc_col
            merge_cell(sheet, tc_row, col, row - 1, col, testcase.name, form.cell)
            col += 1
            merge_cell(sheet, tc_row, col, row - 1, col,
                            "%d/%d" % (testcase.fail, testcase.fail + testcase.success), form.cell)

            col += 4
            merge_cell(sheet, tc_row, col, row-1, col, '', form.cell_wrap)
            col += 1
            merge_cell(sheet, tc_row, col, row-1, col, '', form.cell)
            col += 1

            test_target = ''
            if testcase.name in list(testspec):
                test_target = testspec[testcase.name].target

            description = '[Device: %s %s]\n' %(platform, target)\
                        + '[TC: %s]\n' % (testcase.name)\
                        + '[Target: %s]\n\n' %(test_target) \
                        + DEFECT_DESCRIPTION
            merge_cell(sheet, tc_row, col, row-1, col, description, form.cell_wrap)

            col = tc_col

            return row, col

        passsheet = workbook.add_worksheet('PassTC')
        self.report_result(passsheet, form, RESULT_TITLE, write_pass_result, self.passtc)

        failsheet = workbook.add_worksheet('FailedTC')
        self.report_result(failsheet, form, RESULT_TITLE, write_fail_result, self.failtc)

        defectsheet = workbook.add_worksheet('DefectReport')
        self.report_result(defectsheet, form, DEFECT_TITLE, write_defect_result, self.failtc)

        workbook.close()
