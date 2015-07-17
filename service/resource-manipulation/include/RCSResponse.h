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

#ifndef SERVERBUILDER_RCSRESPONSE_H
#define SERVERBUILDER_RCSRESPONSE_H

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

        class RCSGetResponse
        {
        public:
            static RCSGetResponse defaultAction();

            static RCSGetResponse create(const OCEntityHandlerResult&, int errorCode);

            static RCSGetResponse create(const ResourceAttributes&);
            static RCSGetResponse create(const ResourceAttributes&,
                    const OCEntityHandlerResult&, int errorCode);

            static RCSGetResponse create(ResourceAttributes&&);
            static RCSGetResponse create(ResourceAttributes&&, const OCEntityHandlerResult&,
                    int errorCode);

            RequestHandler* getHandler() const;

        private:
            RCSGetResponse(std::shared_ptr< RequestHandler >&&);

        private:
            std::shared_ptr< RequestHandler > m_handler;
        };

        class RCSSetResponse
        {
        public:
            enum class AcceptanceMethod
            {
                DEFAULT,
                ACCEPT,
                IGNORE
            };

            static RCSSetResponse defaultAction();

            static RCSSetResponse accept();
            static RCSSetResponse accept(const OCEntityHandlerResult&, int errorCode);

            static RCSSetResponse ignore();
            static RCSSetResponse ignore(const OCEntityHandlerResult&, int errorCode);

            static RCSSetResponse create(const OCEntityHandlerResult&, int errorCode);

            static RCSSetResponse create(const ResourceAttributes&);
            static RCSSetResponse create(const ResourceAttributes&,
                    const OCEntityHandlerResult&, int errorCode);

            static RCSSetResponse create(ResourceAttributes&&);
            static RCSSetResponse create(ResourceAttributes&&, const OCEntityHandlerResult&,
                    int errorCode);

            SetRequestHandler* getHandler() const;

            AcceptanceMethod getAcceptanceMethod() const;

            RCSSetResponse& setAcceptanceMethod(AcceptanceMethod);

        private:
            RCSSetResponse(std::shared_ptr< SetRequestHandler >&&);
            RCSSetResponse(std::shared_ptr< SetRequestHandler >&&, AcceptanceMethod);

        private:
            AcceptanceMethod m_acceptanceMethod;
            std::shared_ptr< SetRequestHandler > m_handler;
        };
    }
}

#endif // SERVERBUILDER_RCSRESPONSE_H
