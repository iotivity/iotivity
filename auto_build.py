#!/usr/bin/python

import os
import sys
import platform
import subprocess
import multiprocessing


# help message
def helpmsg(script):
    helpstr = '''
Usage:

To build:
    python %s <targetbuild>
To clean:
    python %s -c

EXEC_MODE=True/False (default false) to suppress running the commands.
VERBOSE=0/1 (default 1) to tell scons to print more information.

Allowed values for <target_build>: 
    all linux linux_unsecured linux_secured linux_full linux_unsecured_with_tcp
    linux_unsecured_with_rd linux_secured_with_rd linux_unsecured_with_mq
    linux_secured_with_tcp linux_unsecured_with_java linux_secured_with_java
    android android_universal android_universal_unsecured
    android_unsecured android_secured android_x86 android_x86_with_ip
    android_x86_with_bt android_x86_with_ble android_x86_with_rm_and_ip
    android_x86_with_rm_and_bt android_x86_with_rm_and_ble android_armeabi
    android_armeabi_with_ip android_armeabi_with_bt android_armeabi_with_ble
    android_armeabi_with_rm_and_ip android_armeabi_with_rm_and_bt
    android_armeabi_with_rm_and_ble
    windows msys simulator darwin tizen tizen_unsecured tizen_secured
    tizen_extra tizen_extra_unsecured tizen_extra_secured
    unit_tests unit_tests_secured unit_tests_unsecured
    unit_tests_memcheck_secured unit_tests_memcheck_unsecured

    Note: nearly all of these are combination builds, which means they
    are not useful to get a specific build to work on, rather they are
    useful for "make sure all these build" tests as used by the Jenkins CI.
    Any selection will build both debug and release versions of all available
    targets in the scope you've selected except for unit tests, which by
    their nature are debug only.
    To choose any specific command, please use the SCons commandline directly.
    Use \"EXEC_MODE=False %s <targetbuild>\"  to see commands to run.
    '''
    print (helpstr % (script, script, script))
    sys.exit(1)


def call_scons(build_options, scons_options):
    """
    Format and run a single scons command.

    Args:
        build_options (dict): build flags with values.
        scons_options (str): additional flags to scons.
    """
    cmd_line = "scons VERBOSE=" + VERBOSE
    for key in build_options:
        cmd_line += " " + key + "=" + str(build_options[key])

    cmd_line += " " + str(scons_options)

    if not EXEC_MODE:
        print ("Would run : " + cmd_line)
    else:
        print ("Running : " + cmd_line)
        sys.stdout.flush()
        exit_code = subprocess.Popen(cmd_line, shell=True).wait()
        if exit_code != 0:
            sys.exit(exit_code)


def build_all(flag, scons_options):
    """
    Build all the variants appropriate for the host platform.

    Args:
        flag (bool): if True, build in Release mode.
        scons_options (dict): additional flags to scons.
    """
    if platform.system() == "Linux":
        build_linux_unsecured(flag, scons_options)
        build_linux_secured(flag, scons_options)
        build_linux_unsecured_with_rm(flag, scons_options)
        build_linux_unsecured_with_rd(flag, scons_options)
        build_linux_secured_with_rd(flag, scons_options)
        build_linux_unsecured_with_mq(flag, scons_options)
        build_linux_unsecured_with_tcp(flag, scons_options)
        build_linux_secured_with_tcp(flag, scons_options)
        build_linux_unsecured_with_java(flag, scons_options)
        build_linux_secured_with_java(flag, scons_options)
        build_simulator(flag, scons_options)

        build_android_unsecured(flag, scons_options)
        build_android_secured(flag, scons_options)
        build_tizen(flag, scons_options)

    # Note: supported Windows mode is to use run.bat instead
    if platform.system() == "Windows":
        build_windows(flag, scons_options)

    if platform.system() == "Darwin":
        build_darwin(flag, scons_options)


def build_linux(flag, scons_options):
    build_linux_unsecured(flag, scons_options)
    build_linux_secured(flag, scons_options)


def build_linux_secured(flag, scons_options):
    print ("*********** Build for linux with Security *************")
    build_options = {
        'RELEASE': flag,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_unsecured(flag, scons_options):
    print ("*********** Build for linux ************")
    build_options = {
        'RELEASE': flag,
        'SECURED': 0,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_secured_with_tcp(flag, scons_options):
    print ("*********** Build for linux with Secured TCP ************")
    build_options = {
        'RELEASE': flag,
        'WITH_TCP': 1,
        'WITH_CLOUD': 1,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_unsecured_with_tcp(flag, scons_options):
    print ("*********** Build for linux with TCP ************")
    build_options = {
        'RELEASE': flag,
        'WITH_TCP': 1,
        'TARGET_TRANSPORT': 'IP',
        'SECURED': 0,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_unsecured_with_java(flag, scons_options):
    print ("*********** Build for linux with Java support ************")
    build_options = {
        'RELEASE': flag,
        'BUILD_JAVA': 1,
        'TARGET_TRANSPORT': 'IP',
        'SECURED': 0,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_secured_with_java(flag, scons_options):
    print ("*********** Build for linux with Java support and secured ************")
    build_options = {
        'RELEASE': flag,
        'BUILD_JAVA': 1,
        'TARGET_TRANSPORT': 'IP',
        'SECURED': 1,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_unsecured_with_rm(flag, scons_options):
    print ("*********** Build for linux with RoutingManager************")
    build_options = {
        'ROUTING': 'GW',
        'RELEASE': flag,
        'SECURED': 0,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_full(flag, scons_options):
    """
    Secured build with as many options selected as possible.
    """
    print ("*********** Build for linux with full features *************")
    build_options = {
        'BUILD_JAVA': 1,
        'MULTIPLE_OWNER': 1,
        'RELEASE': flag,
        'SECURED': 1,
        'WITH_CLOUD': 1,
        'WITH_PROXY': 1,
        'WITH_RA': 1,
        'WITH_RA_IBB': 1,
        'WITH_TCP': 1,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_unsecured_with_rd(flag, scons_options):
    print ("*********** Build for linux With Resource Directory *************")
    build_options = {
        'RELEASE': flag,
        'RD_MODE': 'all',
        'SECURED': 0,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_secured_with_rd(flag, scons_options):
    print ("*********** Build for linux With Resource Directory & Security ************")
    build_options = {
        'RELEASE': flag,
        'RD_MODE': 'all',
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_linux_unsecured_with_mq(flag, scons_options):
    print ("*********** Build for linux With Message Queue ************")
    build_options = {
        'RELEASE': flag,
        'WITH_MQ': 'PUB,SUB,BROKER',
        'SECURED': 0,
        'ERROR_ON_WARN': 1,
    }
    call_scons(build_options, scons_options)


def build_android(flag, scons_options):
    # Note: for android, as oic-resource uses C++11 feature stoi and to_string,
    # it requires gcc-4.9, currently only android-ndk-r10(for linux)
    # and windows android-ndk-r10(64bit target version) support these features.
    print ("*********** Build for android armeabi *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)


def build_android_universal(flag, scons_options):
    print('''
*******************************************************************************
    Starting Android Universal build
    This Will take a very long time to complete. It will build the android
    build for the following architectures.
       - armeabi
       - armeabi-v7a
       - arm64-v8a
       - x86
       - x86_64
    If you do not need to build a universal APK or you can target a specific
    architecture we recommend you build android for that specific target. 
*******************************************************************************
''')
    print ("*********** Build for android armeabi *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)
    print ("*********** Build for android x86 *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)
    print ("*********** Build for android armeabi-v7a *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi-v7a',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)
    print ("*********** Build for android x86_64 *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86_64',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)
    print ("*********** Build for android arm64-v8a *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'arm64-v8a',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)
    print ("*********** Finishing Android universal build *************")


def build_android_secured(flag, scons_options):
    build_android(flag, scons_options + " SECURED=1")


def build_android_unsecured(flag, scons_options):
    build_android(flag, scons_options + " SECURED=0")


def build_android_x86(flag, scons_options):
    """ Build Android x86 Suite """
    build_android_x86_with_ip(flag, scons_options)
    build_android_x86_with_bt(flag, scons_options)
    build_android_x86_with_ble(flag, scons_options)


def build_android_x86_with_ip(flag, scons_options):
    print ("*********** Build for android x86 *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'IP',
    }
    call_scons(build_options, scons_options)


def build_android_x86_with_bt(flag, scons_options):
    print ("*********** Build for android x86 with Bluetooth *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BT',
    }
    call_scons(build_options, scons_options)


def build_android_x86_with_ble(flag, scons_options):
    print ("*********** Build for android x86 with Bluetooth Low Energy *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BLE',
    }
    call_scons(build_options, scons_options)


def build_android_x86_with_rm(flag, scons_options):
    """ Build Android x86 Routing Manager Suite """
    build_android_x86_with_rm_and_ip(flag, scons_options)
    build_android_x86_with_rm_and_bt(flag, scons_options)
    build_android_x86_with_rm_and_ble(flag, scons_options)


def build_android_x86_with_rm_and_ip(flag, scons_options):
    print ("*********** Build for android x86 with Routing Manager *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'ROUTING': 'GW',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'IP',
    }
    call_scons(build_options, scons_options)


def build_android_x86_with_rm_and_bt(flag, scons_options):
    print ("*********** Build for android x86 with Routing Manager and Bluetooth *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'ROUTING': 'GW',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BT',
    }
    call_scons(build_options, scons_options)


def build_android_x86_with_rm_and_ble(flag, scons_options):
    print ("*********** Build for android x86 with Routing Manager and Bluetooth Low Energy *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'x86',
        'ROUTING': 'GW',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BLE',
    }
    call_scons(build_options, scons_options)


def build_android_armeabi(flag, scons_options):
    """ Build Android Armeabi Suite """
    build_android_armeabi_with_ip(flag, scons_options)
    build_android_armeabi_with_bt(flag, scons_options)
    build_android_armeabi_with_ble(flag, scons_options)


def build_android_armeabi_with_ip(flag, scons_options):
    print ("*********** Build for android armeabi *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'IP',
    }
    call_scons(build_options, scons_options)


def build_android_armeabi_with_bt(flag, scons_options):
    print ("*********** Build for android armeabi with Bluetooth *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BT',
    }
    call_scons(build_options, scons_options)


def build_android_armeabi_with_ble(flag, scons_options):
    print ("*********** Build for android armeabi with Bluetooth Low Energy *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BLE',
    }
    call_scons(build_options, scons_options)


def build_android_armeabi_with_rm(flag, scons_options):
    """ Build Android Armeabi Routing Manager Suite """
    build_android_armeabi_with_rm_and_ip(flag, scons_options)
    build_android_armeabi_with_rm_and_bt(flag, scons_options)
    build_android_armeabi_with_rm_and_ble(flag, scons_options)


def build_android_armeabi_with_rm_and_ip(flag, scons_options):
    print ("*********** Build for android armeabi with Routing Manager *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'ROUTING': 'GW',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'IP',
    }
    call_scons(build_options, scons_options)


def build_android_armeabi_with_rm_and_bt(flag, scons_options):
    print ("*********** Build for android armeabi with Routing Manager and Bluetooth *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'ROUTING': 'GW',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BT',
    }
    call_scons(build_options, scons_options)


def build_android_armeabi_with_rm_and_ble(flag, scons_options):
    print ("*********** Build for android armeabi with Routing Manager and Bluetooth Low Energy *************")
    build_options = {
        'TARGET_OS': 'android',
        'TARGET_ARCH': 'armeabi',
        'ROUTING': 'GW',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'BLE',
    }
    call_scons(build_options, scons_options)


def build_tizen(flag, scons_options):
    print ("*********** Build for Tizen with options *************")
    cmd_line = os.getcwd() + "/gbsbuild.sh" + " " + scons_options
    if not EXEC_MODE:
        print ("Would run : " + cmd_line)
    else:
        print ("Running : " + cmd_line)
        exit_code = subprocess.Popen([cmd_line], shell=True).wait()
        if exit_code != 0:
            sys.exit(exit_code)


def build_tizen_extra(flag, scons_options):
    print ("*********** Build for Tizen octbstack lib and sample *************")
    # make sure the following file as been updated for these builds:
    # resource/csdk/stack/samples/tizen/build/gbsbuild.sh
    build_extra_options = "-f resource/csdk/stack/samples/tizen/build/SConscript " + scons_options
    build_options = {
        'TARGET_OS': 'tizen',
        'TARGET_TRANSPORT': 'IP',
        'LOGGING': 'true',
        'RELEASE': flag,
    }
    call_scons(build_options, build_extra_options)

    print ("*********** Build for Tizen octbstack lib and sample with Routing Manager*************")
    build_options['ROUTING'] = 'GW'
    call_scons(build_options, build_extra_options)

    print ("*********** Build for Tizen Easy-Setup sample *************")
    # make sure the following file has been updated for these builds:
    # service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample/build/tizen/gbsbuild.sh
    build_options['ROUTING'] = 'EP'
    build_options['ES_TARGET_ENROLLEE'] = 'tizen'
    build_extra_options = "-f service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample/build/tizen/SConscript " + scons_options
    call_scons(build_options, build_extra_options)

    print ("*********** Build for Tizen Easy-Setup sample with multiple owner *************")
    build_options['MULTIPLE_OWNER'] = 1
    call_scons(build_options, build_extra_options)


def build_tizen_secured(flag, scons_options):
    build_tizen(flag, scons_options + " SECURED=1")


def build_tizen_unsecured(flag, scons_options):
    build_tizen(flag, scons_options + " SECURED=0")


def build_tizen_extra_secured(flag, scons_options):
    build_tizen_extra(flag, scons_options + " SECURED=1")


def build_tizen__extraunsecured(flag, scons_options):
    build_tizen_extra(flag, scons_options + " SECURED=0")


# Mac OS and iOS
def build_darwin(flag, scons_options):
    print ("*********** Build for OSX *************")
    build_options = {
        'TARGET_OS': 'darwin',
        'SYS_VERSION': '10.9',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)

    print ("*********** Build for IOS i386 *************")
    build_options = {
        'TARGET_OS': 'ios',
        'TARGET_ARCH': 'i386',
        'SYS_VERSION': '7.0',
        'RELEASE': flag,
    }
    call_scons(build_options, scons_options)

    print ("*********** Build for IOS x86_64 *************")
    build_options['TARGET_ARCH'] = 'x86_64'
    call_scons(build_options, scons_options)

    print ("*********** Build for IOS armv7 *************")
    build_options['TARGET_ARCH'] = 'armv7'
    call_scons(build_options, scons_options)

    print ("*********** Build for IOS armv7s *************")
    build_options['TARGET_ARCH'] = 'armv7s'
    call_scons(build_options, scons_options)

    print ("*********** Build for IOS arm64 *************")
    build_options['TARGET_ARCH'] = 'arm64'
    call_scons(build_options, scons_options)


# Windows
def build_windows(flag, scons_options):
    print ("*********** Build for Windows *************")
    os.environ["SCONSFLAGS"] = ""
    build_options = {
        'TARGET_OS': 'windows',
        'TARGET_ARCH': 'amd64',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'IP',
        'WITH_TCP': 0,
        'BUILD_SAMPLE': 'ON',
        'LOGGING': 'off',
        'TEST': 1,
        'RD_MODE': 'all',
    }
    call_scons(build_options, scons_options)


# Windows msys
def build_msys(flag, scons_options):
    print ("*********** Build for msys_nt *************")
    os.environ["SCONSFLAGS"] = ""
    build_options = {
        'TARGET_OS': 'msys_nt',
        'TARGET_ARCH': 'x86_64',
        'RELEASE': flag,
        'TARGET_TRANSPORT': 'IP',
        'WITH_TCP': 0,
        'BUILD_SAMPLE': 'ON',
        'LOGGING': 'off',
        'TEST': 1,
        'RD_MODE': 'all',
    }
    call_scons(build_options, scons_options)


def build_simulator(flag, scons_options):
    print ("*********** Build for simulator plugin *************")
    build_options = {
        'SIMULATOR': 1,
        'RELEASE': flag,
        'SECURED': 0,
        'TARGET_TRANSPORT': 'IP',
    }
    call_scons(build_options, scons_options)


def do_unit_tests(secured=True, memcheck=False):
    """ build full stack and run unit tests in selected mode.

    All the unit test builders have to run in test (non-release) mode.
    This option is used by the CI system, which is a virtual machine -
    thus it should be built with only the IP transport to avoid timeouts
    setting up infrastructure that is not present (i.e. BT and NFC).

    Args:
        secured (bool): build in secured mode.
        memcheck (bool): run tests under control of memory checker.
    """

    build_options = {
        'TEST': 1,
        'RELEASE': 'false',
        'TARGET_TRANSPORT': 'IP',
        'SECURED': secured,
        'VALGRIND_CHECKS': memcheck,
    }
    print ("*********** Unit test Start *************")
    call_scons(build_options, "-c")  # clean first
    call_scons(build_options, "")
    print ("*********** Unit test Stop *************")


def unit_tests_secured():
    do_unit_tests(secured=1)


def unit_tests_unsecured():
    do_unit_tests(secured=0)


def unit_tests_memcheck_secured():
    do_unit_tests(secured=1, memcheck=True)


def unit_tests_memcheck_unsecured():
    do_unit_tests(secured=0, memcheck=True)


def unit_tests():
    unit_tests_secured()
    unit_tests_unsecured()


# Main module starts here
if os.getenv("SCONSFLAGS", "") == "":
    os.environ["SCONSFLAGS"] = "-Q -j " + str(multiprocessing.cpu_count())

arg_num = len(sys.argv)
script_name = sys.argv[0]

# May be overridden in user's shell
VERBOSE = os.getenv("VERBOSE", "1")
EXEC_MODE = os.getenv("EXEC_MODE", True)
if EXEC_MODE in ['false', 'False', '0']:
    EXEC_MODE = False

if arg_num == 1:
    build_all("true", "")
    build_all("false", "")
    unit_tests()

elif arg_num == 2:
    if str(sys.argv[1]) == '-c':
        build_all("true", "-c")
        build_all("false", "-c")

    elif str(sys.argv[1]) == "all":
        build_all("true", "")
        build_all("false", "")
        unit_tests()

    elif str(sys.argv[1]) == "linux":
        build_linux("true", "")
        build_linux("false", "")

    elif str(sys.argv[1]) == "linux_unsecured":
        build_linux_unsecured("true", "")
        build_linux_unsecured("false", "")
        build_linux_unsecured_with_rm("true", "")
        build_linux_unsecured_with_rm("false", "")

    elif str(sys.argv[1]) == "linux_secured":
        build_linux_secured("true", "")
        build_linux_secured("false", "")

    elif str(sys.argv[1]) == "linux_full":
        build_linux_full("true", "")
        build_linux_full("false", "")

    elif str(sys.argv[1]) == "linux_unsecured_with_rd":
        build_linux_unsecured_with_rd("true", "")
        build_linux_unsecured_with_rd("false", "")

    elif str(sys.argv[1]) == "linux_secured_with_rd":
        build_linux_secured_with_rd("true", "")
        build_linux_secured_with_rd("false", "")

    elif str(sys.argv[1]) == "linux_unsecured_with_mq":
        build_linux_unsecured_with_mq("true", "")
        build_linux_unsecured_with_mq("false", "")

    elif str(sys.argv[1]) == "linux_unsecured_with_tcp":
        build_linux_unsecured_with_tcp("true", "")
        build_linux_unsecured_with_tcp("false", "")

    elif str(sys.argv[1]) == "linux_secured_with_tcp":
        build_linux_secured_with_tcp("false", "")
        build_linux_secured_with_tcp("true", "")

    elif str(sys.argv[1]) == "linux_unsecured_with_java":
        build_linux_unsecured_with_java("false", "")
        build_linux_unsecured_with_java("true", "")

    elif str(sys.argv[1]) == "linux_secured_with_java":
        build_linux_secured_with_java("false", "")
        build_linux_secured_with_java("true", "")

    elif str(sys.argv[1]) == "android":
        build_android("true", "")
        build_android("false", "")

    elif str(sys.argv[1]) == "android_universal":
        # only build the release version due to long amount of time to build.
        build_android_universal("true", "")

    elif str(sys.argv[1]) == "android_universal_unsecured":
        # only build the release version due to long amount of time to build.
        build_android_universal("true", "SECURED=0")

    elif str(sys.argv[1]) == "android_unsecured":
        build_android_unsecured("true", "")
        build_android_unsecured("false", "")

    elif str(sys.argv[1]) == "android_secured":
        build_android_secured("true", "")
        build_android_secured("false", "")

    elif str(sys.argv[1]) == "android_x86":
        build_android_x86("true", "")
        build_android_x86("false", "")
        build_android_x86_with_rm("true", "")
        build_android_x86_with_rm("false", "")

    elif str(sys.argv[1]) == "android_x86_with_ip":
        build_android_x86_with_ip("true", "")
        build_android_x86_with_ip("false", "")

    elif str(sys.argv[1]) == "android_x86_with_bt":
        build_android_x86_with_bt("true", "")
        build_android_x86_with_bt("false", "")

    elif str(sys.argv[1]) == "android_x86_with_ble":
        build_android_x86_with_ble("true", "")
        build_android_x86_with_ble("false", "")

    elif str(sys.argv[1]) == "android_x86_with_rm_and_ip":
        build_android_x86_with_rm_and_ip("true", "")
        build_android_x86_with_rm_and_ip("false", "")

    elif str(sys.argv[1]) == "android_x86_with_rm_and_bt":
        build_android_x86_with_rm_and_bt("true", "")
        build_android_x86_with_rm_and_bt("false", "")

    elif str(sys.argv[1]) == "android_x86_with_rm_and_ble":
        build_android_x86_with_rm_and_ble("true", "")
        build_android_x86_with_rm_and_ble("false", "")

    elif str(sys.argv[1]) == "android_armeabi":
        build_android_armeabi("true", "")
        build_android_armeabi("false", "")
        build_android_armeabi_with_rm("true", "")
        build_android_armeabi_with_rm("false", "")

    elif str(sys.argv[1]) == "android_armeabi_with_ip":
        build_android_armeabi_with_ip("true", "")
        build_android_armeabi_with_ip("false", "")

    elif str(sys.argv[1]) == "android_armeabi_with_bt":
        build_android_armeabi_with_bt("true", "")
        build_android_armeabi_with_bt("false", "")

    elif str(sys.argv[1]) == "android_armeabi_with_ble":
        build_android_armeabi_with_ble("true", "")
        build_android_armeabi_with_ble("false", "")

    elif str(sys.argv[1]) == "android_armeabi_with_rm_and_ip":
        build_android_armeabi_with_rm_and_ip("true", "")
        build_android_armeabi_with_rm_and_ip("false", "")

    elif str(sys.argv[1]) == "android_armeabi_with_rm_and_bt":
        build_android_armeabi_with_rm_and_bt("true", "")
        build_android_armeabi_with_rm_and_bt("false", "")

    elif str(sys.argv[1]) == "android_armeabi_with_rm_and_ble":
        build_android_armeabi_with_rm_and_ble("true", "")
        build_android_armeabi_with_rm_and_ble("false", "")

    elif str(sys.argv[1]) == "windows":
        build_windows("true", "")
        build_windows("false", "")

    elif str(sys.argv[1]) == "msys":
        build_msys("true", "")
        build_msys("false", "")

    elif str(sys.argv[1]) == "tizen":
        build_tizen("true", "")
        build_tizen("false", "")

    elif str(sys.argv[1]) == "tizen_extra":
        build_tizen_extra("true", "")
        build_tizen_extra("false", "")

    elif str(sys.argv[1]) == "tizen_unsecured":
        build_tizen_unsecured("true", "")
        build_tizen_unsecured("false", "")

    elif str(sys.argv[1]) == "tizen_secured":
        build_tizen_secured("true", "")
        build_tizen_secured("false", "")

    elif str(sys.argv[1]) == "tizen_extra_unsecured":
        build_tizen_extra_unsecured("true", "")
        build_tizen_extra_unsecured("false", "")

    elif str(sys.argv[1]) == "tizen_extra_secured":
        build_tizen_extra_secured("true", "")
        build_tizen_extra_secured("false", "")

    elif str(sys.argv[1]) == "simulator":
        build_simulator("true", "")
        build_simulator("false", "")

    elif str(sys.argv[1]) == "darwin":
        build_darwin("true", "")
        build_darwin("false", "")

    elif str(sys.argv[1]) == "unit_tests_secured":
        unit_tests_secured()

    elif str(sys.argv[1]) == "unit_tests_unsecured":
        unit_tests_unsecured()

    elif str(sys.argv[1]) == "unit_tests_memcheck_secured":
        unit_tests_memcheck_secured()

    elif str(sys.argv[1]) == "unit_tests_memcheck_unsecured":
        unit_tests_memcheck_unsecured()

    elif str(sys.argv[1]) == "unit_tests":
        unit_tests()

    else:
        helpmsg(script_name)
else:
    helpmsg(script_name)

print ("===================== done =====================")
