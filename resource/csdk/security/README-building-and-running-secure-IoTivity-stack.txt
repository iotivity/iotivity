LAST UPDATED 5/19/2015

Note: These instructions assume the "security-M3" branch of IoTivity has been checked out.

To build the IoTivity stack with the security features enabled:

1) Build TinyDTLS:
	$ cd <iotivity-base>/extlibs/tinydtls
	$ autoconf
	$ ./configure --without-debug
	$ make

2) Build IoTivity with security enabled:
	$ cd <iotivity-base>
	$ scons resource SECURED=1

3) Verify functionality using secure sample apps:
	$ cd <iotivity-base>/out/<...>/release/resource/csdk/stack/samples/linux/secure
	$ export LD_LIBRARY_PATH=<iotivity-base>/out/<...>/release
	$ ./ocserverbasicops &
	$ ./occlientbasicops -t 1

	Message "INFO: occlientbasicops: Secure -- YES" indicates success!
