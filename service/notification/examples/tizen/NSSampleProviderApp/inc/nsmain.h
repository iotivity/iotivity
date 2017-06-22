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

#ifndef NSMAIN_H__
#define NSMAIN_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <EWebKit.h>

#include "OCPlatform.h"
#include "OCApi.h"

#include <tizen.h>
#include <app_control.h>

using namespace OC;

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "nsserver"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.nssampleprovider"
#endif

#define ELM_DEMO_EDJ "opt/usr/apps/org.tizen.nssampleprovider/res/ui_controls.edj"

typedef struct appdata
{
    Evas_Object *win;
    Evas_Object *conform;
    Evas_Object *layout;
    Evas_Object *nf;
    Evas_Object *findButton;
    Evas_Object *logtext;
    Evas_Object *listview;
    Evas_Object *web_view;
} appdata_s;
static appdata_s ad = {0,};
static appdata_s *g_ad;

static app_control_h app_handle = NULL;

void serverCreateUI(void *data, Evas_Object *obj, void *event_info);

void load_url(char *url);

void __on_web_url_change(void *data, Evas_Object *obj, void *event_info);

void __on_web_url_load_error(void *data, Evas_Object *obj, void *event_info);

void __on_web_url_load_finished(void *data, Evas_Object *obj, void *event_info);

void __on_web_url_load_started(void *data, Evas_Object *obj, void *event_info);

void do_login();

#endif // NSMAIN_H__
