# Introduction
This is a sample secure application using the "justworks" security model
There are 2 applications; server and client, which need to be running from 2
different terminals regardless whether those 2 terminals are running within
the same machine or not as long as they can discover each other.
These 2 applications are verified on
* Regular Ubuntu machine
* Ubuntu running on Intel Joule
* Raspbian running on Raspberry Pi 3 and Raspberry Pi Zero W

These applications can be used to verify the build environment is setup
properly. They can serve as a baseline and a reference for new developers to
learn how to write simple server and client applications and implement OCF
security and pass the OCF Conformance Test Tool (CTT).

The client application does not know how to onboard, so the server is
provisioned already onboarded and owned by the client (Ready for Normal
Operation or RFNOP state). If the server application is to be used with a
different client, it will need to be reset first.

# Building the applications

To build the applications, type the following scons command from the root
directory of IoTivity

```
$ scons examples/OCFSecure TARGET_TRANSPORT=IP
```

To speed up the build procerss and utilize more than a single core, you can
add the -j option followed by the number of cores to utilize for building
the applications

To build the applications in the debug mode, you can add the following option
to the scons command RELEASE=0

# Running the applications

To run the applications on a regular machine with Ubuntu, change the directory
to `out/linux/x86_64/release/examples/OCFSecure` with the following command
```
$ cd out/linux/x86_64/release/examples/OCFSecure/
```
Next, run the server application with the following command
```
$ ./server
```
open up another terminal window within the same directory
(shortcut:Ctrl+Shift+n) and run the client application with the following
command.
```
$ ./client
```
Since there is no physical led attached to a regular machine with Ubuntu, the
led resource will be simulated. Otherwise, if you are running these
applications on a Raspberry Pi or Intel Joule board then you would need to
run the server application with the sudo command so the server application
can have access to the hardware of the board.

If successful, you should be able to see the list of discovered resources
in the client terminal as shown below.
```
Discovered Resources:
    #0: /oic/res    @224.0.1.187:5683    resource type: nil
    #1: /oic/sec/doxm    @fe80::3749:e171:d50f:966d%enp0s3:42825    resource type
    #2: /oic/sec/pstat    @fe80::3749:e171:d50f:966d%enp0s3:0    resource type
    #3: /oic/sec/acl2    @10.0.2.15:47132    resource type: oic.r.acl2
    #4: /oic/sec/cred    @10.0.2.15:47132    resource type: oic.r.cred
    #5: /oic/sec/crl    @10.0.2.15:47132    resource type: oic.r.crl
    #6: /oic/sec/csr    @10.0.2.15:47132    resource type: oic.r.csr
    #7: /oic/sec/roles    @10.0.2.15:47132    resource type: oic.r.roles
    #8: /oic/d    @10.0.2.15:47132    resource type: oic.wk.d
    #9: /oic/p    @10.0.2.15:0    resource type: oic.wk.p
    #10: /introspection    @10.0.2.15:0    resource type: oic.wk.introspection

Request Methods:    1: GET    4: POST
Usage:\<resource number> \<request method> :
```
Usually, you might need to press enter a couple of times to refresh the stdout
to finally see the complete list like so.
```
Discovered Resources:
    #0: /oic/res    @224.0.1.187:5683    resource type: nil
    #1: /oic/sec/doxm    @fe80::3749:e171:d50f:966d%enp0s3:42825    resource type
    #2: /oic/sec/pstat    @fe80::3749:e171:d50f:966d%enp0s3:0    resource type
    #3: /oic/sec/acl2    @10.0.2.15:47132    resource type: oic.r.acl2
    #4: /oic/sec/cred    @10.0.2.15:47132    resource type: oic.r.cred
    #5: /oic/sec/crl    @10.0.2.15:47132    resource type: oic.r.crl
    #6: /oic/sec/csr    @10.0.2.15:47132    resource type: oic.r.csr
    #7: /oic/sec/roles    @10.0.2.15:47132    resource type: oic.r.roles
    #8: /oic/d    @10.0.2.15:47132    resource type: oic.wk.d
    #9: /oic/p    @10.0.2.15:0    resource type: oic.wk.p
    #10: /introspection    @10.0.2.15:0    resource type: oic.wk.introspection
    #11: /oic/sec/doxm    @fe80::5c79:f160:691d:40bd%enp0s8:42825    resource type
    #12: /oic/sec/pstat    @fe80::5c79:f160:691d:40bd%enp0s8:0    resource type
    #13: /oic/sec/acl2    @192.168.56.101:47132    resource type: oic.r.acl2
    #14: /oic/sec/cred    @192.168.56.101:47132    resource type: oic.r.cred
    #15: /oic/sec/crl    @192.168.56.101:47132    resource type: oic.r.crl
    #16: /oic/sec/csr    @192.168.56.101:47132    resource type: oic.r.csr
    #17: /oic/sec/roles    @192.168.56.101:47132    resource type: oic.r.roles
    #18: /oic/d    @192.168.56.101:47132    resource type: oic.wk.d
    #19: /oic/p    @192.168.56.101:0    resource type: oic.wk.p
    #20: /introspection    @192.168.56.101:0    resource type: oic.wk.introspection
    #21: /oic/sec/doxm    @fe80::3749:e171:d50f:966d%enp0s3:59488    resource type
    #22: /oic/sec/pstat    @fe80::3749:e171:d50f:966d%enp0s3:0    resource type
    #23: /oic/sec/acl2    @10.0.2.15:50400    resource type: oic.r.acl2
    #24: /oic/sec/cred    @10.0.2.15:50400    resource type: oic.r.cred
    #25: /oic/sec/crl    @10.0.2.15:50400    resource type: oic.r.crl
    #26: /oic/sec/csr    @10.0.2.15:50400    resource type: oic.r.csr
    #27: /oic/sec/roles    @10.0.2.15:50400    resource type: oic.r.roles
    #28: /oic/d    @10.0.2.15:50400    resource type: oic.wk.d
    #29: /oic/p    @10.0.2.15:0    resource type: oic.wk.p
    #30: /introspection    @10.0.2.15:0    resource type: oic.wk.introspection
    #31: /switch    @10.0.2.15:50400    resource type: oic.r.switch.binary
    #32: /oic/sec/doxm    @fe80::5c79:f160:691d:40bd%enp0s8:59488    resource type
    #33: /oic/sec/pstat    @fe80::5c79:f160:691d:40bd%enp0s8:0    resource type
    #34: /oic/sec/acl2    @192.168.56.101:50400    resource type: oic.r.acl2
    #35: /oic/sec/cred    @192.168.56.101:50400    resource type: oic.r.cred
    #36: /oic/sec/crl    @192.168.56.101:50400    resource type: oic.r.crl
    #37: /oic/sec/csr    @192.168.56.101:50400    resource type: oic.r.csr
    #38: /oic/sec/roles    @192.168.56.101:50400    resource type: oic.r.roles
    #39: /oic/d    @192.168.56.101:50400    resource type: oic.wk.d
    #40: /oic/p    @192.168.56.101:0    resource type: oic.wk.p
    #41: /introspection    @192.168.56.101:0    resource type: oic.wk.introspection
    #42: /switch    @192.168.56.101:50400    resource type: oic.r.switch.binary

Request Methods:    1: GET    4: POST
Usage:\<resource number> \<request method> :
```
Notice the "/switch" uri on resources #31 and #42. This is a known issue will
be fixed at the soonest chance. be aware, the "/switch" uri might occure more
than twice and not all of them are going to work. However, one of them will
work.

As instructed in the client application. you can enter the number of the
discovered resource you want to interact with (which is either 31 or 42 in this
case) followed by 1 for issuing a GET request or 4 for a POST request.
If you decided to issue a GET request, you should see the following GET
response if everything went OK.

```
=========================================================
Result: (0) - OC_STACK_OK
07:12.062 INFO: PayloadLog: Payload Type: Representation
07:12.062 INFO: PayloadLog:     Resource #1
07:12.062 INFO: PayloadLog:     Resource Types:
07:12.062 INFO: PayloadLog:         oic.r.switch.binary
07:12.062 INFO: PayloadLog:     Interfaces:
07:12.062 INFO: PayloadLog:         oic.if.baseline
07:12.062 INFO: PayloadLog:         oic.if.a
07:12.062 INFO: PayloadLog:     Values:
07:12.062 INFO: PayloadLog:         value(bool):false
==========================================================
```
If you decided to issue a POST request, you will be prompted to create a custom
payload to be attached to the POST request. As of right now, the only supported
feature is turning the led on and off. As a result, type 0 to select the
boolean data type then follow it with the name of the property (the key) which
is "value" (without quotes as it is writing in the GET response above)
and then follow that with (the value) either 1 to turn the led on
or 0 to turn the led off. Next press ENTER twice; once to end the first
key-value pair entry and once again for completing the custom payload creation.

Below is a copy of an example usage.
```
    #42: /switch    @192.168.56.101:50400    resource type: oic.r.switch.binary

Request Methods:    1: GET    4: POST
Usage:<resource number> <request method> :42 4

Need to create a custom POST payload
Enter key value pairs as:    <type(int)> <key> <value>
Type: 0:bool      1:int      2:double      3:string
press ENTER to finish :0 value 1
press ENTER to finish :
```

If successful, you should see the following POST response
```
==========================================================
Result: (4) - OC_STACK_RESOURCE_CHANGED
08:24.366 INFO: PayloadLog: Payload Type: Representation
08:24.366 INFO: PayloadLog:     Resource #1
08:24.366 INFO: PayloadLog:     Resource Types:
08:24.366 INFO: PayloadLog:         oic.r.switch.binary
08:24.366 INFO: PayloadLog:     Interfaces:
08:24.366 INFO: PayloadLog:         oic.if.baseline
08:24.366 INFO: PayloadLog:         oic.if.a
08:24.366 INFO: PayloadLog:     Values:
08:24.366 INFO: PayloadLog:         value(bool):true
==========================================================
```
If for whatever reason you noticed the following response instead, then there
is a security issue. Hopefully, you will not see this!
```
==========================================================
Result: (46) - OC_STACK_UNAUTHORIZED_REQ
10:16.370 INFO: PayloadLog: NULL Payload
==========================================================
```
To run the applications on either the Intel Joule or the Raspberry Pi, the mraa
library must be installed! this is the library that is used to control the
physical hardware. Without it, you will still get the simulated led but it is
not as fun as the real led turning on and off in the physical world that you
can control with your own software! Install it, it is worth it ;)

Installing mraa on the intel joule. From the home directory, type commands
* $sudo add-apt-repository ppa:mraa/mraa
* $sudo apt-get update
* $sudo apt-get install libmraa1 libmraa-dev mraa-tools python-mraa python3-mraa

Installing mraa on the raspberry pi, from the parent directory of IoTivity's
root directory, type the following commands.
* $ git clone https://github.com/intel-iot-devkit/mraa.git
* $ cd mraa && mkdir build && cd build && cmake .. && make
* $ sudo make install

To run the applications on Intel Joule running ubuntu then the built-in
led on gpio 100 will be connected to the server and you would be able to
control it with the client application. It is important to remember to run the
server applications in sudo mode as hardware access is a super user privilege!
```
$ sudo ./server
```
Otherwise, you would get an error with unknown gpio or sometimes you will not
get any errors but the built-in led will not respond to your control. It is the
same usage as running the applications on a regular machine with Ubuntu which
should be described above.

To run the applications on Raspberry Pi with Raspian then make sure to connect
an LED on hardware pin 4 which is gpio 7! The pin mapping of raspberry pi is
just messed up like that! Google raspberry pi pinout
This pin has all the following names
* BCM pin 4
* Physical pin 7
* Wiring Pi pin 7

Anyways, have a jumper wire from GPIO 7 to the positive terminal of the led.
Then connect a resistor from the negative terminal of the led to ground.

Also, it is important to remember to run the server application in sudo mode as
hardware access is a super user privilege. Otherwise, you might get an error of
unknown gpio or the app might run but will not control the led and you might
think you connected the led on the wrong pin which may not be the case.

You can also connect the Enviro pHat sensor board if you have it. Its LED is
already connected to gpio 7.

# Resetting the server app
Sometimes it is necessary to reset the server application. For example, the
server app needs to be reset during OCF conformance testing or to be
onboarded by an OnBoarding tool.

In order to reset the server app, make sure it is not running. If it is running,
then kill it with Ctrl+C.
Copy the `ocf_svr_db_server_RFOTM.dat` from the project directory to the project
output directory and name it as `ocf_svr_db_server.dat` as shown
in the following command
From the output directory from which the server application can be exexuted,
type
 
```
cp ~/iot/iotivity/examples/OCFSecure/ocf_svr_db_server_RFOTM.dat ocf_svr_db_server.dat
```

# Testing the server app against CTT
You need to install the OCF Certification Test Tool 2.0 on a Windows machine
and start it and if the windows machine on the same network as the server and
the server app is running then the CTT should discover the server. If not,
please check the correct network interface from the options menu and then
press the Select IUT button or from the File menu. A pop-up window will
show the discovered devices and you should be able to see the server device
as 12345678-1234-1234-1234-123456789012 and in the details section, you
should be able to see the /switch uri. click on Next. Now, browse to select
the PICS file which should be included in this example named
`PICS_server_OCF10_vprime.json` then click on Next. From the Testing Profiles
uncheck everything and check OCF 1.0 Server. Next, click on
Run All Test Cases button. Most likely, you will get a prompt saying
"Please initiate device to revert to "ready for OTM" state" and there are
2 options to click on; OK and Cancel because this sample is shipped in the
"Ready for Normal Operation" state. In this case, reset the server as explained
in the "Resetting the server app" section of this document. Then run the server
application. You might get this prompt again since the CTT does not un-onboard
the device but now you know what to do!
Also, you will be prompted to power cycle the device. In this case, you can
either kill the server app and restart it again or literally power cycle
your device and re-run the server app once your device is back up and
connected to the same network.
Please note, you might see tests passing with warnings and CT1.7.8.11 test
Case failing but that is OK. In order to know which tests are required for
certification, refer to the Certification Requirements Status List (CRST) that
is associated with the CTT version.
This device is tested using CTT version 2.1, which is associated with CRSL 4.3.

# Known issues

1. Sometimes, the applications will not run because of not finding some library.
In this case, you would need to export the `LD_LIBRARY_PATH` to the environment.
    ```
    export LD_LIBRARY_PATH=<output dir orwherever the library is>
    ```
    Also, since you need to run the server application in privileged mode, you
would need to type this command
    ```
    $sudo ldconfig
    ```
and you might also need to type
    ```
    $sudo env LD_LIBRARY_PATH=<output dir or wherever the library is>
    ```
Hopefully, you will not run into any library issues but if you run into any
issue with this application, please send me an e-mail at ralshafi@vprime.com
and file a bug in JIRA and assign it to me (username: alshafi).

2. Multiple endpoints get discovered and they need to be filtered out
eventually. In the meantime, there will be multiple /switch links and only one
of them works and the user will need to issues GET requests to all of them
until the good one is found. The wrong /switch links will result in
Result: (255) - `OC_STACK_ERROR`.
The correct /a/led link will result in Result: (0) - `OC_STACK_OK`

# Example Directory

There are 16 files in the example directory.
* `client.c`
    * This is the client program
* `device_properties.dat`
    * This is a file storing the device properties in cbor format which is
generated automatically by the server application
* `ocf_svr_db_client.dat`
    * This is the cbor format of the secure virtual resource database, defined
by the human-readable version `ocf_svr_db_client.json` file, and it is used by
the client application
* `ocf_svr_db_client.json`
    * This is the human-readable version of `ocf_svr_db_client.dat`.
* `ocf_svr_db_server.dat`
    * This is the cbor format of the secure virtual resource database and it is
an exact copy from the `ocf_svr_db_server_RFNOP.dat` which is the cbor version
of the human-readable version `ocf_svr_db_server_RFNOP.json` file. This is the
case because the client application does not support the onboarding and
provisioning process currently and we need to set the state in the "Ready For
Normal Operation" manually.

We also need to set the state in the "Ready For Ownership Method Transfer"
when testing the application with the OCF Certification Test Tool (CTT).
In this case, you would need to copy `ocf_svr_db_server_RFOTM.dat` into
`ocf_svr_db_server.dat` since that is the file that will be read by the server.
* `ocf_svr_db_server_RFNOP.dat`
    * This is the cbor format of the secure virtual resource database, defined
by the human-readable version `ocf_svr_db_server_RFNOP.json` file and it is
*NOT* used by the server application. Rename it without the _RFNOP suffix to be
read by the server
* `ocf_svr_db_server_RFNOP.json`
    * This is the human-readable version of `ocf_svr_db_server_RFNOP.dat`
* `ocf_svr_db_server_RFOTM.dat`
    * This is the cbor format of the secure virtual resource database, defined
by the human-readable version `ocf_svr_db_server_RFOTM.json` file and it is
*NOT* used by the server application. Rename it without the `_RFOTM` suffix to
be read by the server
* `ocf_svr_db_server_RFOTM.json`
    * This is the human-readable version of `ocf_svr_db_server_RFOTM.dat`
* `PICS_server_OCF10_vprime.json`
    * This is the file that was used as the input to the OCF Certification
Test Tool.
* `README.md`
    * This is this file :)
* `SConscript`
    * This is the script that is being used by the scons tool to know how
to build the sample applications and what needs to be copied to the output
directory.
* `server.cpp`
    * This is the server program.
* `switch_introspection.dat`
    * This is the cbor format of the introspection file (also known as
Introspection Device Data IDD) the server needs to read to implement
the introspection feature.
* `switch_introspection.json`
    * This is the human-readable version of `switch_introspection.dat` file
which is also know as the "swagger" file.
* `utilities.c`
    * this is a supplementary program containing custom utility c functions
that help with reporting log messages mainly as of current.
