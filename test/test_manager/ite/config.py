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

TESTCASE_TYPES = ('BTC', 'STC')
TESTSUITE_MODULES = ('IC', 'SM', 'RE', 'RC', 'CA', 'PM', 'IS', 'ES', 'NS', 'RI', 'CS', 'TM', 'RH', 'RD')
TESTCASE_CATEGORY = ('_P', '_N')

TITLE = (('Transport', 8),
         ('Network', 8),
         ('Type', 8),
         ('Module', 8),
         ('Test Suite', 30),
         ('Test Case', 40),
         ('Line', 7))

RESULT_CAT_TITLE = (('Platform', 10),
                    ('Target', 12),
                    ('Transport', 12),
                    ('Network', 8),
                    ('Type', 8),
                    ('Module', 8),
                    ('Test Suite', 30),
                    ('Test Case', 40))


RESULT_TITLE = (('Fail Rate', 10),
                ('Result', 12),
                ('Run Type', 10),
                ('Time(ms)', 8),
                ('Message', 80),
                ('Log', 80),
                ('Objective', 60),
                ('Test Target', 60),
                ('Test Data', 50),
                ('Pre-Condition', 60),
                ('Test Procedure', 60),
                ('Post-Condition', 60),
                ('Expected Result', 60))

DEFECT_TITLE = (('Fail Rate', 10),
                ('Result', 12),
                ('Run Type', 10),
                ('Time(ms)', 8),
                ('Defect', 80),
                ('Jira ID', 10),
                ('Description', 80))

PM_TYPES = namedtuple('PM_TYPES',
                           ['PMCSDK', 'PMCPP'])\
                           ('pmcsdk', 'pmcpp')

RI_TYPES = namedtuple('RI_TYPES',
                           ['RICSDK', 'RICPP'])\
                           ('ricsdk', 'ricpp')

TESTFW_TYPES = namedtuple('TESTFW_TYPES',
                           ['GTEST', 'JUNIT', 'ARDUINOUNIT'])\
                           ('gtest', 'junit', 'arduinounit')

TEST_PLATFORM = namedtuple('TEST_PLATFORM',
                           ['LINUX', 'TIZEN', 'ANDROID', 'ARDUINO'])\
                           ('LINUX', 'TIZEN', 'ANDROID', 'ARDUINO')

NO_TRANSPORT = 'NONE'
NO_NETWORK = 'NONE'

TEST_NETWORK = namedtuple('TEST_NETWORK',
                           ['NONE', 'ETH', 'WIFI'])\
                           (NO_NETWORK, 'ETH', 'WIFI')

TEST_TRANSPORT = namedtuple('TEST_TRANSPORT',
                           ['NONE', 'IP', 'BT', 'BLE', 'TCP'])\
                           (NO_TRANSPORT, 'IP', 'BT', 'BLE', 'TCP')

DEFINE_TRANSPORT = namedtuple('TEST_TRANSPORT',
                           ['ALL_TRANSPORT', 'IP', 'BT', 'BLE', 'TCP'])\
                           ('ALL_TRANSPORT', 'IP', 'BT', 'BLE', 'TCP')

TRANSPORT_VALUES = {
                    'IP'  : 1 << 1,
                    'BLE' : 1 << 2,
                    'BT'  : 1 << 3,
                    'TCP' : 1 << 4
                  }

ALL_TRANSPORT_VALUES = {
                        'LINUX'   : '(IP+TCP)',
                        'TIZEN'   : '(IP+BT+BLE)',
                        'ANDROID' : '(IP+TCP)',
                        'ARDUINO' : '(IP+BLE)'
                      }

DEFECT_STATUS = namedtuple('DEFECT_STATUS',
                           ['OPEN', 'RESOLVED', 'CLOSED', 'TOTAL'])\
                           (('Open', 8), ('Resolved', 8), ('Closed', 8), ('Total', 8))

DEFECT_SEVERITY_STATUS = namedtuple('DEFECT_SEVERITY_STATUS',
                           ['ACTIVE', 'CLOSED', 'TOTAL'])\
                           ('Active', 'Closed', 'Total')

DEFECT_SEVERITY_TYPE = namedtuple('DEFECT_SEVERITY_TYPE',
                           ['CRITICAL', 'MAJOR', 'MINOR'])\
                           (('Critical', 8), ('Major', 8), ('Minor', 8))

JIRA_DEFECT_TITLE = (('Module', 8),
                     ('ID', 10),
                     ('Status', 10),
                     ('Resolution', 10),
                     ('Priority', 10),
                     ('Severity', 8),
                     ('Title', 60),
                     ('Description', 60),
                     ('Comments', 60),
                     ('Link', 35),
                     ('Reporter', 20),
                     ('Closable', 10))

TAG_DIC = { #'author'        : (1, '', 65),
           'reviewer'       : (2, 'Reviewer', 65),
           'since'          : (3, '', 12),
#           'see'            : (4, 'See', 50),
           'objective'      : (5, 'Objective', 60),
           'target'         : (6, 'Test Target', 60),
           'test_data'      : (7, 'Test Data', 50),
           'pre_condition'  : (8, 'Pre-Condition', 60),
           'procedure'      : (9, 'Test Procedure', 60),
           'post_condition' : (10, 'Post-Condition', 60),
           'expected'       : (11, 'Expected Result', 60) }

'''
Excel Fromatting
'''
XLSL_CELL_FORMAT = {'border':1, 'align':'center', 'valign':'vcenter'}
XLSX_FORMAT = {
               'cell'       : XLSL_CELL_FORMAT,
               'cell_wrap'  : {'border':1, 'align':'justify', 'valign':'vcenter'},
               'cell_bold'  : dict(XLSL_CELL_FORMAT.items() | {'bold':1}.items()),
               'title'      : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#FABF8F', 'bold':1}.items()),
               'total'      : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#CCC0DA', 'bold':1}.items()),
               'total_no'   : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#CCC0DA'}.items()),
               'pass_title' : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#C4D79B', 'bold':1}.items()),
               'fail_title' : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#DA9694', 'bold':1}.items()),
               'pass'       : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#C4D79B'}.items()),
               'fail'       : dict(XLSL_CELL_FORMAT.items() | {'bg_color':'#DA9694'}.items()),
               }

