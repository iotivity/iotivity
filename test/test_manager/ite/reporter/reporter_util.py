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

from ite.config import *

def walk_through_testcases_by_transport(buildtype_action, transport_action, network_action, tctype_action, module_action, suite_action, tc_action):
    buildtypes = buildtype_action()
    networks = transport_action(buildtypes)
    types = network_action(networks)
    modules = tctype_action(types)
    suites = module_action(modules)
    testcases = suite_action(suites)
    tc_action(testcases)

def walk_through_testcases_by_module(platform, buildtype_action, tctype_action, module_action, transport_action, network_action, suite_action, tc_action):
    buildtypes = buildtype_action(platform)
    modules = tctype_action(buildtypes)
    transports = module_action(modules)
    networks = transport_action(transports)
    suites = network_action(networks)
    testcases = suite_action(suites)
    tc_action(testcases)

def walk_through_results(platform_action, target_action, build_type_action, transport_action, network_action, tctype_action, module_action, suite_action, tc_action):
    targets = platform_action(TEST_PLATFORM)
    build_types = target_action(targets)
    transports = build_type_action(build_types)
    networks = transport_action(transports)
    types = network_action(networks)
    modules = tctype_action(types)
    suites = module_action(modules)
    testcases = suite_action(suites)
    tc_action(testcases)
