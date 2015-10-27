== Quick guide: build and run ConformanceSimulator on Ubuntu ==

1. Pre-Requisite:
	To build this project, IoTivity 1.0.0 release is required. The 'iotivity' project must be downloaded and build for linux first.

2. Build IoTivity
	Go to the top(root) directory of 'iotivity' project.
	Follow the pre-requisite steps described ifor IoTivity project.
	Run the below command (for non-secured resource):
		$ scons TARGET_OS=linux
			(Note: C sdk requires tinycbor. Please follow the instruction in the build message to install tinycbor).
	To build Iotivity using Secured resource option, run the below command:
	$ scons TARGET_OS=linux SECURED=1

3. Build ConformanceSimulator
	Go to the top(root) directory of 'ConformanceSimulator' project.
	Set the loaction of 'iotivity' project as IOTIVITY_HOME:
		$ export IOTIVITY_HOME=<path-to-iotivity-root>
	Run the below command to build Conformance simulator
		scons

	Help:
		$ scons -h

	Clear:
		$ scons -c

4. Run ConformanceSimulator
	From the top(root) directory of ConformanceSimulator, go to linux binary folder:
		$ cd bin/linux/
	Append the library location of iotivity to system library path:
		$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path-to-iotivity-root>/out/linux/x86/release/
	Alternatively, for 64 bit operating system:
		$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path-to-iotivity-root>/out/linux/x86_64/release/
	To run ConformanceSimulator indefault settings(NON-confirmable ,IPv6, non-secure server):
		$ ./SonformanceSimulator
	
	General command to run ConformanceSimulator:
		$ ./ConformanceSimulator [QoS<0/1>] [IP_Version<4/6>] [Security<0/1/2>]
		QoS: 0 = NON, 1 = CON
		IP_Version: 4 = IPv4, 6 = IPv6
		Security: 0 = unsecure, 1 = Secure_client, 2 = Secure_server
	e.g.,to Run IpV4 non-secure NON-type server:
		$ ./ConformancSiulator 0 4 0
	Alternatively,
		$ ./ConformancSiulator 0 4

	To run IpV6 secure CON-type client:
		$ ./ConformancSiulator 1 6 1

	To run IpV6 secure CON server:
		$ ./ConformancSiulator 1 6 2		



=== ConformanceSimulator project build tool scons ===

Scons is a cross-platform build tool, its usage is quite similar to GNU make.
To build a project, you just require to run following command at the directory
where a SConstruct file exists(SConstruct is the entrance of scons build, it's
equivalent to Makefile of 'make') :
      $ scons [options] [target]

In additional, usually the scons build script of a project provides useful help
information(include build options). To see the help information:
      $ scons [options] -h

Note: If no value is specified for an option, the default value will be used.
The change of options value may impact the help information and the behavior
of the building.

Generally, it's required to specify the target OS and target ARCH, that's to say
tell Scons which OS and which ARCH you'd like build this project for. By default,
the target OS and ARCH is the same as the host.

Some more options may be required, please care the 'error' notification when build.
For help about how to set an option, please run:
     $ scons TARGET_OS=xxx TARGET_ARCH=yyy [XXX=zzz ...] -h


=== Prerequites ===

* 1. Scons

Please refer to the following page to install scons:
   http://www.scons.org/doc/production/HTML/scons-user.html#chap-build-install


* 2. IoTivity

ConformanceSimulator is based on IoTivity project libraries. To downlaod and build IoTivity
properly, please follow to the below link:
   https://www.iotivity.org/downloads/iotivity-1.0.0
For environment setup guid, follow the below link:
   https://www.iotivity.org/documentation/linux/getting-started 


* 3. External libraries

ConformanceSimulator project depends on some external libraries, such as boost, expat etc.
During building, the existence of external library will be checked, if it doesn't
exist, the build script will try to download, unpack and build the library or
notify user to install it.

Downloading and unpacking may fail due to network problem or required unpacking
tool isn't installed. An message will be displayed, please follow the message
to skip it.

