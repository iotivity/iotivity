== How to build IoTivity projects ==

IoTivity includes a series of projects. You can find all these projects here:
    https://gerrit.iotivity.org/gerrit/#/admin/projects/

You can build IoTivity project on Linux / Windows / MAC OSX for various OS(
Linux, Tizen, Android, Arduino, Windows, MAC OSX, IOS ...).
The output of the build is in:
  <top directory of the project>/out/<target_os>/<target_arch>/<build_type>/
e.g.
	oic-resource/out/android/armeabi-v7a/release/.

This document takes 'iotivity' project as example, the way to build other
projects is almost the same.

=== IoTivity project build tool scons ===

Scons is a cross-platform build tool, its usage is quite similar to GNU make.
To build a project, you just require to run following command at the directory
where a SConstruct file exists(SConstruct is the entrance of scons build, it's
equivalent to Makefile of 'make') :
      $ scons [options] [target]

In additional, usually the scons build script of a project provides useful help
information(include build options). To see the help information:
      $ scons [options] -h

Note: If no value is specified for an option, the default value will be used.
The change of options value may impact the output.


=== Prerequites ===

* 1. Scons

Please refer to the following page to install scons:
   http://www.scons.org/doc/production/HTML/scons-user.html#chap-build-install
(Note: on Windows, install Python 2.x before installing scons)

* 2. IDE/SDK Prerequites
To build for some OS (Android / Arduino / IOS ...), an IDE/SDK may be required,
please go to the relative page to download and install the required IDE/SDK.

Android:
To build for Android, Andorid NDK and SDK are required.
  Android NDK: http://developer.android.com/tools/sdk/ndk/index.html
  Android SDK: http://developer.android.com/sdk/index.html
(Note: as in some IoTivity projects, C++11 features are used, recommend Android
 NDK >= r10, according to our test result r10c is the best one currently)

Arduino:
To build for Arduino, Arduino IDE is required.
  Arduino IDE: http://arduino.cc/en/Main/Software
  (Note: recommend install Arduino IDE >=1.5.7)

Arduino builds are dependent on latest Time library. Download it from here:
    http://www.pjrc.com/teensy/td_libs_Time.html
and extract to <arduino_ide_root>/libraries/

(Note: If you are using Arduino IDE 1.5.8 BETA on Windows, it may pop up some
dll isn't found. please copy relative dll from the IDE directory to
C:\Windows\SysWOW64. IDE 1.5.7 doesn't have this issue. Other version IDE isn't
tested.)

Apple:
To build for Mac OSX or IOS, Xcode is required.
  Xcode: https://developer.apple.com/xcode/downloads/

Java:
To build the Java code, JDK is required.
  JDK: http://www.oracle.com/technetwork/java/javase/downloads/index.html
(If the project doesn't include Java code or you wouldn't like build the
Java codes, this isn't required)

(Note: for convenience, suggest add the IDE/SDK path in environment variable,
so you don't need to add it in command line each time. The build script will
guide you to do that.)

* 3. External libraries
For Android and IOS build, most of the external libraries are provided as
binary in oic-utilities project (https://oic-review.01.org/gerrit/oic-utilities).
Please download it in the same directory as other IoTivity projects. If it's
in different directory, an additional option (OIC_UITLS) will be required. The
build command should be:
      $ scons OIC_UITLS=<path to oic-utilities> [other options] [target]

(Note: for convenience, you can also add it in environment variable. So you
don't need to add this option in command line each time.)
  e.g.:
      $ export OIC_UITLS=<path to oic-utilities project>


=== Build IoTivity project on Linux(Ubuntu) ===

Generally, it's required to specify the target OS and target ARCH, that's to say
tell Scons which OS and which ARCH you'd like build this project for. By default,
the target OS and ARCH is the same as the host.

Some more options may be required, please care the 'error' notification when build.
For help about how to set an option, please run:
	$ scons TARGET_OS=xxx TARGET_ARCH=yyy [XXX=zzz ...] -h

1. Build IoTivity project for Linux
      $ cd <top directory of the project>
      $ sudo apt-get install libboost-dev libboost-program-options-dev libexpat1-dev
      $ scons

2. Build IoTivity project for Android
      $ cd <top directory of the project>
      $ scons TARGET_OS=android TARGET_ARCH=xxx
(xxx can be x86, armeabi, armeabi-v7a, armeabi-v7a-hard. To see all of its
allowed value, please execute command 'scons TARGET_OS=android -Q -h')

Note: Currently as x86_64/arm64_v8a external library binaries aren't provided,
you may meet link problem if build executable binary which depends on external
library for x86_64/arm64_v8a.

3. Build IoTivity project for Arduino
      $ cd <top directory of the project>
      $ scons TARGET_OS=arduino TARGET_ARCH=xxx BOARD=yyy
(xxx can be avr, arm; yyy is the name of the board, to get its allowed value
run: scons TARGET_OS=arduino TARGET_ARCH=xxx -h. You may see a option 'CPU' in
the output of above command line, that's due to some boards have different
processors, to specify the processor, add 'CPU=zzz' in the command line. If no
'CPU' option exists, that means the board only support one kind of processor,
it's unnecessary to specify it)

4. Build Iotivity project for Tizen
      $ cd <top directory of the project>
      $ gbs build -A xxx --packaging-dir tools/tizen/
(xxx can be i586 or armv7l, we provide the spec file required by gbs tool at
toools/tizen directory. gbs is default build tool for Tizen platfrom, we can
refer the following wiki to setup Tizen development environment:
https://source.tizen.org/documentation/developer-guide/getting-started-guide)

=== Build IoTivity project on Windows ===

1. Build IoTivity project for Android(It's the same as on Ubuntu)
      $ cd <top directory of the project>
      $ scons TARGET_OS=android TARGET_ARCH=xxx
(xxx can be x86, armeabi, armeabi-v7a, armeabi-v7a-hard ...)

2. Build IoTivity project for Arduino(It's the same as on Ubuntu)
      $ cd <top directory of the project>
      $ scons TARGET_OS=arduino TARGET_ARCH=xxx BOARD=yyy
(xxx can be avr, arm; yyy is the name of the board, to get its allowed value
run: scons TARGET_OS=arduino TARGET_ARCH=xxx -h. You may see a option 'CPU' in
the output of above command line, that's due to some boards have different
processor, to specify the processor, add 'CPU=zzz' in the command line. If no
'CPU' option exists, that means the board only support one kind of processor,
it's unnecessary to specify it)


Note: Currently most IoTivity project doesn't support Windows, so you can't set
TARGET_OS to 'windows' except the project support Windows.

That's to say if the project doesn't support Windows, run:
      $ scons TARGET_OS=windows ....
or run on Windows
      $ scons
may always fail.


=== Build IoTivity project on Mac OSX ===

1. Build IoTivity project for Mac OSX
      $ cd <top directory of the project>
      $ scons SYS_VERSION=yyy
(yyy is the OSX version, e.g. 10.9)

2. Build IoTivity project for Android(It's the same as on Ubuntu)
      $ cd <top directory of the project>
      $ scons TARGET_OS=android TARGET_ARCH=xxx
(xxx can be x86, armeabi, armeabi-v7a, armeabi-v7a-hard)

3. Build IoTivity project for IOS
      $ cd <top directory of the project>
      $ scons TARGET_OS=ios TARGET_ARCH=xxx SYS_VERSION=yyy
(xxx can be i386, x86_64, armv7, armv7s, arm64, yyy is IOS version, e.g. 7.0)

Note:
1) for convenience, a script (auto_build.sh) is provided to run possible build
at once. Following is the usage:

To build:
     $ auto_build.sh <path-to-android-ndk> <path-to-arduino-home>
To clean:
     $ auto_build.sh -c

2) For Arduino build, the Time library should >=1.3. The old can only be built
with Arduino IDE 1.0.x
