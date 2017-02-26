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
                      ['TESTSPEC_REPORT', 'SUMMARY', 'TESTSPEC', 'PLATFORM', 'TRANSPORT', 'NETWORK', 'TYPE', 'MODULE', 'TESTSUITE', 'TESTCASE'])\
                      ('testspec_report', 'summary', 'testspec', 'platform', 'transport', 'network', 'type', 'module', 'testsuite', 'testcase')
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
