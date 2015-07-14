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

#include <cstdint>
#include <memory>

#include <octypes.h>

namespace OIC
{
    namespace Service
    {
        class ResourceAttributes;

        class RequestHandler;
        class SetRequestHandler;

        class PrimitiveGetResponse
        {
        public:
            static PrimitiveGetResponse defaultAction();

            static PrimitiveGetResponse create(const OCEntityHandlerResult&, int errorCode);

            static PrimitiveGetResponse create(const ResourceAttributes&);
            static PrimitiveGetResponse create(const ResourceAttributes&,
                    const OCEntityHandlerResult&, int errorCode);

            static PrimitiveGetResponse create(ResourceAttributes&&);
            static PrimitiveGetResponse create(ResourceAttributes&&, const OCEntityHandlerResult&,
                    int errorCode);

            RequestHandler* getHandler() const;

        private:
            PrimitiveGetResponse(std::shared_ptr< RequestHandler >&&);

        private:
            std::shared_ptr< RequestHandler > m_handler;
        };

        class PrimitiveSetResponse
        {
        public:
            enum class AcceptanceMethod
            {
                DEFAULT,
                ACCEPT,
                IGNORE
            };

            static PrimitiveSetResponse defaultAction();

            static PrimitiveSetResponse accept();
            static PrimitiveSetResponse accept(const OCEntityHandlerResult&, int errorCode);

            static PrimitiveSetResponse ignore();
            static PrimitiveSetResponse ignore(const OCEntityHandlerResult&, int errorCode);

            static PrimitiveSetResponse create(const OCEntityHandlerResult&, int errorCode);

            static PrimitiveSetResponse create(const ResourceAttributes&);
            static PrimitiveSetResponse create(const ResourceAttributes&,
                    const OCEntityHandlerResult&, int errorCode);

            static PrimitiveSetResponse create(ResourceAttributes&&);
            static PrimitiveSetResponse create(ResourceAttributes&&, const OCEntityHandlerResult&,
                    int errorCode);

            SetRequestHandler* getHandler() const;

            AcceptanceMethod getAcceptanceMethod() const;

            PrimitiveSetResponse& setAcceptanceMethod(AcceptanceMethod);

        private:
            PrimitiveSetResponse(std::shared_ptr< SetRequestHandler >&&);
            PrimitiveSetResponse(std::shared_ptr< SetRequestHandler >&&, AcceptanceMethod);

        private:
            AcceptanceMethod m_acceptanceMethod;
            std::shared_ptr< SetRequestHandler > m_handler;
        };
    }
}

#endif
