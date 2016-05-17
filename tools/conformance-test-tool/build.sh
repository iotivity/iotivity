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


cur_dir=$(pwd)


if [ ! -z $1 ]; then
	build=$1
else
	build="help"
fi

RunCommand () {
	echo $green''$2' Started'$reset
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

Failed()
{
	echo $red'[ERROR]'$1' FAILED'
	echo ""
	echo " ============================================"
	echo "||                "$bold"BUILD FAILED$reset$red              ||"
	echo " ============================================"
	echo ""$reset
	exit 1
}


BuildCttGui()
{
	cd $cur_dir
	RunCommand "rm -rf bin/linux/ConformanceTestTool" "Removing previously build directory"

	mkdir -p bin/linux/ConformanceTestTool
	mkdir -p bin/linux/ConformanceTestTool/libs
	mkdir -p bin/linux/ConformanceTestTool/testsuite
	mkdir -p bin/linux/ConformanceSimulator
	mkdir -p bin/linux/DtlsRelay
	mkdir -p bin/linux/ConformanceTestTool/RAMLParser

	echo $(pwd)

	#copy jackson library from raml parser
	if [ -e bin/linux/RAMLParser/lib ]; then
		RunCommand "cp bin/linux/RAMLParser/lib/jackson-core-2.6.3.jar res/ConformanceTestTool/libs" "Copying lib:jackson-core-2.6.3.jar"
		RunCommand "cp bin/linux/RAMLParser/lib/jackson-annotations-2.6.3.jar res/ConformanceTestTool/libs" "Copying lib:jackson-annotations-2.6.3.jar"
		RunCommand "cp bin/linux/RAMLParser/lib/jackson-databind-2.6.3.jar res/ConformanceTestTool/libs" "Copying lib:jackson-databind-2.6.3.jar"
	else
		Failed "ERROR: bin/linux/RAMLParser/lib does does not exists"
	fi


	cd res/ConformanceTestTool/build
	./build_gui.sh
	cd ../../../


	if [ ! -e res/ConformanceTestTool/bin/ConformanceTestTool ]; then
		echo $bold$red'GUI Binary not found, only CLI will be supported!'$reset
		mkdir -p bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs
		RunCommand "cp -r res/ConformanceTestTool/provisioning res/ConformanceTestTool/testsuite bin/linux/ConformanceTestTool" "Required Files Installation"
	else
		echo $bold$green'GUI Binary found, Both GUI and CLI will be supported!'$reset
		RunCommand "cp -r res/ConformanceTestTool/bin/ConformanceTestTool/* res/ConformanceTestTool/provisioning res/ConformanceTestTool/testsuite bin/linux/ConformanceTestTool" "Required Files Installation"
	fi


	if [ -e bin/linux/RAMLParser/ramls ]; then
		RunCommand "cp -r bin/linux/RAMLParser/ramls bin/linux/ConformanceTestTool" "Copying ramls files"
	else
		Failed "ramls does not exists"
	fi

	if [ -e bin/linux/RAMLParser/bin/RAMLParser ]; then
		RunCommand "cp bin/linux/RAMLParser/bin/RAMLParser bin/linux/ConformanceTestTool/RAMLParser" "Copying RAMLParser Script"
	else
		Failed "RAMLParser does not exists"
	fi
	
	if [ -e bin/linux/RAMLParser/lib ]; then
	RunCommand "cp -r bin/linux/RAMLParser/lib bin/linux/ConformanceTestTool" "Copying lib"
	else
		Failed "lib does not exists"
	fi

	RunCommand "cp res/ConformanceTestTool/RAMLExecutor.sh bin/linux/ConformanceTestTool/RAMLParser" "Copying RAMLExecutor.sh"
	chmod 777 bin/linux/ConformanceTestTool/RAMLParser/*
}

BuildConformanceSimulator()
{
	cd res/ConformanceSimulator
	RunCommand "scons" "Conformance Simulator Build" 
	cd ../..

}

BuildDtlsRelay()
{

	cd res/DtlsRelay
	RunCommand "scons" "Dtls Relay Build" 
	cd ../..
}


BuildRamlParser()
{
	# Buiding RAMLParser
	cd res/RAMLParser/
	RunCommand "gradle installApp" "RAML Pareser Build"
	cd ../..
	mkdir -p bin/linux/RAMLParser
	RunCommand "cp -r res/RAMLParser/build/install/RAMLParser/* res/RAMLParser/ramls  bin/linux/RAMLParser/" "RAML Pareser Installation"
}



BuildJCOAP()
{
	# Buiding JCOAP
	cd extlibs/ws4d-jcoap
	RunCommand "gradle install" "JCOAP Build"
	cd ../../ 
}

BuildIotivityStack()
{
	# Buiding Iotivity Stack
	cd extlibs/iotivity-stack
	RunCommand "gradle install" "Iotivity Stack Build" 
	cd ../../
}



BuildConformanceLib()
{
	#echo $(pwd)
	# Buiding Conformance Lib
	RunCommand "gradle install" "Conformance Lib Build"
}

#####################################################################################################
##################### BUILD Start ###################################################################
#####################################################################################################

if [ "$build" = "help" ] ; then
	echo $green
	echo "#############################################################################"
	echo "Conformance Test Tool Build"
	echo "Use follwing for specific build"
	echo "./build all		To build all modules"
	echo "./build help		To see help"
	echo "./build simulator	For Simulator build (this will include dtls relay build)"
	echo "./build gui		For GUI build (this will include Raml Parser if it is not already build)"
	echo "./build rp		For Raml Parser Build"
	echo "./build lib		For Conformance Lib (this will include JCOAP)"
	echo "#############################################################################"
	echo $reset
	exit 0
fi

# Clearing out dir
if [ "$build" = "all" ] ; then
	echo "Clearing bin directory"
	rm -rf bin
fi

if [ "$build" = "all" ] || [ "$build" = "simulator" ] ; then
	# Buiding ConformanceSimulator
	BuildConformanceSimulator

	# Buiding DtlsRelay
	BuildDtlsRelay
fi


# Preparing Conformance Tool
echo ''
echo 'Preparing Conformance Tool. It will take several minutes. Please Wait ...'
echo ''


if [ "$build" = "all" ] || [ "$build" = "rp" ] ; then
	# Buiding raml parser
	BuildRamlParser
fi


if [ "$build" = "all" ] || [ "$build" = "gui" ]
then
	
	#check if raml parser is empty
	#if empty or all selected then build ramls parsesr
	DIR=$(pwd)/bin/linux/RAMLParser
	
	if [ ! "$(ls -A $DIR)" ] || [ "$build" = "all" ] ; then
		#echo "$DIR"
		echo "Building RAML Parser"
		BuildRamlParser
	else
		echo "Raml parser folder is not empty (already built) or all not selected"
	fi

	BuildCttGui
fi



cd res/ConformanceSimulator/bin/linux/
RunCommand "find . -type f -not -iname '*.o' -exec cp '{}' '../../../../bin/linux/ConformanceSimulator' ';' " "Conformance Simulator Installation"
cd ../../../../

cd res/DtlsRelay/bin/linux/
RunCommand "find . -type f -not -iname '*.o' -exec cp '{}' '../../../../bin/linux/DtlsRelay' ';' " "Dtls Relay Installation"
cd ../../../../


#RunCommand "cp -r res/DtlsRelay/bin/linux bin/linux/DtlsRelay" "Dtls Relay Installation"
RunCommand "cp -r res/ConformanceTestTool/libs/* bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/" "Dependent Jars Installation"

if [ "$build" = "all" ] || [ "$build" = "lib" ]
then
	BuildJCOAP	
	BuildIotivityStack
	BuildConformanceLib
	RunCommand "cp extlibs/ws4d-jcoap/build/libs/*.jar libs/" "JCOAP Installation"
	RunCommand "cp extlibs/iotivity-stack/build/libs/*.jar libs/" "Iotivity Stack Installation"
fi



RunCommand "cp libs/*.jar bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/" "Dependent Library Installation"
RunCommand "cp bin/linux/ConformanceTestTool/libs/IoTConformanceTestLib.jar bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/IoTConformanceTestLib.jar" "Conformance Lib Installation"

RunCommand "mv bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/logger.config bin/linux/ConformanceTestTool/" "Logger Config Installation"
RunCommand "mv bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/DUTDescriptor.json bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/OICDescriptor.json bin/linux/ConformanceTestTool/plugins/oic.ctt.ui_1.0.0/libs/payload.json bin/linux/ConformanceTestTool/libs/" "Required JSON Files Installation"
RunCommand "mv bin/linux/ConformanceTestTool/testsuite/run_tc.sh bin/linux/ConformanceTestTool/" "Command Line Interface Installation"
RunCommand "rm -rf res/ConformanceTestTool/bin/oic res/ConformanceTestTool/bin/repository" "Unnecessary File Remove"

echo $green""
echo " ============================================"
echo "||              "$bold"BUILD SUCCESSFUL$reset$green            ||"
echo " ============================================"
echo ""$reset
