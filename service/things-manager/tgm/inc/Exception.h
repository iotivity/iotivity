//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file Exception.h

/// @brief

#ifndef __OC_THINGS_GRAPH_EXCEPTION__
#define __OC_THINGS_GRAPH_EXCEPTION__

#include <string>
#include <iostream>

#define GRAPH_EXCEPTION							(int)0x00000001
#define GRAPH_INTERNAL_EXCEPTION				(int)0x00000002
#define GRAPH_DEVICE_NOT_FOUND_EXCEPTION		(int)0x00000003

namespace OC
{

    using namespace std;

    class GraphException
    {
    private:
        int err;
        string msg;

    public:
        GraphException(string detailMessage = "Graph Exception") :
                err(GRAPH_EXCEPTION), msg(detailMessage)
        {
        }
        ~GraphException()
        {
        }

        virtual string what()
        {
            return msg;
        }
    };

    class GraphInternalServiceException: public GraphException
    {
    public:
        GraphInternalServiceException(string detailMessage = "Graph Internal Exception") :
                GraphException(detailMessage)
        {
        }
        ~GraphInternalServiceException()
        {
        }
    };

    class GraphDeviceNotFoundException: public GraphException
    {
    public:
        GraphDeviceNotFoundException(string detailMessage = "Graph Device Not Found Exception") :
                GraphException(detailMessage)
        {
        }
        ~GraphDeviceNotFoundException()
        {
        }
    };

}

#endif // __OC_THINGS_GRAPH_EXCEPTION__
