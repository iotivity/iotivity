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

from collections import namedtuple

TC_RESULT = namedtuple('TC_RESULT',
                    ['PASS', 'FAIL'])\
                    ('Pass', 'Fail')

DEFECT_DESCRIPTION = '''[Procedure]

[Expected]

[Observation]
'''


# GTest Result Constants
GT_LOG = namedtuple('GTLOG_RESULT',
                    ['RUN', 'OK', 'FAILED', 'CRASHED', 'HANG'])\
                    ('RUN', 'OK', 'FAILED', 'CRASHED', 'HANG')
GT_ELM = namedtuple('GT_ELM',
                    ['TESTSUITES', 'TESTSUITE', 'TESTCASE', 'FAILURE'])\
                    ('testsuites', 'testsuite', 'testcase', 'failure')
GT_ATT = namedtuple('GT_ATT',
                    ['NAME', 'STATUS', 'TIME', 'MESSAGE'])\
                    ('name', 'status', 'time', 'message')
GT_ATT_STATUS = {GT_LOG.OK : 'run',
                 GT_LOG.FAILED : 'run',
                 GT_LOG.CRASHED : 'crashed',
                 GT_LOG.HANG : 'hang'
                 }
GT_RUN_TYPE = namedtuple('GT_RUN_TYPE',
                         ['STANDALONE', 'GROUPED'])\
                         ('StandAlone', 'Grouped')

GT_HANG_LOG = '[  HANG  ]'


# Test Spec XML Constants
SPEC_ELM = namedtuple('SPEC_ELM',
                      ['TESTSPEC_REPORT', 'SUMMARY', 'TESTSPEC', 'PLATFORM' , 'BUILDTYPE' , 'TRANSPORT', 'NETWORK', 'TYPE', 'MODULE', 'TESTSUITE', 'TESTCASE'])\
                      ('testspec_report', 'summary', 'testspec', 'platform', 'buildtype' , 'transport', 'network', 'type', 'module', 'testsuite', 'testcase')
SPEC_ATT = namedtuple('SPEC_ATT',
                      ['NAME', 'NO', 'FILELINE', 'REVIEW'])\
                      ('name', 'NO', 'fileline', 'Review')

# Jira Issue XML Constants
JIRA_ELM = namedtuple('JIRA_ELM',
                      ['CHANNEL', 'ITEM', 'KEY', 'SUMMARY', 'LINK', 'DESCRIPTION',
                       'TYPE', 'STATUS', 'PRIORITY', 'RESOLUTION', 'COMMENTS', 'COMMENT', "REPORTER",
                       'CUSTOMFIELDS', 'CUSTOMFIELD', 'CUSTOMFIELDNAME', 'CUSTOMFIELDVALUES', 'CUSTOMFIELDVALUE'])\
                      ('channel', 'item', 'key', 'summary', 'link', 'description',
                       'type', 'status', 'priority', 'resolution', 'comments', 'comment', "reporter",
                       'customfields', 'customfield', 'customfieldname', 'customfieldvalues', 'customfieldvalue')

JIRA_ATT = namedtuple('JIRA_ATT',
                      ['AUTHOR'])\
                      ('author')

JIRA_TYPE_BUG = 'Bug'
JIRA_SEVERITY_FIELD_NAME = 'Issue Severity'
JIRA_STATUS_RESOLVED = 'Resolved'
JIRA_STATUS_CLOSED = 'Closed'
JIRA_RESOLUTION_FIXED = 'Fixed'

WEB_MODEL_ELM = namedtuple('WEB_MODEL_ELM',
                           ['TEST_PROJECT', 'STRUCTURE', 'HEADER', 'SUMMARY'])\
                           ('test_project', 'structure', 'header', 'summary')
WEB_MODEL_ATT = namedtuple('WEB_MODEL_ATT',
                           ['NAME', 'TC', 'REVIEW'])\
                           ('name', 'tc', 'review')
