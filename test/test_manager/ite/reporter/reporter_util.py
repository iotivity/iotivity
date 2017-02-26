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

from ite.config import *

def walk_through_testcases_by_transport(transport_action, network_action, tctype_action, module_action, suite_action, tc_action):
    networks = transport_action()
    types = network_action(networks)
    modules = tctype_action(types)
    suites = module_action(modules)
    testcases = suite_action(suites)
    tc_action(testcases)

def walk_through_testcases_by_module(platform, tctype_action, module_action, transport_action, network_action, suite_action, tc_action):
    modules = tctype_action(platform)
    transports = module_action(modules)
    networks = transport_action(transports)
    suites = network_action(networks)
    testcases = suite_action(suites)
    tc_action(testcases)

def walk_through_results(platform_action, target_action, transport_action, network_action, tctype_action, module_action, suite_action, tc_action):
    targets = platform_action(TEST_PLATFORM)
    transports = target_action(targets)
    networks = transport_action(transports)
    types = network_action(networks)
    modules = tctype_action(types)
    suites = module_action(modules)
    testcases = suite_action(suites)
    tc_action(testcases)
