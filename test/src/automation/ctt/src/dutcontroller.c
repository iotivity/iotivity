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

#include <dutcontroller.h>
#include <mini_http_server.h>
#include <libuv/uv.h>

static resource_map_t* s_resource_map = 0;
static uv_thread_t s_server_id;

char* DocToString(xmlDocPtr doc)
{
    char* ret = 0;

    if (doc != 0)
    {
        xmlChar* xmlbuff;
        int buffersize;
        xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
        ret = (char*)xmlbuff;
    }

    return ret;
}

xmlDocPtr StringToDoc(const char* str)
{
    return xmlParseMemory(str, strlen(str));
}

char* OnHttpRequest(const char* method, const char* url, const char* body)
{
    xmlDocPtr req_doc = 0;
    xmlDocPtr rsp_doc = 0;
    char* ret = 0;
    resource_cbs_t resource_cbs;

    if (body != 0)
    {
        req_doc = StringToDoc(body);

        if (req_doc == 0) goto error;
    }

    resource_cbs = GetResourceCbs(s_resource_map, method, url);

    if (resource_cbs.on_resource)
    {
        rsp_doc = resource_cbs.on_resource(req_doc);
        ret = DocToString(rsp_doc);
    }

    if (resource_cbs.on_after_resource)
    {
        resource_cbs.on_after_resource(rsp_doc);
    }

    goto cleanup;

error:
    printf("Error for %s %s\n", method, url);

cleanup:
    if (req_doc != 0)
    {
        xmlFreeDoc(req_doc);
    }

    return ret;
}

void OnAfterHttpRequest(char* rsp_body)
{
    free(rsp_body);
}

void InitDutController()
{
    s_resource_map = CreateResourceMap();
}

void DisposeDutController()
{
    DeleteResourceMap(s_resource_map);
}

void AddRoute(const char* method, const char* path, resource_cb on_resource, after_resource_cb on_after_resource)
{
    AddResourceCbs(s_resource_map, method, path, on_resource, on_after_resource);
}

void StartDutController(const char* ip, int port)
{
    StartMiniHttpServer(ip, port, OnHttpRequest, OnAfterHttpRequest);
}

void StopDutController()
{
    StopMiniHttpServer();
}
