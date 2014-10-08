//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file musicApp.h

/// @brief

#ifndef __MUSICAPP_H__
#define __MUSICAPP_H__

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

	// for later use
	typedef struct
	{
		Evas_Object *win;
		Evas_Object *layout_main;
		Evas_Object *conformant;

		Evas_Object *layout;

		Evas_Object *bg;	
		Evas_Object *btn;
		Evas_Object *btnExit;		

		Evas_Object *nf;

	}MusicAppData;


#ifdef __cplusplus
}
#endif

#endif //__MUSICAPP_H__
