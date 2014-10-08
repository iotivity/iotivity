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

	typedef enum {
		OICAPP_MODE_NONE = 0,
		OICAPP_MODE_CLIENT,
		OICAPP_MODE_SERVER,
		OICAPP_MODE_MAX
	}oicappMode;

	enum {
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

		Elm_Object_Item *itemTitle;
		Elm_Object_Item *itemUri;
		Elm_Object_Item *itemHost;
		Elm_Object_Item *itemTemp;
		Elm_Object_Item *itemHumid;
		Elm_Object_Item *itemMultiLine;
		Elm_Object_Item *itemApplyExtraStr;


		Elm_Genlist_Item_Class itcTitle;
		Elm_Genlist_Item_Class itcText;
		Elm_Genlist_Item_Class itcSeperator;
		Elm_Genlist_Item_Class itcEdit;
		Elm_Genlist_Item_Class itcEdit2;
		Elm_Genlist_Item_Class itcBtnStartHost;
		Elm_Genlist_Item_Class itcBtnApplyExtraStr;
		Elm_Genlist_Item_Class itcBtnFindResource;

		char *ipAddr;
		char *inputMsg;
		char *outputMsg;

	}oicappData;

	typedef struct
	{
		oicappData *ad;
		const char *title;

	}oicappItemData;



#ifdef __cplusplus
}
#endif

#endif //__OICAPP_TEST_H__

