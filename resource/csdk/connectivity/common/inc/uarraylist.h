/* ****************************************************************
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

#ifndef U_ARRAYLIST_H_
#define U_ARRAYLIST_H_

#include <stdint.h>
#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * array list structure.
 *
 * @note
 * Members should be treated as private and not accessed directly. Instead
 * all access should be through the defined u_arraylist_*() functions.
 */
typedef struct u_arraylist_t
{
    void **data;
    uint32_t length;
    uint32_t size;
} u_arraylist_t;

/**
 * API to creates array list and initializes the elements.
 * @return  u_arraylist_t if Success, NULL otherwise.
 */
u_arraylist_t *u_arraylist_create();

/**
 * Resets and deletes the array list.
 * Arraylist elements are deleted. Calling function must take care of free
 * dynamic memory allocated before freeing the arraylist.
 * @param[in] list       u_arraylist pointer
 * @return ::CAResult_t.
 * ::CA_STATUS_OK if Success, ::CA_STATUS_INVALID_PARAM if pointer to list is NULL.
 */
CAResult_t u_arraylist_free(u_arraylist_t **list);

/**
 * Returns the data of the index from the array list.
 * @param[in] list         pointer of array list.
 * @param[in] index        index of array list.
 * @return void pointer of data if success or NULL pointer otherwise.
 */
void *u_arraylist_get(const u_arraylist_t *list, uint32_t index);

/**
 * Add data in the array list.
 * @param[in] list        pointer of array list.
 * @param[in] data        pointer of data.
 * @return CAResult_t.
 * ::CA_STATUS_OK if Success, ::CA_MEMORY_ALLOC_FAILED if memory allocation fails.
 */
CAResult_t u_arraylist_add(u_arraylist_t *list, void *data);

/**
 * Remove the data of the index from the array list.
 * @param[in] list       pointer of array list.
 * @param[in] index      index of array list.
 * @return void pointer of the data if success or NULL pointer otherwise.
 */
void *u_arraylist_remove(u_arraylist_t *list, uint32_t index);

/**
 * Returns the length of the array list.
 * @param[in] list       pointer of array list.
 * @return length of the array list.
 */
uint32_t u_arraylist_length(const u_arraylist_t *list);

/**
 * Returns whether the data exists or not.
 * @param[in] list       pointer of array list.
 * @param[in] data       pointer of data.
 * @return true if exists, false otherwise.
 */
bool u_arraylist_contains(const u_arraylist_t *list,const void *data);

/**
 * Destroys array list and elements (assuming elements are shallow).
 * @param[in] list       pointer of array list.
 */
void u_arraylist_destroy(u_arraylist_t *list);

#ifdef __cplusplus
}
#endif

#endif /* U_ARRAYLIST_H_ */
