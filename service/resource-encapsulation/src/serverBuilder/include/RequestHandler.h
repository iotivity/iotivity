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

#ifndef SERVERBUILDER_REQUESTHANDLER_H
#define SERVERBUILDER_REQUESTHANDLER_H

#include <RCSResponse.h>
#include <ResourceAttributesUtils.h>

namespace OC
{
    class OCResourceResponse;
    class OCRepresentation;
}

namespace OIC
{
    namespace Service
    {

        class RCSResourceObject;

        class RequestHandler
        {
        private:
            typedef std::function< std::shared_ptr< OC::OCResourceResponse >(RCSResourceObject&) >
                        BuildResponseHolder;

        public:
            typedef std::shared_ptr< RequestHandler > Pre;

            static constexpr int DEFAULT_ERROR_CODE = 200;
            static constexpr OCEntityHandlerResult DEFAULT_RESULT = OC_EH_OK;

            RequestHandler();

            RequestHandler(const RequestHandler&) = delete;
            RequestHandler(RequestHandler&&) = default;

            RequestHandler(const OCEntityHandlerResult& result, int errorCode);

            RequestHandler(const RCSResourceAttributes&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            RequestHandler(RCSResourceAttributes&&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);


            virtual ~RequestHandler() { };

            std::shared_ptr< OC::OCResourceResponse > buildResponse(RCSResourceObject&);

        private:
            const BuildResponseHolder m_holder;
        };

        class SetRequestHandler: public RequestHandler
        {
        public:
            typedef std::shared_ptr< SetRequestHandler > Ptr;

            SetRequestHandler(const SetRequestHandler&) = delete;
            SetRequestHandler(SetRequestHandler&&) = default;

            SetRequestHandler();

            SetRequestHandler(const OCEntityHandlerResult& result, int errorCode);

            SetRequestHandler(const RCSResourceAttributes&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            SetRequestHandler(RCSResourceAttributes&&,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            AttrKeyValuePairs applyAcceptanceMethod(RCSSetResponse::AcceptanceMethod,
                    RCSResourceObject&, const RCSResourceAttributes&) const;
        };

    }
}

#endif // SERVERBUILDER_REQUESTHANDLER_H
