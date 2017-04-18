/* *****************************************************************
 *
 * Copyright 2017 Microsoft
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

#ifndef IPCATESTDATA_H_
#define IPCATESTDATA_H_

static char* ELEVATOR_PLATFORM_ID = "2386034c-8099-11e6-ae22-56b6b6499611";
static char* ELEVATOR_PLATFORM_INDEPENDENT_ID = "be182fc4-f336-11e6-bc64-92361f002671";
static char* ELEVATOR_DATA_MODEL_VERSION_1 = "x.org.iotivity.test.1.0.0";
static char* ELEVATOR_DATA_MODEL_VERSION_2 = "x.org.iotivity.test.2.0.0";
static char* ELEVATOR_DATA_MODEL_VERSION_3 = "x.org.iotivity.test.3.0.0";

static char* ELEVATOR_RESOURCE_PATH = "/ipca/test/elevator";
static char* ELEVATOR_RESOURCE_TYPE = "x.org.iotivity.test.elevator";
static char* ELEVATOR_PROPERTY_CURRENT_FLOOR = "x.org.iotivity.CurrentFloor";
static char* ELEVATOR_PROPERTY_TARGET_FLOOR = "x.org.iotivity.TargetFloor";
static char* ELEVATOR_PROPERTY_DIRECTION = "x.org.iotivity.Direction";

static char* ELEVATOR_RESOURCE_MADE_UP_INTERFACE = "non.existence.if";

static char* ELEVATOR_CO_RESOURCE_PATH = "/ipca/test/carbonmonoxide";
static char* ELEVATOR_CO_RESOURCE_TYPE = "oic.r.sensor.carbonmonoxide";
static char* ELEVATOR_CO_PRIVATE_INTERFACE = "my.private.if";

static char* ELEVATOR_RESOURCE_CREATE_RELATIVE_PATH = "/ipca/new/resource/relative";
static char* ELEVATOR_RESOURCE_CREATE_RELATIVE_PATH_TYPE = "x.ipca.test.elevator.create.resource.relative";
static char* ELEVATOR_RESOURCE_NEW_RESOURCE_PATH = "/ipca/new/resource/relative/1";

static char* ELEVATOR_RESOURCE_CREATE_EXPLICIT_PATH = "/ipca/new/resource/explicit";
static char* ELEVATOR_RESOURCE_CREATE_EXPLICIT_PATH_TYPE = "x.ipca.test.elevator.create.resource.explicit";

static char* ELEVATOR_RESOURCE_DELETE_PATH = "/ipca/resource/delete";
static char* ELEVATOR_RESOURCE_DELETE_TYPE = "x.ipca.test.elevator.delete";

static char* ELEVATOR_KEYWORD = "elevator";
#endif // IPCATESTDATA_H_
