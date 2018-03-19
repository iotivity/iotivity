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
#include <pthread.h>

bool isExit = false;

static void
win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
    //ui_app_exit();
}

static void
list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *it = (Elm_Object_Item *)event_info;
    elm_list_item_selected_set(it, EINA_FALSE);
}

static Eina_Bool
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
    ui_app_exit();
    return EINA_FALSE;
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
    if (g_ad->web_view)
    {
        evas_object_show(g_ad->conform);
        evas_object_show(g_ad->win);
        evas_object_del(g_ad->web_view);
        g_ad->web_view = NULL;
        //when exiting from the Webview(url) prompt the user for Login
        do_login();
    }
    else
        ui_app_exit();

}

static void
create_base_gui(appdata_s *ad)
{
    g_ad = ad;
    // Window
    ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
    elm_win_conformant_set(ad->win, EINA_TRUE);
    elm_win_autodel_set(ad->win, EINA_TRUE);

    if (elm_win_wm_rotation_supported_get(ad->win))
    {
        int rots[4] = { 0, 90, 180, 270 };
        elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
    }

    evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);

    // Conformant
    ad->conform = elm_conformant_add(ad->win);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);

    // Base Layout
    ad->layout = elm_layout_add(ad->conform);
    evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_layout_theme_set(ad->layout, "layout", "application", "default");
    evas_object_show(ad->conform);;

    elm_object_content_set(ad->conform, ad->layout);

    // Naviframe
    ad->nf = elm_naviframe_add(ad->layout);

    clientCreateUI(ad->nf, NULL, NULL);
    elm_object_part_content_set(ad->layout, "elm.swallow.content", ad->nf);
    eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, win_back_cb, NULL);
    eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_MORE, eext_naviframe_back_cb, NULL);

    eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

    // Show window after base gui is set up
    evas_object_show(ad->win);
}

void load_url(char *url)
{
    appdata_s *ad = g_ad;
    Evas_Object *box = elm_box_add(ad->win);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, box);
    evas_object_show(box);

    /* Table */
    Evas_Object *table = elm_table_add(ad->win);
    evas_object_size_hint_align_set(table, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(box, table);
    evas_object_show(table);

    //WebView
    ewk_init();
    Evas *evas = evas_object_evas_get(ad->win);
    ad->web_view = ewk_view_add(evas);

    evas_object_size_hint_weight_set(ad->web_view, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(ad->web_view, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(ad->web_view, "url,changed", __on_web_url_change, NULL);
    evas_object_smart_callback_add(ad->web_view, "load,error", __on_web_url_load_error, NULL);
    evas_object_smart_callback_add(ad->web_view, "load,finished", __on_web_url_load_finished, NULL);
    evas_object_smart_callback_add(ad->web_view, "load,started", __on_web_url_load_started, NULL);
    int ret = ewk_view_url_set(ad->web_view, url);
    if (ret != true)
    {
        dlog_print(DLOG_ERROR, LOG_TAG, "ewk_view_url_set() is failed. err = %d", ret);
        return;
    }
    else
        dlog_print(DLOG_ERROR, LOG_TAG, "ewk_view_url_set() is passed. err = %d", ret);
    elm_box_pack_end(box, ad->web_view);
    evas_object_show(ad->web_view);
    evas_object_show(ad->win);

}


void *OCProcessThread(void *ptr)
{
    (void) ptr;
    while (!isExit)
    {
        usleep(2000);
        if (OCProcess() != OC_STACK_OK)
        {
            std::cout << "OCStack process error" << std::endl;
            return NULL;
        }
    }

    return NULL;
}

static bool
app_create(void *data)
{
    /* Hook to take necessary actions before main event loop starts
       Initialize UI resources and application's data
       If this function returns true, the main loop of application starts
       If this function returns false, the application is terminated */
    appdata_s *ad = (appdata_s *)data;

    elm_app_base_scale_set(1.8);
    pthread_t processThread = 0;

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        std::cout << "OCStack init error" << std::endl;
        return 0;
    }
    pthread_create(&processThread, NULL, OCProcessThread, NULL);
    create_base_gui(ad);
    return true;
}

static void
app_control(app_control_h app_control, void *data)
{
    // Handle the launch request.


}

static void
app_pause(void *data)
{
    // Take necessary actions when application becomes invisible.
}

static void
app_resume(void *data)
{
    // Take necessary actions when application becomes visible.
}

static void
app_terminate(void *data)
{
    // Release all resources.

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
                             APP_EVENT_DEVICE_ORIENTATION_CHANGED,
                             ui_app_orient_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
                             APP_EVENT_LANGUAGE_CHANGED,
                             ui_app_lang_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
                             APP_EVENT_REGION_FORMAT_CHANGED,
                             ui_app_region_changed, &ad);
    ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

    ret = ui_app_main(argc, argv, &event_callback, &ad);
    if (APP_ERROR_NONE != ret)
    {
        dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
    }
    return ret;
}
