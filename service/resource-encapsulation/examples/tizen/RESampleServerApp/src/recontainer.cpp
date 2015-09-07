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

#include "recontainer.h"

#include "remain.h"
#include "PrimitiveResource.h"
#include "RCSResourceContainer.h"

#include <string>

using namespace std;
using namespace OC;
using namespace OIC::Service;

# define checkContainer NULL!=container?true:false

RCSResourceContainer *container;
static bool s_containerFlag = false;
static bool s_hueBundleFlag = false;

static Evas_Object *log_entry = NULL;
static Evas_Object *listnew = NULL;
static Evas_Object *naviframe = NULL;

// Function to update the log in UI
void *updateContainerLog(void *data)
{
    string *log = (string *)data;
    // Show the log
    elm_entry_entry_append(log_entry, (*log).c_str());
    elm_entry_cursor_end_set(log_entry);
    return NULL;
}

static void list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *it = (Elm_Object_Item *)event_info;
    elm_list_item_selected_set(it, EINA_FALSE);
}

static void onDestroy()
{
    //stopContainer(NULL, NULL, NULL);
}

static void listBundles(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### listBundles starting");
    string logMessage = "";

    if (checkContainer)
    {
        std::list<RCSBundleInfo *> bundles = container->listBundles();
        std::list<RCSBundleInfo *>::iterator bundleIt;
        logMessage += "Bundle List Size : " + to_string(bundles.size()) + "<br>";

        for (bundleIt = bundles.begin(); bundleIt != bundles.end(); bundleIt++)
        {
            string bundleString((*bundleIt)->getID().c_str());
            logMessage += "Bundle ID : " + bundleString + "<br>";
        }
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### listBundles exit");
}

static void listHueResources(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### listHueResources starting");
    string logMessage = "";

    if (checkContainer)
    {
        std::list<string> resources = container->listBundleResources("oic.bundle.hueSample");
        std::list<string>::iterator resourceIt;
        logMessage += "Resource Bundle Size : " + to_string(resources.size()) + "<br>";
        for (resourceIt = resources.begin(); resourceIt != resources.end(); resourceIt++)
        {
            string resourceString((*resourceIt).c_str());
            logMessage += "Resource URI : " +  resourceString + "<br>";
        }
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### listHueResources exit");
}

static void listDiscomfortResources(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### listDiscomfortResources starting");
    string logMessage = "";

    if (checkContainer)
    {
        std::list<string> resources = container->listBundleResources("oic.bundle.discomfortIndexSensor");
        std::list<string>::iterator resourceIt;
        logMessage += "Resource Bundle Size : " + to_string(resources.size()) + "<br>";
        for (resourceIt = resources.begin(); resourceIt != resources.end(); resourceIt++)
        {
            string resourceString((*resourceIt).c_str());
            logMessage += "Resource URI : " +  resourceString + "<br>";
        }
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### listDiscomfortResources exit");
}

static void addResourceConfig(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### addResourceConfig starting");
    string logMessage = "";

    if (checkContainer)
    {
        std::map<string, string> resourceParams;
        resourceParams["resourceType"] = "oic.light.control";
        resourceParams["address"] = "http://192.168.0.2/api/newdeveloper/lights/1";
        if (s_hueBundleFlag)
        {
            container->addResourceConfig("oic.bundle.hueSample", "", resourceParams);
            logMessage += "Resource added<br>";
            listHueResources(NULL, NULL, NULL);
        }
        else
        {
            logMessage += "BUNDLE NOT FOUND<br>";
        }

    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### addResourceConfig exit");
}

static void removeResourceConfig(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### removeResourceConfig starting");
    string logMessage = "";

    if (checkContainer)
    {
        std::list<string> resources = container->listBundleResources("oic.bundle.hueSample");
        std::list<string>::iterator resourceIt;
        if (!resources.size())
        {
            logMessage += "No Resource to remove <br>";
        }
        else
        {
            resourceIt = resources.begin();
            string resourceString((*resourceIt).c_str());
            if (s_hueBundleFlag)
            {
                container->removeResourceConfig("oic.bundle.hueSample", resourceString);
                logMessage += "Resource added<br>";
                listHueResources(NULL, NULL, NULL);
            }
            else
            {
                logMessage += "BUNDLE NOT FOUND<br>";
            }
        }
        logMessage += "Resource removed <br>";
        listHueResources(NULL, NULL, NULL);
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### removeResourceConfig exit");
}

static void addBundle(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### addBundle starting");

    string logMessage = "";

    if (checkContainer)
    {
        std::map<string, string> bundleParams;
        container->addBundle("oic.bundle.hueSample", "",
                             "/opt/usr/apps/org.tizen.resampleserver/lib/libHueBundle.so", bundleParams);
        logMessage += "Hue Bundle added <br>";
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### addBundle exit");
}

static void removeBundle(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### removeBundle starting");
    string logMessage = "";

    if (checkContainer)
    {
        container->removeBundle("oic.bundle.hueSample");
        logMessage += "Hue Bundle removed <br>";
        s_hueBundleFlag = false;
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### removeBundle exit");
}

static void startBundle(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### startBundle starting");
    string logMessage = "";

    if (checkContainer)
    {
        container->startBundle("oic.bundle.hueSample");
        logMessage += "Hue Bundle started <br>";
        s_hueBundleFlag = true;
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### startBundle exit");
}

static void stopBundle(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### stopBundle starting");
    string logMessage = "";

    if (checkContainer)
    {
        container->stopBundle("oic.bundle.hueSample");
        logMessage += "HueBundle stopped <br>";
        s_hueBundleFlag = false;
    }
    else
    {
        logMessage = "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    logMessage += "----------------------<br>";
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);

    dlog_print(DLOG_INFO, LOG_TAG, "#### stopBundle exit");
}

void *showContainerAPIs(void *data)
{
    // Add items to the list only if the list is empty
    const Eina_List *eina_list = elm_list_items_get(listnew);
    int count = eina_list_count(eina_list);
    if (!count)
    {
        elm_list_item_append(listnew, "1. List Bundles", NULL, NULL,
                             listBundles, NULL);

        elm_list_item_append(listnew, "2. List Hue resources", NULL, NULL,
                             listHueResources, NULL);

        elm_list_item_append(listnew, "3. Add Resource Config", NULL, NULL,
                             addResourceConfig, NULL);

        elm_list_item_append(listnew, "4. Remove Resource Config", NULL, NULL,
                             removeResourceConfig, NULL);

        elm_list_item_append(listnew, "5. Remove Bundle", NULL, NULL,
                             removeBundle, NULL);

        elm_list_item_append(listnew, "6. Add Bundle", NULL, NULL,
                             addBundle, NULL);

        elm_list_item_append(listnew, "7. Start Bundle", NULL, NULL,
                             startBundle, NULL);

        elm_list_item_append(listnew, "8. Stop Bundle", NULL, NULL,
                             stopBundle, NULL);

        elm_list_go(listnew);
    }
    return NULL;
}

// Method to be called when the start container UI Button is selected
static void startContainer(void *data, Evas_Object *obj, void *event_info)
{
    std::string xmlDescription =
        "/opt/usr/apps/org.tizen.resampleserver/lib/ResourceContainerConfig.xml";
    string logMessage = "";

    if (NULL != listnew)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Container starting");
        container = RCSResourceContainer::getInstance();

        if (!s_containerFlag)
        {
            container->startContainer(xmlDescription);
            s_containerFlag = true;
            s_hueBundleFlag = true;
            logMessage += "CONTAINER STARTED<br>";
        }
        else
        {
            logMessage += "ALREADY STARTED<br>";
        }
    }
    else
    {
        dlog_print(DLOG_ERROR, LOG_TAG, "list is NULL - So unable to add items!!!");
    }

    // Show the UI list of group APIs
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))showContainerAPIs, NULL);

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);
}

// Method to be called when the stop container UI Button is selected
static void stopContainer(void *data, Evas_Object *obj, void *event_info)
{
    string logMessage = "";

    dlog_print(DLOG_INFO, LOG_TAG, "#### Container stopped");

    if (checkContainer)
    {
        removeBundle(NULL, NULL, NULL);
        stopBundle(NULL, NULL, NULL);

        container->stopContainer();
        logMessage += "CONTAINER STOPPED<br>";
        container = NULL;
    }
    else
    {
        logMessage += "NO CONTAINER <br>";
    }

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void * ( *)(void *))updateContainerLog,
                                          &logMessage);
}

static Eina_Bool
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
    onDestroy();

    if (NULL != log_entry)
    {
        evas_object_del(log_entry);
        log_entry = NULL;
    }
    if (NULL != listnew)
    {
        evas_object_del(listnew);
        listnew = NULL;
    }
    return EINA_TRUE;
}

// Method to set up server screens
void containerCreateUI(void *data, Evas_Object *obj, void *event_info)
{
    s_containerFlag = false;
    s_hueBundleFlag = false;
    Evas_Object *layout;
    Evas_Object *scroller;
    Evas_Object *nf = (Evas_Object *)data;
    Evas_Object *start_button;
    Evas_Object *stop_button;
    Elm_Object_Item *nf_it;
    naviframe = nf;

    // Scroller
    scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

    // Layout
    layout = elm_layout_add(nf);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "container_layout");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_object_content_set(scroller, layout);

    // Button
    start_button = elm_button_add(layout);
    elm_object_part_content_set(layout, "start_button", start_button);
    elm_object_text_set(start_button, "Start Container");
    evas_object_smart_callback_add(start_button, "clicked", startContainer, NULL);

    // Button
    stop_button = elm_button_add(layout);
    elm_object_part_content_set(layout, "stop_button", stop_button);
    elm_object_text_set(stop_button, "Stop Container");
    evas_object_smart_callback_add(stop_button, "clicked", stopContainer, NULL);

    // List
    listnew = elm_list_add(layout);
    elm_list_mode_set(listnew, ELM_LIST_COMPRESS);
    evas_object_smart_callback_add(listnew, "selected", list_selected_cb, NULL);
    elm_object_part_content_set(layout, "listnew", listnew);
    elm_list_go(listnew);

    // log_entry - text area for log
    log_entry = elm_entry_add(layout);
    elm_entry_scrollable_set(log_entry, EINA_TRUE);
    elm_entry_editable_set(log_entry, EINA_FALSE);
    elm_object_part_text_set(log_entry, "elm.guide", "Logs will be updated here!!!");
    evas_object_size_hint_weight_set(log_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(log_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_part_content_set(layout, "log", log_entry);

    nf_it = elm_naviframe_item_push(nf, "Resource Container", NULL, NULL, scroller, NULL);
    elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, NULL);
}
