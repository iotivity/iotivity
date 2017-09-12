#!/usr/bin/python3

import xml.etree.ElementTree as ET
import os
import xlsxwriter


class XMLResult:
    def __init__(self):
        self.error_summary = dict()
        self.error_dictionary = dict()

    def add_error_to_dictionary(self,name_of_error,name_of_call, call_funciton, name_of_termination,
                                termination_function, name_of_test):
        def create_key (key, parent_dict):
            if(not key in parent_dict):
                parent_dict[key] =dict()

        def create_value (key, parent_dict):
            if(not key in parent_dict):
                parent_dict[key] =[]

        def append_value(value, parent_dict):
            if (not value in parent_dict):
                parent_dict.append(value)

        create_key(name_of_error,self.error_dictionary)
        create_key(name_of_call,self.error_dictionary[name_of_error])
        create_key(call_funciton, self.error_dictionary[name_of_error][name_of_call])
        create_key(name_of_termination, self.error_dictionary[name_of_error][name_of_call][call_funciton])
        create_value(termination_function, self.error_dictionary[name_of_error][name_of_call][call_funciton][name_of_termination])
        append_value(name_of_test, self.error_dictionary[name_of_error][name_of_call][call_funciton][name_of_termination][termination_function])

    def add_error_summary(self,name_of_error, name_of_test):

        def create_summary (key, parent_dict):
            if(not key in parent_dict):
                parent_dict[key] =[]

        def append_summary_value(value, parent_dict):
            if (not value in parent_dict):
                parent_dict.append(value)

        create_summary(name_of_error,self.error_summary)
        append_summary_value(name_of_test, self.error_summary[name_of_error])


class MemcheckReporter:
    def __init__(self):
        self.result = XMLResult()

    def create_dictionary(self, name_of_call, call_funciton, name_of_termination, termination_function, name_of_test):
        global errorDictionary

    def collect_XML_results(self, path):
        if (not os.path.isdir(path)):
            return
        for dirpath, dirnames, filenames in os.walk(path):
            for filename in filenames:
                filepath= os.path.join(dirpath, filename)
                if filename.endswith('.xml'):
                    self.details(filepath)

    def putResultsInExcel(self, excel_filename, summary_only= True ):
        workBook = xlsxwriter.Workbook(excel_filename)
        result_dict = self.result
        # format for the lable in the first line
        lable_format = workBook.add_format()
        lable_format.set_border()
        lable_format.set_align('center')
        lable_format.set_align('vcenter')
        lable_format.bold

        # format for the content
        endFormat = workBook.add_format()
        endFormat.set_border()
        endFormat.set_align('center')
        endFormat.set_align('vcenter')

        row = 0
        col = 0
        summary_sheet = workBook.add_worksheet('Summary')
        summary_lable_list = ["ErrorName", "Test Case" ]
        for lable in summary_lable_list:
            np = summary_sheet.write(row, col, lable, lable_format)
            col += 1
        else:
            row = 1
            col = 1


        for error_name in result_dict.error_summary.keys():
            row_start = row
            for test_case_names in result_dict.error_summary[error_name]:
                np = summary_sheet.write(row, col, test_case_names, endFormat)
                row += 1
            else:
                summary_sheet.merge_range(row_start, col-1, row - 1, col-1, error_name, endFormat)


        if not summary_only:
            row = 0
            col = 0
            comprehensive_sheet = workBook.add_worksheet('Comprehensive')
            comprehensive_sheet_lable_list = ["ErrorName","Called SO file","Called Method","End SO File","End Function","Testcase Name"]
            for lable in comprehensive_sheet_lable_list:
                np = comprehensive_sheet.write(row, col, lable, lable_format)
                col += 1
            else:
                row = 1
                col = 0

            for error_name in result_dict.error_dictionary.keys():
                col+=1
                error_name_row = row
                for name_of_call in result_dict.error_dictionary[error_name]:
                    col+=1
                    call_row = row
                    for name_of_function in result_dict.error_dictionary[error_name][name_of_call]:
                        col+=1
                        function_row = row
                        for name_of_termination in result_dict.error_dictionary [error_name][name_of_call][name_of_function]:
                            col+=1
                            termination_row = row
                            for termination_funciton in result_dict.error_dictionary[error_name][name_of_call][name_of_function][name_of_termination]:
                                col+=1
                                termination_funciton_row = row
                                for test_case_names in result_dict.error_dictionary[error_name][name_of_call][name_of_function][name_of_termination][termination_funciton]:
                                    comprehensive_sheet.write(row, col, test_case_names, endFormat)
                                    row = row + 1
                                else:
                                    col -= 1
                                    if termination_funciton_row < row - 1 :
                                        comprehensive_sheet.merge_range(termination_funciton_row, col, row-1, col, termination_funciton, endFormat)
                                    else:
                                        comprehensive_sheet.write(row-1, col, termination_funciton, endFormat)
                            else:
                                col -= 1
                                if termination_row < row - 1 :
                                    comprehensive_sheet.merge_range(termination_row, col , row-1, col, name_of_termination, endFormat)
                                else:
                                    comprehensive_sheet.write(row -1 , col, name_of_termination, endFormat)
                        else:
                            col -= 1
                            if (function_row < row -1):
                                comprehensive_sheet.merge_range(function_row, col, row-1, col, name_of_function, endFormat)
                            else:
                                comprehensive_sheet.write(row-1, col, name_of_function, endFormat)
                    else:
                        col -= 1
                        if call_row < row-1:
                            comprehensive_sheet.merge_range(call_row, col, row-1, col, name_of_call, endFormat)
                        else:
                            comprehensive_sheet.write(row-1,col, name_of_call, endFormat)
                else:
                    col -= 1
                    if error_name_row < row-1:
                        comprehensive_sheet.merge_range(error_name_row, col, row-1, col, error_name, endFormat)
                    else:
                        comprehensive_sheet.write(row-1, col, error_name, endFormat)







        workBook.close()

    def create_exception_dictionary(self, name_of_the_test, name_of_exception):
        global exception_dictinoary


    def details (self, filename):
        xml_root = ET.parse(filename)
        name_found = 0
        name_of_test = 'name not found'
        try:
            arguments = xml_root.find('args').find('argv').findall('arg')
        except Exception as e:
            name_of_test = 'xml cannot open %s' %filename       # print in the console in the last about the failures
            exception_dictinoary[filename]= e           # make a log file with exceptions and filenames
        for arg in arguments:
            if '--gtest_filter' in arg.text:
                name_of_test = (arg.text.split('='))[1]
                name_found = 1
        errors = xml_root.findall('error')
        for error in errors:
            required_error_found = 0
            name_of_error = None
            name_of_call = "Not_found"
            function_in_cal = "Not_found"
            name_of_termination = "Not_found"
            name_of_termination_function = "Not_found"
            kind = error.find('kind')
            if kind != None:
                name_of_error= kind.text
            stack = error.find('stack')
            if stack:
                frames = stack.findall('frame')
                for index, frame in enumerate(frames):
                    if frame.find('obj') != None:
                        if '/out/linux/' in frame.find('obj').text:
                            if index + 1 < len(frames):
                                if frames[index+1].find('obj') != None:
                                    if '/test/bin/linux/' in frames[index+1].find('obj').text:
                                        name_of_call = frame.find('obj').text
                                        if frame.find('fn') != None:
                                            function_in_cal = frame.find('fn').text
                            if name_of_termination == "Not_found":
                                name_of_termination = frame.find('obj').text
                                if frame.find('fn') != None:
                                    name_of_termination_function = frame.find('fn').text

            if(name_of_call != "Not_found" or name_of_termination != "Not_found") and (name_of_error != None):
                self.result.add_error_to_dictionary(name_of_error,name_of_call,function_in_cal,name_of_termination,name_of_termination_function,name_of_test)
                self.result.add_error_summary(name_of_error, name_of_test)