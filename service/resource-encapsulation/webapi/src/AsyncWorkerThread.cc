/* *****************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
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
******************************************************************************/

#include "AsyncWorkerThread.h"

#include<string>
#include<iostream>

using namespace std;

void WorkResourceDiscoveredAsyncComplete(uv_work_t *req, int status)

{
    if (NULL == req)
    {
        cout << "req is NULL" << endl;
        return;
    }

    Isolate *isolate = Isolate::GetCurrent();
    DiscoveryEventWork *work = static_cast<DiscoveryEventWork *>(req->data);
    if (NULL == work)
    {
        cout << "work is NULL" << endl;
        return;
    }

    std::string context = work->context;

    Local<Value> argv[1] = {String::NewFromUtf8(isolate, context.c_str())};

    Local<Function>::New(isolate, work->callback)->
    Call(isolate->GetCurrentContext()->Global(), 1, argv);

    delete work;

}

void WorkResourceStateChangedAsyncComplete(uv_work_t *req, int status)

{
    if (NULL == req)
    {
        cout << "req is NULL" << endl;
        return;
    }

    Isolate *isolate = Isolate::GetCurrent();
    StatusEventWork  *work = static_cast<StatusEventWork *>(req->data);
    if (NULL == work)
    {
        cout << "work is NULL" << endl;
        return;
    }


    std::string context = work->context;

    int resStatus = work->status;

    Local<Value> argv[2] = {String::NewFromUtf8(isolate, context.c_str()), Integer::New(isolate, resStatus) };

    Local<Function>::New(isolate, work->callback)->
    Call(isolate->GetCurrentContext()->Global(), 2, argv);

    delete work;

}

void WorkCacheUpdatedAsyncComplete(uv_work_t *req, int status)

{
    if (NULL == req)
    {
        cout << "req is NULL" << endl;
        return;
    }


    Isolate *isolate = Isolate::GetCurrent();
    AttrChangeEventWork *work = static_cast<AttrChangeEventWork *>(req->data);
    if (NULL == work)
    {
        cout << "work is NULL" << endl;
        return;
    }


    std::string context = work->context;

    Local<Object> attrMap = work->attrMap;

    Local<Value> argv[2] = { String::NewFromUtf8(isolate, context.c_str()), attrMap };

    Local<Function>::New(isolate, work->callback)->
    Call(isolate->GetCurrentContext()->Global(), 2, argv);

    delete work;

}

void WorkAttributesReceivedAsyncComplete(uv_work_t *req, int status)

{
    if (NULL == req)
    {
        cout << "req is NULL" << endl;
        return;
    }

    Isolate *isolate = Isolate::GetCurrent();

    AttrChangeEventWork *work = static_cast<AttrChangeEventWork *>(req->data);
    if (NULL == work)
    {
        cout << "work is NULL" << endl;
        return;
    }

    std::string context = work->context;

    Local<Object> attrMap = work->attrMap;

    Local<Value> argv[2] = { String::NewFromUtf8(isolate, context.c_str()), attrMap };

    Local<Function>::New(isolate, work->callback)->
    Call(isolate->GetCurrentContext()->Global(), 2, argv);

    delete work;

}

void WorkAsync(uv_work_t *req)
{
    // cout << "workAsync API" << endl;
}

