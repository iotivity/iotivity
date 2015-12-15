Indoor Activity Sensor Bundle
-----------------------------

Indoor Activity Sensor Bundle detects a person's indoor location based on the
information gathered from a WirelessSensor and a MotionSensor.

It then generates WalkingSessions which include information about from where to
where the person has moved including the start and stop time.

Based on the WalkingSession information, the Indoor Activity Sensor Bundle can
immediately calculate the EnergyConsumption of each WalkingSession.

EnergyConsumption is calculated based on the following formula:

    Calorie burn = (BMR/24) x MET x T

Furthermore, it packs the generated indoor activity information (WalkingSession and
EnergyConsumption) to a JSON object and then sends it to a server.

We believe accumulation of such indoor activity information on the server have great
application potentials such as: visualizing daily routine, reporting health status,
etc.

How to Use
----------

Run wirelesse and motion sensors:

  % {Iotivit_Root}/out/linux/x86_64/release/service/resource-container/MotionSensorApp
  % {Iotivit_Root}/out/linux/x86_64/release/service/resource-container/WirelessSensorApp

Run IAContainer:

  % {Iotivit_Root}/out/linux/x86_64/release/service/resource-container/IAContainer

The web server is then turned on, and can be accessed through the address 
http://localhost:5000/


Assumptions
-----------

In this example, our focus is to show how information gathered from indoor location
detection can be used in Iotivity and as a result, we simply assume that PinPlace
(or any equivalent which could detect indoor locations) could provide an
individual's WalkingSession information described above.

The indoor location detection in this example imitates behaviors described in the
following paper:

    * PinPlace: associate semantic meanings with indoor locations without active
    fingerprintinge (UbiComp '15)
