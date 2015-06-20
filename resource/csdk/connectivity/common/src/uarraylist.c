/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include <stdlib.h>
#include <string.h>
#include "uarraylist.h"
#include "logger.h"
#include "oic_malloc.h"

#define TAG "UARRAYLIST"

/**
 * Use this default size when initialized
 */
#define U_ARRAYLIST_DEFAULT_SIZE 1

u_arraylist_t *u_arraylist_create()
{
    u_arraylist_t *list = NULL;

    list = (u_arraylist_t *) OICMalloc(sizeof(u_arraylist_t));
    if (!list)
    {
        return NULL;
    }

    list->size = U_ARRAYLIST_DEFAULT_SIZE;
    list->length = 0;

    list->data = (void *) OICMalloc(list->size * sizeof(void *));
    if (!list->data)
    {
        OIC_LOG(DEBUG, TAG, "Out of memory");
        OICFree(list);
        return NULL;
    }
    return list;
}

CAResult_t u_arraylist_free(u_arraylist_t **list)
{
    if (!list || !(*list))
    {
        return CA_STATUS_INVALID_PARAM;
    }

    OICFree((*list)->data);
    OICFree(*list);

    *list = NULL;

    return CA_STATUS_OK;
}

void *u_arraylist_get(const u_arraylist_t *list, uint32_t index)
{
    if (!list )
    {
        return NULL;
    }

    if ((index < list->length) && (list->data))
    {
        return list->data[index];
    }

    return NULL;
}

CAResult_t u_arraylist_add(u_arraylist_t *list, void *data)
{
    if (!list)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    if (list->size <= list->length)
    {

    	uint32_t new_size = list->size + 1;
        if (!(list->data = (void **) realloc(list->data, new_size * sizeof(void *))))
        {
            return CA_MEMORY_ALLOC_FAILED;
        }

        memset(list->data + list->size, 0, (new_size - list->size) * sizeof(void *));
        list->size = new_size;
    }

    list->data[list->length] = data;
    list->length++;

    return CA_STATUS_OK;
}

void *u_arraylist_remove(u_arraylist_t *list, uint32_t index)
{
    void *removed = NULL;

    if (!list)
    {
        return NULL;
    }

    if (index >= list->length)
    {
        return NULL;
    }

    removed = list->data[index];

    if (index < list->length - 1)
    {
        memmove(&list->data[index], &list->data[index + 1],
                (list->length - index - 1) * sizeof(void *));
    }

    list->size--;
    list->length--;

    // check minimum size.
    list->size = (list->size <= U_ARRAYLIST_DEFAULT_SIZE) ? U_ARRAYLIST_DEFAULT_SIZE : list->size;

    if (!(list->data = (void **) realloc(list->data, list->size * sizeof(void *))))
    {
        return NULL;
    }

    return removed;
}

uint32_t u_arraylist_length(const u_arraylist_t *list)
{
    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "Invalid Parameter");
        return 0;
    }
    return list->length;
}

bool u_arraylist_contains(const u_arraylist_t *list,const void *data)
{
    uint32_t i = 0;

    if (!list)
    {
        return false;
    }

    uint32_t length = u_arraylist_length(list);

    for (i = 0; i < length; i++)
    {
        if (data == u_arraylist_get(list, i))
        {
            return true;
        }
    }

    return false;
}

// Assumes elements are shallow (have no pointers to allocated memory)
void u_arraylist_destroy(u_arraylist_t *list)
{
    if (!list)
    {
        return;
    }
    uint32_t len = u_arraylist_length(list);
    for (uint32_t i = 0; i < len; i++)
    {
        OICFree(u_arraylist_get(list, i));
    }
    (void)u_arraylist_free(&list);
}
