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

#include <string>
#include <vector>

#include "rest_request_handler.h"
#include "resource_request_handler.h"

namespace webservice
{

    typedef Singleton<RestRequestHandler, LeakySingletonTraits<RestRequestHandler> >
    RestRequestHandlerSingleton;
    static RestRequestHandler *rest_request_handler_instance = NULL;
    static ResourceRequestHandler *rsc_req_handler = NULL;

    /**
       * Returns pointer to RestRequestHandler singleton object
       * @param void
       * @return RestRequestHandler*
       */
    RestRequestHandler *RestRequestHandler::GetInstance()
    {
        if (NULL == rest_request_handler_instance)
        {
            rest_request_handler_instance = RestRequestHandlerSingleton::get();
            if (!rsc_req_handler)
            {
                rsc_req_handler = ResourceRequestHandler::GetInstance();
            }
        }
        return rest_request_handler_instance;
    }

    /**
       * Deletes RestRequestHandler singleton object
       * @param void
       * @return void
       */
    void RestRequestHandler::DestroyInstance()
    {
        static base::Lock destroy_lock;
        destroy_lock.Acquire();
        if (NULL != rest_request_handler_instance)
        {
            if (rsc_req_handler)
            {
                rsc_req_handler->DestroyInstance();
            }
            delete rest_request_handler_instance;
            rest_request_handler_instance = NULL;
        }
        destroy_lock.Release();
    }

    /**
       * Initializes the rest framework module
       * @param void
       * @return bool
       */
    bool RestRequestHandler::Init(void)
    {
        if (rsc_req_handler)
        {
            return rsc_req_handler->Init();
        }
        return false;
    }

    /**
       * Un-initializes the rest framework module
       * @param void
       * @return bool
       */
    bool RestRequestHandler::DeInit(void)
    {
        if (rsc_req_handler)
        {
            return rsc_req_handler->DeInit();
        }
        return false;
    }

    /**
       * Processes the URI request and updates o/p parameter
       *                   (response_code, content_type) with result
       * @param[in] req_type : http request type
       * @param[in] req_uri : request uri
       * @param[in] query_parameters : http query params
       * @param[in] req_body : json request body
       * @param[in] header_request_info : extra http header information
       * @param response_code : http response code
       * @param header_response_info : http response headers from SLEP
       * @return string
       */
    web_util::RestResponse RestRequestHandler::ProcessRequest(int req_type, std::string req_uri,
            std::string query_parameters,
            std::string req_body, const web_util::HttpHeaderData &http_request_info, int *response_code,
            web_util::HttpHeaderData *http_response_info, std::string *response)
    {
        if (rsc_req_handler)
        {
            return rsc_req_handler->ProcessRequest(req_type, req_uri, query_parameters, req_body,
                                                   http_request_info, response_code, http_response_info, response);
        }
        return web_util::kRestFwUnknownError;
    }
}
