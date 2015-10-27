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

# Clearing out dir
rm -rf bin

RunCommand () {
	if eval $1; then
		echo $green''$2' SUCCESSFUL'$reset
	else
		echo $red'[ERROR]'$2' FAILED'
		echo ""
		echo " ============================================"
		echo "||                "$bold"BUILD FAILED$reset$red              ||"
		echo " ============================================"
		echo ""$reset
		exit 1
fi
}

# Buiding ConformanceSimulator
cd res/ConformanceSimulator
RunCommand "scons" "Conformance Simulator Build" 
cd ../..

#rm -rf res/ConformanceTestTool/ConformanceTool/workspace
mkdir -p bin/linux/ConformanceTestTool
mkdir -p bin/linux/ConformanceSimulator
RunCommand "cp -r res/ConformanceTestTool/* bin/linux/ConformanceTestTool" "Required Files Installation"
RunCommand "cp -r res/ConformanceSimulator/bin/linux/* bin/linux/ConformanceSimulator" "Conformance Simulator Installation"
RunCommand "cp -r libs/*.jar bin/linux/ConformanceTestTool/libs/" "Dependent Library Installation"

# Buiding JCOAP
cd extlibs/ws4d-jcoap
RunCommand "gradle install" "JCOAP Build" 

# Buiding Conformance Lib
cd ../../
RunCommand "gradle install" "Conformance Lib Build"
RunCommand "cp libs/jcoap.jar bin/linux/ConformanceTestTool/libs" "JCOAP Installation"

# Preparing Conformance Tool
echo ''
echo 'Preparing Conformance Tool. It will take 2/3 minute. Please Wait ...'
echo ''


#RunCommand "cp -r res/* bin/linux" "Conformance Tool Installation"
#RunCommand "cp res/ConformanceTestTool/ConformanceTool/plugins/com.sec.swc.toi.ui_1.0.0/libs/logger.config bin/linux/ConformanceTestTool/ConformanceTool" "Logger Config Installation Installation"
#RunCommand "cp res/ConformanceTestTool/ConformanceTool/plugins/com.sec.swc.toi.ui_1.0.0/libs/DUTDescriptor.json bin/linux/ConformanceTestTool/ConformanceTool" "JSON Descriptor Installation"

echo $green""
echo " ============================================"
echo "||              "$bold"BUILD SUCCESSFUL$reset$green            ||"
echo " ============================================"
echo ""$reset
