#!/usr/bin/python3

import sys
import os
from xlsxwriter.workbook import Workbook
from ite.result.MemcheckReporter import MemcheckReporter

class DynamicReportGen:
    def __init__(self):
        self.summary  = dict()
        self.memfailures = dict()

    def find_processes(self, data):
        pass

    def killProcesses(self, data):
        pass

    def collectErrors(self, xml_data_location, memcheck_xls_name):
        collector = MemcheckReporter()
        collector.collect_XML_results(xml_data_location)
        collector.putResultsInExcel(memcheck_xls_name, summary_only = False)
        return
