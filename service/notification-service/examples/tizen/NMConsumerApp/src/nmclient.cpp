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

#include "nmclient.h"

#include "NotificationDiscoveryManager.h"
#include "NotificationConsumer.h"
#include "RCSAddress.h"

#include "OCPlatform.h"

#include "nmclientmain.h"

# define checkResource nullptr == resource?false:true

using namespace std;
using namespace OC;
using namespace OIC::Service;

constexpr int CORRECT_INPUT = 1;
constexpr int INCORRECT_INPUT = 2;
constexpr int QUIT_INPUT = 3;

std::shared_ptr<NotificationConsumer>  selectedResource;
std::vector<std::shared_ptr<NotificationConsumer>> notificationResourceList;

const std::string defaultKey = "Temperature";
const std::string resourceType = "oic.r.notify";

static Evas_Object *log_entry = NULL;
static Evas_Object *list = NULL;
static Evas_Object *naviframe = NULL;

typedef struct temperature_popup
{
    Evas_Object *popup;
    Evas_Object *entry;
} temperature_popup_fields;

// Function to update the log in UI
void *updateGroupLog(void *data)
{
    string *log = (string *)data;
    // Show the log
    elm_entry_entry_append(log_entry, (*log).c_str());
    elm_entry_cursor_end_set(log_entry);
    return NULL;
}

static void onDestroy()
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### Destroy sequence called");
    notificationResourceList.clear();
    selectedResource = nullptr;
}

void onResourceDiscovered(std::shared_ptr<NotificationConsumer> foundResource)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### onResourceDiscovered callback");

    const std::string resourceURI = foundResource->getUri();
    const std::string hostAddress = foundResource->getAddress();

    int resourceSize = notificationResourceList.size() + 1;
    string logMessage = "Resource Found : " + std::to_string(resourceSize) + "<br>";
    logMessage = logMessage + "URI: " + resourceURI + "<br>";
    logMessage = logMessage + "Host:" + hostAddress + "<br>";
    logMessage += "----------------------<br>";
    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);

    notificationResourceList.push_back(foundResource);

    if ("/oic/notify" == resourceURI)
        selectedResource = foundResource;
}

void onResourceUpdated(NotificationObject *m_notificationObjectPtr)
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### onResourceUpdated callback..........");
    string logMessage;

    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Text)
    {
        TextNotification *textNotificationPtr = (TextNotification *) m_notificationObjectPtr;

        std::string nObjType = "text";

        logMessage = "============= NOTIFICATION RECIEVED ============<br>";
        logMessage = logMessage + "Type                       : " + nObjType + "<br>";
        logMessage = logMessage + "Time                        : " + textNotificationPtr->mNotificationTime
                     + "<br>";
        logMessage = logMessage + "Sender                     : " + textNotificationPtr->mNotificationSender
                     + "<br>";
        logMessage = logMessage + "TTL                         : " + std::to_string(
                         textNotificationPtr->mNotificationTtl) + "<br>";
        logMessage = logMessage + "Message                  : " + textNotificationPtr->mNotificationMessage
                     + "<br>";
        logMessage = logMessage + "ID                  : " + std::to_string(
                         textNotificationPtr->mNotificationId) + "<br>";
        logMessage = logMessage +  "==========================================" + "<br>";

        dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
    }

    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Image)
    {
        ImageNotification *imageNotificationPtr = (ImageNotification *) m_notificationObjectPtr;

        std::string nObjType = "image";

        logMessage = "============= NOTIFICATION RECIEVED ============<br>";
        logMessage = logMessage + "Type                       : " + nObjType + "<br>";
        logMessage = logMessage + "Time                        : " + imageNotificationPtr->mNotificationTime
                     + "<br>";
        logMessage = logMessage + "Sender                     : " +
                     imageNotificationPtr->mNotificationSender + "<br>";
        logMessage = logMessage + "TTL                         : " + std::to_string(
                         imageNotificationPtr->mNotificationTtl) + "<br>";
        logMessage = logMessage + "Icon                  : " + imageNotificationPtr->mNotificationIconUrl +
                     "<br>";
        logMessage = logMessage + "Message                  : " + imageNotificationPtr->mNotificationMessage
                     + "<br>";
        logMessage = logMessage + "ID                  : " + std::to_string(
                         imageNotificationPtr->mNotificationId) + "<br>";
        logMessage = logMessage +  "==========================================" + "<br>";

        dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
    }

    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Video)
    {
        VideoNotification *videoNotificationPtr = (VideoNotification *) m_notificationObjectPtr;

        std::string nObjType = "video";

        logMessage = "============= NOTIFICATION RECIEVED ============<br>";
        logMessage = logMessage + "Type                       : " + nObjType + "<br>";
        logMessage = logMessage + "Time                        : " + videoNotificationPtr->mNotificationTime
                     + "<br>";
        logMessage = logMessage + "Sender                     : " +
                     videoNotificationPtr->mNotificationSender + "<br>";
        logMessage = logMessage + "TTL                         : " + std::to_string(
                         videoNotificationPtr->mNotificationTtl) + "<br>";
        logMessage = logMessage + "Video                  : " + videoNotificationPtr->mNotificationVideoUrl
                     + "<br>";
        logMessage = logMessage + "ID                  : " + std::to_string(
                         videoNotificationPtr->mNotificationId) + "<br>";
        logMessage = logMessage +  "==========================================" + "<br>";

        dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
    }
}

static void list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *it = (Elm_Object_Item *)event_info;
    elm_list_item_selected_set(it, EINA_FALSE);
}

void discoverResource()
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### discovery started");

    NotificationDiscoveryManager::getInstance()->discoverNotificationResource(
        RCSAddress::multicast(), &onResourceDiscovered);

    dlog_print(DLOG_INFO, LOG_TAG, "#### Discovery over");
}

void cancelDiscoverResource()
{
    dlog_print(DLOG_INFO, LOG_TAG, "#### cancelDiscoverResource entry");
    string logMessage = "";
    int resourceSize = notificationResourceList.size();

    logMessage += std::to_string(resourceSize) + " : Resource Discovered <br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))showClientAPIs, NULL);

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
}

static void
list_scheduled_actionset_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *popup, *btn;
    Evas_Object *nf = naviframe;
    Evas_Object *entry;
    Evas_Object *layout;

    /* popup */
    popup = elm_popup_add(nf);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_text_set(popup, "title,text", "Enter the temperature");

    layout = elm_layout_add(popup);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_datetime_text");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(popup, layout);

    entry = elm_entry_add(layout);
    elm_entry_single_line_set(entry, EINA_TRUE);
    elm_entry_scrollable_set(entry, EINA_TRUE);
    evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    eext_entry_selection_back_event_allow_set(entry, EINA_TRUE);
    elm_object_part_text_set(entry, "elm.guide", "in degree celsius");
    elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content", entry);

    temperature_popup_fields *popup_fields;
    popup_fields = (temperature_popup_fields *)malloc(sizeof(temperature_popup_fields));
    if (NULL == popup_fields)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Memory allocation failed");
    }
    else
    {
        popup_fields->popup = popup;
        popup_fields->entry = entry;
    }


    /* Cancel button */
    /*
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Cancel");
    elm_object_part_content_set(popup, "button1", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_cancel_clicked_cb, popup_fields);
    */

    /* Set button */
    /*
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Set");
    elm_object_part_content_set(popup, "button2", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_set_clicked_cb, popup_fields);
    */

    evas_object_show(popup);
}

// Method to be called when the Start Discovery UI Button is selected
static void
find_resource_cb(void *data, Evas_Object *obj, void *event_info)
{
    if (NULL != list)
    {
        discoverResource();
    }
    else
    {
        dlog_print(DLOG_ERROR, "find_resource_cb", "list is NULL - So unable to add items!!!");
    }
}

// Method to be called when the Cancel Discovery UI Button is selected
static void
cancel_resource_cb(void *data, Evas_Object *obj, void *event_info)
{
    if (NULL != list)
    {
        cancelDiscoverResource();
    }
    else
    {
        dlog_print(DLOG_ERROR, "cancel_resource_cb", "list is NULL - So unable to add items!!!");
    }
}

static void startSubscribeNotifications(void *data, Evas_Object *obj, void *event_info)
{
    selectedResource->subscribeNotifications(&onResourceUpdated);
    dlog_print(DLOG_INFO, LOG_TAG, "#### Subscribing Started..........");
}

static void stopSubscribeNotifications(void *data, Evas_Object *obj, void *event_info)
{
    selectedResource->unSubscribeNotifications();
    dlog_print(DLOG_INFO, LOG_TAG, "#### Subscribing stopped...");
    string logMessage = "Subscribing stopped....";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
}

void *showClientAPIs(void *data)
{
    // Add items to the list only if the list is empty
    const Eina_List *eina_list = elm_list_items_get(list);
    int count = eina_list_count(eina_list);
    if (!count)
    {

        elm_list_item_append(list, "1. Start Subscribing", NULL, NULL,
                             startSubscribeNotifications, NULL);
        elm_list_item_append(list, "2. Stop Subscribing", NULL, NULL,
                             stopSubscribeNotifications, NULL);


        elm_list_go(list);
    }
    return NULL;
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
    if (NULL != list)
    {
        evas_object_del(list);
        list = NULL;
    }
    return EINA_TRUE;
}

// Method to be called when the Group APIs UI Button is selected
void group_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *layout;
    Evas_Object *scroller;
    Evas_Object *nf = (Evas_Object *)data;
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

    // Start Discovery Button
    button1 = elm_button_add(layout);
    elm_object_part_content_set(layout, "button1", button1);
    elm_object_text_set(button1, "Start Notification Resource Discovery");
    evas_object_smart_callback_add(button1, "clicked", find_resource_cb, NULL);

    // Cancel Discovery Button
    button2 = elm_button_add(layout);
    elm_object_part_content_set(layout, "button2", button2);
    elm_object_text_set(button2, "Cancel Notification Resource Discovery");
    evas_object_smart_callback_add(button2, "clicked", cancel_resource_cb, NULL);

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
    elm_object_part_text_set(log_entry, "elm.guide", "Logs will be updated here!!!");
    evas_object_size_hint_weight_set(log_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(log_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_part_content_set(layout, "log", log_entry);

    nf_it = elm_naviframe_item_push(nf, "Resource Encapsulation", NULL, NULL, scroller, NULL);
    elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, NULL);
}

