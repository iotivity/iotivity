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

/**
 * @file
 *
 * This file contains the classes for creating Get & Set response for the Get & Set request.
 */
#ifndef SERVERBUILDER_RCSRESPONSE_H
#define SERVERBUILDER_RCSRESPONSE_H

#include <cstdint>
#include <memory>

#include <octypes.h>

namespace OIC
{
    namespace Service
    {
        //forward declaration of classes
        class ResourceAttributes;

        class RequestHandler;
        class SetRequestHandler;

        /**
         * @class   RCSGetResponse
         * @brief   This class provides APIs to create the response for the recieved Get request.
         */
        class RCSGetResponse
        {
        public:
                /**
                * Create RCSGetResponse with default response.
                *
                * @return RCSGetResponse - instance of RCSGetResponse
                */
            static RCSGetResponse defaultAction();

                /**
                * Create RCSGetResponse using OCEntityHandlerResult value and error code provided.
                *
                * @param result -OCEntityHandlerResult
                *
                * @param errorCode - error code to set in response
                *
                * @return RCSGetResponse - instance of RCSGetResponse
                *
                * NOTE : OCEntityHandlerResult is defined in octypes.h
                */
                static RCSGetResponse create(const OCEntityHandlerResult &result, int errorCode);

                /**
                * Create RCSGetResponse using resource attributes.
                *
                * @param attrs -ResourceAttributes to set
                *
                * @return RCSGetResponse - instance of RCSGetResponse
                *
                * @see ResourceAttributes
                *
                * NOTE : ResourceAttributes is defined in ResourceAttributes.h
                */
                static RCSGetResponse create(const ResourceAttributes &attrs);
                /**
                * Create RCSGetResponse using ResourceAttributes, OCEntityHandlerResult and error code.
                *
                * @param attrs - ResourceAttributes to set
                *
                * @param result - OCEntityHandlerResult
                *
                * @param errorCode - error code for response
                *
                * @return RCSGetResponse - instance of RCSGetResponse
                *
                * @see ResourceAttributes
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h.
                *           ResourceAttributes is defined in ResourceAttributes.h.
                */
                static RCSGetResponse create(const ResourceAttributes &attrs,
                                             const OCEntityHandlerResult &result, int errorCode);

                /**
                * Create RCSGetResponse using ResourceAttributes value.
                *
                * @param attrs - ResourceAttributes to set
                *
                * @return RCSGetResponse - instance of RCSGetResponse
                *
                * @see ResourceAttributes
                *
                *NOTE : ResourceAttributes is defined in ResourceAttributes.h.
                */
                static RCSGetResponse create(ResourceAttributes &&attrs);
                /**
                * Create RCSGetResponse using ResourceAttributes value.
                *
                * @param attrs - ResourceAttributes to set
                *
                * @param result - OCEntityHandlerResult
                *
                * @param errorCode - error code for response
                *
                * @return RCSGetResponse - instance of RCSGetResponse
                *
                * @see ResourceAttributes
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h.
                *           ResourceAttributes is defined in ResourceAttributes.h.
                */
                static RCSGetResponse create(ResourceAttributes &&attrs, const OCEntityHandlerResult &result,
                                             int errorCode);

                /**
                * Get the request handler.
                *
                * @return RequestHandler - RequestHandler pointer.
                *
                */
            RequestHandler* getHandler() const;

        private:
            RCSGetResponse(std::shared_ptr< RequestHandler >&&);

        private:
            std::shared_ptr< RequestHandler > m_handler;
        };

        /**
         * @class   RCSSetResponse
         * @brief   This class provides API to create a response for received Set request.
         */
        class RCSSetResponse
        {
        public:
                /**
                 * AcceptanceMethod enum provides values for different acceptance method policy to be
                 * used in setting response.
                 */
                enum class AcceptanceMethod
            {
                DEFAULT,
                ACCEPT,
                IGNORE
            };

                /**
                * Create default RCSSetResponse with default response.
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                */
            static RCSSetResponse defaultAction();

                /**
                * Internally it invokes the defaultAction() API.
                *  It sets the AcceptanceMethod as ACCEPT.
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                * @see AcceptanceMethod
                */
            static RCSSetResponse accept();
                /**
                * Internally it invokes the create(const OCEntityHandlerResult&, int errorCode) API.
                * It sets the AcceptanceMethod as ACCEPT.
                *
                * @param result - OCEntityHandlerResult value.
                *
                * @param errorCode - error code for the response
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                * @see AcceptanceMethod
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h
                */
                static RCSSetResponse accept(const OCEntityHandlerResult &result, int errorCode);

                /**
                * Internally it invokes the defaultAction() API.
                * It sets the AcceptanceMethod as IGNORE.
                *
                * @return RCSSetResponse - instance of RCSSetResponse.
                *
                *  @see AcceptanceMethod
                */
            static RCSSetResponse ignore();
                /**
                * Internaly it invokes the create(const OCEntityHandlerResult&, int errorCode) API.
                * It sets the AcceptanceMethod as IGNORE.
                *
                * @param result - OCEntityHandlerResult value.
                *
                * @param errorCode - error code for the response.
                *
                * @return RCSSetResponse - instance of RCSSetResponse.
                *
                *  @see AcceptanceMethod
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h
                */
                static RCSSetResponse ignore(const OCEntityHandlerResult &result, int errorCode);

                /**
                * Create RCSSetResponse with OCEntityHandlerResult and errorCode.
                *
                * @param result - OCEntityHandlerResult value
                *
                * @param errorCode - error code for the response
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h
                */
                static RCSSetResponse create(const OCEntityHandlerResult &result, int errorCode);

                /**
                * Create RCSSetResponse with ResourceAttributes.
                *
                * @param attrs - ResourceAttributes to set
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                * @see ResourceAttributes
                */
                static RCSSetResponse create(const ResourceAttributes &attrs);
                /**
                * Create RCSSetResponse with ResourceAttributes, OCEntityHandlerResult and errorCode.
                *
                * @param attrs - ResourceAttributes to set.
                *
                * @param result - OCEntityHandlerResult value
                *
                * @param errorCode - error code for the response
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                * @see ResourceAttributes
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h.
                */
                static RCSSetResponse create(const ResourceAttributes &attrs,
                                             const OCEntityHandlerResult &result, int errorCode);

                /**
                * Create RCSSetResponse with ResourceAttributes.
                *
                * @param attrs - ResourceAttributes value to set
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                * @see ResourceAttributes
                */
                static RCSSetResponse create(ResourceAttributes &&attrs);
                /**
                * Create RCSSetResponse with ResourceAttributes, OCEntityHandlerResult and errorCode.
                *
                * @param attrs - ResourceAttributes value to set
                *
                * @param result - OCEntityHandlerResult value
                *
                * @param errorCode - error code for the response
                *
                * @return RCSSetResponse - instance of RCSSetResponse
                *
                * @see ResourceAttributes
                *
                *NOTE : OCEntityHandlerResult is defined in octypes.h.
                */
                static RCSSetResponse create(ResourceAttributes &&attrs, const OCEntityHandlerResult &result,
                                             int errorCode);

                /**
                * API to get the set request handler.
                *
                * @return SetRequestHandler - pointer to SetRequestHandler.
                *
                */
            SetRequestHandler* getHandler() const;

                /**
                * API to get the acceptance method of the  RCSSetResponse.
                *
                * @return AcceptanceMethod - acceptance methods enum value.
                *
                * @see AcceptanceMethod
                *
                */
            AcceptanceMethod getAcceptanceMethod() const;

                /**
                * API to get the acceptance method of the  RCSSetResponse.
                *
                * @param method - AcceptanceMethod value to set
                *
                * @return RCSSetResponse - reference to RCSSetResponse
                *
                * @see AcceptanceMethod
                *
                */
                RCSSetResponse &setAcceptanceMethod(AcceptanceMethod method);

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
