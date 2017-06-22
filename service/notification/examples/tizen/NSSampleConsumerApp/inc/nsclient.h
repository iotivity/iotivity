/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "nsmain.h"

#include "RCSResourceObject.h"

// ns
#include "NSCommon.h"
#include "NSConsumerService.h"
#include "NSUtils.h"
#include "NSTopicsList.h"

#ifndef NSCLIENT_H__
#define NSCLIENT_H__

using namespace std;
using namespace OIC::Service;

typedef void(*ClientMenuHandler)();
typedef int ReturnValue;

constexpr int DEFAULT_VALUE = 0;

constexpr int PRESENCE_ON = 1;
constexpr int PRESENCE_OFF = 2;

const std::string TEMPERATURE_URI = "/a/TempSensor";
const std::string LIGHT_URI = "/a/light";
const std::string TEMPERATURE_RT = "oic.r.temperaturesensor";
const std::string LIGHT_RT = "oic.r.light";
const std::string TEMPERATURE_AK = "Temperature";
const std::string LIGHT_AK = "Brightness";
const std::string RESOURCE_INTERFACE = "oic.if.";

const char *gauthCode;

enum class Control
{
    INCREASE,
    DECREASE
};

void printAttribute(const RCSResourceAttributes &attrs);

void start_consumer(void *data, Evas_Object *obj, void *event_info);

void stop_consumer(void *data, Evas_Object *obj, void *event_info);

static void signUp(void *data, Evas_Object *obj, void *event_info);

static void signIn(void *data, Evas_Object *obj, void *event_info);

static void signOut(void *data, Evas_Object *obj, void *event_info);

static void enableRemoteService(void *data, Evas_Object *obj, void *event_info);

static void subscribeMQ(void *data, Evas_Object *obj, void *event_info);

static void getTopic(void *data, Evas_Object *obj, void *event_info);

static void updateTopic(void *data, Evas_Object *obj, void *event_info);

void *showAPIs(void *data);

#endif // NSCLIENT_H__
