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

/// @file PluginProvider.h

/// @brief

#ifndef __PLUGIN_PROVIDER_H__
#define __PLUGIN_PROVIDER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpluff.h"
#include <unistd.h>



/**
 * Creates a new plug-in instance. We use classifier instance as plug-in
 * instance because it includes all the data our plug-in instance needs.
 */
static void *create(cp_context_t *ctx);

/**
 * Initializes and starts the plug-in.
 */
static int start(void *d);

/**
 * Destroys a plug-in instance.
 */
static void destroy(void *d);


#endif //__PLUGIN_PROVIDER_H__

