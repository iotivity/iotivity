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

#include "reservermain.h"
#include <tizen.h>

typedef struct appdata
{
    Evas_Object *win;
    Evas_Object *conform;
    Evas_Object *layout;
    Evas_Object *nf;
    Evas_Object *findButton;
    Evas_Object *logtext;
    Evas_Object *listview;
} appdata_s;

static void
win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
    ui_app_exit();
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
create_list_view(appdata_s *ad)
{
    Evas_Object *list;
    Evas_Object *btn;
    Evas_Object *nf = ad->nf;
    Elm_Object_Item *nf_it;

    // List
    list = elm_list_add(nf);
    elm_list_mode_set(list, ELM_LIST_COMPRESS);
    evas_object_smart_callback_add(list, "selected", list_selected_cb, NULL);

    // Main Menu Items Here
    elm_list_item_append(list, "Create Resource [Auto control]", NULL, NULL, start_server, nf);

    elm_list_item_append(list, "Create Resource [Developer control]", NULL, NULL, start_server_cb, nf);

    elm_list_go(list);

    // This button is set for devices which doesn't have H/W back key.
    btn = elm_button_add(nf);
    elm_object_style_set(btn, "naviframe/end_btn/default");
    nf_it = elm_naviframe_item_push(nf, "Resource Encapsulation", btn, NULL, list, NULL);
    elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, ad->win);
}
