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
#ifndef __PRIMITIVERESOURCE_H
#define __PRIMITIVERESOURCE_H

#include <functional>
#include <string>
#include <vector>

#include <OCResource.h>

using HeaderOption = OC::HeaderOption::OCHeaderOption;
using HeaderOptions = std::vector<HeaderOption>;

class ResourceAttributes;
class ResponseStatement;

class PrimitiveResource {
public:
    using Ptr = std::shared_ptr<PrimitiveResource>;

    using GetCallback = std::function<void(const HeaderOptions&, const ResponseStatement&, int)>;

    using SetCallback = std::function<void(const HeaderOptions&, const ResponseStatement&, int)>;

    using ObserveCallback = std::function<void(const HeaderOptions&, const ResponseStatement&, int, int)>;

private:
    using BaseResource = OC::OCResource;
    using BaseResourcePtr = std::shared_ptr<BaseResource>;

public:
    static PrimitiveResource::Ptr create(const BaseResourcePtr&);

    void requestGet(GetCallback);
    void requestSet(const ResourceAttributes&, SetCallback);
    void requestObserve(ObserveCallback);
    void cancelObserve();

// @brief Properties getters.
    std::string getUri() const;
    std::string getHost() const;
    std::vector<std::string> getTypes() const;
    std::vector<std::string> getInterfaces() const;

    bool isObservable() const;

private:
    PrimitiveResource(const BaseResourcePtr&);

private:
    BaseResourcePtr m_ocResource;
};

#endif // __PRIMITIVERESOURCE_H
