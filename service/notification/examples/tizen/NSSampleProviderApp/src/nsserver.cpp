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

#include "nsserver.h"
#include "nsmain.h"
#include "OCPlatform.h"
#include <string>
#include "pthread.h"

using namespace std;
using namespace OC;
using namespace OIC::Service;

static bool serverStarted = false;
static Evas_Object *log_entry = NULL;
static Evas_Object *listview = NULL;
static Evas_Object *naviframe = NULL;

uint64_t mainMessageId = 0;
static int radio_val = 0;
static Evas_Object *radiogroup;
static bool gAcceptor = false;
std::shared_ptr<OIC::Service::NSConsumer> g_consumer;
std::string g_remoteAddress;
const char *gauthProvider;
static bool isSignedIn = false;
static bool gRemoteService = true;
std::string gaccess_token;
std::string guserid;

typedef struct datetime_popup
{
    Evas_Object *popup;
    Evas_Object *entry_obj1;
    Evas_Object *entry_obj2;
    Evas_Object *entry_obj3;
} datetime_popup_fields;


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
}

void subscribeRequestCallback(std::shared_ptr<OIC::Service::NSConsumer> consumer)
{
    string logMessage = "consumer requested to subscribe";

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
    g_consumer = consumer;
    consumer->acceptSubscription(true);
}

void syncCallback(OIC::Service::NSSyncInfo sync)
{
    string logMessage;
    logMessage = "SyncInfo Received ";
    logMessage += "Sync State: ";
    logMessage += to_string((int)sync.getState());
    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
}

static void accountHandler(const HeaderOptions &, const OCRepresentation &, const int code)
{
}

static void onSignUp(const HeaderOptions &, const OCRepresentation &rep, const int code)
{
    dlog_print(DLOG_INFO, LOG_TAG, "%s", __FUNCTION__);
    rep.getValue("accesstoken", gaccess_token);
    rep.getValue("uid", guserid);
}

static const char *get_msg_string(Evas_Object *entry)
{
    const char *attributeString = elm_entry_entry_get(entry);
    string logMessage = "";
    int beginning = 0;

    if (NULL == attributeString)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Read NULL Value");
        return NULL;
    }

    while (attributeString[beginning] == ' ')
    {
        (beginning)++;
    }

    int len = strlen(attributeString);
    if (1 > len)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Read NULL Value");
        return NULL;
    }
    return attributeString;
}

void __on_web_url_load_started(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_ERROR, LOG_TAG, "%s", __FUNCTION__);
}

void __on_web_url_change(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_ERROR, LOG_TAG, "%s", __FUNCTION__);
}

void __on_web_url_load_error(void *data, Evas_Object *obj, void *event_info)
{
    dlog_print(DLOG_ERROR, LOG_TAG, "%s", __FUNCTION__);
}

void __on_web_url_load_finished(void *data, Evas_Object *obj, void *event_info)
{
    const char *url = ewk_view_url_get(obj);
    dlog_print(DLOG_ERROR, LOG_TAG, "%s", __FUNCTION__);

    //Get the Auth Code from the URL
    if (url)
    {
        dlog_print(DLOG_ERROR, LOG_TAG, "%s", url);
        std::string strurl(url);
        std::string delimiter = "&";
        size_t pos = 0;
        std::string token;
        std::string code_token;

        pos = strurl.find(delimiter);
        token = strurl.substr(0, pos);

        while ((pos = strurl.find(delimiter)) != std::string::npos)
        {
            size_t found = 0;
            std::string code_delimiter = "code=";
            token = strurl.substr(0, pos);
            if ((found = token.find(code_delimiter)) != std::string::npos)
            {
                token.erase(0, found + code_delimiter.length());
                code_token = token;
            }
            strurl.erase(0, pos + delimiter.length());
        }

        if (!(code_token.empty()))
        {
            gauthCode = code_token.c_str();
            dlog_print(DLOG_ERROR, LOG_TAG, "Received Auth code from URL: [%s]" , gauthCode);
        }
        else
        {
            gauthCode = NULL;
        }
    }
}

static void
popup_login_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    datetime_popup_fields *popup_fields = (datetime_popup_fields *)data;

    Evas_Object *entry_addr = popup_fields->entry_obj1;
    string logMessage;
    const char *remoteAddress;

    remoteAddress = get_msg_string(entry_addr);
    g_remoteAddress = std::string(remoteAddress);

    OCAccountManager::Ptr accountManager = OCPlatform::constructAccountManagerObject(remoteAddress,
                                           CT_DEFAULT);
    QueryParamsMap options = {};
    accountManager->signUp(gauthProvider, gauthCode, options, &onSignUp);

    evas_object_del(popup_fields->popup);
    free(popup_fields);
}

static void
popup_cancel_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    datetime_popup_fields *popup_fields = (datetime_popup_fields *)data;
    evas_object_del(popup_fields->popup);
    free(popup_fields);
}

static void
popup_signUp_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    datetime_popup_fields *popup_fields = (datetime_popup_fields *)data;
#ifdef WITH_CLOUD
    Evas_Object *entry_authProv = popup_fields->entry_obj1;
    Evas_Object *entry_url = popup_fields->entry_obj2;
    string logMessage;
    const char *url, *authProvider, *authCode;

    authProvider = get_msg_string(entry_authProv);
    url = get_msg_string(entry_url);
    if ((NULL == authProvider) || (NULL == url))
    {
        logMessage = "Entry Cannot be NULL<br>";
        logMessage += "----------------------<br>";
        dlog_print(DLOG_INFO, LOG_TAG, " %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        evas_object_del(popup_fields->popup);
        free(popup_fields);
        return;
    }
    gauthProvider = authProvider;
    load_url((char *)url);

#else if
    string logMessage = "Cloud Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
    evas_object_del(popup_fields->popup);
    free(popup_fields);
}

static void popup_MQsubscribe_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    datetime_popup_fields *popup_fields = (datetime_popup_fields *)data;
#ifdef WITH_MQ
    Evas_Object *entry_addr = popup_fields->entry_obj1;
    Evas_Object *entry_topic = popup_fields->entry_obj2;
    string logMessage;
    const char *addr, *topic;

    addr = get_msg_string(entry_addr);
    topic = get_msg_string(entry_topic);

    if ((NULL == addr) || (NULL == topic))
    {
        logMessage = "Entry Cannot be NULL<br>";
        logMessage += "----------------------<br>";
        dlog_print(DLOG_INFO, LOG_TAG, " %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        evas_object_del(popup_fields->popup);
        free(popup_fields);
        return;
    }
    NSProviderService::getInstance()->subscribeMQService(addr, topic);
#else if
    string logMessage = "MQ Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
    evas_object_del(popup_fields->popup);
    free(popup_fields);
}

static void
popup_send_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    datetime_popup_fields *popup_fields = (datetime_popup_fields *)data;
    Evas_Object *entry_title = popup_fields->entry_obj1;
    Evas_Object *entry_body = popup_fields->entry_obj2;
    Evas_Object *entry_topic = popup_fields->entry_obj3;
    const char *msgTitle, *msgBody, *msgTopic;
    string logMessage;

    msgTitle = get_msg_string(entry_title);
    if (NULL == msgTitle)
    {
        logMessage = "Title Cannot be NULL<br>";
        logMessage += "----------------------<br>";
        dlog_print(DLOG_INFO, LOG_TAG, " %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        evas_object_del(popup_fields->popup);
        free(popup_fields);
        return;
    }

    msgBody = get_msg_string(entry_body);
    if (NULL == msgBody)
    {
        logMessage = "Body Cannot be NULL<br>";
        logMessage += "----------------------<br>";
        dlog_print(DLOG_INFO, LOG_TAG, " %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        evas_object_del(popup_fields->popup);
        free(popup_fields);
        return;
    }

    msgTopic = get_msg_string(entry_topic);

    //Send Message
    OIC::Service::NSMessage msg = NSProviderService::getInstance()->createMessage();
    msg.setType(OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_INFO);
    msg.setTitle(msgTitle);
    msg.setContentText(msgBody);
    msg.setSourceName("OCF");
    if (NULL != msgTopic)
        msg.setTopic(msgTopic);
    msg.setTTL(40);
    msg.setTime("12:30");
    OC::OCRepresentation rep;
    rep.setValue("Key1", "Value1");
    rep.setValue("Key2", "Value2");
    msg.setExtraInfo(rep);

    mainMessageId = msg.getMessageId();
    NSProviderService::getInstance()->sendMessage(msg);

    logMessage = "Notification Sent<br>";
    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
    evas_object_del(popup_fields->popup);
    free(popup_fields);
}

void do_login()
{
    if (gauthCode == NULL)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "Sign Up not success. Auth Code not received");
        string logMessage = "Sign Up not success. Auth Code not received<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    Evas_Object *popup, *btn;
    Evas_Object *nf = naviframe;
    Evas_Object *entry1;
    Evas_Object *layout;

    Evas_Object *scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);


    /* pop up */
    popup = elm_popup_add(nf);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_object_part_text_set(popup, "title,text", "Remote Server Address");

    layout = elm_layout_add(popup);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_datetime_text");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(popup, layout);
    //elm_object_content_set(scroller, layout);

    //entry1
    entry1 = elm_entry_add(layout);
    elm_entry_single_line_set(entry1, EINA_TRUE);
    elm_entry_scrollable_set(entry1, EINA_TRUE);
    evas_object_size_hint_weight_set(entry1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry1, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_entry_cursor_begin_set(entry1);
    eext_entry_selection_back_event_allow_set(entry1, EINA_TRUE);
    elm_object_part_text_set(entry1, "elm.guide", "Enter Server Address");
    elm_entry_input_panel_layout_set(entry1, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content", entry1);

    //popup
    datetime_popup_fields *popup_fields;
    popup_fields = (datetime_popup_fields *)malloc(sizeof(datetime_popup_fields));
    if (NULL == popup_fields)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Memory allocation failed");
    }
    else
    {
        popup_fields->popup = popup;
        popup_fields->entry_obj1 = entry1;
    }

    /* Cancel button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Cancel");
    elm_object_part_content_set(popup, "button1", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_cancel_clicked_cb, popup_fields);

    /* Set button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "LogIn");
    elm_object_part_content_set(popup, "button2", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_login_clicked_cb, popup_fields);

    evas_object_show(popup);

}

static void signUp(void *data, Evas_Object *obj, void *event_info)
{
#ifdef WITH_CLOUD
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    Evas_Object *popup, *btn;
    Evas_Object *nf = naviframe;
    Evas_Object *entry1, *entry2, *entry3;
    Evas_Object *layout;
    Evas_Object *scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

    /* pop up */
    popup = elm_popup_add(nf);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_object_part_text_set(popup, "title,text", "Login Details");

    layout = elm_layout_add(popup);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_datetime_text");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(popup, layout);

    //entry1
    entry1 = elm_entry_add(layout);
    elm_entry_single_line_set(entry1, EINA_TRUE);
    elm_entry_scrollable_set(entry1, EINA_TRUE);
    evas_object_size_hint_weight_set(entry1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry1, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_entry_cursor_begin_set(entry1);
    eext_entry_selection_back_event_allow_set(entry1, EINA_TRUE);
    elm_object_part_text_set(entry1, "elm.guide", "Auth Provider Name");
    elm_entry_input_panel_layout_set(entry1, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content", entry1);

    //entry2
    entry2 = elm_entry_add(layout);
    elm_entry_single_line_set(entry2, EINA_TRUE);
    elm_entry_scrollable_set(entry2, EINA_TRUE);
    evas_object_size_hint_weight_set(entry2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry2, EVAS_HINT_FILL, EVAS_HINT_FILL);
    eext_entry_selection_back_event_allow_set(entry2, EINA_TRUE);
    elm_object_part_text_set(entry2, "elm.guide", "Url");
    elm_entry_input_panel_layout_set(entry2, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content2", entry2);

    //popup
    datetime_popup_fields *popup_fields;
    popup_fields = (datetime_popup_fields *)malloc(sizeof(datetime_popup_fields));
    if (NULL == popup_fields)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Memory allocation failed");
    }
    else
    {
        popup_fields->popup = popup;
        popup_fields->entry_obj1 = entry1;
        popup_fields->entry_obj2 = entry2;;
    }

    /* Cancel button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Cancel");
    elm_object_part_content_set(popup, "button1", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_cancel_clicked_cb, popup_fields);

    /* Set button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Login");
    elm_object_part_content_set(popup, "button2", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_signUp_clicked_cb, popup_fields);

    evas_object_show(popup);

#else if
    string logMessage = "Cloud Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
}

static void signIn(void *data, Evas_Object *obj, void *event_info)
{
#ifdef WITH_CLOUD
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    if ((g_remoteAddress.empty()) || (guserid.empty()) || (gaccess_token.empty()))
    {
        string logMessage = "SignUp not successfull required. NULL params<br>";
        logMessage += "----------------------<br>";
        dlog_print(DLOG_INFO, LOG_TAG, " %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    OCAccountManager::Ptr accountManager = OCPlatform::constructAccountManagerObject(g_remoteAddress,
                                           CT_DEFAULT);
    accountManager->signIn(guserid, gaccess_token, &accountHandler);
    isSignedIn = true;
#else if
    string logMessage = "Cloud Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
}

static void signOut(void *data, Evas_Object *obj, void *event_info)
{
#ifdef WITH_CLOUD
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    if (!isSignedIn)
    {
        string logMessage = "Sign In First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    OCAccountManager::Ptr accountManager = OCPlatform::constructAccountManagerObject(g_remoteAddress,
                                           CT_DEFAULT);
    accountManager->signOut(gaccess_token, &accountHandler);
    isSignedIn = false;

#else if
    string logMessage = "Cloud Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
}
static void enableRemoteService(void *data, Evas_Object *obj, void *event_info)
{
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    if (g_remoteAddress.empty())
    {
        string logMessage = "Prior SignUp required. Address NULL<br>";
        logMessage += "----------------------<br>";
        dlog_print(DLOG_INFO, LOG_TAG, " %s", logMessage.c_str());
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
#ifdef WITH_CLOUD
    if (gRemoteService)
    {
        string logMessage = "Enabling Remote Services<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        Elm_Object_Item *it;
        it = elm_list_selected_item_get(listview);
        elm_object_item_text_set(it, "4. Disable Remote Service");
        NSProviderService::getInstance()->enableRemoteService(g_remoteAddress);
        gRemoteService = false;
    }
    else
    {
        string logMessage = "Disabling Remote Services<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        Elm_Object_Item *it;
        it = elm_list_selected_item_get(listview);
        elm_object_item_text_set(it, "4. Enable Remote Service");
        NSProviderService::getInstance()->disableRemoteService(g_remoteAddress);
        gRemoteService = true;
    }

#else if
    string logMessage = "Cloud Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
}
static void subscribeMQ(void *data, Evas_Object *obj, void *event_info)
{
#ifdef WITH_MQ
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    Evas_Object *popup, *btn;
    Evas_Object *nf = naviframe;
    Evas_Object *entry1, *entry2, *entry3;
    Evas_Object *layout;

    Evas_Object *scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

    /* pop up */
    popup = elm_popup_add(nf);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_text_set(popup, "title,text", "Subscribe MQ");
    layout = elm_layout_add(popup);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_datetime_text");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(popup, layout);

    //entry1
    entry1 = elm_entry_add(layout);
    elm_entry_single_line_set(entry1, EINA_TRUE);
    elm_entry_scrollable_set(entry1, EINA_TRUE);
    evas_object_size_hint_weight_set(entry1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry1, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_entry_cursor_begin_set(entry1);
    eext_entry_selection_back_event_allow_set(entry1, EINA_TRUE);
    elm_object_part_text_set(entry1, "elm.guide", "Server Address");
    elm_entry_input_panel_layout_set(entry1, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content", entry1);

    //entry2
    entry2 = elm_entry_add(layout);
    elm_entry_single_line_set(entry2, EINA_TRUE);
    elm_entry_scrollable_set(entry2, EINA_TRUE);
    evas_object_size_hint_weight_set(entry2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry2, EVAS_HINT_FILL, EVAS_HINT_FILL);
    eext_entry_selection_back_event_allow_set(entry2, EINA_TRUE);
    elm_object_part_text_set(entry2, "elm.guide", "Topic Name");
    elm_entry_input_panel_layout_set(entry2, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content2", entry2);

    //popup
    datetime_popup_fields *popup_fields;
    popup_fields = (datetime_popup_fields *)malloc(sizeof(datetime_popup_fields));
    if (NULL == popup_fields)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Memory allocation failed");
    }
    else
    {
        popup_fields->popup = popup;
        popup_fields->entry_obj1 = entry1;
        popup_fields->entry_obj2 = entry2;
    }

    /* Cancel button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Cancel");
    elm_object_part_content_set(popup, "button1", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_cancel_clicked_cb, popup_fields);

    /* Set button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Subscribe");
    elm_object_part_content_set(popup, "button2", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_MQsubscribe_clicked_cb, popup_fields);

    evas_object_show(popup);

#else if
    string logMessage = "MQ Services Not enabled<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
#endif
}
static void registerTopic(void *data, Evas_Object *obj, void *event_info)
{
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    NSProviderService::getInstance()->registerTopic("OCF_TOPIC1");
    NSProviderService::getInstance()->registerTopic("OCF_TOPIC2");
    NSProviderService::getInstance()->registerTopic("OCF_TOPIC3");
    NSProviderService::getInstance()->registerTopic("OCF_TOPIC4");
    return;
}
static void setTopic(void *data, Evas_Object *obj, void *event_info)
{
    if (!serverStarted)
    {
        string logMessage = "Start Provider First<br>";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    if (g_consumer != nullptr)
    {
        if (gAcceptor == false)
        {
            string logMessage = "Operation Not Permitted: Start Provider Service with provider as acceptor<br>";
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                                  &logMessage);
            return;
        }
        g_consumer->setTopic("OCF_TOPIC1");
        g_consumer->setTopic("OCF_TOPIC2");
    }
    return;
}

static void sendNotification(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *popup, *btn;
    Evas_Object *nf = naviframe;
    Evas_Object *entry1, *entry2, *entry3;
    Evas_Object *layout;
    Evas_Object *scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);


    /* pop up */
    popup = elm_popup_add(nf);
    elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
    eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
    evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_object_part_text_set(popup, "title,text", "Enter the Message");

    layout = elm_layout_add(popup);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "popup_datetime_text");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(popup, layout);

    //entry1
    entry1 = elm_entry_add(layout);
    elm_entry_single_line_set(entry1, EINA_TRUE);
    elm_entry_scrollable_set(entry1, EINA_TRUE);
    evas_object_size_hint_weight_set(entry1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry1, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_entry_cursor_begin_set(entry1);
    eext_entry_selection_back_event_allow_set(entry1, EINA_TRUE);
    elm_object_part_text_set(entry1, "elm.guide", "Add Msg Title");
    elm_entry_input_panel_layout_set(entry1, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content", entry1);


    //entry2
    entry2 = elm_entry_add(layout);
    elm_entry_single_line_set(entry2, EINA_TRUE);
    elm_entry_scrollable_set(entry2, EINA_TRUE);
    evas_object_size_hint_weight_set(entry2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry2, EVAS_HINT_FILL, EVAS_HINT_FILL);
    eext_entry_selection_back_event_allow_set(entry2, EINA_TRUE);
    elm_object_part_text_set(entry2, "elm.guide", "Add Msg Body");
    elm_entry_input_panel_layout_set(entry2, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content2", entry2);

    //entry3
    entry3 = elm_entry_add(layout);
    elm_entry_single_line_set(entry3, EINA_TRUE);
    elm_entry_scrollable_set(entry3, EINA_TRUE);
    evas_object_size_hint_weight_set(entry3, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry3, EVAS_HINT_FILL, EVAS_HINT_FILL);
    eext_entry_selection_back_event_allow_set(entry3, EINA_TRUE);
    elm_object_part_text_set(entry3, "elm.guide", "Add Topic 'OCF_TOPIC1(1 - 4)'");
    elm_entry_input_panel_layout_set(entry3, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content3", entry3);

    //popup
    datetime_popup_fields *popup_fields;
    popup_fields = (datetime_popup_fields *)malloc(sizeof(datetime_popup_fields));
    if (NULL == popup_fields)
    {
        dlog_print(DLOG_INFO, LOG_TAG, "#### Memory allocation failed");
    }
    else
    {
        popup_fields->popup = popup;
        popup_fields->entry_obj1 = entry1;
        popup_fields->entry_obj2 = entry2;
        popup_fields->entry_obj3 = entry3;
    }

    /* Cancel button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Cancel");
    elm_object_part_content_set(popup, "button1", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_cancel_clicked_cb, popup_fields);

    /* Set button */
    btn = elm_button_add(popup);
    elm_object_style_set(btn, "popup");
    elm_object_text_set(btn, "Send");
    elm_object_part_content_set(popup, "button2", btn);
    evas_object_smart_callback_add(btn, "clicked", popup_send_clicked_cb, popup_fields);

    evas_object_show(popup);
}

static void clearLog(void *data, Evas_Object *obj, void *event_info)
{
    const char *log = "";
    // Show the log
    elm_entry_entry_set(log_entry, log);
    elm_entry_cursor_end_set(log_entry);
    return;
}

static void list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *it = (Elm_Object_Item *)event_info;
    elm_list_item_selected_set(it, EINA_FALSE);
}


void *showAPIs(void *data)
{
    // Add items to the list only if the list is empty
    const Eina_List *eina_list = elm_list_items_get(listview);
    int count = eina_list_count(eina_list);
    if (!count)
    {
        elm_list_item_append(listview, "1. Register Topic", NULL, NULL, registerTopic, NULL);

        elm_list_item_append(listview, "2. Set Topic", NULL, NULL, setTopic, NULL);

        elm_list_item_append(listview, "3. Send Notification", NULL, NULL, sendNotification, NULL);

        elm_list_item_append(listview, "4. Sign Up", NULL, NULL, signUp, NULL);

        elm_list_item_append(listview, "5. Sign In", NULL, NULL, signIn, NULL);

        elm_list_item_append(listview, "6. Sign Out", NULL, NULL,  signOut, NULL);

        elm_list_item_append(listview, "7. Enable Remote Service", NULL, NULL, enableRemoteService, NULL);

        elm_list_item_append(listview, "8. Subscribe MQ service", NULL, NULL, subscribeMQ, NULL);

        elm_list_item_append(listview, "9. Clear Logs", NULL, NULL, clearLog, NULL);


        elm_list_go(listview);
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
    if (NULL != listview)
    {
        evas_object_del(listview);
        listview = NULL;
    }
    ui_app_exit();
    return EINA_FALSE;
}

// Method to set up server screens
void serverCreateUI(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *layout;
    Evas_Object *scroller;
    Evas_Object *nf = (Evas_Object *)data;
    Evas_Object *button1;
    Evas_Object *button2;
    Evas_Object *radio1, *radio2;
    Evas_Object *entry2;

    Elm_Object_Item *nf_it;
    naviframe = nf;

    // Scroller
    scroller = elm_scroller_add(nf);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

    // Layout
    layout = elm_layout_add(nf);
    elm_layout_file_set(layout, ELM_DEMO_EDJ, "server_layout");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_object_content_set(scroller, layout);

    // Button
    button1 = elm_button_add(layout);
    elm_object_part_content_set(layout, "button1", button1);
    elm_object_text_set(button1, "Start");
    evas_object_smart_callback_add(button1, "clicked", start_provider, NULL);

    // Button
    button2 = elm_button_add(layout);
    elm_object_part_content_set(layout, "button2", button2);
    elm_object_text_set(button2, "Stop");
    evas_object_smart_callback_add(button2, "clicked", stop_provider, NULL);

    //radio 1
    radiogroup = radio1 = elm_radio_add(layout);
    elm_object_part_content_set(layout, "radio1", radio1);
    elm_object_text_set(radio1, "Provider");
    elm_radio_state_value_set(radio1, 1);
    elm_radio_value_pointer_set(radio1, &radio_val);

    //radio 2
    radio2 = elm_radio_add(layout);
    elm_object_part_content_set(layout, "radio2", radio2);
    elm_object_text_set(radio2, "Consumer");
    elm_radio_state_value_set(radio2, 2);
    elm_radio_value_pointer_set(radio2, &radio_val);
    elm_radio_group_add(radio2, radiogroup);

    // List
    listview = elm_list_add(layout);
    elm_list_mode_set(listview, ELM_LIST_COMPRESS);
    evas_object_smart_callback_add(listview, "selected", list_selected_cb, NULL);
    elm_object_part_content_set(layout, "list", listview);
    elm_list_go(listview);

    entry2 = elm_entry_add(layout);
    elm_entry_single_line_set(entry2, EINA_TRUE);
    elm_entry_scrollable_set(entry2, EINA_TRUE);
    evas_object_size_hint_weight_set(entry2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(entry2, EVAS_HINT_FILL, EVAS_HINT_FILL);
    eext_entry_selection_back_event_allow_set(entry2, EINA_TRUE);
    elm_object_part_text_set(entry2, "elm.guide", "Add Msg Body");
    elm_entry_input_panel_layout_set(entry2, ELM_INPUT_PANEL_LAYOUT_NUMBER);
    elm_object_part_content_set(layout, "elm.swallow.content2", entry2);

    // log_entry - text area for log
    log_entry = elm_entry_add(layout);
    elm_entry_scrollable_set(log_entry, EINA_TRUE);
    elm_entry_editable_set(log_entry, EINA_FALSE);
    elm_object_part_text_set(log_entry, "elm.guide", "Logs will be updated here!!!");
    evas_object_size_hint_weight_set(log_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(log_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_part_content_set(layout, "log", log_entry);

    nf_it = elm_naviframe_item_push(nf, "Notification Provider", NULL, NULL, scroller, NULL);
    elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, NULL);
}


void  on_url_launched(app_control_h request, app_control_h reply, app_control_result_e result,
                      void *user_data)
{
    string logMessage = "Url launched<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
}

void start_provider(void *data, Evas_Object *obj, void *event_info)
{
    string logMessage;
    if (!serverStarted)
    {
        NSProviderService::ProviderConfig cfg;
        cfg.m_subscribeRequestCb = subscribeRequestCallback;
        cfg.m_syncInfoCb = syncCallback;
        if (radio_val == 1)
        {
            logMessage = "Provider Started with acceptor as Provider<br>";
            cfg.subControllability = true;
            gAcceptor = true;
        }
        else if (radio_val == 2)
        {
            logMessage = "Provider Started with acceptor as consumer<br>";
            cfg.subControllability = false;
            gAcceptor = false;
        }
        else
        {
            logMessage = "Select (1) Start provider with provider as acceptor<br>";
            logMessage += "(2) Start provider with consumer as acceptor<br>";
            ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                                  &logMessage);
            return;
        }
        NSProviderService::getInstance()->start(cfg);
        serverStarted = true;
    }
    else
        logMessage = "Provider Already Started<br>";

    dlog_print(DLOG_INFO, LOG_TAG, "#### %s", logMessage.c_str());
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))showAPIs, NULL);
}

void stop_provider(void *data, Evas_Object *obj, void *event_info)
{
    if (!serverStarted)
    {
        string logMessage = "Provider Not Started";
        ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                              &logMessage);
        return;
    }
    NSProviderService::getInstance()->stop();
    string logMessage = "Provider Stopped<br>";
    ecore_main_loop_thread_safe_call_sync((void *( *)(void *))updateGroupLog,
                                          &logMessage);
    serverStarted = false;
}

