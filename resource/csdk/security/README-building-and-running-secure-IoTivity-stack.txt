LAST UPDATED 9/29/2016

To build the IoTivity stack with the security features enabled:

1) Build IoTivity with security enabled:
	$ cd <iotivity-base>
	$ scons SECURED=1

2) Verify functionality using secure sample apps:
	$ cd <iotivity-base>/out/<...>/release/resource/csdk/stack/samples/linux/secure
	$ export LD_LIBRARY_PATH=<iotivity-base>/out/<...>/release
	$ ./ocserverbasicops &
	$ ./occlientbasicops -t 1
        NOTE: Console messages below indicate success:
         "INFO: occlientbasicops: Uri -- /a/led
          INFO: occlientbasicops: SECUREPORT udp: 51516
          INFO: occlientbasicops: Secure -- YES"
	$ ./occlientbasicops -t 2
        NOTE: Completion of 'GET' and 'PUT' query to /a/led indicates success!

