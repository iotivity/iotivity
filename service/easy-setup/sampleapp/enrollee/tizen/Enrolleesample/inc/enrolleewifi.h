/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RECLIENTMAIN_H__
#define RECLIENTMAIN_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include "OCPlatform.h"
#include "OCApi.h"

#include "easysetup.h"
#include "wifi.h"
#include "escommon.h"
using namespace OC;

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "TIZEN ES"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.esenrolleesample"
#endif

#define ELM_DEMO_EDJ "opt/usr/apps/org.tizen.esenrolleesample/res/ui_controls.edj"

void StartEnrollee(void *data, Evas_Object *obj, void *event_info);
void *listeningFunc(void*);
static char ssid[] = "EasySetup123";
static char passwd[] = "EasySetup123";
static bool g_OnBoardingSucceeded = false;
static bool g_ProvisioningSucceeded = false;

static bool g_isInitialized = false;
#endif // RECLIENTMAIN_H__
