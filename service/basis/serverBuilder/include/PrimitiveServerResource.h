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

#ifndef __PRIMITIVESERVERRESOURCE_H
#define __PRIMITIVESERVERRESOURCE_H

class PrimitiveServerResource
{
    private:
        PrimitiveServerResource();
        PrimitiveServerResource(OCResourceHandle baseResourceHandle);
        OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request);

    public:
        class Builder
        {
            public:
                Builder(const std::string &uri, const std::string &type,
                        const std::string &interface);

                Builder &setDiscoverable(bool discoverable);
                Builder &setObservable(bool observable);

                Builder &setAttributes(const ResourceAttributes &attrs);

                PrimitiveServerResource create() const;

            private:
                std::string m_uri;
                std::string m_type;
                std::string m_interface;
                uint8_t m_property;
        };

        typedef std::function <
        PrimitiveResponse(const PrimitiveRequest &, const ResourceAttributes &) > GetRequestHandler;
        typedef std::function <
        PrimitiveResponse(const PrimitiveRequest &, const ResourceAttributes &) > SetRequestHandler;

        template<typename T>
        void setAttribute(const std::string &key, const T &value);

        template<typename T>
        T getAttribute(T &key) const;

        bool hasAttribute(const std::string &key) const;

        const ResourceAttributes &getAttributes() const;
        ResourceAttributes &getAttributes();

        bool isObservable() const;
        bool isDiscoverable() const;

        void setGetRequestHandler(GetRequestHandler);
        void setSetRequestHandler(SetRequestHandler);

        void notify();

        std::string getUri() const;
        std::vector<std::string> getTypes() const;
        std::vector<std::string> getInterfaces() const;

    private:
        OCResourceHandle m_resourceHandle;
};


#endif
