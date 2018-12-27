import os
from lxml import etree

from .utility import *


class TestCase:

    def __init__(self, _tc_content, module_name, _tc_env):
        self.tc_env = _tc_env
        self.tc_content = _tc_content
        self.platform = ""
        self.module_name = module_name
        self.version = ""
        self.bit = ""
        self.network = "Wi-Fi"
        self.module_name = ""
        self.suite_name = "Need Modification"
        self.name = ""
        self.type = ""
        self.objective = ""
        self.target = ""
        self.preCondition = ""
        self.procedure = ""
        self.postCondition = ""
        self.expected = ""
        self.result = ""
        self.description = ""
        self.log_files =list()
        self.logs = list()

    def set_tc_name(self):
        self.name = self.tc_content.get('name')

    def set_tc_type(self):
        tc_tags = self.tc_content.find('tags')
        for tag in tc_tags:
            if "positive" in tag.text.lower():
                self.type = "Positive"
                break
            elif "negative" in tag.text.lower():
                self.type = "Negative"
                break

    def set_tc_result(self):
        result = self.tc_content.find('status').get('status')
        if result == "PASS":
            self.result = 1
        else:
            self.result = 0

    def set_tc_scenario(self):
        tc_scenario = self.tc_content.find('doc').text
        self.parse_doc(tc_scenario)

    def set_tc_logs(self):
        # print("Extracting Log")
        tc = self.tc_content
        logs = list()
        teardown_node = tc.xpath(Constants.teardown_node_xpath)
        logs = teardown_node[0].xpath(Constants.LOG_XPATH)

        for log in logs:
            msg = log.find("msg").text
            self.logs.append(msg)

        curr_dir = os.getcwd()
        platform = self.tc_env['os1'] + "_" + self.tc_env['os2']
        qos = self.tc_env['qos']

        transport = self.tc_env['transport']
        secured = "secured"

        if self.tc_env['secured'] == "0":
            secured = "nonsecured"

        log_dir = Utility.interop_report_dir + os.sep + Utility.module_name.lower() + '/logs/' + transport + os.sep + qos + os.sep + \
                  platform + os.sep + secured
        # print("LOG DIR : " + log_dir)
        if not os.path.isdir(log_dir):
            os.makedirs(log_dir)
        os.chdir(log_dir)

        firstapp_log_file_name = self.name.replace(' ', '_') + "_Log1.txt"
        self.log_files.append(firstapp_log_file_name)
        firstapp_log_file = open(self.log_files[0], "w")
        firstapp_log_file.write(self.logs[0])
        firstapp_log_file.close()

        secondapp_log_file_name = self.name.replace(' ', '_') + "_Log2.txt"
        self.log_files.append(secondapp_log_file_name)
        secondapp_log_file = open(self.log_files[1], "w")
        secondapp_log_file.write(self.logs[1])
        secondapp_log_file.close()
        os.chdir(curr_dir)

    def parse_doc(self, spec):
        details = list()
        items = spec.split('|')
        for item in items:
            if item == '\n':
                continue
            details.append(str(item).strip())
        # print(details)
        self.find_tc_element(details)

    def find_tc_element(self, info):
        self.objective = info[info.index('objective') + 1]
        self.target = info[info.index('target') + 1]

        precondition_index = info.index('pre_condition')
        procedure_index = info.index('procedure')
        postcondition_index = info.index('post_condition')
        expected_index = info.index('expected')

        for i in range(precondition_index + 1, procedure_index):
            if not info[i] == '':
                self.preCondition += info[i] + '\n'
            if self.preCondition == "":
                self.preCondition = "None"

        for i in range(procedure_index + 1, postcondition_index):
            if not info[i] == '':
                self.procedure += info[i] + '\n'

        for i in range(postcondition_index + 1, expected_index):
            if not info[i] == '':
                self.postCondition += info[i] + '\n'
            if self.postCondition == "":
                self.postCondition = "None"

        self.expected = info[expected_index + 1]
        self.make_tc_description()
        # print(self.description)

    def make_tc_description(self):
        self.description = "Scenario:\n\n"
        self.description += "[Pre-Condition]\n"
        self.description += self.preCondition.replace("pre_condition\n", "") + "\n"
        self.description += "[Procedure]\n"
        self.description += self.procedure.replace("procedure\n", "") + "\n"
        self.description += "[Post-Condition]\n"
        self.description += self.postCondition.replace("post_condition\n", "") + "\n"
        self.description += "[Expected]\n"
        self.description += self.expected + "\n"
        self.description += "[Actual]\n"

        print(self.description)

    def get_tc_content(self):
        self.set_tc_name()
        self.set_tc_type()
        self.set_tc_result()
        self.set_tc_logs()
        self.set_tc_scenario()

    # TODO Print TC Details
    def print_tc_details(self):
        pass


