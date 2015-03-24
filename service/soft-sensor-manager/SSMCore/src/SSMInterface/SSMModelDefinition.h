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
#ifndef _SSMModelDefinition_H_
#define _SSMModelDefinition_H_
#include <map>
#include <string>
#include <vector>

#define SSM_MODEL_RETRY 3
typedef enum {SSM_ONCE, SSM_REPEAT} TypeofEvent;
typedef enum {SSM_EVENT_NORMAL, SSM_EVENT_ADDED, SSM_REMOVED, SSM_UPDATED} RESOURCE_EVENT_TYPE;
typedef enum {SENSOR_LOCATION_REMOTE, SENSOR_LOCATION_LOCAL} SENSOR_LOCATION;
class ISSMResource
{
    public:
        ISSMResource()
        {
            location = SENSOR_LOCATION_LOCAL;
        }
        ISSMResource(const std::string &n, const std::string &t) :
            name(n), type(t)
        {
        }
        SENSOR_LOCATION location;
        std::string name;
        std::string type;
        std::string friendlyName;
        std::string ip;
        std::vector<std::string> inputList;
        std::vector<std::map<std::string, std::string> > outputProperty;
};

class ContextData
{
    public:
        std::string rootName;
        int outputPropertyCount;
        std::vector< std::map<std::string, std::string> > outputProperty;
};

struct ResourceResult
{
    std::string deviceID;
    TypeofEvent callType;
    ContextData ctxData;
};

enum CTX_EVENT_TYPE {SPF_START, SPF_UPDATE, SPF_END};

class ICtxEvent
{
    public:
        virtual void onCtxEvent(enum CTX_EVENT_TYPE, std::vector<ContextData>) = 0 ;
        virtual ~ICtxEvent() {};
};

class IEvent
{
    public:
        virtual int onEvent(std::string deviceID, TypeofEvent callType,
                            std::vector<ContextData> ctxData) = 0;
        std::string appId;
        virtual ~IEvent() {};
};

class IResourceEvent
{
    public:
        virtual int onResourceEvent(RESOURCE_EVENT_TYPE eventType, ISSMResource *pSSMResource,
                                    std::string info) = 0;
        virtual ~IResourceEvent() {};
};

class ICtxDelegate
{
    public:
        virtual void registerCallback(ICtxEvent *pEvent) = 0;
        virtual void addOutput(std::vector<ContextData>) = 0;
        virtual void getDataFromDatabase(std::string modelName, int startIndex, int count,
                                         std::vector<ContextData> *data, int *pLastIndex) = 0;
        virtual ~ICtxDelegate() {};
};

#endif
