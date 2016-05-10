//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

/*
 * Persistent storage interface
 *
 * Used for security objects
 */

#ifndef OC_STORAGE_H_
#define OC_STORAGE_H_

#include <stdio.h>

/**
 * Persistent storage handlers. An APP must provide OCPersistentStorage handler pointers
 * when it calls OCRegisterPersistentStorageHandler.
 * Persistent storage open handler points to default file path.
 * Application can point to appropriate SVR database path for it's IoTivity Server.
 */
typedef struct {
    /** Persistent storage file path.*/
    FILE* (* open)(const char *path, const char *mode);

    /** Persistent storage read handler.*/
    size_t (* read)(void *ptr, size_t size, size_t nmemb, FILE *stream);

    /** Persistent storage write handler.*/
    size_t (* write)(const void *ptr, size_t size, size_t nmemb, FILE *stream);

    /** Persistent storage close handler.*/
    int (* close)(FILE *fp);

    /** Persistent storage unlink handler.*/
    int (* unlink)(const char *path);
} OCPersistentStorage;

#endif // OC_STORAGE_H_
