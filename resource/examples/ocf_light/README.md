# Introduction
This contains sample implementation of an OCF smart light using resources
defined by the OCF smart home project.

The code was originally generated using the
[OCF DeviceBuilder](https://github.com/openconnectivityfoundation/DeviceBuilder)
project.

The output from DeviceBuilder is a single giant C++ file. That output was
separated into individual classes.

The code has several goals:
    - provide a reference implementation showing how implement a C++ IoTivity
      server.
    - Try and demonstrate using IoTivity in a way that is reusable for other
      developers

# What this sample code contains
- `dimming_light_server.cpp`: contains the main function for the code.
- `Platform.h` and `Platform.cpp`: code responsible for starting the
  IoTivity platform and registering platform information like the date of
  manufacture, platform ID, etc. This code is also responsible registering
  device information like device type and device name.
- `Resource.h`: base class for all OCF resources.
- `BinarySwitchResource.h` and `BinarySwitchResource.cpp`: an
  implementation of the `oic.r.switch.binary` resource type.
- `DimmingResource.h` and `DimmingResource.cpp`: an implementation
  of the `oic.r.light.dimming` resource type.
- `DimmingLightServer.h` and `DimmingLightServer.cpp`: combine
  BinarySwitchResource and the DimmingResource and make them available as a
  single object.
- `DimmingLightControl.h` and `DimmingLightControl.cpp` adds commandline
  control of the dimming light sample when running. This is useful for testing
  notifications or verifying operation of the device.
- `dl_server_introspection.json`: introspect file representing the dimming
  light. This file is primarily a reference file provided so the developer knows
  the contents of the `dl_server_introspection.dat` file.
- `dl_server_introspection.dat`: contains the same information as
  `dl_server_introspection.json` but it is [cbor](http://cbor.io) encoded.
- `dl_server_security.json.in` This a reference file that is the same as
  `dl_server_security.json` except is it annotated with comments describing the
  different parts of the security file.
- `dl_server_security.json`: reference file to see the contents of
  `dl_server_security.dat` This contains the initial security settings used
  by the device at startup. This is file is specifically designed for to place
  the device in the Ready for Ownership Transfer Method (RFOTM) state.
- `dl_server_security.dat`: [cbor](http://cbor.io) encoded version of the
  `dl_server_security.json` file. This must be encoded using the
  `json2cbor` tool provided with IoTivity.
- `dl_PICS.json`: This file contains the settings needed to run the dimming
  light sample on the OCF Conformance Test Tool (CTT)
- `SConscript`: build script used to build code using
  [SCons](http://scons.org/) build tool
- `readme.md`: this readme file.

# The OCF Conformance Test Tool (CTT)
## Obtaining the CTT

The OCF Conformance Test Tool or CTT is the primary tool used to certify that a
server or client is conforms to the OCF specifications. The CTT tool is only
available to members of the Open Connectivity Foundation(OCF).

There are many membership levels for individuals wishing to join the
Open Connectivity Foundation OCF. Go [here](https://openconnectivity.org/foundation/join)
for information on joining.

The CTT is available to all OCF members. Even to the members at a Basic Membership
level which if free and has no annual dues. At the Basic Membership level the CTT
can only be used for pre-testing purposes. It can not be used to certify devices.

Once OFC membership has been obtained the CTT can be downloaded from the
[OCF Test Tool Resources page](https://workspace.openconnectivity.org/kws/test_tools/).

The dimming light server was tested against CTT2.1 that was released January 2018.

## PICS file

The dimming light server comes with a `PICS.json` file. This file contains
the information the CTT needs to test the server. Please, select this file when
asked to select a PICS file by the CTT. Do not use the default PICS file.

IoTivity implements security virtual resources that are currently optional in
the OCF specification. Currently there is no mechanism available to opt out of
these resources. The `oic.r.crl`, `oic.r.csr`, and `oic.r.roles`
resource need to be declared in the PICS file.

## CTT info

When CTT pops up:
"reset to ready for OTM" of "reset to RFOTM" means you need to:
1. Stop your device
2. Reset/replace security databases with a new/unowned one.
    e.g. copy the ORIGINAL security file to the executable directory.
3. Start your device.

Note if the device being tested crashes during testing you will also need to
reset the security state to a not onboarded state. This is mentioned in the test
case log of CTT when the CTT can not reset the device state properly.

## Running the `dimming_light_server` against the CTT

All tests should pass or be skipped with the following four exceptions.

1. `CT1.1.5 Authority of an OCF URI` will fail with a warning stating
the anchor properties values are the same before and after onboarding. This
warning is due to Section 13.12.1 of the OCF Security Specification v1.3.1.
Before a the device is onboarded it **should** present a temporary non-repeated
values for device id. This is not required for certification. Currently IoTivity
does not support this security/privacy feature.

2. `CT1.7.3.2 Device Identity (persistent and semi-persistent)`
This warning is a result of the same fake device id privacy feature as `CT1.1.5`
It is not required for certification.

3. `CT1.7.8.11 Implicit Behavior of SVR Properties in Each Device State`
is currently failing due to a specification change so this can be unchecked or
ignored.

4. `CT1.7.12.5 Verify RFOTM behavior when OTM sequence does not complete`
will fail with a warning stating the connection should be dropped after 65
seconds. This warning is due to a optional requirement in section 13.7 of the
OCF Security Specification v1.3.1 that states an ownership transfer session
**should** not exceed 60 seconds. The OTM should fail, be disconnected, and
transition to RESET. Once again IoTivity currently does not support this
optional requirement and it is not required for certification.
