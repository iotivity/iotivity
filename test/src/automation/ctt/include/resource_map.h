/******************************************************************
 *
 * Copyright 2016 Granite River Labs All Rights Reserved.
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

#ifndef __RESOURCE_MAP_H__
#define __RESOURCE_MAP_H__

#include <libxml/parser.h>
#ifdef __cplusplus
extern "C"{
#endif
typedef xmlDocPtr (*resource_cb)(xmlDocPtr);
typedef void (*after_resource_cb)(xmlDocPtr);

struct resource_map_s;

struct resource_cbs_s
{
    resource_cb on_resource;
    after_resource_cb on_after_resource;
};

typedef struct resource_map_s resource_map_t;
typedef struct resource_cbs_s resource_cbs_t;

resource_map_t* CreateResourceMap();
void AddResourceCbs(resource_map_t* self, const char* method, const char* path, resource_cb on_resource, after_resource_cb on_after_resource);
resource_cbs_t GetResourceCbs(resource_map_t* self, const char* method, const char* path);
void DeleteResourceMap(resource_map_t* self);
#ifdef __cplusplus
}
#endif
#endif //__RESOURCE_MAP_H__
