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

===============================================================================
==                What is Indoor Trajectory Scenario                         ==
===============================================================================

Here we prepare Indoor Trajectory Sample for Soft Sensor Manager.

There are 2 main components and 1 sub module to show how Indoor Trajectory works.

1. Reference Thing is Arduino application that shold be deployed at somewhere

   which should working as signal beacon.

2. Indoor Trajectory Sensor is the soft sensor for tizen platform that 
   
   checks the RSSI signal between reference thing and tizen platform using

   tizen BLE api.

3. Trackee_Thing is sub sensor which almost same behavior as Indoor Trajector Sensor
  
   which not only belongs to tizen platform. This also can check the RSSI for 

   checking distance and send its measurements to other device using iotivity protocol.

   So you can also get the measurement without using tizen BLE api.