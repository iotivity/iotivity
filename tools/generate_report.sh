#!/bin/bash

set -e
#Colors
RED="\033[0;31m"
GREEN="\033[0;32m"
BLUE="\033[0;34m"
NO_COLOUR="\033[0m"

#Defaults
time_stamp=$(date -u +%Y-%b-%d)
module_name="CA"
report_format="html"
report_flags="--html --html-details";

#IOTIVITY Flags
IOTIVITY_BASE="${PWD}"
IOTIVITY_TARGET_OS="linux"
IOTIVITY_TARGET_ARCH="$(uname -m)"
USE_TIMESTAMP="yes"
UNITTEST_XML_REPORT="yes"

case $IOTIVITY_TARGET_ARCH in
i*86)
IOTIVITY_TARGET_ARCH=x86
;;
esac


usage() {
    echo "Usage: tools/generate_report.sh <options>"
    echo "Options:"
    echo "      -h / --help                                     :  Display help and exit"
    echo "      -c                                              :  Clean IoTivity Repository; Should be used to clean existing repository"
    echo "      -f [html|gcov|xml] (default: html)              :  Report Format."
    echo "      --format=[html|gcov|xml] (default: html)        :  Report Format."
    echo "      --module=[CA|ES|RE|SM|NS|CHP|ALL] (default: CA) :  Module for which report needs to be generated."
    echo "      --timestamp=[yes|no] (default: yes)             :  Remove Time Stamp from the report output. If directory exits, overwrites the report to the same directory"
    echo "      --ut_report=[yes|no] (default: yes)             :  The unit test report will be generated in xml format (as gtest only supports xml)."
    echo "      --target_arch=[x86|x86_64] (default: x86)       :  Choose Target Architecture for running test executables."
    echo "Example:"
    echo "  $ cd path/to/iotivity "
    echo "  $ scons TARGET_TRANSPORT=IP LOGGING=0 RELEASE=0 SECURED=0 WITH_TCP=0 TARGET_ARCH=x86 WITH_PROXY=1 TEST=0 "
    echo "  $ tools/generate_report.sh --format=html --module=ALL --timestamp=yes --ut_report=yes"
}

clean_iotivity() {
    echo -e "Cleaning ${BLUE}${IOTIVITY_BASE}${NO_COLOUR}"
    echo -e "Deleting  ${RED}${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug${NO_COLOUR}"
    echo -e "Deleting  ${RED}${IOTIVITY_BASE}/.sconf_temp${NO_COLOUR}"
    echo -e "Deleting  ${RED}${IOTIVITY_BASE}/.sconsign.dblite${NO_COLOUR}"
    rm -r "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug" "${IOTIVITY_BASE}/.sconf_temp" "${IOTIVITY_BASE}/.sconsign.dblite"
    find "${IOTIVITY_BASE}" -name "*.memcheck" -delete -o -name "*.gcno" -delete -o -name "*.gcda" -delete -o -name "*.os" -delete -o -name "*.o" -delete
    echo -e "Finished Cleaning ${BLUE}${IOTIVITY_BASE}${NO_COLOUR}"
}

process_cmd_args() {
    while [ "$#" -gt 0  ]; do
        case "$1" in
            -c)
                clean_iotivity
                shift 1; exit 0
                ;;

            -f)
                report_format="$2";
                if [ "gcov" != ${report_format} -a "html" != ${report_format} -a "xml" != ${report_format} ]; then
                    usage; exit 1;
                fi
                case "$report_format" in
                    "html")
                        report_flags="--html --html-details";
                        ;;
                    "gcov")
                        report_flags="";
                        ;;
                    "xml")
                        report_flags="--xml";
                        ;;
                esac
                shift 2
                ;;

            --format=*)
                report_format="${1#*=}";
                if [ "gcov" != ${report_format} -a "html" != ${report_format} -a "xml" != ${report_format} ]; then
                    usage; exit 1;
                fi
                case "$report_format" in
                    "html")
                        report_flags="--html --html-details";
                        ;;
                    "gcov")
                        report_flags="";
                        ;;
                    "xml")
                        report_flags="--xml --xml-pretty";
                        ;;
                esac
                shift 1
                ;;

            --format)
                echo "$1 requires an argument [gcov|html|xml]" >&2;
                usage;
                exit 1
                ;;

            --module=*)
                module_name="${1#*=}";
                if [ "CA" != ${module_name} -a "ES" != ${module_name} -a "RE" != ${module_name} -a "SM" != ${module_name} -a "NS" != ${module_name} -a "CHP" != ${module_name} -a "ALL" != ${module_name} ]; then
                    usage; exit 1;
                fi
                shift 1
                ;;

            --timestamp=*)
                USE_TIMESTAMP="${1#*=}";
                if [ "yes" != ${USE_TIMESTAMP} -a "no" != ${USE_TIMESTAMP} ]; then
                    usage; exit 1;
                fi
                shift 1
                ;;
            --timestamp)
                echo "$1 requires an argument [yes|no]" >&2;
                usage;
                exit 1
                ;;

            --ut_report=*)
                UNITTEST_XML_REPORT="${1#*=}";
                if [ "yes" != ${UNITTEST_XML_REPORT} -a "no" != ${UNITTEST_XML_REPORT} ]; then
                    usage; exit 1;
                fi
                shift 1
                ;;
            --ut_report)
                echo "$1 requires an argument [yes|no]" >&2;
                usage;
                exit 1
                ;;

            --target_arch=*)
                IOTIVITY_TARGET_ARCH="${1#*=}";
                if [ "x86" != ${IOTIVITY_TARGET_ARCH} -a "x86_64" != ${IOTIVITY_TARGET_ARCH} ]; then
                    usage; exit 1;
                fi
                shift 1
                ;;
            --target_arch)
                echo "$1 requires an argument" >&2;
                usage;
                exit 1
                ;;

            -h)
                usage;
                shift 1; exit 0
                ;;
            --help)
                usage;
                shift 1; exit 0
                ;;

            -*)
                echo "unknown option: $1" >&2;
                usage;
                exit 1
                ;;
        esac
    done
}

generate_report_CA()
{
    # Setting Parameters
    if [ "yes" = ${USE_TIMESTAMP} ]; then
        report_dir="${module_name}_${time_stamp}"
    else
        report_dir="${module_name}"
    fi

    report_file="report.${report_format}"

    test_report_dir="TestReport/${report_format}/${report_dir}"
    test_report_file="${test_report_dir}/${report_file}"

    rm -rf "${test_report_dir}"
    mkdir -p "${test_report_dir}"

    LD_LIBRARY_PATH="${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug"

    #Setting Proper Location for UnitTest XML report generation
    unittest_report_dir="UnitTestReport/${report_dir}"
    if [ "yes" = ${UNITTEST_XML_REPORT} ]; then
        rm -rf "${unittest_report_dir}"
        mkdir -p "${unittest_report_dir}"
        UNITTEST_XML_REPORT_FLAG_PREFIX="--gtest_output=xml:${unittest_report_dir}"
    fi

    tests_list=(
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/resource/csdk/connectivity/test/catests"
               );

    for exe in ${tests_list[@]}; do
        filename=$(basename -- "${exe}")
        if [ -n "${UNITTEST_XML_REPORT_FLAG_PREFIX}" ]; then
            UNITTEST_XML_REPORT_FLAG="${UNITTEST_XML_REPORT_FLAG_PREFIX}/${filename}.xml"
        fi
        eval "${exe} ${UNITTEST_XML_REPORT_FLAG}"
    done

    unset tests_list

    sleep 1

    echo -e "Generating ${GREEN}${module_name}${NO_COLOUR} Reports"

    # Printing Unit Test Report Location
    if [  "yes" = ${UNITTEST_XML_REPORT} ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} UnitTest Report Location: ${BLUE}${unittest_report_dir}${NO_COLOUR}"
    fi

    gcovr -r . \
        -e ".sconf_temp*" \
        -e "examples.OICMiddle" \
        -e "extlibs.*" \
        -e "extlibs.hippomocks-master.*" \
        -e "extlibs.rapidxml.*" \
        -e "out.linux.*" \
        -e "plugins.*" \
        -e "resource.csdk.connectivity.lib.*" \
        -e "resource.csdk.connectivity.samples.linux.*" \
        -e "resource.csdk.security.provisioning.ck_manager.sample.*" \
        -e "resource.csdk.security.provisioning.ck_manager.unittest.*" \
        -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_le_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.test.*" \
        -e "resource.csdk.logger.*" \
        -e "resource.src.*" \
        -e "resource.csdk.security.provisioning.include.*" \
        -e "resource.csdk.security.provisioning.sample.*" \
        -e "resource.csdk.security.provisioning.src.cloud.*" \
        -e "resource.csdk.security.provisioning.unittest.*" \
        -e "resource.csdk.security.unittest.*" \
        -e "resource.csdk.stack.*" \
        -e "resource.examples.*" \
        -e "resource.unittests.*" \
        -e "resource.include.*" \
        -e "resource.oc_logger.*" \
        -e "resource.provisioning.examples.*" \
        -e "resource.provisioning.unittests.*" \
        -e "resource.csdk.connectivity.common.src.logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_logger.c.*" \
        -e "service.resource-encapsulation.include.*" \
        -e "service.resource-encapsulation.src.common.expiryTimer.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" \
        -e "service.resource-encapsulation.src.resourceBroker.unittest.*" \
        -e "service.resource-encapsulation.src.resourceCache.unittest.*" \
        -e "service.resource-encapsulation.src.serverBuilder.unittest.*" \
        -e "service.resource-encapsulation.unittest.*" \
        -e "service.resource-hosting.src.unittest.*" \
        -e "service.resource-hosting.SampleApp.*" \
        -e "service.things-manager.*" \
        -e "service.notification.unittest.*" \
        -e "service.easy-setup.unittest.*" \
        -e "service.easy-setup.sample.*" \
        -e "service.easy-setup.mediator.csdk.unittests.*" \
        -e "service.easy-setup.mediator.richsdk.unittests.*" \
        -e "service.easy-setup.enrollee.unittest.*" \
        -e "service.resource-container.examples.*" \
        -e "service.resource-container.unittests." \
        -e "service.resource-container.bundle-api." \
        -e "service.resource-encapsulation.examples.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests." \
        -e "service.resource-encapsulation.src.resourceBroker.unittest." \
        -e "service.resource-encapsulation.src.resourceCache.unittest." \
        -e "service.resource-encapsulation.src.serverBuilder.unittest." \
        -e "service.resource-encapsulation.unittest." \
        -e "service.resource-encapsulation.src.common.utils.*" \
        -e "service.things-manager.sampleapp.*" \
        -e "service.resource-hosting.unittest" \
        -e "resource.c_common.oic_string.test.*" \
        -e "service.notification.unittests.*" \
        -e "service.notification.cpp-wrapper.unittest.*" \
        -e "resource.c_common.*" \
        -e "service.resource-directory.samples.*" \
        -e "resource.csdk.security.src.*" \
        -e "resource.csdk.connectivity.src.adapter_util.pkix.*" \
        -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" \
        -e "service.scene-manager.sampleapp.*" \
        -e "service.scene-manager.unittests.*" \
        -e "service.coap-http-proxy.unittests.*" \
        -e "service.*" \
        -e "resource.csdk.security.*" \
        -e "resource.csdk.resource-directory.*" \
        -e "resource.csdk.routing.*" \
        -e "resource.csdk.include.*" \
        ${report_flags} -o ${test_report_file}

    if [  $? -eq 0 ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} Coverage Report Location: ${BLUE}${test_report_file}${NO_COLOUR}"
        echo -e "${GREEN}${module_name}${NO_COLOUR} Report Generated ${GREEN}Successfully!${NO_COLOUR}"
    else
        echo -e "${RED}${module_name}${NO_COLOUR} Report Generation ${RED}Failed!${NO_COLOUR}"
    fi
}

generate_report_ES()
{
    # Setting Parameters
    if [ "yes" = ${USE_TIMESTAMP} ]; then
        report_dir="${module_name}_${time_stamp}"
    else
        report_dir="${module_name}"
    fi
    report_file="report.${report_format}"

    test_report_dir="TestReport/${report_format}/${report_dir}"
    test_report_file="${test_report_dir}/${report_file}"

    rm -rf "${test_report_dir}"
    mkdir -p "${test_report_dir}"

    LD_LIBRARY_PATH="${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug"

    #Setting Proper Location for UnitTest XML report generation
    unittest_report_dir="UnitTestReport/${report_dir}"
    if [ "yes" = ${UNITTEST_XML_REPORT} ]; then
        rm -rf "${unittest_report_dir}"
        mkdir -p "${unittest_report_dir}"
        UNITTEST_XML_REPORT_FLAG_PREFIX="--gtest_output=xml:${unittest_report_dir}"
    fi

    tests_list=(
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/easy-setup/enrollee/unittests/easysetup_enrollee_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/easy-setup/mediator/richsdk/unittests/easysetup_mediator_test"
               );

    for exe in ${tests_list[@]}; do
        filename=$(basename -- "${exe}")
        if [ -n "${UNITTEST_XML_REPORT_FLAG_PREFIX}" ]; then
            UNITTEST_XML_REPORT_FLAG="${UNITTEST_XML_REPORT_FLAG_PREFIX}/${filename}.xml"
        fi
        eval "${exe} ${UNITTEST_XML_REPORT_FLAG}"
    done

    unset tests_list

    sleep 1

    echo -e "Generating ${GREEN}${module_name}${NO_COLOUR} Reports"

    # Printing Unit Test Report Location
    if [  "yes" = ${UNITTEST_XML_REPORT} ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} UnitTest Report Location: ${BLUE}${unittest_report_dir}${NO_COLOUR}"
    fi

    gcovr -r . \
        -e ".sconf_temp*" \
        -e "examples.OICMiddle" \
        -e "extlibs.*" \
        -e "extlibs.hippomocks-master.*" \
        -e "extlibs.rapidxml.*" \
        -e "out.linux.*" \
        -e "plugins.*" \
        -e "resource.csdk.connectivity.lib.*" \
        -e "resource.csdk.connectivity.samples.linux.*" \
        -e "resource.csdk.security.provisioning.ck_manager.sample.*" \
        -e "resource.csdk.security.provisioning.ck_manager.unittest.*" \
        -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_le_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.test.*" \
        -e "resource.csdk.logger.*" \
        -e "resource.src.*" \
        -e "resource.csdk.security.provisioning.include.*" \
        -e "resource.csdk.security.provisioning.sample.*" \
        -e "resource.csdk.security.provisioning.src.cloud.*" \
        -e "resource.csdk.security.provisioning.unittest.*" \
        -e "resource.csdk.security.unittest.*" \
        -e "resource.csdk.stack.*" \
        -e "resource.examples.*" \
        -e "resource.unittests.*" \
        -e "resource.include.*" \
        -e "resource.oc_logger.*" \
        -e "resource.provisioning.examples.*" \
        -e "resource.provisioning.unittests.*" \
        -e "resource.csdk.connectivity.common.src.logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_logger.c.*" \
        -e "service.resource-encapsulation.include.*" \
        -e "service.resource-encapsulation.src.common.expiryTimer.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" \
        -e "service.resource-encapsulation.src.resourceBroker.unittest.*" \
        -e "service.resource-encapsulation.src.resourceCache.unittest.*" \
        -e "service.resource-encapsulation.src.serverBuilder.unittest.*" \
        -e "service.resource-encapsulation.unittest.*" \
        -e "service.resource-hosting.src.unittest.*" \
        -e "service.resource-hosting.SampleApp.*" \
        -e "service.things-manager.*" \
        -e "service.notification.unittest.*" \
        -e "service.easy-setup.unittest.*" \
        -e "service.easy-setup.sample.*" \
        -e "service.easy-setup.mediator.csdk.unittests.*" \
        -e "service.easy-setup.mediator.richsdk.unittests.*" \
        -e "service.easy-setup.enrollee.unittest.*" \
        -e "service.resource-container.examples.*" \
        -e "service.resource-container.unittests." \
        -e "service.resource-container.bundle-api." \
        -e "service.resource-encapsulation.examples.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests." \
        -e "service.resource-encapsulation.src.resourceBroker.unittest." \
        -e "service.resource-encapsulation.src.resourceCache.unittest." \
        -e "service.resource-encapsulation.src.serverBuilder.unittest." \
        -e "service.resource-encapsulation.unittest." \
        -e "service.resource-encapsulation.src.common.utils.*" \
        -e "service.things-manager.sampleapp.*" \
        -e "service.resource-hosting.unittest" \
        -e "resource.c_common.oic_string.test.*" \
        -e "service.notification.unittests.*" \
        -e "service.notification.cpp-wrapper.unittest.*" \
        -e "resource.c_common.*" \
        -e "service.resource-directory.samples.*" \
        -e "resource.csdk.security.src.*" \
        -e "resource.csdk.connectivity.src.adapter_util.pkix.*" \
        -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" \
        -e "service.scene-manager.sampleapp.*" \
        -e "service.scene-manager.unittests.*" \
        -e "service.coap-http-proxy.unittests.*" \
        -e "resource.*" \
        -e "service.notification.*" \
        -e "service.resource-encapsulation.*" \
        -e "service.scene-manager.*" \
        -e "service.coap-http-proxy.*" \
        ${report_flags} -o ${test_report_file}

    if [  $? -eq 0 ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} Coverage Report Location: ${BLUE}${test_report_file}${NO_COLOUR}"
        echo -e "${GREEN}${module_name}${NO_COLOUR} Report Generated ${GREEN}Successfully!${NO_COLOUR}"
    else
        echo -e "${RED}${module_name}${NO_COLOUR} Report Generation ${RED}Failed!${NO_COLOUR}"
    fi
}

generate_report_RE()
{
    # Setting Parameters
    if [ "yes" = ${USE_TIMESTAMP} ]; then
        report_dir="${module_name}_${time_stamp}"
    else
        report_dir="${module_name}"
    fi
    report_file="report.${report_format}"
    test_report_dir="TestReport/${report_format}/${report_dir}"
    test_report_file="${test_report_dir}/${report_file}"

    rm -rf "${test_report_dir}"
    mkdir -p "${test_report_dir}"

    LD_LIBRARY_PATH="${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug"

    #Setting Proper Location for UnitTest XML report generation
    unittest_report_dir="UnitTestReport/${report_dir}"
    if [ "yes" = ${UNITTEST_XML_REPORT} ]; then
        rm -rf "${unittest_report_dir}"
        mkdir -p "${unittest_report_dir}"
        UNITTEST_XML_REPORT_FLAG_PREFIX="--gtest_output=xml:${unittest_report_dir}"
    fi

    tests_list=(
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/resource-encapsulation/src/serverBuilder/rcs_server_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/resource-encapsulation/unittests/rcs_client_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/resource-encapsulation/src/common/primitiveResource/unittests/rcs_common_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/resource-encapsulation/src/resourceCache/unittests/cache_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/resource-encapsulation/src/resourceBroker/unittest/broker_test"
               );

    for exe in ${tests_list[@]}; do
        filename=$(basename -- "${exe}")
        if [ -n "${UNITTEST_XML_REPORT_FLAG_PREFIX}" ]; then
            UNITTEST_XML_REPORT_FLAG="${UNITTEST_XML_REPORT_FLAG_PREFIX}/${filename}.xml"
        fi
        eval "${exe} ${UNITTEST_XML_REPORT_FLAG}"
    done

    unset tests_list

    sleep 1

    echo -e "Generating ${GREEN}${module_name}${NO_COLOUR} Reports"

    # Printing Unit Test Report Location
    if [  "yes" = ${UNITTEST_XML_REPORT} ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} UnitTest Report Location: ${BLUE}${unittest_report_dir}${NO_COLOUR}"
    fi

    gcovr -r . \
        -e ".sconf_temp*" \
        -e "examples.OICMiddle" \
        -e "extlibs.*" \
        -e "extlibs.hippomocks-master.*" \
        -e "extlibs.rapidxml.*" \
        -e "out.linux.*" \
        -e "plugins.*" \
        -e "resource.csdk.connectivity.lib.*" \
        -e "resource.csdk.connectivity.samples.linux.*" \
        -e "resource.csdk.security.provisioning.ck_manager.sample.*" \
        -e "resource.csdk.security.provisioning.ck_manager.unittest.*" \
        -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_le_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.test.*" \
        -e "resource.csdk.logger.*" \
        -e "resource.src.*" \
        -e "resource.csdk.security.provisioning.include.*" \
        -e "resource.csdk.security.provisioning.sample.*" \
        -e "resource.csdk.security.provisioning.src.cloud.*" \
        -e "resource.csdk.security.provisioning.unittest.*" \
        -e "resource.csdk.security.unittest.*" \
        -e "resource.csdk.stack.*" \
        -e "resource.examples.*" \
        -e "resource.unittests.*" \
        -e "resource.include.*" \
        -e "resource.oc_logger.*" \
        -e "resource.provisioning.examples.*" \
        -e "resource.provisioning.unittests.*" \
        -e "resource.csdk.connectivity.common.src.logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_logger.c.*" \
        -e "service.resource-encapsulation.include.*" \
        -e "service.resource-encapsulation.src.common.expiryTimer.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" \
        -e "service.resource-encapsulation.src.resourceBroker.unittest.*" \
        -e "service.resource-encapsulation.src.resourceCache.unittest.*" \
        -e "service.resource-encapsulation.src.serverBuilder.unittest.*" \
        -e "service.resource-encapsulation.unittest.*" \
        -e "service.resource-hosting.src.unittest.*" \
        -e "service.resource-hosting.SampleApp.*" \
        -e "service.things-manager.*" \
        -e "service.notification.unittest.*" \
        -e "service.easy-setup.unittest.*" \
        -e "service.easy-setup.sample.*" \
        -e "service.easy-setup.mediator.csdk.unittests.*" \
        -e "service.easy-setup.mediator.richsdk.unittests.*" \
        -e "service.easy-setup.enrollee.unittest.*" \
        -e "service.resource-container.examples.*" \
        -e "service.resource-container.unittests." \
        -e "service.resource-container.bundle-api." \
        -e "service.resource-encapsulation.examples.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests." \
        -e "service.resource-encapsulation.src.resourceBroker.unittest." \
        -e "service.resource-encapsulation.src.resourceCache.unittest." \
        -e "service.resource-encapsulation.src.serverBuilder.unittest." \
        -e "service.resource-encapsulation.unittest." \
        -e "service.resource-encapsulation.src.common.utils.*" \
        -e "service.things-manager.sampleapp.*" \
        -e "service.resource-hosting.unittest" \
        -e "resource.c_common.oic_string.test.*" \
        -e "service.notification.unittests.*" \
        -e "service.notification.cpp-wrapper.unittest.*" \
        -e "resource.c_common.*" \
        -e "service.resource-directory.samples.*" \
        -e "resource.csdk.security.src.*" \
        -e "resource.csdk.connectivity.src.adapter_util.pkix.*" \
        -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" \
        -e "service.scene-manager.sampleapp.*" \
        -e "service.scene-manager.unittests.*" \
        -e "service.coap-http-proxy.unittests.*" \
        -e "resource.*" \
        -e "service.easy-setup.*" \
        -e "service.notification.*" \
        -e "service.scene-manager.*" \
        -e "service.coap-http-proxy.*" \
        ${report_flags} -o ${test_report_file}

    if [  $? -eq 0 ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} Coverage Report Location: ${BLUE}${test_report_file}${NO_COLOUR}"
        echo -e "${GREEN}${module_name}${NO_COLOUR} Report Generated ${GREEN}Successfully!${NO_COLOUR}"
    else
        echo -e "${RED}${module_name}${NO_COLOUR} Report Generation ${RED}Failed!${NO_COLOUR}"
    fi
}

generate_report_SM()
{
    # Setting Parameters
    if [ "yes" = ${USE_TIMESTAMP} ]; then
        report_dir="${module_name}_${time_stamp}"
    else
        report_dir="${module_name}"
    fi
    report_file="report.${report_format}"
    test_report_dir="TestReport/${report_format}/${report_dir}"
    test_report_file="${test_report_dir}/${report_file}"

    rm -rf "${test_report_dir}"
    mkdir -p "${test_report_dir}"

    LD_LIBRARY_PATH="${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug"

    #Setting Proper Location for UnitTest XML report generation
    unittest_report_dir="UnitTestReport/${report_dir}"
    if [ "yes" = ${UNITTEST_XML_REPORT} ]; then
        rm -rf "${unittest_report_dir}"
        mkdir -p "${unittest_report_dir}"
        UNITTEST_XML_REPORT_FLAG_PREFIX="--gtest_output=xml:${unittest_report_dir}"
    fi

    tests_list=(
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/remote_scene_action_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/remote_scene_col_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/remote_scene_list_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/remote_scene_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/scene_action_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/scene_collection_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/scene_list_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/scene-manager/unittests/scene_test"
               );

    for exe in ${tests_list[@]}; do
        filename=$(basename -- "${exe}")
        if [ -n "${UNITTEST_XML_REPORT_FLAG_PREFIX}" ]; then
            UNITTEST_XML_REPORT_FLAG="${UNITTEST_XML_REPORT_FLAG_PREFIX}/${filename}.xml"
        fi
        eval "${exe} ${UNITTEST_XML_REPORT_FLAG}"
    done

    unset tests_list

    sleep 1

    echo -e "Generating ${GREEN}${module_name}${NO_COLOUR} Reports"

    # Printing Unit Test Report Location
    if [  "yes" = ${UNITTEST_XML_REPORT} ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} UnitTest Report Location: ${BLUE}${unittest_report_dir}${NO_COLOUR}"
    fi

    gcovr -r . \
        -e ".sconf_temp*" \
        -e "examples.OICMiddle" \
        -e "extlibs.*" \
        -e "extlibs.hippomocks-master.*" \
        -e "extlibs.rapidxml.*" \
        -e "out.linux.*" \
        -e "plugins.*" \
        -e "resource.csdk.connectivity.lib.*" \
        -e "resource.csdk.connectivity.samples.linux.*" \
        -e "resource.csdk.security.provisioning.ck_manager.sample.*" \
        -e "resource.csdk.security.provisioning.ck_manager.unittest.*" \
        -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_le_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.test.*" \
        -e "resource.csdk.logger.*" \
        -e "resource.src.*" \
        -e "resource.csdk.security.provisioning.include.*" \
        -e "resource.csdk.security.provisioning.sample.*" \
        -e "resource.csdk.security.provisioning.src.cloud.*" \
        -e "resource.csdk.security.provisioning.unittest.*" \
        -e "resource.csdk.security.unittest.*" \
        -e "resource.csdk.stack.*" \
        -e "resource.examples.*" \
        -e "resource.unittests.*" \
        -e "resource.include.*" \
        -e "resource.oc_logger.*" \
        -e "resource.provisioning.examples.*" \
        -e "resource.provisioning.unittests.*" \
        -e "resource.csdk.connectivity.common.src.logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_logger.c.*" \
        -e "service.resource-encapsulation.include.*" \
        -e "service.resource-encapsulation.src.common.expiryTimer.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" \
        -e "service.resource-encapsulation.src.resourceBroker.unittest.*" \
        -e "service.resource-encapsulation.src.resourceCache.unittest.*" \
        -e "service.resource-encapsulation.src.serverBuilder.unittest.*" \
        -e "service.resource-encapsulation.unittest.*" \
        -e "service.resource-hosting.src.unittest.*" \
        -e "service.resource-hosting.SampleApp.*" \
        -e "service.things-manager.*" \
        -e "service.notification.unittest.*" \
        -e "service.easy-setup.unittest.*" \
        -e "service.easy-setup.sample.*" \
        -e "service.easy-setup.mediator.csdk.unittests.*" \
        -e "service.easy-setup.mediator.richsdk.unittests.*" \
        -e "service.easy-setup.enrollee.unittest.*" \
        -e "service.resource-container.examples.*" \
        -e "service.resource-container.unittests." \
        -e "service.resource-container.bundle-api." \
        -e "service.resource-encapsulation.examples.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests." \
        -e "service.resource-encapsulation.src.resourceBroker.unittest." \
        -e "service.resource-encapsulation.src.resourceCache.unittest." \
        -e "service.resource-encapsulation.src.serverBuilder.unittest." \
        -e "service.resource-encapsulation.unittest." \
        -e "service.resource-encapsulation.src.common.utils.*" \
        -e "service.things-manager.sampleapp.*" \
        -e "service.resource-hosting.unittest" \
        -e "resource.c_common.oic_string.test.*" \
        -e "service.notification.unittests.*" \
        -e "service.notification.cpp-wrapper.unittest.*" \
        -e "resource.c_common.*" \
        -e "service.resource-directory.samples.*" \
        -e "resource.csdk.security.src.*" \
        -e "resource.csdk.connectivity.src.adapter_util.pkix.*" \
        -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" \
        -e "service.scene-manager.sampleapp.*" \
        -e "service.scene-manager.unittests.*" \
        -e "service.coap-http-proxy.unittests.*" \
        -e "resource.*" \
        -e "service.easy-setup.*" \
        -e "service.notification.*" \
        -e "service.resource-encapsulation.*" \
        -e "service.coap-http-proxy.*" \
        ${report_flags} -o ${test_report_file}

    if [  $? -eq 0 ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} Coverage Report Location: ${BLUE}${test_report_file}${NO_COLOUR}"
        echo -e "${GREEN}${module_name}${NO_COLOUR} Report Generated ${GREEN}Successfully!${NO_COLOUR}"
    else
        echo -e "${RED}${module_name}${NO_COLOUR} Report Generation ${RED}Failed!${NO_COLOUR}"
    fi
}

generate_report_NS()
{
    # Setting Parameters
    if [ "yes" = ${USE_TIMESTAMP} ]; then
        report_dir="${module_name}_${time_stamp}"
    else
        report_dir="${module_name}"
    fi
    report_file="report.${report_format}"
    test_report_dir="TestReport/${report_format}/${report_dir}"
    test_report_file="${test_report_dir}/${report_file}"

    rm -rf "${test_report_dir}"
    mkdir -p "${test_report_dir}"

    LD_LIBRARY_PATH="${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug"

    #Setting Proper Location for UnitTest XML report generation
    unittest_report_dir="UnitTestReport/${report_dir}"
    if [ "yes" = ${UNITTEST_XML_REPORT} ]; then
        rm -rf "${unittest_report_dir}"
        mkdir -p "${unittest_report_dir}"
        UNITTEST_XML_REPORT_FLAG_PREFIX="--gtest_output=xml:${unittest_report_dir}"
    fi

    tests_list=(
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/notification/unittest/notification_consumer_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/notification/unittest/notification_provider_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/notification/cpp-wrapper/unittest/notification_consumer_wrapper_test"
                "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/notification/cpp-wrapper/unittest/notification_provider_wrapper_test"
               );

    for exe in ${tests_list[@]}; do
        filename=$(basename -- "${exe}")
        if [ -n "${UNITTEST_XML_REPORT_FLAG_PREFIX}" ]; then
            UNITTEST_XML_REPORT_FLAG="${UNITTEST_XML_REPORT_FLAG_PREFIX}/${filename}.xml"
        fi
        eval "${exe} ${UNITTEST_XML_REPORT_FLAG}"
    done

    unset tests_list

    sleep 1

    echo -e "Generating ${GREEN}${module_name}${NO_COLOUR} Reports"

    # Printing Unit Test Report Location
    if [  "yes" = ${UNITTEST_XML_REPORT} ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} UnitTest Report Location: ${BLUE}${unittest_report_dir}${NO_COLOUR}"
    fi

    gcovr -r . \
        -e ".sconf_temp*" \
        -e "examples.OICMiddle" \
        -e "extlibs.*" \
        -e "extlibs.hippomocks-master.*" \
        -e "extlibs.rapidxml.*" \
        -e "out.linux.*" \
        -e "plugins.*" \
        -e "resource.csdk.connectivity.lib.*" \
        -e "resource.csdk.connectivity.samples.linux.*" \
        -e "resource.csdk.security.provisioning.ck_manager.sample.*" \
        -e "resource.csdk.security.provisioning.ck_manager.unittest.*" \
        -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_le_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.test.*" \
        -e "resource.csdk.logger.*" \
        -e "resource.src.*" \
        -e "resource.csdk.security.provisioning.include.*" \
        -e "resource.csdk.security.provisioning.sample.*" \
        -e "resource.csdk.security.provisioning.src.cloud.*" \
        -e "resource.csdk.security.provisioning.unittest.*" \
        -e "resource.csdk.security.unittest.*" \
        -e "resource.csdk.stack.*" \
        -e "resource.examples.*" \
        -e "resource.unittests.*" \
        -e "resource.include.*" \
        -e "resource.oc_logger.*" \
        -e "resource.provisioning.examples.*" \
        -e "resource.provisioning.unittests.*" \
        -e "resource.csdk.connectivity.common.src.logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_logger.c.*" \
        -e "service.resource-encapsulation.include.*" \
        -e "service.resource-encapsulation.src.common.expiryTimer.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" \
        -e "service.resource-encapsulation.src.resourceBroker.unittest.*" \
        -e "service.resource-encapsulation.src.resourceCache.unittest.*" \
        -e "service.resource-encapsulation.src.serverBuilder.unittest.*" \
        -e "service.resource-encapsulation.unittest.*" \
        -e "service.resource-hosting.src.unittest.*" \
        -e "service.resource-hosting.SampleApp.*" \
        -e "service.things-manager.*" \
        -e "service.notification.unittest.*" \
        -e "service.easy-setup.unittest.*" \
        -e "service.easy-setup.sample.*" \
        -e "service.easy-setup.mediator.csdk.unittests.*" \
        -e "service.easy-setup.mediator.richsdk.unittests.*" \
        -e "service.easy-setup.enrollee.unittest.*" \
        -e "service.resource-container.examples.*" \
        -e "service.resource-container.unittests." \
        -e "service.resource-container.bundle-api." \
        -e "service.resource-encapsulation.examples.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests." \
        -e "service.resource-encapsulation.src.resourceBroker.unittest." \
        -e "service.resource-encapsulation.src.resourceCache.unittest." \
        -e "service.resource-encapsulation.src.serverBuilder.unittest." \
        -e "service.resource-encapsulation.unittest." \
        -e "service.resource-encapsulation.src.common.utils.*" \
        -e "service.things-manager.sampleapp.*" \
        -e "service.resource-hosting.unittest" \
        -e "resource.c_common.oic_string.test.*" \
        -e "service.notification.unittests.*" \
        -e "service.notification.cpp-wrapper.unittest.*" \
        -e "resource.c_common.*" \
        -e "service.resource-directory.samples.*" \
        -e "resource.csdk.security.src.*" \
        -e "resource.csdk.connectivity.src.adapter_util.pkix.*" \
        -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" \
        -e "service.scene-manager.sampleapp.*" \
        -e "service.scene-manager.unittests.*" \
        -e "service.coap-http-proxy.unittests.*" \
        -e "resource.*" \
        -e "service.easy-setup.*" \
        -e "service.resource-encapsulation.*" \
        -e "service.scene-manager.*" \
        -e "service.coap-http-proxy.*" \
        -e "service.notification.cpp-wrapper.provider.inc.*" \
        -e "service.notification.cpp-wrapper.consumer.inc.*" \
        ${report_flags} -o ${test_report_file}

    if [  $? -eq 0 ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} Coverage Report Location: ${BLUE}${test_report_file}${NO_COLOUR}"
        echo -e "${GREEN}${module_name}${NO_COLOUR} Report Generated ${GREEN}Successfully!${NO_COLOUR}"
    else
        echo -e "${RED}${module_name}${NO_COLOUR} Report Generation ${RED}Failed!${NO_COLOUR}"
    fi
}

generate_report_CHP()
{
    # Setting Parameters
    if [ "yes" = ${USE_TIMESTAMP} ]; then
        report_dir="${module_name}_${time_stamp}"
    else
        report_dir="${module_name}"
    fi

    report_file="report.${report_format}"
    test_report_dir="TestReport/${report_format}/${report_dir}"
    test_report_file="${test_report_dir}/${report_file}"

    rm -rf "${test_report_dir}"
    mkdir -p "${test_report_dir}"

    LD_LIBRARY_PATH="${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug"

    #Setting Proper Location for UnitTest XML report generation
    unittest_report_dir="UnitTestReport/${report_dir}"
    if [ "yes" = ${UNITTEST_XML_REPORT} ]; then
        rm -rf "${unittest_report_dir}"
        mkdir -p "${unittest_report_dir}"
        UNITTEST_XML_REPORT_FLAG_PREFIX="--gtest_output=xml:${unittest_report_dir}"
    fi

    tests_list=(
               "${IOTIVITY_BASE}/out/${IOTIVITY_TARGET_OS}/${IOTIVITY_TARGET_ARCH}/debug/service/coap-http-proxy/unittests/CoAP_unit_test"
               );

    for exe in ${tests_list[@]}; do
        filename=$(basename -- "${exe}")
        if [ -n "${UNITTEST_XML_REPORT_FLAG_PREFIX}" ]; then
            UNITTEST_XML_REPORT_FLAG="${UNITTEST_XML_REPORT_FLAG_PREFIX}/${filename}.xml"
        fi
        eval "${exe} ${UNITTEST_XML_REPORT_FLAG}"
    done

    unset tests_list

    sleep 1

    echo -e "Generating ${GREEN}${module_name}${NO_COLOUR} Reports"

    # Printing Unit Test Report Location
    if [  "yes" = ${UNITTEST_XML_REPORT} ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} UnitTest Report Location: ${BLUE}${unittest_report_dir}${NO_COLOUR}"
    fi

    gcovr -r . \
        -e ".sconf_temp*" \
        -e "examples.OICMiddle" \
        -e "extlibs.*" \
        -e "extlibs.hippomocks-master.*" \
        -e "extlibs.rapidxml.*" \
        -e "out.linux.*" \
        -e "plugins.*" \
        -e "resource.csdk.connectivity.lib.*" \
        -e "resource.csdk.connectivity.samples.linux.*" \
        -e "resource.csdk.security.provisioning.ck_manager.sample.*" \
        -e "resource.csdk.security.provisioning.ck_manager.unittest.*" \
        -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_le_adapter.*" \
        -e "resource.csdk.connectivity.src.bt_edr_adapter.*" \
        -e "resource.csdk.connectivity.test.*" \
        -e "resource.csdk.logger.*" \
        -e "resource.src.*" \
        -e "resource.csdk.security.provisioning.include.*" \
        -e "resource.csdk.security.provisioning.sample.*" \
        -e "resource.csdk.security.provisioning.src.cloud.*" \
        -e "resource.csdk.security.provisioning.unittest.*" \
        -e "resource.csdk.security.unittest.*" \
        -e "resource.csdk.stack.*" \
        -e "resource.examples.*" \
        -e "resource.unittests.*" \
        -e "resource.include.*" \
        -e "resource.oc_logger.*" \
        -e "resource.provisioning.examples.*" \
        -e "resource.provisioning.unittests.*" \
        -e "resource.csdk.connectivity.common.src.logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" \
        -e "resource.csdk.connectivity.common.src.oic_logger.c.*" \
        -e "service.resource-encapsulation.include.*" \
        -e "service.resource-encapsulation.src.common.expiryTimer.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" \
        -e "service.resource-encapsulation.src.resourceBroker.unittest.*" \
        -e "service.resource-encapsulation.src.resourceCache.unittest.*" \
        -e "service.resource-encapsulation.src.serverBuilder.unittest.*" \
        -e "service.resource-encapsulation.unittest.*" \
        -e "service.resource-hosting.src.unittest.*" \
        -e "service.resource-hosting.SampleApp.*" \
        -e "service.things-manager.*" \
        -e "service.notification.unittest.*" \
        -e "service.easy-setup.unittest.*" \
        -e "service.easy-setup.sample.*" \
        -e "service.easy-setup.mediator.csdk.unittests.*" \
        -e "service.easy-setup.mediator.richsdk.unittests.*" \
        -e "service.easy-setup.enrollee.unittest.*" \
        -e "service.resource-container.examples.*" \
        -e "service.resource-container.unittests." \
        -e "service.resource-container.bundle-api." \
        -e "service.resource-encapsulation.examples.*" \
        -e "service.resource-encapsulation.src.common.primitiveResource.unittests." \
        -e "service.resource-encapsulation.src.resourceBroker.unittest." \
        -e "service.resource-encapsulation.src.resourceCache.unittest." \
        -e "service.resource-encapsulation.src.serverBuilder.unittest." \
        -e "service.resource-encapsulation.unittest." \
        -e "service.resource-encapsulation.src.common.utils.*" \
        -e "service.things-manager.sampleapp.*" \
        -e "service.resource-hosting.unittest" \
        -e "resource.c_common.oic_string.test.*" \
        -e "service.notification.unittests.*" \
        -e "service.notification.cpp-wrapper.unittest.*" \
        -e "resource.c_common.*" \
        -e "service.resource-directory.samples.*" \
        -e "resource.csdk.security.src.*" \
        -e "resource.csdk.connectivity.src.adapter_util.pkix.*" \
        -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" \
        -e "service.scene-manager.sampleapp.*" \
        -e "service.scene-manager.unittests.*" \
        -e "service.coap-http-proxy.unittests.*" \
        -e "resource.*" \
        -e "service.easy-setup.*" \
        -e "service.notification.*" \
        -e "service.resource-encapsulation.*" \
        -e "service.scene-manager.*" \
        ${report_flags} -o ${test_report_file}

    if [  $? -eq 0 ]; then
        echo -e "${GREEN}${module_name}${NO_COLOUR} Coverage Report Location: ${BLUE}${test_report_file}${NO_COLOUR}"
        echo -e "${GREEN}${module_name}${NO_COLOUR} Report Generated ${GREEN}Successfully!${NO_COLOUR}"
    else
        echo -e "${RED}${module_name}${NO_COLOUR} Report Generation ${RED}Failed!${NO_COLOUR}"
    fi
}

generate_report()
{
    case ${module_name} in
        CA)
            generate_report_CA
            ;;
        ES)
            generate_report_ES
            ;;
        RE)
            generate_report_RE
            ;;
        SM)
            generate_report_SM
            ;;
        NS)
            generate_report_NS
            ;;
        CHP)
            generate_report_CHP
            ;;
        ALL)
            module_name="ES"
            generate_report_ES
            module_name="RE"
            generate_report_RE
            module_name="SM"
            generate_report_SM
            module_name="NS"
            generate_report_NS
            module_name="CHP"
            generate_report_CHP
            module_name="CA"
            generate_report_CA
            ;;
    esac
}

process_cmd_args "$@"
generate_report

