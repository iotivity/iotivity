/*
 * Copyright (c) 2010 Samsung Electronics, Inc.
 * All rights reserved.
 *
 * This software is a confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

#ifndef __OICAPP_TEST_H__
#define __OICAPP_TEST_H__

#include <Elementary.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "oicapp-log.h"

#if !defined(PACKAGE)
#  define PACKAGE "oicapp-test"
#endif

#if !defined(LOCALEDIR)
#  define LOCALEDIR "/usr/apps/com.samsung.oicapp-test/res/locale"
#endif

#if !defined(EDJDIR)
#  define EDJDIR "/usr/apps/com.samsung.oicapp-test/res/edje"
#endif

#define GRP_MAIN "main"

typedef enum
{
    OICAPP_MODE_NONE = 0,
    OICAPP_MODE_CLIENT,
    OICAPP_MODE_SERVER,
    OICAPP_MODE_MAX
} oicapp_mode;

enum
{
    OICAPP_GENLIST_GRP_NONE = 0,
    OICAPP_GENLIST_GRP_TOP,
    OICAPP_GENLIST_GRP_CENTER,
    OICAPP_GENLIST_GRP_BOTTOM
};


typedef struct
{
    Evas_Object *win;
    Evas_Object *base;
    Evas_Object *bg;
    Evas_Object *navi;
    Evas_Object *genlist;
    Evas_Object *popup;
    Evas_Object *conform;

    Elm_Object_Item *item_multiline;
    Elm_Object_Item *item_client_pwr;
    Elm_Object_Item *item_client_lev;
    Elm_Object_Item *item_server;
    Elm_Object_Item *item_server_pwr;
    Elm_Object_Item *item_server_lev;

    Elm_Genlist_Item_Class itc_seperator;
    Elm_Genlist_Item_Class itc_edit;
    Elm_Genlist_Item_Class itc_btn;
    Elm_Genlist_Item_Class itc_multiline;

    char *ip_addr;
    char *input_msg;
    char *output_msg;
} oicapp_data;
#ifdef __cplusplus
}
#endif

#endif //__OICAPP_TEST_H__


