#!/usr/bin/python3
from lxml import etree


class XmlParser:
    def __init__(self, _file_path):
        self.file_path = _file_path
        self.xml_content = self.get_xml_content()
        self.suite_content = self.get_suite_content()
        self.setup_content = self.get_setup_content()
        self.teardown_content = ""
        self.testcase_lists = self.get_testcase_lists()

    def get_xml_content(self):
        return etree.parse(self.file_path).getroot()

    def get_suite_content(self):
        return self.xml_content.find('suite')

    def get_setup_content(self):
        pass
        # a = self.suite_content.find('setup')
        # print("Suite Setup Content:\n" + str(etree.tostring(a, method='xml', pretty_print=True)))
        # return a

    def get_teardown_content(self):
        # TODO
        pass

    def get_testcase_lists(self):
        return self.suite_content.findall('test')
