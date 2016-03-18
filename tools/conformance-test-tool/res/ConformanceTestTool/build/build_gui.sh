#!/bin/sh
#******************************************************************
#
# Copyright 2015 Samsung Electronics All Rights Reserved.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


# Initializing log text properties
red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
bold=`tput bold`

RunCommand () {
	echo $green''$2' Started'$reset
	if eval $1; then
		echo $green''$2' SUCCESSFUL'$reset
	else
		echo $red'[ERROR]'$2' FAILED'
		echo ""
		echo " ==============================================="
		echo "||                "$bold"GUI BUILD FAILED$reset$red              ||"
		echo " ==============================================="
		echo ""$reset
		exit 1
	fi
}


#RunCommand "wget http://iweb.dl.sourceforge.net/project/robotide/stable/plugins/com.nitorcreations.robotframework.eclipseide_1.4.0.jar" "Acquiring Required Libraries"
#mv com.nitorcreations.robotframework.eclipseide_1.4.0.jar ../libs/com.nitorcreations.robotframework.eclipseide_1.4.0.jar

RunCommand "wget https://repo.maven.apache.org/maven2/org/slf4j/slf4j-api/1.7.12/slf4j-api-1.7.12.jar --no-check-certificate"
mv slf4j-api-1.7.12.jar ../libs/slf4j-api-1.7.12.jar

RunCommand "wget https://repo.maven.apache.org/maven2/org/slf4j/slf4j-log4j12/1.7.12/slf4j-log4j12-1.7.12.jar --no-check-certificate"
mv slf4j-log4j12-1.7.12.jar ../libs/slf4j-log4j12-1.7.12.jar

RunCommand "wget http://central.maven.org/maven2/com/android/tools/ddms/ddmlib/22.0/ddmlib-22.0.jar --no-check-certificate"
mv ddmlib-22.0.jar ../libs/ddmlib.jar

RunCommand "wget http://central.maven.org/maven2/org/xerial/sqlite-jdbc/3.8.6/sqlite-jdbc-3.8.6.jar --no-check-certificate"
mv sqlite-jdbc-3.8.6.jar ../libs/sqlite-jdbc-3.8.6.jar

RunCommand "wget http://central.maven.org/maven2/org/apache/commons/commons-exec/1.3/commons-exec-1.3.jar --no-check-certificate"
mv commons-exec-1.3.jar ../libs/commons-exec-1.3.jar

RunCommand "wget http://central.maven.org/maven2/commons-io/commons-io/2.4/commons-io-2.4.jar --no-check-certificate"
mv commons-io-2.4.jar ../libs/commons-io-2.4.jar

RunCommand "wget http://central.maven.org/maven2/log4j/log4j/1.2.17/log4j-1.2.17.jar --no-check-certificate"
mv log4j-1.2.17.jar ../libs/log4j-1.2.17.jar

RunCommand "wget http://central.maven.org/maven2/org/slf4j/slf4j-api/1.7.10/slf4j-api-1.7.10.jar --no-check-certificate"
mv slf4j-api-1.7.10.jar ../libs/slf4j-api-1.7.10.jar

RunCommand "wget http://central.maven.org/maven2/org/slf4j/slf4j-log4j12/1.7.10/slf4j-log4j12-1.7.10.jar --no-check-certificate"
mv slf4j-log4j12-1.7.10.jar ../libs/slf4j-log4j12-1.7.10.jar

mkdir robotide
cp pom.xml ./robotide/
GIT_SSL_NO_VERIFY=true git clone https://github.com/NitorCreations/RobotFramework-EclipseIDE.git
cp -R ./RobotFramework-EclipseIDE/plugin/* ./robotide/
rm -rf ./robotide/src/test


rm -rf ../bin/ConformanceTestTool
rm -rf ./oic.ctt.product/target/products/*
cd oic.ctt.parent
RunCommand "mvn clean verify -DskipTest=true" "Building Conformance Test Tool GUI"
mkdir -p ../../bin/ConformanceTestTool
cd ..
cp "oic.ctt.product/target/products/OIC Conformance Test Tool-linux.gtk.x86.zip" ./CTT.zip
unzip CTT.zip -d ../bin/ConformanceTestTool
rm -rf CTT.zip
rm -rf oic.ctt.product/target
rm -rf ../target
#rm -f ../libs/*.jar

echo $green""
echo " ================================================"
echo "||              "$bold"GUI BUILD SUCCESSFUL$reset$green            ||"
echo " ================================================"
echo ""$reset
