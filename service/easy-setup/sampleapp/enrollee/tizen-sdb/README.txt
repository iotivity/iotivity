/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

Build Procedure
===============

1) If you are building first time, then delete GBS-ROOT folder from home directory.
   Note: Default build server URL for Tizen is set in gbs configuration file @ iotivity/tools/tizen/.gbs.conf.
         If needed, same can be modified as per appropriate target.
2) Make the changes to spec file.
	File name: iotivity/tools/tizen/iotivity.spec
	Attributes to change[line no. 28~32] :
		attribute		value
		es_target_enrollee 	tizen
		es_role 		enrollee
		softap_mode 		ENROLLEE_SOFTAP

3) Go to "iotivity/" folder.
4) Execute following command(s) to start build based on transport selection required :

scons -f service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample/build/tizen/SConscript TARGET_OS=tizen TARGET_TRANSPORT=IP ROUTING=EP RELEASE=0 LOGGING=true
			ES_ROLE=enrollee ES_TARGET_ENROLLEE=android ES_SOFTAP_MODE=ENROLLEE_SOFTAP

(If needed, modify the parameters accordingly)
Note :- Upon successful execution of above command(s) RI library and sample console application(for tizen enrollee)
        will be generated at path "~/GBS-ROOT/local/repos/<target>/armv7l/RPMS".

Procedure to Execute Sample App:
================================
    1) Copy the generated rpms(iotivity-1.0.0-0.armv7l.rpm,iotivity-devel-1.0.0-0.armv7l.rpm,iotivity-service-1.0.0-0.armv7l.rpm,com-oic-es-sample-0.1-1.armv7l.rpm) to Tizen Device
       (say /opt/usr/media/xxx/)
    2) sdb shell
    3) su
    4) cd /opt/usr/media/xxx/
    5) change-booting-mode.sh --update

    6) Install the rpms using the following commands:
        a) rpm -Uvh iotivity-1.0.0-0.armv7l.rpm --force --nodeps
        a) rpm -Uvh iotivity-devel-1.0.0-0.armv7l.rpm --force --nodeps
        a) rpm -Uvh iotivity-service-1.0.0-0.armv7l.rpm --force --nodeps
        b) rpm -Uvh com-oic-es-sample-0.1-1.armv7l.rpm --force --nodeps
    7) For IP transport, apply net-config smack rules using below command:
        a) chsmack -a "net-config" /usr/apps/com.oic.es.sample/bin/*
        b) chsmack -e "net-config" /usr/apps/com.oic.es.sample/bin/*
    7) RPM will be installed in "/usr/apps/"
       Execute the sample app by below commands
        i) cd /usr/apps/com.oic.es.sample/bin
            a) ./enrollee_wifi
