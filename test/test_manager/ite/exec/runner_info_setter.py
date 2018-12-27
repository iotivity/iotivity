import os
import re
import subprocess
from subprocess import Popen, PIPE

from ite.multi_logger import print_runner_output

from ite.config import *

class RunnerInfoSetter:

    def __init__(self, test_root):
        self.test_root = test_root

    def set_gtest_tc_list(self, test_result_dir, given_module, file_filter, given_testsuites, given_testcases, dynamic_runner):

        binary_list = dynamic_runner.get_binary_list()

        sz = 0

        for binary_name in binary_list:

            print_runner_output(binary_name)

            if file_filter:
                if file_filter not in binary_name:
                    continue

            if given_module:
                if len(binary_name.split('_')) < 3:
                    continue

                if binary_name.split('_')[1] not in given_module:
                    continue

            testsuite = ''

            command = dynamic_runner.get_tc_list_command(binary_name)

            rc = subprocess.check_output(command, shell=True)
            log = re.sub(r'(b\'|\')', '', str(rc))

            log = dynamic_runner.remove_invalid_character_from_log(log)

            for line in log.split('\n'):
                line = line.strip()

                if not line:
                    continue

                if line.endswith('.') == True :
                    testsuite = line
                    continue

                if given_testsuites and testsuite[:-1] not in given_testsuites:
                    continue

                if given_testcases and line not in given_testcases:
                    continue

                if testsuite != '' and line != '':
                    if dynamic_runner.tc_scenario:
                        if testsuite.split('_')[-1] not in dynamic_runner.tc_scenario:
                            continue
                    dynamic_runner.add_new_tc(binary_name, testsuite[:-1], line, '')


    def set_junit_tc_list(self, test_result_dir, given_module, given_package_name, given_testsuites, given_testcases, dynamic_runner):

        for testcase_type in TESTCASE_TYPES:

            print_runner_output('testcase_type: {}'.format(testcase_type))

            if dynamic_runner.tc_scenario:
                found = False
                for temp in dynamic_runner.tc_scenario:
                    if temp.lower() == testcase_type.lower():
                        found = True
                        break
                if not found:
                    continue

            for module in TESTSUITE_MODULES:

                if given_testsuites:
                    found = False
                    for given_suite in given_testsuites:
                        if module in given_suite:
                            found = True
                            break
                    if not found:
                        continue

                if given_module:
                    if module.lower() not in given_module.lower():
                        continue

                if given_package_name:
                    package_name = given_package_name
                else:
                    package_name = 'org.iotivity.test.' + module.lower() + '.tc.' + testcase_type.lower()

                print_runner_output('package_name: {}'.format(package_name))
                cwd = os.getcwd()
                print_runner_output(cwd)

                build_dir = os.path.join(cwd, self.test_root, 'bin', dynamic_runner.platform_type, module.lower(), 'intermediates', 'classes', 'debug', package_name.replace('.', os.sep))
                print_runner_output(build_dir)

                if not os.path.exists(build_dir):
                    print_runner_output('path {} is not exist'.format(build_dir))
                    continue

                os.chdir(build_dir)
                print_runner_output(os.getcwd())

                file_list = [f for f in os.listdir(build_dir) if os.path.isfile(os.path.join(build_dir, f))]

                for suite in file_list :
                    if "$" not in suite and suite.endswith('Test.class'):
                        suite_name = suite.split('.',1)[0]
                        
                        if given_testsuites and suite_name not in given_testsuites:
                            continue

                        class_command = "find -name '{}.class' | xargs javap -p".format(suite_name)
                        rc = subprocess.check_output(class_command, shell=True)
                        log = re.sub(r'(b\'|\')', '', str(rc))

                        for line in log.split('\\n'):
                            line = re.sub(r'(b\'|\')', '', str(line.strip()))

                            if 'public void test' in line:
                                begin_index = line.find('test')
                                end_index = line.find('(')
                                tc_name = line[begin_index:end_index]

                                if given_testcases and tc_name not in given_testcases:
                                    continue

                                dynamic_runner.add_new_tc('', suite_name, tc_name, package_name)

                os.chdir(cwd)
