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

#ifndef __MINI_HTTP_SERVER__
#define __MINI_HTTP_SERVER__

#ifdef __cplusplus
extern "C"{
#endif

typedef char* (*HttpRequestCb)(const char* method, const char* url, const char* body);
typedef void (*AfterHttpRequestCb)(char* rsp_body);
void StartMiniHttpServer(const char* addr, int port, HttpRequestCb on_http_request, AfterHttpRequestCb on_after_http_request);
void StopMiniHttpServer();

#ifdef __cplusplus
}
#endif

#endif //__MINI_HTTP_SERVER__
