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

#include "ppmsampleapp.h"

#include <tizen.h>
#include <wifi.h>
#include <algorithm>
#include "string.h"
#include <time.h>
#include <pthread.h>

#include "PluginManager.h"
#include "OCPlatform.h"
#include "PluginManager.h"
#include "OCApi.h"

using namespace OC;
using namespace OIC;

class Fan
{
    public:

        bool m_state;
        int m_power;
        std::string m_name;

        Fan() : m_state(false), m_power(0), m_name("")
        {
        }
};

typedef struct appdata
{
    Evas_Object *win;
    Evas_Object *conform;
    Evas_Object *naviframe;
    Evas_Object *box;

    Evas_Object *log;

    Evas_Object *send_msg_button;
    Evas_Object *clear_log_button;
} appdata_s;

typedef struct threadContext
{
    appdata_s *ad;
    const char *log;
} threadContext_s;

threadContext_s m_ThreadContext;
bool isSendMessage = true;
const int SUCCESS_RESPONSE = 0;
std::shared_ptr<OCResource> curFanResource;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;
int count = 0;
Fan myfan;
char log_buffer[10000];
PluginManager *m_pm = NULL;
char temp_string[2000];
char buf[100];

void putFanRepresentation(std::shared_ptr<OCResource> resource);

void updateLog(appdata_s *ad, const char *newlog)
{
    const char *log_text = NULL;

    log_text = elm_entry_entry_get(ad->log);
    strcpy(log_buffer, log_text);
    strcat(log_buffer, newlog);
    elm_entry_entry_set(ad->log, log_buffer);
    elm_entry_cursor_end_set(ad->log);
}

void *updateCallbackLog(void *data)
{
    threadContext_s  *pThreadContext = (threadContext_s *)data;
    updateLog(pThreadContext->ad, pThreadContext->log);
    return NULL;
}


int observe_count()
{
    static int oc = 0;
    return ++oc;
}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep,
               const int &eCode, const int &sequenceNumber)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "OBSERVE RESULT:" << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        sprintf(buf, "OBSERVE RESULT: sequence number = %d<br>", sequenceNumber);
        strcpy(temp_string, buf);
        sprintf(buf, "state: = %d<br>", myfan.m_state);
        strcat(temp_string, buf);
        sprintf(buf, "power: = %d<br>", myfan.m_power);
        strcat(temp_string, buf);
        sprintf(buf, "name: = %s<br>", myfan.m_name.c_str());
        strcat(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);

        if (observe_count() > 30)
        {
            std::cout << "Cancelling Observe..." << std::endl;
            OCStackResult result = curFanResource->cancelObserve();

            std::cout << "Cancel result: " << result << std::endl;
            sleep(10);
            std::cout << "DONE" << std::endl;
            std::exit(0);
        }
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        sprintf(buf, "onObserve Response error:  = %d<br>", eCode);
        strcpy(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);
        std::exit(-1);
    }
}

void onPost2(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        std::cout << "POST request was successful" << std::endl;

        if (rep.hasAttribute("createduri"))
        {
            std::cout << "\tUri of the created resource: "
                      << rep.getValue<std::string>("createduri") << std::endl;
        }
        else
        {
            rep.getValue("state", myfan.m_state);
            rep.getValue("power", myfan.m_power);
            rep.getValue("name", myfan.m_name);

            std::cout << "\tstate: " << myfan.m_state << std::endl;
            std::cout << "\tpower: " << myfan.m_power << std::endl;
            std::cout << "\tname: " << myfan.m_name << std::endl;

            strcpy(temp_string, "POST2 request was successful<br>");
            sprintf(buf, "state: = %d<br>", myfan.m_state);
            strcat(temp_string, buf);
            sprintf(buf, "power: = %d<br>", myfan.m_power);
            strcat(temp_string, buf);
            sprintf(buf, "name: = %s<br>", myfan.m_name.c_str());
            strcat(temp_string, buf);
            m_ThreadContext.log = temp_string;
            dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                                  &m_ThreadContext);
        }

        if (OBSERVE_TYPE_TO_USE == ObserveType::Observe)
            std::cout << std::endl << "Observe is used." << std::endl;
        else if (OBSERVE_TYPE_TO_USE == ObserveType::ObserveAll)
            std::cout << std::endl << "ObserveAll is used." << std::endl;

        // curFanResource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);

    }
    else
    {
        std::cout << "onPost Response error: " << eCode << std::endl;
        sprintf(buf, "onPost Response error:  = %d<br>", eCode);
        strcpy(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);
        std::exit(-1);
    }
}

void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        std::cout << "POST request was successful" << std::endl;

        if (rep.hasAttribute("createduri"))
        {
            std::cout << "\tUri of the created resource: "
                      << rep.getValue<std::string>("createduri") << std::endl;
        }
        else
        {
            rep.getValue("state", myfan.m_state);
            rep.getValue("power", myfan.m_power);
            rep.getValue("name", myfan.m_name);

            std::cout << "\tstate: " << myfan.m_state << std::endl;
            std::cout << "\tpower: " << myfan.m_power << std::endl;
            std::cout << "\tname: " << myfan.m_name << std::endl;

            strcpy(temp_string, "POST request was successful<br>");
            sprintf(buf, "state: = %d<br>", myfan.m_state);
            strcat(temp_string, buf);
            sprintf(buf, "power: = %d<br>", myfan.m_power);
            strcat(temp_string, buf);
            sprintf(buf, "name: = %s<br>", myfan.m_name.c_str());
            strcat(temp_string, buf);
            m_ThreadContext.log = temp_string;
            dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                                  &m_ThreadContext);
        }

        OCRepresentation rep2;

        std::cout << "Posting fan representation..." << std::endl;

        myfan.m_state = true;
        myfan.m_power = 55;

        rep2.setValue("state", myfan.m_state);
        rep2.setValue("power", myfan.m_power);

        curFanResource->post(rep2, QueryParamsMap(), &onPost2);
    }
    else
    {
        std::cout << "onPost Response error: " << eCode << std::endl;
        sprintf(buf, "onPost Response error:  = %d<br>", eCode);
        strcpy(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void postFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Posting fan representation..." << std::endl;

        myfan.m_state = false;
        myfan.m_power = 105;

        rep.setValue("state", myfan.m_state);
        rep.setValue("power", myfan.m_power);

        // Invoke resource's post API with rep, query map and the callback parameter
        resource->post(rep, QueryParamsMap(), &onPost);
    }
}

// Callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "PUT request was successful" << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        strcpy(temp_string, "PUT request was successful<br>");
        sprintf(buf, "state: = %d<br>", myfan.m_state);
        strcat(temp_string, buf);
        sprintf(buf, "power: = %d<br>", myfan.m_power);
        strcat(temp_string, buf);
        sprintf(buf, "name: = %s<br>", myfan.m_name.c_str());
        strcat(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);

        putFanRepresentation(curFanResource);
    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        sprintf(buf, "onPut Response error:  = %d<br>", eCode);
        strcpy(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Putting fan representation..." << std::endl;

        myfan.m_state = true;
        if (myfan.m_power == 1)
            myfan.m_power = 0;
        else
            myfan.m_power = 1;
        sleep(5);
        rep.setValue("state", myfan.m_state);
        rep.setValue("power", myfan.m_power);

        // Invoke resource's put API with rep, query map and the callback parameter
        resource->put(rep, QueryParamsMap(), &onPut);
    }
}

// Callback handler on GET request
void onFanGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "GET Fan request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        rep.getValue("state", myfan.m_state);
        rep.getValue("power", myfan.m_power);
        rep.getValue("name", myfan.m_name);

        std::cout << "\tstate: " << myfan.m_state << std::endl;
        std::cout << "\tpower: " << myfan.m_power << std::endl;
        std::cout << "\tname: " << myfan.m_name << std::endl;

        strcpy(temp_string, "GET Fan request was successful<br>");
        sprintf(buf, "state: = %d<br>", myfan.m_state);
        strcat(temp_string, buf);
        sprintf(buf, "power: = %d<br>", myfan.m_power);
        strcat(temp_string, buf);
        sprintf(buf, "name: = %s<br>", myfan.m_name.c_str());
        strcat(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);

        putFanRepresentation(curFanResource);
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        sprintf(buf, "onGET Response error:  = %d<br>", eCode);
        strcpy(temp_string, buf);
        m_ThreadContext.log = temp_string;
        dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                              &m_ThreadContext);
        std::exit(-1);
    }
}


// Local function to get representation of fan resource
void getFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        std::cout << "Getting Fan Representation..." << std::endl;

        // Invoke resource's get API with the callback parameter
        QueryParamsMap test;
        resource->get(test, &onFanGet);
    }
}

// Callback to found resources
void foundResourceFan(std::shared_ptr<OCResource> resource)
{
    if (curFanResource)
    {
        std::cout << "Found another resource, ignoring" << std::endl;
    }

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            strcpy(temp_string, "DISCOVERED Resource:");

            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;
            sprintf(buf, "URI of the resource: = %s<br>", resourceURI.c_str());
            strcat(temp_string, buf);

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
            sprintf(buf, "Host address of the resource: = %s<br>", hostAddress.c_str());
            strcat(temp_string, buf);

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            strcat(temp_string, "List of resource types: <br>");
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
                sprintf(buf, "%s<br>", resourceTypes.c_str());
                strcat(temp_string, buf);
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
                sprintf(buf, "%s<br>", resourceInterfaces.c_str());
                strcat(temp_string, buf);
            }

            if (resourceURI == "/a/fan")
            {
                curFanResource = resource;
                /* Call a local function which will internally invoke get API
                   on the resource pointer */
                putFanRepresentation(curFanResource);
            }
            m_ThreadContext.log = temp_string;
            dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                                  &m_ThreadContext);
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
            strcpy(temp_string, "Resource is invalid");
            m_ThreadContext.log = temp_string;
            dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                                  &m_ThreadContext);
        }

    }
    catch (std::exception &e)
    {
        // log(e.what());
    }
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : simpleclient <ObserveType>" << std::endl;
    std::cout << "   ObserveType : 1 - Observe" << std::endl;
    std::cout << "   ObserveType : 2 - ObserveAll" << std::endl;
}

static void
win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
    ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
    ui_app_exit();
}

static void
send_msg_clicked_cb(void *data , Evas_Object *obj , void *event_info)
{
    appdata_s *ad = (appdata_s *)data;
    if (isSendMessage == true)
    {
        try
        {
            m_pm = new PluginManager();

            // Get Plugins
            std::vector<Plugin> user_plugin;

            user_plugin = m_pm->getPlugins();

            for (unsigned int i = 0; i < user_plugin.size(); i++)
            {
                strcpy(temp_string, "Calling Get Plugin<br>");
                sprintf(buf, "value Name = %s<br>", user_plugin[i].getName().c_str());
                strcat(temp_string, buf);
                sprintf(buf, "value ID = %s<br>", user_plugin[i].getID().c_str());
                strcat(temp_string, buf);
                m_ThreadContext.log = temp_string;
                dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
                ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                                      &m_ThreadContext);
                std::cout << "value Name = " << user_plugin[i].getName() << std::endl;
                std::cout << "value ID = " << user_plugin[i].getID() << std::endl;
            }


            std::cout << "start fan Plugin by Resource Type"  << std::endl;
            strcpy(temp_string, "start fan Plugin<br>");
            strcat(temp_string, "fan Plugin is getting started. Please wait...<br>");
            m_ThreadContext.log = temp_string;
            dlog_print(DLOG_INFO, LOG_TAG, " %s", m_ThreadContext.log);
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateCallbackLog,
                                                  &m_ThreadContext);
            m_pm->startPlugins("ResourceType", "oic.fan");
            sleep(2);
            // Makes it so that all boolean values are printed as 'true/false' in this stream
            std::cout.setf(std::ios::boolalpha);
            std::cout << "Before find Resource... " << std::endl;

            // Find fan resources
            std::ostringstream requestURI;
            requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=core.fan";
            OCPlatform::findResource("", requestURI.str(), OC_ALL, &foundResourceFan);
            std::cout << "Finding Resource... " << std::endl;
        }
        catch (OCException &e)
        {
            // log(e.what());
        }

        elm_object_text_set(ad->send_msg_button, "Press here to close the App");
        isSendMessage = false;
    }
    else
    {
        isSendMessage = true;
        ui_app_exit();
    }
}

static void
clear_log_clicked_cb(void *data , Evas_Object *obj , void *event_info)
{
    appdata_s *ad = (appdata_s *)data;
    elm_entry_entry_set(ad->log, "");
}

static void
create_base_gui(appdata_s *ad)
{
    // Window
    ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
    elm_win_autodel_set(ad->win, EINA_TRUE);

    if (elm_win_wm_rotation_supported_get(ad->win))
    {
        int rots[4] = { 0, 90, 180, 270 };
        elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
    }

    evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
    eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

    // Conformant
    ad->conform = elm_conformant_add(ad->win);
    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);

    // naviframe
    ad->naviframe = elm_naviframe_add(ad->conform);
    elm_object_content_set(ad->conform, ad->naviframe);
    evas_object_show(ad->naviframe);

    // Box container
    ad->box = elm_box_add(ad->naviframe);
    elm_box_horizontal_set(ad->box, EINA_FALSE);
    evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(ad->box, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_padding_set(ad->box, 0, 15 * elm_config_scale_get());
    elm_naviframe_item_push(ad->naviframe, "ProtocolPlugin App", NULL, NULL, ad->box, NULL);
    evas_object_show(ad->box);

    ad->send_msg_button = elm_button_add(ad->box);
    evas_object_size_hint_align_set(ad->send_msg_button, EVAS_HINT_FILL, 0.0);
    elm_object_text_set(ad->send_msg_button, "Invoke Fan Plugin");
    evas_object_smart_callback_add(ad->send_msg_button, "clicked", send_msg_clicked_cb, ad);
    elm_box_pack_end(ad->box, ad->send_msg_button);
    evas_object_show(ad->send_msg_button);

    ad->clear_log_button = elm_button_add(ad->box);
    evas_object_size_hint_align_set(ad->clear_log_button, EVAS_HINT_FILL, 0.0);
    elm_object_text_set(ad->clear_log_button, "Clear Log");
    evas_object_smart_callback_add(ad->clear_log_button, "clicked", clear_log_clicked_cb, ad);
    elm_box_pack_end(ad->box, ad->clear_log_button);
    evas_object_show(ad->clear_log_button);

    ad->log = elm_entry_add(ad->box);
    elm_object_part_text_set(ad->log, "elm.guide", "Log messages will be update here!!!");
    elm_entry_scrollable_set(ad->log, EINA_TRUE);
    evas_object_size_hint_weight_set(ad->log, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(ad->log, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(ad->box, ad->log);
    evas_object_show(ad->log);

    // Show window after base gui is set up
    evas_object_show(ad->win);

    std::string name;
    std::string key = "Name";
    std::string  state = "";
    std::string  id = "";

    // Create PlatformConfig object
    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);
}

static bool
app_create(void *data)
{
    /* Hook to take necessary actions before main event loop starts
        Initialize UI resources and application's data
        If this function returns true, the main loop of application starts
        If this function returns false, the application is terminated */
    appdata_s *ad = (appdata_s *)data;
    m_ThreadContext.ad = ad;
    m_ThreadContext.log = NULL;
    create_base_gui(ad);

    return true;
}

static void
app_control(app_control_h app_control, void *data)
{
    // Handle the launch request.
    dlog_print(DLOG_INFO, LOG_TAG, "#### in app_control");
}

static void
app_pause(void *data)
{
    // Take necessary actions when application becomes invisible.
    dlog_print(DLOG_INFO, LOG_TAG, "#### in app_pause");
}

static void
app_resume(void *data)
{
    // Take necessary actions when application becomes visible.
    dlog_print(DLOG_INFO, LOG_TAG, "#### in app_resume");
}

static void
app_terminate(void *data)
{
    // Release all resources.
    dlog_print(DLOG_INFO, LOG_TAG, "#### in app_terminate");

}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
    // APP_EVENT_LANGUAGE_CHANGED
    char *locale = NULL;
    system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
    elm_language_set(locale);
    free(locale);
    return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
    // APP_EVENT_DEVICE_ORIENTATION_CHANGED
    dlog_print(DLOG_INFO, LOG_TAG, "#### app orient changed");
    return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
    // APP_EVENT_REGION_FORMAT_CHANGED
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
    // APP_EVENT_LOW_BATTERY
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
    // APP_EVENT_LOW_MEMORY
}

int
main(int argc, char *argv[])
{
    appdata_s ad = {0,};
    int ret = 0;

    ui_app_lifecycle_callback_s event_callback = {0,};
    app_event_handler_h handlers[5] = {NULL, };

    event_callback.create = app_create;
    event_callback.terminate = app_terminate;
    event_callback.pause = app_pause;
    event_callback.resume = app_resume;
    event_callback.app_control = app_control;

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY,
                             ui_app_low_battery, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY,
                             ui_app_low_memory, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
                             APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED,
                             ui_app_lang_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
                             APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
    ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

    ret = ui_app_main(argc, argv, &event_callback, &ad);

    if (ret != APP_ERROR_NONE)
    {
        dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
    }

    return ret;
}
