Fire Alarm Softsensor
======
###### Team 22

This Softsensor is a part of disaster detection system based on smartphone's sensors and crowd sourcing.

In IoT environment, with various sensor functionality of various 'things', the fire alarm softsensor automatically detects fire, and people can quickly response to the situation.

We make use of some basic sensors to predict fire.

Light, humidity, temperature sensor is used to observe change of ambient environment. 

Accelerometer is used to make Running softsensor, which represents grasp evacuation of nearby people, to make sure that fire occured.

In addition to android bundle, we provide linux bundle which operates same way except not having accelerometer since we defined static sensor model for linux environment.


Components
------------------------

#### Light Sensor
Measures the ambient light level (illumination) in lx.

#### Humidity Sensor
Measures the relative ambient humidity in percent (%).

#### Temperature Sensor
Measures the ambient room temperature in degrees Celsius (°C)

#### Accelerometer
Measures the acceleration force in m/s2 that is applied to a device on all three physical axes (x, y, and z), including the force of gravity.

#### Running Sensor
Softsensor composed with accelerometer. It distinguishes people are running or not based on magnitude of vector returned by accelerometer.

##### THL Sensor
This is just for linux implementation and measures Temperature, Humidity, Light resources.


Output
------------------------
##### The softsensor returns an index representing probability of fire. More probable of fire for bigger index.

#### FIRE_0
Temperature is lower than certain threshold that can be perceived as fire

#### FIRE_1
Temperature is higher than Base threshold. 

#### FIRE_2
Temperature sensor or other sensors indicate hazard of fire

#### FIRE_3
Temperature sensor or other sensors indicate hazard of fire in high probability

#### FIRE_4
Fire level based on sensor is FIRE_3 and nearby people are running away.
