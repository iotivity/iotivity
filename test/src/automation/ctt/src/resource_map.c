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

#include <stdio.h>
#include <string.h>
#include <libxml/tree.h>
#include <resource_map.h>

#define MAX_METHOD 32
#define MAX_PATH 256

typedef struct resource_s
{
    char method[MAX_METHOD + 1];
    char path[MAX_PATH + 1];
    resource_cbs_t resource_cbs;
    struct resource_s* next;
} resource_t;

struct resource_map_s
{
    resource_t* head;
};

resource_map_t* CreateResourceMap()
{
    resource_map_t* map = (resource_map_t*)malloc(sizeof(resource_map_t));
    map->head = 0;

    return map;
}

void AddResourceCbs(resource_map_t* self, const char* method, const char* path, resource_cb on_resource, after_resource_cb on_after_resource)
{
    resource_t* new_element = (resource_t*)malloc(sizeof(resource_t));

    strncpy(new_element->method, method, MAX_METHOD);
    strncpy(new_element->path, path, MAX_PATH);
    new_element->path[MAX_PATH] = 0;
    new_element->resource_cbs.on_resource = on_resource;
    new_element->resource_cbs.on_after_resource = on_after_resource;
    new_element->next = self->head;

    self->head = new_element;
}

resource_cbs_t GetResourceCbs(resource_map_t* self, const char* method, const char* path)
{
    resource_t* res = self->head;
    resource_cbs_t resource_cbs;

    resource_cbs.on_resource = 0;
    resource_cbs.on_after_resource = 0;

    while (res != 0)
    {
        if (strcmp(res->method, method) == 0 && strcmp(res->path, path) == 0)
        {
            resource_cbs = res->resource_cbs;
            break;
        }

        res = res->next;
    };

    return resource_cbs;
}

void DeleteResourceMap(resource_map_t* self)
{
    resource_t* head = self->head;
    resource_t* next_head = 0;

    while (head != 0)
    {
        next_head = head->next;
        free(head);
        head = next_head;
    };
}
