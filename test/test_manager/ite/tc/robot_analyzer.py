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

import re
import os
import operator
from itertools import takewhile

from ite.config import *
from ite.util import *

class RobotTCAnalyzer:

    FILE_PATTERN = re.compile(r'(?P<testsuite>\w+)\.\w+')
    TESTSUITE_PATTERN = re.compile(r'(?P<service>\w+)_(?P<testarea>\w+)_(?P<module>\w+)_(?P<objective>\w+)')
    TC_HEADER_PATTERN = re.compile(r'\*\*\* Test Cases \*\*\*', re.DOTALL)
    SECTION_PATTERN = re.compile(r'\*\*\*.+\*\*\*', re.DOTALL)
    TC_NAME_PATTERN = re.compile(r'^(?P<testcase>\w+.*)')
    DOCUMENTATION_PATTERN = re.compile(r'\[Documentation\]')
    TAG_PATTERN = re.compile(r'\[Tags\]')

    def count_file_bytes(self, iterable):
        file_bytes = 0
        for item in iterable:
            file_bytes += len(item)
            yield file_bytes

    def get_line_number(self, path, pos):
        file = open_read_file(path);
        if file == None:
            return 0

        line = 1+len(list(takewhile(lambda x: x <= pos, self.count_file_bytes(file))))

        file.close()

        return line

    def get_testsuite_info(self, path):
        match = self.FILE_PATTERN.search(path)
        if match == None:
            return None, None, None
        testsuite = match.group('testsuite')

        match = self.TESTSUITE_PATTERN.search(testsuite)
        if match == None:
            return None, None, None
        module = match.group('module')
        objective = match.group('objective')

        platform = ''
        for candidate in TEST_PLATFORM:
            if candidate.lower() in objective.lower():
                platform = candidate.upper()
                break;

        return testsuite, platform, module

    def get_testcase_section(self, source):
        header_match = self.TC_HEADER_PATTERN.search(source)
        if header_match == None:
            return ''
        header_pos = header_match.end()

        testcase_section = source[header_pos:]
        end_match = self.SECTION_PATTERN.search(testcase_section)
        if end_match != None:
            testcase_section = testcase_section[:end_match.start()]

        return testcase_section, header_pos

    def get_comments_area(self, tc_area):
        doc_match = self.DOCUMENTATION_PATTERN.search(tc_area)
        tag_match = self.TAG_PATTERN.search(tc_area)

        if doc_match == None:
            return '', ''

        comments = tc_area[doc_match.start():]
        tag = ''

        if not tag_match == None:
            comments = tc_area[doc_match.start():tag_match.start()]
            tag = tc_area[tag_match.end():].splitlines()[0].strip()

        return comments.strip(), tag.strip()

    def get_tc_info(self, path):
        source = read_file(path)
        if source == '':
            return

        testsuite, platform, module = self.get_testsuite_info(path)
        testcase_section, pos = self.get_testcase_section(source)

        comments = ''
        comment = ''
        testcase = ''
        line = 0
        line_count = self.get_line_number(path, pos)
        tag = ''
        for strline in testcase_section.splitlines():
            line_count += 1
            match = self.TC_NAME_PATTERN.search(strline)
            if match == None:
                comments += strline + '\n'
                continue

            if not testcase == '':
                comment, tag = self.get_comments_area(comments)
                yield line, platform, module, testsuite, testcase, comment, tag

            testcase = strline
            line = line_count - 1
            comments = ''

        comment, tag = self.get_comments_area(comments)
        yield line, platform, module, testsuite, testcase, comment, tag

    def analyze_tc_file(self, path):
        if (os.path.isfile(path) == False):
            return

        print("### Start to analyze test case file: " + path)

        test_data = list()
        invalid_tc = list()
        for line, platform, module, suite, name, comments, tag in self.get_tc_info(path):
            #print("line: %d, platform: %s, moudle: %s, testsuite: %s, testcase: %s, tag: %s, comments: %s" %
            #      (line, platform, module, suite, name, tag, comments))

            spec = TestSpec(line, suite, name)
            success, message = spec.check_tc_naming_rule()
            if (success == False):
                invalid_tc.append((line, suite, name, message))
                continue

            if len(comments) == 0:
                invalid_tc.append((line, suite, name, 'Cannot Find TC Comments'))
                continue

            if len(tag) == 0:
                invalid_tc.append((line, suite, name, 'Cannot Find TC Tag'))
                continue

            if not 'positive' in tag.lower() and not 'negative' in tag.lower():
                invalid_tc.append((line, suite, name, 'TC Tag do not include (Positive|Negative) Category: ' + tag))
                continue

            success, message = spec.parse_comment(comments)
            if (success == False):
                invalid_tc.append((line, suite, name, message))
                continue

            if not platform in TEST_PLATFORM:
                invalid_tc.append((line, suite, name, 'Invalid Platform Definition: ' + platform))
                continue

            test_data.append((platform, module, spec))

            #print("test spec: " + spec.to_string())

        return test_data, invalid_tc

class TestSpec:


    COMMENT_PATTERN = re.compile('\|(?P<tag>.*)\|(?P<description>.*)\|')

    def __init__(self, line, suite, name):
        self.line = line
        self.suite = suite
        self.name = name

        for tag in list(TAG_DIC):
            self.__dict__[tag] = ''

    def to_string(self):
        result = "%s.%s(line: %d)\n" %(self.suite, self.name, self.line)
        for key, title in sorted(TAG_DIC.items(), key=operator.itemgetter(1)):
            if (self.__dict__[key] == None or title[1] == ''):
                continue
            result += "\t%-15s : %s\n" %(title[1], self.__dict__[key].replace("\n", "\t\n                      "))

        return result

    def check_tc_naming_rule(self):
        if (self.suite == '' or self.name == ''):
            return False, 'Empty Test Suite Name or Empty Test Case Name'

        return True, '';

    def parse_comment(self, comments):
        tag = ''
        description = ''

        prev_tag = ''
        for match in self.COMMENT_PATTERN.finditer(comments):
            tag = match.group('tag').strip()
            description = match.group('description').strip()

            if tag == '':
                tag = prev_tag

            if (tag in self.__dict__):
                self.__dict__[tag] = '\n'.join([self.__dict__[tag], description]).strip()

            prev_tag = tag

        return True, ''