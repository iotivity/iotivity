README
======

OS: Ubuntu 12.0.4 and above
Requires boost version 1.55 to build. 
Code Builds with g++ 4.6, 4.8, 4.9.
NOTE: Boost is not distributed but should be installed in the Ubuntu machine. 
Set the BOOST_PATH variable in the Top level Makefile or pass it as command line while running make.

Control manager Code Structure in OIC
- /oic-resource/service/control-manager/
								- /controlmanager/
								- /controllee/
								- /RESTFramework/
								- /samples/
								- /opensource/
								- /build_common
								- /makefiles/
								- /Makefile
								- /README.txt
								- SConstruct

Building with Makefile:
=======================

Make sure that you build oic-resource/resource as per guidelines.
								
Building libcontrolmanager.a, libcontrollee.a, samples/controller-client and samples/controlee-server
=====================================================================================================
Release Build: (By default CONFIG is "release", by default TARGET_OS is "linux32")
control-manager$ make CONFIG=release TARGET_OS={linux32/linux64}

output:
/control-manager/release/linux32/controlmanager/static/libcontrolmanager.a
/control-manager/release/linux32/controllee/static/libcontrollee.a
/control-manager/samples/controller-client/release/controller-client
/control-manager/samples/controlee-server/release/controlee-server

Debug Build:
control-manager$ make CONFIG=debug TARGET_OS={linux32/linux64}

Output:
/control-manager/debug/linux32/controlmanager/static/libcontrolmanager.a
/control-manager/debug/linux32/controllee/static/libcontrollee.a
/control-manager/samples/controller-client/debug/controller-client
/control-manager/samples/controlee-server/debug/controlee-server


Building for Android
====================
There is no Makefile build for Android provided.



Building with Scons:
====================

Building libcontrolmanager.a, libcontrollee.a, RESTFramework (librestfw.so):
===========================================================================
Make sure that you have built oic-resource/resource using scons.

(By default RELEASE_BUILD=false, TARGET_OS=linux, TARGET_ARCH=x86)

Run following command:
control-manager$ scons TARGET_OS=linux TARGET_ARCH=x86 RELEASE_BUILD={true/false}

output(Debug):
/control-manager/output/linux/x86/debug/libcontrolamanger.a
/control-manager/output/linux/x86/debug/libcontrollee.a
/control-manager/output/linux/x86/debug/librestfw.so

output(Release):
/control-manager/output/linux/x86/release/libcontrolamanger.a
/control-manager/output/linux/x86/release/libcontrollee.a
/control-manager/output/linux/x86/release/librestfw.so



Building samples:
=================

Run following command:
control-manager$ scons SAMPLE=yes TARGET_OS=linux TARGET_ARCH=x86 RELEASE_BUILD={true/false}

output(Debug):
/control-manager/output/linux/x86/debug/controller-client
/control-manager/output/linux/x86/debug/controllee-server
/control-manager/output/linux/x86/debug/REST-client

output(Release):
/control-manager/output/linux/x86/release/controller-client
/control-manager/output/linux/x86/release/controllee-server
/control-manager/output/linux/x86/release/REST-client


RESTFramework dependencies:
==========================

Dependency files for librestfw.so:
  libbase.so, libuuid.so, libcontrolmanager.a, liboc.a, liboc_logger.a, liboctbstack.a, boost_system, boost_regex, boost_thread

Sample Test Execution: ./REST-client
  Set the LD_LIBRARY_PATH with 'libbase.so, libuuid.so, boost lib path and actual output path (e.g, /control-manager/build/linux/x86/release)
  Run the 'RESTClient_Sample' from output path (E.g, './output/linux/x86/release/REST-client')
  Once the sample starts running send any http request using 'cURL or Python scripts.
  Note: The 'RESTClient_Sample' runs at port 3115.


Building Control Manager for Android
====================================

Make sure that you have built oic-resource/resource using scons for Android.

control-manager$ scons TARGET_OS=android TARGET_ARCH=armeabi ANDROID_NDK={Android NDK Path}
