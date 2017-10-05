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

1) If you are building first time, then delete GBS-ROOT-OIC folder from home directory.
   Note: Default build server URL for Tizen is set in gbs configuration file @ iotivity/tools/tizen/.gbs.conf.
         If needed, same can be modified as per appropriate target.

2) Go to "iotivity" folder.

3) Execute following command(s) to start build based on some build flags required :

   # EXEC_MODE=true ./prep.sh
   # scons -f service/easy-setup/sampleapp/enrollee/tizen-sdb/EnrolleeSample/build/tizen/SConscript TARGET_OS=tizen TARGET_TRANSPORT=IP ROUTING=EP RELEASE=0 LOGGING=true ES_TARGET_ENROLLEE=tizen SECURED=0 WITH_TCP=false WITH_CLOUD=false

   NOTE: If needed, modify the build arguments value accordingly (Ex: To enable security, set SECURED=1).
         If there are any permission issues occured while running "scons" command then run with "sudo" permissions.

4) On successful build RPMs will be generated at path "~/GBS-ROOT-OIC/local/repos/<target>/<arch>/RPMS"


Procedure to Execute Sample App:
================================
    1) Copy generated rpms to tizen device (For example: /opt/usr/media/enrollee/)
       a) iotivity-1.3.1-0.armv7l.rpm
       b) iotivity-devel-1.3.1-0.armv7l.rpm
       c) iotivity-service-1.3.1-0.armv7l.rpm
       d) com-oic-es-sample-1.3.1-0.armv7l.rpm

    2) sdb shell

    3) su

    4) cd /opt/usr/media/enrollee/

    5) change-booting-mode.sh --update

    6) Install the rpms using the following commands:
        a) rpm -Uvh iotivity-1.3.1-0.armv7l.rpm --force --nodeps
        b) rpm -Uvh iotivity-devel-1.3.1-0.armv7l.rpm --force --nodeps
        c) rpm -Uvh iotivity-service-1.3.1-0.armv7l.rpm --force --nodeps
        d) rpm -Uvh com-oic-es-sample-1.3.1-0.armv7l.rpm --force --nodeps

    7) For IP transport, apply net-config smack rules using below command:
        a) chsmack -a "net-config" /usr/apps/com.oic.es.sample/bin/*
        b) chsmack -e "net-config" /usr/apps/com.oic.es.sample/bin/*

    8) Enrollee sample will be installed in "/usr/apps/"
        a) cd /usr/apps/com.oic.es.sample/bin
        b) If executing with secure mode enabled then copy the .dat file
           cp /usr/lib/oic_svr_db_server.dat .

    9) Execute console application
       ./enrollee_wifi

