# ------------------------------------------------------------------------
# Copyright 2015 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------------

import os
import platform
import string

def run_test(env, run_valgrind, test, test_targets=None):
    """
    Run test using the given SCons environment.

    Test results go to the test_out subdir of the build directory,
    that path is passed to Googletest.

    If run_valgrind is true, build a log filename from the test path
    and run the test under valgrind (if the platform supports it).
    Jenkins is configured to pick up valgrind output files ending in
    .memcheck and include them in the build report.

    Note that the test path should not include the build directory
    where binaries are placed.  The build directory will be prepended
    to the test path automatically.
    """

    build_dir = env.get('BUILD_DIR')
    result_dir = os.path.join(build_dir, 'test_out/')
    if not os.path.isdir(result_dir):
        os.makedirs(result_dir)

    # Dump test report in XML format to the results directory.
    env.AppendENVPath('GTEST_OUTPUT', 'xml:' + result_dir, delete_existing=0)

    # Make sure tests find any iotivity shared lib they may have linked with
    env.AppendENVPath('LD_LIBRARY_PATH', build_dir)

    if not test_targets:
        test_targets = ['test']

    if run_valgrind:
        # transform path to test to a string form
        # we are ignoring the string value even if one was passed
        out_file = test.replace('_', '-').replace(os.sep, '_')
        out_file += '.memcheck'

    if env.get('VALGRIND_CHECKS'):
        have_valgrind = True
    else:
        have_valgrind = False

    test_cmd = os.path.join(build_dir, test)

    if run_valgrind and have_valgrind:
        # modify test command line to run test under valgrind

        # Environment variables to be made available during the
        # Valgrind run.
        valgrind_env = ''

        # GLib uses a custom memory allocation scheme that can
        # sometimes confuse Valgrind.  Configure GLib to be Valgrind
        # friendly.
        valgrind_env += 'G_DEBUG=gc-friendly G_SLICE=always-malloc'

        # Valgrind suppressions file. This is passed to valgrind, not
        # handled by SCons, so need the path to the real file, not variant.
        suppression_file = env.File('#tools/valgrind/iotivity.supp').srcnode().path

        # Set up to run the test under Valgrind.
        # The "--num-callers" option specifies the callstack depth (default 12)
        # We are increasing it here to allow unit test name to be visible
        # in the leak report.
        vg_opt = string.join([
            "--suppressions=" + suppression_file,
            "--leak-check=full",
            "--num-callers=24",
            "--xml=yes --xml-file=" + out_file,
        ])
        test_cmd = '%s valgrind %s %s' % (valgrind_env, vg_opt, test_cmd)

    if env.get('TARGET_OS') in ['linux']:
        env.Depends('ut' + test , os.path.join(build_dir, test))
    # a hyphen in front of the action means ignore the result: let all tests run
    # in a Jenkins run, results are collected at the end, so
    # we don't have to decide anything here.
    ut = env.Command('ut' + test, None, '-' + test_cmd)
    env.Depends(ut, test_targets)
    env.AlwaysBuild(ut)


def run_uwp_wack_test(env, cert_file, appx_file, report_output_path):
    if env.get('TARGET_OS') != 'windows' or env.get('MSVC_UWP_APP') != '1':
        return

    import winreg

    # Add WACK path to path
    appcert_reg_path = 'SOFTWARE\\Microsoft\\Windows App Certification Kit'
    # The WACK registry key is only in the default location, i.e. SOFTWARE\\Microsoft\...
    # If the machine is 64bit, then we need to look into the 64bit Registry View for the key
    reg_view_64bit = (platform.machine() == 'AMD64')
    appcert_path = env.ReadRegistryStringValue(winreg.HKEY_LOCAL_MACHINE,
                                               appcert_reg_path,
                                               'InstallLocation',
                                               reg_view_64bit)
    if not appcert_path:
        Exit('Error: Could not Find the WACK Registry Key/Value')

    env.AppendUnique(PATH = [appcert_path])
    # Need to update the 'ENV' dictionary PATH as that's what is used when executing
    # commands
    env['ENV']['PATH'] = env.get('PATH')

    # testid 38 is the "Supported APIs" test and verifies that all APIs used are available for
    # store apps.
    # Note that appcert.exe requires admin privileges and will prompt for elevation unless the
    # build is run from an elevated command shell.
    report_output_file = os.path.join(report_output_path, 'wack_output.xml')
    command = 'certutil -addstore root %s' % (cert_file)
    command += ' && appcert.exe reset'
    command += ' && appcert.exe test -AppxPackagePath %s -reportoutputpath %s -testid [38]' \
               % (appx_file, report_output_file)
    command += ' && certutil %s | for /f "delims=: tokens=1*" %%i in (\'findstr "Serial Number:"\') do (certutil -delstore root %%j)' % (cert_file)
    command += ' && findstr OVERALL_RESULT=.PASS %s' % (report_output_file)

    print('Running WACK Test')
    wack_test = env.Command(report_output_file, appx_file, command)
    env.AlwaysBuild(wack_test)

