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

#include "enrollee_wifi.h"
#include <pthread.h>
#include "logger.h"
#include <iostream>

using namespace std;

static Evas_Object *log_entry = NULL;
static Evas_Object *list = NULL;
static Evas_Object *naviframe = NULL;

// Function to update the log in UI
void *updateGroupLog(void *data)
{
    string *log = (string *) data;
    *log = *log + "<br>";
    // Show the log
    elm_entry_entry_append(log_entry, (*log).c_str());
    elm_entry_cursor_end_set(log_entry);
    return NULL;
}

void EventCallbackInApp(ESResult esResult, EnrolleeState enrolleeState)

{
    string logmessage = "callback!!! in app";
    OIC_LOG(INFO,LOG_TAG,"callback!!! in app");

    if (esResult == ES_OK)
    {
        if (!g_OnBoardingSucceeded)
        {
            OIC_LOG(INFO,LOG_TAG,"Device is successfully OnBoarded");
            logmessage += "Device is successfully OnBoarded";
            g_OnBoardingSucceeded = true;
        }
        else if (g_OnBoardingSucceeded & enrolleeState == ES_ON_BOARDED_STATE)
        {
            OIC_LOG(INFO,LOG_TAG,"Device is successfully OnBoared with SoftAP");
            logmessage += "Device is successfully OnBoared with SoftAP";
            g_ProvisioningSucceeded = true;
        }

        else if (enrolleeState == ES_PROVISIONED_STATE)
        {
            OIC_LOG(INFO,LOG_TAG,"Device is provisioned");
            logmessage += "Device is provisioned";
            g_ProvisioningSucceeded = true;
        }

    }
    else if (esResult == ES_ERROR)
    {
        if (g_OnBoardingSucceeded)
        {
            OIC_LOG_V(ERROR,LOG_TAG,
                    "Failure in Provisioning. \
                                        Current Enrollee State: %d",
                    enrolleeState);
            logmessage += "Failure in Provisioning\n";
            g_OnBoardingSucceeded = false;

        }
        else if (g_ProvisioningSucceeded)
        {
            OIC_LOG_V(ERROR,LOG_TAG,
                    "Failure in connect to target network. \
                                        Current Enrollee State: %d",
                    enrolleeState);
            logmessage += "Failure in connect to target network\n";
            g_ProvisioningSucceeded = false;
        }
    }

    ecore_main_loop_thread_safe_call_sync((void * (*)(void *))updateGroupLog, &logmessage);
}

static void list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *it = (Elm_Object_Item *) event_info;
    elm_list_item_selected_set(it, EINA_FALSE);
}

void copy_files()
{
    dlog_print(DLOG_DEBUG, LOG_TAG, "#####text=%s", app_get_data_path());
    FILE *source, *target;
    char ch, *source_file, *target_file;
    source_file = "/opt/usr/apps/org.tizen.esenrolleesample/res/oic_svr_db_server.json";
    target_file = "/opt/usr/media/Images/oic_svr_db_server.json";
    source = fopen(source_file, "r");

    if (source == NULL)
    {
        dlog_print(DLOG_DEBUG, LOG_TAG, "source is null");
    }

    target = fopen(target_file, "w");

    if (target == NULL)
    {
        dlog_print(DLOG_DEBUG, LOG_TAG, "target is null");
        fclose(source);
    }

    while ((ch = fgetc(source)) != ';')
        fputc(ch, target);
    fputc(EOF, target);
    dlog_print(DLOG_DEBUG, LOG_TAG, "File copied successfully.\n");

    fclose(source);
    fclose(target);
}
static void init_easysetup(void *data, Evas_Object *obj, void *event_info)
{

    string logmessage = "Init Easysetup\n";
    copy_files();
    if (InitEasySetup(CT_ADAPTER_IP, ssid, passwd, EventCallbackInApp) == ES_ERROR)
    {
        OIC_LOG(ERROR,LOG_TAG,"OnBoarding Failed");
        logmessage = logmessage + "OnBoarding Failed. Connect to " + ssid + " pasword= " + passwd;
        ecore_main_loop_thread_safe_call_sync((void * (*)(void *))updateGroupLog, &logmessage);
        return;

    }
    g_isInitialized = true;
    OIC_LOG_V(ERROR,LOG_TAG, "OnBoarding succeeded. Successfully connected to ssid : %s",
            ssid);
    logmessage = logmessage + "OnBoarding succeeded. Successfully connected to ssid:" + ssid;
    ecore_main_loop_thread_safe_call_sync((void * (*)(void *))updateGroupLog, &logmessage);
}

void StartProvisioning(void *data, Evas_Object *obj, void *event_info)
{
    OIC_LOG(INFO,LOG_TAG,"StartProvisioning");
    string logmessage = "StartProvisioning\n";

    if (InitProvisioning() == ES_ERROR)
    {
        OIC_LOG(ERROR,LOG_TAG,"Init Provisioning Failed");
        logmessage += "Init Provisioning Failed\n";
        return;
    }

    pthread_t thread_handle;

    if (pthread_create(&thread_handle, NULL, listeningFunc, NULL))
    {
        OIC_LOG(ERROR,LOG_TAG,"Thread creation failed");

    }

    ecore_main_loop_thread_safe_call_sync((void * (*)(void *))updateGroupLog, &logmessage);
}
void *listeningFunc(void*)
{
    while (true)
    {
        OCStackResult result;

        result = OCProcess();

        if (result != OC_STACK_OK)
        {
            OIC_LOG(ERROR,LOG_TAG,"OCStack stop error");
        }

        // To minimize CPU utilization we may wish to do this with sleep

    }
    return NULL;
}
static Eina_Bool naviframe_pop_cb(void *data, Elm_Object_Item *it)
{

    if (NULL != log_entry)
    {
        evas_object_del(log_entry);
        log_entry = NULL;
    }
    if (NULL != list)
    {
        evas_object_del(list);
        list = NULL;
    }
    return EINA_TRUE;
}
void client_cb(void *data)
{
    Evas_Object *layout;
    Evas_Object *scroller;
    Evas_Object *nf = (Evas_Object *) data;
    Evas_Object *button1;
    Evas_Object *button2;
    Elm_Object_Item *nf_it;

    naviframe = nf;

    // Scroller
    scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

    // Layout
    layout = elm_layout_add(nf);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "group_layout");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_object_content_set(scroller, layout);

    // Init Easysetup Discovery Button
    button1 = elm_button_add(layout);
    elm_object_part_content_set(layout, "button1", button1);
    elm_object_text_set(button1, "Init Easysetup");
    evas_object_smart_callback_add(button1, "clicked", init_easysetup, NULL);

    // start Provisioning Discovery Button
    button2 = elm_button_add(layout);
    elm_object_part_content_set(layout, "button2", button2);
    elm_object_text_set(button2, "start Provisioning");
    evas_object_smart_callback_add(button2, "clicked", StartProvisioning, NULL);

    // List
    list = elm_list_add(layout);
    elm_list_mode_set(list, ELM_LIST_COMPRESS);
    evas_object_smart_callback_add(list, "selected", list_selected_cb, NULL);
    elm_object_part_content_set(layout, "list", list);
    elm_list_go(list);

    // log_entry - textarea for log
    log_entry = elm_entry_add(layout);
    elm_entry_scrollable_set(log_entry, EINA_TRUE);
    elm_entry_editable_set(log_entry, EINA_FALSE);
    elm_object_part_text_set(log_entry, "elm.guide",
            "Logs will be updated here!!!<br>Please connect to network :Easysetup before starting!!");
    evas_object_size_hint_weight_set(log_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(log_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_part_content_set(layout, "log", log_entry);
    nf_it = elm_naviframe_item_push(nf, "EasySetup-Enrollee", NULL, NULL, scroller, NULL);
    elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, NULL);
}

void StartEnrollee(void *data, Evas_Object *obj, void *event_info)
{
    client_cb(data);
}
