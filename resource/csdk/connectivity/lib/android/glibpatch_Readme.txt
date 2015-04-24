/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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

Pre-requisites :
Install libffi6 and libffi-dev packages
#sudo apt-get install libffi6 libffi-dev


Download Gnome Glib 2.40.2 from http://ftp.gnome.org/pub/GNOME/sources/glib/2.40/
1.Download glib-2.40.2.tar.xz

2.untar the Downloaded file
#tar -xJf glib-2.40.2.tar.xz

3.change to glib-2.40.2 directory
#cd glib-2.40.2

4.
#./configure

5.If configure is successful, your base folder is ready to apply the patch :
#cd ..
#patch -p1 < resource/csdk/connectivity/lib/android/patches/glib-2.40.2-x64.patch

There are two kinds of patches :
	- with Android.mk files, automatically added in the respective folders
	- without Android.mk files (If you want to use your own Android.mk files)

	glib-2.40.2.patch
	glib-2.40.2-wihtoutMakefile.patch

	glib-2.40.2-x64.patch
	glib-2.40.2-x64-wihtoutMakefile.patch

If you are using 64-bit linux build, please apply the patch named with "x64"

6.To build glib libraries for Android, Andoroid.mk and Application.mk are placed at
resource/csdk/connectivity/lib/android/jni
#cd resource/csdk/connectivity/lib/android

7.Edit the resource/csdk/connectivity/lib/android/jni/Android.mk
#vi jni/Android.mk

8.Copy your glib-2.40.2 directory to extlibs/glib.
Edit the GLIB_SRC_PATH and add the path to glib source directory
GLIB_SRC_PATH ?= ../../../../../extlibs/glib/glib-2.40.2

9a-1.run the ndk-build
#$(NDK_PATH)/ndk-build

9a-2.copy libglib-2.40.2.so and libgthread-2.40.2.so from libs/armeabi folder to
resource/csdk/connectivity/lib/android directory
Change the folder path as per the TARGET_ARCH
#cp libs/armeabi/*.* ./

or

9b-1.
#chmod 777 ./glib_build.sh

9b-2. Edit the NDK_PATH and set the ndk folder path
#vi glib_build.sh
NDK_PATH = ~/ndk

9b-3 Edit the folder path as per TARGET_ARCH
#cp libs/armeabi/*.* .

9b-4.
#./glib_build.sh

10.This will build the glib libraries and copy to the current folder

11. To build with NDK Edit the resource/csdk/connectivity/build/Android.mk file.
set the GLIB_PATH to your glib-2.40.2 folder path.

GLIB_PATH				= $(PROJECT_LIB_PATH)/android/glib-2.40.2

