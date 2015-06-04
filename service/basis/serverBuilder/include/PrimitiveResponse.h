//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef __PRIMITIVERESPONSE_H
#define __PRIMITIVERESPONSE_H

class Handler;

class PrimitiveResponse
{
    public:
        static PrimitiveResponse DEFAULT;

        static PrimitiveResponse createResponse(const ResourceAttributes &attrs);

        shared_ptr<Handler> m_handler;
};

class GetDefaultHandler
{
    public:
        OCResponse handle(ServerResource &resource)
        {
            // build response
        }
};

GetRequestHandler m_getRequestHandler;

void onGet()
{
    Request request;
    Attributes attributes;

    if (m_getRequestHandler)
    {
        GetPrimitiveResponse response = m_getRequestHandler(request, attributes);

        return response->getHandler()->handle(*this);
    }
}

#endif
