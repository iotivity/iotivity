//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef _OUT_OF_PROC_CLIENT_WRAPPER_H_
#define _OUT_OF_PROC_CLIENT_WRAPPER_H_

#include <OCApi.h>

namespace OC
{
    class OutOfProcClientWrapper : public IClientWrapper
    {
    public:
        OutOfProcClientWrapper(PlatformConfig cfg) { }
	virtual int ListenForResource(const std::string& serviceUrl, const std::string& resourceType, std::function<void(OCResource::Ptr)>& callback) {return 0;}
    };
}

#endif
