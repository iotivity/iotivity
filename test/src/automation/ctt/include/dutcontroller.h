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

#ifndef __DUTLIBC_H__
#define __DUTLIBC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <resource_map.h>

#ifdef __cplusplus
extern "C"{
#endif

void InitDutController();
void AddRoute(const char* method, const char* path, resource_cb on_resource, after_resource_cb on_after_resource);
void StartDutController(const char* ip, int port);
void StopDutController();
void DisposeDutController();
xmlDocPtr StringToDoc(const char* str);
char* DocToString(xmlDocPtr doc);

#ifdef __cplusplus
}
#endif
#endif //__DUTLIBC_H__
