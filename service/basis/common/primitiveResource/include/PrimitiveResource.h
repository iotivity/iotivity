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

namespace OIC
{
    namespace Service
    {

        using HeaderOption = OC::HeaderOption::OCHeaderOption;
        using HeaderOptions = std::vector<HeaderOption>;

        class ResourceAttributes;
        class ResponseStatement;

        class PrimitiveResource
        {
        public:
            using Ptr = std::shared_ptr<PrimitiveResource>;

            using GetCallback = std::function<
                    void(const HeaderOptions&, const ResponseStatement&, int)>;

            using SetCallback = std::function<
                    void(const HeaderOptions&, const ResponseStatement&, int)>;

            using ObserveCallback = std::function<
                    void(const HeaderOptions&, const ResponseStatement&, int, int)>;

        public:
            static PrimitiveResource::Ptr create(const std::shared_ptr<OC::OCResource>&);

            virtual ~PrimitiveResource() = default;

            virtual void requestGet(GetCallback) = 0;
            virtual void requestSet(const ResourceAttributes&, SetCallback) = 0;
            virtual void requestObserve(ObserveCallback) = 0;
            virtual void cancelObserve() = 0;

            virtual std::string getUri() const = 0;
            virtual std::string getHost() const = 0;
            virtual std::vector< std::string > getTypes() const = 0;
            virtual std::vector< std::string > getInterfaces() const = 0;

            virtual bool isObservable() const = 0;

        protected:
            PrimitiveResource() = default;

            PrimitiveResource(const PrimitiveResource&) = delete;
            PrimitiveResource(PrimitiveResource&&) = delete;

            PrimitiveResource& operator=(const PrimitiveResource&) const = delete;
            PrimitiveResource& operator=(PrimitiveResource&&) const = delete;
        };

        using DiscoverCallback = std::function<void(std::shared_ptr<PrimitiveResource>)>;

        void discoverResource(const std::string& host, const std::string& resourceURI,
                OCConnectivityType, DiscoverCallback);

    }
}

#endif // __PRIMITIVERESOURCE_H
