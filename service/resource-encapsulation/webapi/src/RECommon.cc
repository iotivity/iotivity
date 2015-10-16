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

#include "RECommon.h"


static std::map<std::string, std::shared_ptr<RCSRemoteResourceObject>> gRemoteResouceObjectMap;

void SetRCSRemoteResourceObject(std::string handle, std::shared_ptr<RCSRemoteResourceObject> &ptr)
{
    if (gRemoteResouceObjectMap.end() == gRemoteResouceObjectMap.find(handle))
    {
        gRemoteResouceObjectMap[handle] = ptr;
    }
}

std::shared_ptr<RCSRemoteResourceObject> GetRCSRemoteResourceObject(std::string handle)
{
    if (gRemoteResouceObjectMap.end() != gRemoteResouceObjectMap.find(handle))
    {

        return gRemoteResouceObjectMap[handle];
    }

    return nullptr;
}


std::string CreateJSHandle(std::string address, std::string uri)
{
    std::string handle = address + uri;
    std::cout << handle.c_str() << std::endl;
    return handle;
}