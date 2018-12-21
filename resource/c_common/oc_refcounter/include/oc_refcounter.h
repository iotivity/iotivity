/******************************************************************
 *
 * Copyright 2018 Kistler Group All Rights Reserved.
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
#ifndef OC_REFCOUNTER_H_
#define OC_REFCOUNTER_H_

#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct oc_refcounter_t *oc_refcounter;

/**
 * Desctructor of a data of a refcounter and it is called when a counter of thet refcounter reach 0.
 */
typedef void (*oc_refcounter_dtor_data_func)(void* data);

/**
 * Creates a new refcounter of a data. Ownership of the data is transferred to the refcounter.
 * For deallocates the data use oc_refcounter_dec, otherwise it can cause crash of the application.
 *
 * @param data - Data which will be owned by refcounter
 * @param dtor - Destructor of a data
 *
 * @return Poiner to newly created refcounter, NULL on allocation failure.
 */
oc_refcounter oc_refcounter_create(void* data, oc_refcounter_dtor_data_func dtor);

/**
 * Increments atommically reference count of a refcounter
 *
 * @param ref - A pointer to the refcounter
 *
 * @return returns the ref
 */
oc_refcounter oc_refcounter_inc(oc_refcounter ref);

/**
 * Decrements atommically a reference count of a refcounter. If the count of the refcounter
 * reach 0, it's call destructor of a data and the refcounter.
 *
 * @param ref - A pointer to the refcounter
 *
 * @return
 *     on count == 0, a null pointer is returned
 *     otherwise returns the refcounter
 */
oc_refcounter oc_refcounter_dec(oc_refcounter ref);

/**
 * Gets a current reference count of the refcount
 *
 * @param ref -  A pointer to the refcounter
 *
 * @return returns a current reference count of the block
 */
int32_t oc_refcounter_get_count(oc_refcounter ref);

/**
 * Gets a data of the refcount
 *
 * @param ref -  A pointer to the refcounter
 *
 * @return returns a data of the refcounter
 */
void* oc_refcounter_get_data(oc_refcounter ref);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // OC_REFCOUNTER_H_
