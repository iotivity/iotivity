#!/bin/bash

<<"License"
/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/'
License

### To execute this file, use ./lib_maker.sh <module> <mode> <iotivityHome> <testHome>###
### Example for calling this script: ./lib_maker.sh tm release ###
### Its only for rh and tm module ###
### The default mode is release ###

module=$1
mode=$2
iotivityHomePath=$3
testPath=$4

tmpFolder="tmpDir"
currentDir=$(pwd)

#Project home from tmpDir folder

if [[ ! -z $iotivityHomePath ]]; then
	if [ -d "$iotivityHomePath" ]; then
		iotivityHome="$iotivityHomePath"
		echo "Continue with IoTivity Home Path: $iotivityHome"
	else
		echo "******* $iotivityHomePath ==> Is not a currect directory *******"
		exit 1
	fi
else
	cd ../../..
	iotivityHome=$(pwd)
	cd $currentDir
fi

# TestHome path set
if [[ ! -z $testPath ]]; then
	if [ -d "$testPath" ]; then
		testHome="$testPath"
		echo "Continue with Test Home Path: $testHome"
	else
		echo "******* $testPath ==> Is not a currect directory *******"
		exit 1
	fi
else
	cd ../..
	testHome=$(pwd)
	cd $currentDir
fi



serviceHome="$iotivityHome/service"

extlibsDir="$testHome/extlibs/android"
isCopied=true
arch="armeabi"

tmpLibProject="$testHome/$tmpFolder"

outDir="$iotivityHome/out/android/$arch/$mode"

if [ "$mode" != "debug" ]; then
	mode="release"
else
	mode="debug"
fi

## Checking Module ##
if [[ $module == "rh" || $module == "tm" || $module == "rc" ]]; then
	echo "Creating Library Project for $module."
else
	echo "Please enter valid module. Valid module are rh, rc and tm"
	echo "Exit from Lib Maker...."
	exit 1
fi

copy_api_project()
{
	echo "Copy Command: $copySrcCommand"

	if eval $copySrcCommand; then
		echo "API Project copied successfully"
	else
		isCopied=false
		echo "Can't copy API Project"
	fi
}

copy_bundles()
{
	bmiBundle="libBMISensorBundle.so"
	diBundle="libDISensorBundle.so"

	copyBMIBundle="cp -r $outDir/$bmiBundle $rcAssetDir/$bmiBundle"
	copyDIBundle="cp -r $outDir/$diBundle $rcAssetDir/$diBundle"

	if eval $copyBMIBundle && $copyDIBundle; then
		echo "Copy BMI and DI successfully..."
	else
		echo "Can't copy bundle files...."
	fi

	hueBundleDir="$hueSampleDir/target"

	for i in `find $hueBundleDir -name hue*.jar`;
	do
		copyHueBundle="cp -r $i $rcAssetDir/"
		if eval $copyHueBundle; then
			echo "Copy Hue Bundle successfully..."
		else
			echo "Can't copy Hue Bundle file...."
		fi
	done
}

hue_bundle_mvn_build()
{
	hueHomeCommand="cd $hueSampleDir"
	hueCompileCommand="mvn compile"
	hueInstallCommand="mvn install"
	hueCopyCommand=""

	# Go to hueJavaSample project
	if eval $hueHomeCommand; then
		echo "### Current directory $(pwd) ###"
		if eval $hueCompileCommand; then
			if eval $hueInstallCommand; then
				echo "HueJavaSample build and installed successfully..."
			else
				echo "Fail to execute $hueInstallCommand at $hueSampleDir...!!!"
			fi
		else
			echo "Fail to execute $hueCompileCommand at $hueSampleDir...!!!"
		fi
		
		# Go to build directory
		cd $currentDir
	fi
}

make_library_project()
{
	echo "### API Directory: $apiDir ###"

	android_command="android create lib-project --name $module --target android-21 --path $tmpLibProject --package $package"
	copySrcCommand="cp -r $apiDir $tmpLibProject"

	if eval $android_command; then
		echo "$module Library project is created successfully"

		echo "Copy the API files to Library Project"
		copy_api_project

		baseLibPath="$iotivityHome/android/android_api/base/build/intermediates/bundles/$mode/classes.jar"
		if [ $module == "tm" ]; then
			copyBase="cp $baseLibPath $tmpLibProject/libs/"
			if eval $copyBase; then
				echo "base.jar copied for TM..."
			else
				echo "Can't copy base.jar"
				isCopied=false
			fi
		fi

		if [ $isCopied == true ]; then
			cd $tmpLibProject
			echo "Build Library Project Started..."
			buildcommand="ant clean debug"

			if eval $buildcommand; then
				if [ ! -d "$extlibsDir/$module" ]; then
					mkdir -p $extlibsDir/$module
				fi

				copyAPILib="cp -r $tmpLibProject/bin/classes.jar $extlibsDir/$module/$module.jar"

				if eval $copyAPILib; then
					echo "API Library file created and copied to libs directory successfully as $module.jar"
				else
					echo "API Library is not created properly..."
				fi
			else
				echo "Build Library Project failed."
			fi

			cd $testHome
		else
			echo "Can't build the API project."
		fi
	else
	   echo "$module Library project creation failed"
	fi
}

remove_temp_folder ()
{
	removeTmp="rm -rf "$tmpFolder"/"

	if eval $removeTmp; then
		echo "temp folder removed"
	else
		echo "Can't remove temp folder"
	fi
}

if [ $module == "rh" ]; then
	package="org.iotivity.service.rh"
	rhHome="$serviceHome/resource-hosting"
	apiDir="$rhHome/android/resource_hosting/*"
	make_library_project
elif [ $module == "tm" ]; then
	package="org.iotivity.service.tm"
	tmHome="$serviceHome/things-manager"
	apiDir="$tmHome/sdk/java/*"
	make_library_project
elif [ $module == "rc" ]; then
	rcHome="$serviceHome/resource-container"
	hueSampleDir="$rcHome/examples/HueJavaSampleBundle/hue"
	hue_bundle_mvn_build
	rcAssetDir="$testHome/src/tc/rc/junit/assets/lib"
	echo "### Current directory $(pwd) ###"
	copy_bundles
fi

# Called Removing temp. directory function
remove_temp_folder

echo "API Library Maker has finished..."
