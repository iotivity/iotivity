#!/bin/sh

iotivity_root=$1
API_PROJECT_DIR=$iotivity_root

cd $API_PROJECT_DIR

time_stamp=$(date '+%Y%m%d_%H%M%S')
gcov_result=gcov_result_${time_stamp}

# Gcov Html generation

archive_name=$gcov_result
result_dir=../../../TestReport/gcov/$archive_name
mkdir -p $result_dir

gcovr -r . -e ".sconf_temp*" -e "examples.OICMiddle" -e "extlibs.*" -e "extlibs.hippomocks-master.*" -e "extlibs.rapidxml.*" -e "out.linux.*" -e "plugins.*" -e "resource.csdk.connectivity.lib.*" -e "resource.csdk.connectivity.samples.linux.*" -e "resource.csdk.security.provisioning.ck_manager.sample.*" -e "resource.csdk.security.provisioning.ck_manager.unittest.*" -e "resource.csdk.connectivity.scr.bt_edr_adapter.*" -e "resource.csdk.connectivity.src.bt_le_adapter.*" -e "resource.csdk.connectivity.src.bt_edr_adapter.*" -e "resource.csdk.connectivity.test.*" -e "resource.csdk.logger.*" -e "resource.src.*" -e "resource.csdk.security.provisioning.include.*" -e "resource.csdk.security.provisioning.sample.*" -e "resource.csdk.security.provisioning.src.cloud.*" -e "resource.csdk.security.provisioning.unittest.*" -e "resource.csdk.security.unittest.*" -e "resource.csdk.stack.*" -e "resource.examples.*" -e "resource.unittests.*" -e "resource.include.*" -e "resource.oc_logger.*" -e "resource.provisioning.examples.*" -e "resource.provisioning.unittests.*" -e "resource.csdk.connectivity.common.src.logger.c.*" -e "resource.csdk.connectivity.common.src.oic_console_logger.c.*" -e "resource.csdk.connectivity.common.src.oic_logger.c.*" -e "service.resource-encapsulation.include.*" -e "service.resource-encapsulation.src.common.expiryTimer.*" -e "service.resource-encapsulation.src.common.primitiveResource.unittests.*" -e "service.resource-encapsulation.src.resourceBroker.unittest.*" -e "service.resource-encapsulation.src.resourceCache.unittest.*" -e "service.resource-encapsulation.src.serverBuilder.unittest.*" -e "service.resource-encapsulation.unittest.*" -e "service.resource-hosting.src.unittest.*" -e "service.resource-hosting.SampleApp.*" -e "service.things-manager.*" -e "service.notification.unittest.*" -e "service.easy-setup.unittest.*" -e "service.easy-setup.sample.*" -e "service.easy-setup.mediator.csdk.unittests.*" -e "service.easy-setup.mediator.richsdk.unittests.*" -e "service.easy-setup.enrollee.unittest.*" -e "service.resource-container.examples.*" -e "service.resource-container.unittests." -e "service.resource-container.bundle-api." -e "service.resource-encapsulation.examples.*" -e "service.resource-encapsulation.src.common.primitiveResource.unittests." -e "service.resource-encapsulation.src.resourceBroker.unittest." -e "service.resource-encapsulation.src.resourceCache.unittest." -e "service.resource-encapsulation.src.serverBuilder.unittest." -e "service.resource-encapsulation.unittest." -e "service.resource-encapsulation.src.common.utils.*" -e "service.things-manager.sampleapp.*" -e "service.resource-hosting.unittest" -e "resource.c_common.oic_string.test.*" -e "service.notification.unittests.*" -e "service.notification.cpp-wrapper.unittest.*" -e "resource.c_common.*" -e "service.resource-directory.samples.*" -e "resource.csdk.security.src.*" -e "resource.csdk.connectivity.src.adapter_util.pkix.*" -e "resource.csdk.connectivity.src.adapter_util.caadapternetdtls.c" -e "service.scene-manager.sampleapp.*" -e "service.scene-manager.unittests.*" --html-details -o $result_dir/result.html

find . -name "*.gcno" -type f -delete
find . -name "*.gcda" -type f -delete

