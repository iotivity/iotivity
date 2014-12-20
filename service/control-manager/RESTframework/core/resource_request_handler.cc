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

#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "base/values.h"
#include "base/version.h"
#include "base_resource/base_simple_object.h"
#include "http_body/creator_http_body_generator.h"
#include "http_body/creator_http_body_parser.h"
#include "http_body/interface_http_body_generator.h"
#include "http_body/interface_http_body_parser.h"
#include "http_header/http_header_defined.h"
#include "http_header/http_header_wrapper_functions.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code.h"
#include "http_start_line/http_status_code_defined.h"
#include "resource_request_handler.h"
#include "resources.h"
#include "web_cache/web_cache_manager.h"
#include "web_cache/web_cache_wrapper_functions.h"
#include "web_filter/web_filter_wrapper_functions.h"
#include "base/logging.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    static const char *const   kSessionFieldName               = "X-API-SessionID";
    static const char *const   kApiVersion                     = "X-API-Version";
    static const char *const   kETagField    = "ETag";

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    typedef Singleton<ResourceRequestHandler, LeakySingletonTraits<ResourceRequestHandler> >ResourceRequestHandlerSingleton;

    ResourceRequestHandler *ResourceRequestHandler::resource_handler_instance_ = NULL;
    SimpleRoot *ResourceRequestHandler::root_object_ = NULL;
    web_util::RestFwState ResourceRequestHandler::rest_fw_state_ = web_util::kRestFwUnknownState;
    /**
       * Returns pointer to ResourceRequestHandler singleton object
       * @param void
       * @return ResourceRequestHandler*
       */
    ResourceRequestHandler *ResourceRequestHandler::GetInstance()
    {
        if (NULL == resource_handler_instance_)
        {
            resource_handler_instance_ = ResourceRequestHandlerSingleton::get();
            ResourceRequestHandler::SetSupportableSHPVersion(resource_handler_instance_);
            ResourceRequestHandler::RegisterDefaultResources();
        }
        return resource_handler_instance_;
    }

    /**
       * Deletes ResourceRequestHandler singleton object
       * @param void
       * @return void
       */
    void ResourceRequestHandler::DestroyInstance()
    {
        static base::Lock destroy_lock;
        destroy_lock.Acquire();
        if (NULL != resource_handler_instance_)
        {
            // delete the control manager wrapper instance from here.
            // this will be instantiated when a http request is received.
            ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
            if (cm_wrapper)
            {
                cm_wrapper->DestroyInstance();
            }
            // resource_handler_list_ is the
            // private static member of  ResourceRequestHandler,
            // so it is cleared/freed if ResourceRequestHandler's singleton
            // instance is destroyed
            delete resource_handler_instance_;
            resource_handler_instance_ = NULL;
        }
        destroy_lock.Release();
    }

    /**
       * Initializes the rest framework module
       * @param void
       * @return bool
       */
    bool ResourceRequestHandler::Init(void)
    {
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            if (cm_wrapper->Init())
            {
                rest_fw_state_ = web_util::kRestFwInitialized;
                return true;
            }
        }
        return false;
    }

    /**
       * Un-initializes the rest framework module
       * @param void
       * @return bool
       */
    bool ResourceRequestHandler::DeInit(void)
    {
        LOG(ERROR) << "DeInit Enter";
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            if (cm_wrapper->DeInit())
            {
                rest_fw_state_ = web_util::kRestFwUnInitialized;
                return true;
            }
        }
        return false;
    }

    /**
       * Function pointer to functions which are generated by script
       */
    void ResourceRequestHandler::RegisterDefaultResources()
    {
        Resources::RegisterResources();
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
    web_util::RestResponse ResourceRequestHandler::ProcessRequest(int req_type, std::string req_uri,
            std::string query_parameters,
            std::string req_body, const web_util::HttpHeaderData &http_request_info, int *response_code,
            web_util::HttpHeaderData *http_response_info, std::string *response)
    {
        DLOG(INFO) << "req_uri:: " << req_uri << "  req_type: " << req_type;
        if (rest_fw_state_ != web_util::kRestFwInitialized)
        {
            LOG(ERROR) << "rest framework is uninitialized";
            *response = "Internal Server Error";
            return web_util::kRestFwUnInitializedError;
        }
        // Output response value is NULL!
        if (NULL == response_code || NULL == http_response_info)
        {
            LOG(ERROR) << "response value is NULL! You should check webserver caller function!";
            LOG(ERROR) <<
                       "stub_->Send logic in RequestReceiver::ProcessRequest function of request_receiver.cc";
            *response = "Internal Server Error";
            return web_util::kRestFwProcessRequstFailed;
        }

        // Process Request Line : type, uri, query parameters
        const HTTPRequestLine http_request_line(req_type, req_uri, query_parameters);

        // Process Request Header
        const HTTPRequestHeader http_request_header(http_request_info);

        // Response Header
        HTTPResponseHeader http_response_header;

        // Find object and Process request object
        std::string response_body = ProcessRequestObject(http_request_line, http_request_header, req_body,
                                    http_response_header, response_code);
        LOG(ERROR) << "response_body:: " << response_body;

        // Change data type to defined data type
        http_response_header.CopyHTTPResponseHeader(*http_response_info);
        *response = response_body;

        return web_util::kRestFwProcessRequstSuccess;
    }

    /**
       * Processes the URI request and updates o/p parameter
       *                   (response_code, content_type) with result
       * @param[in] request_line : http request (type, uri, query parameter)
       * @param[in] request_header : http request header
       * @param[in] request_body : request body
       * @param[out] response_header : http response header
       * @param[out] response_code : http status code
       * @return string
       */
    std::string ResourceRequestHandler::ProcessRequestObject(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header,
            const std::string &request_body, HTTPResponseHeader &response_header, int *response_code)
    {
        // http request body which is defined to dictionary value type
        base::DictionaryValue *request_root = NULL;
        // http response body which is defined to dictionary value type
        base::DictionaryValue *response_root = NULL;
        std::string response_body = "";
        // Change format for http request
        HttpRequest req(const_cast<HTTPRequestLine *>(&request_line),
                        const_cast<HTTPRequestHeader *>(&request_header), NULL);
        // Change format for http response
        HttpResponse res(const_cast<HTTPResponseHeader *>(&response_header), NULL);
        // Merge http request and response
        HTTPMessage http_message(req, res);
        //
        bool is_etag_present = false;
        //
        bool is_uri_present_in_cache = false;

        // Check API version validity
        const std::string client_version = request_header.GetClientSHPVersion();
        bool is_api_version_validity = IsVersionValidity(client_version, response_header);
        if (true != is_api_version_validity)
        {
            LOG(ERROR) << "Version mismatch!";
            // Make Response body & status line
            return MakeResponseObject(request_header, response_root, RT_400_NOT_SUPPORTED_X_API_VERSION,
                                      response_code, response_header);
        }
        // Check Session Validity
        const std::string session_id = request_header.GetSessionId();
        bool is_session_validity = IsSessionValidity(request_line, session_id);
        if (true != is_session_validity)
        {
            // Make Response body & status line
            return MakeResponseObject(request_header, response_root, RT_400_NOT_SUPPORTED_X_API_SESSION_ID,
                                      response_code, response_header);
        }

        // Check Web Cache
        const std::string etag = request_header.GetETag();
        bool is_matched = WebCacheWrapperFunctions::ProcessWebCache(request_line, etag, &is_etag_present,
                          &is_uri_present_in_cache);
        if (is_matched)
        {
            DLOG(INFO) << "URI and ETag matches";
            // Make Response body & status line
            return MakeResponseObject(request_header, response_root, RT_304_NOT_MODIFIED, response_code,
                                      response_header);
        }

        // Find Simple object
        std::map<std::string, std::string> id_deps;
        BaseSimpleObject *pSimpleObj = FindSimpleObject(request_line.GetUriFields(), id_deps,
                                       &http_message);
        if (NULL == pSimpleObj || (enSimpleLinkedChild == pSimpleObj->GetSimpleObjectType()))
        {
            // Make Response body & status line
            return MakeResponseObject(request_header, response_root, RT_404_NOT_FOUND_URI, response_code,
                                      response_header);
        }

        // Check 405 Method Not Allowed
        const int &req_type = request_line.GetHTTPMethod();
        if (!pSimpleObj->GetMethodAllowed(req_type))
        {
            // Make Response body & status line
            DLOG(ERROR) << "405 Method Not Allowed in resource_request_handler";
            return MakeResponseObject(request_header, response_root, RT_405_METHOD_NOT_ALLOWED, response_code,
                                      response_header);
        }

        // Parse Request body
        HTTPResponseType response_error_code = ParseRequestBody(request_line, request_header, request_body,
                                               &request_root);

        // Process Request
        if (RT_200_START > response_error_code || RT_300_START <= response_error_code)
        {
            // Make Response body & status line
            return MakeResponseObject(request_header, response_root, response_error_code, response_code,
                                      response_header);
        }

        base::Value *request_child = NULL;
        base::Value *response_child = NULL;
        const std::string &request_uri = request_line.GetOriginUri();
        // Remove request key of json type to process request body as dictionary type.
        if (!pSimpleObj->ParseRequestDict(req_type, request_root, &request_child))
        {
            // Make Response body & status line
            return MakeResponseObject(request_header, response_root, RT_400_INCORRECT_JSON_KEY, response_code,
                                      response_header);
        }
        // Add response key of json type and create it to process response body as dictionary type.
        response_root = pSimpleObj->CreateResponseDict(&response_child);
        // Process found simple object, which create each entire object(identified_object_xxx or resource_xxx ...)
        response_error_code = pSimpleObj->ProcessRequest(request_line, request_header, request_child,
                              response_header, response_child, id_deps, request_uri);

        // Process Web Filter
        base::DictionaryValue *filtered_response;
        DictionaryValue *web_filter_response = new DictionaryValue;

        std::vector<std::string> list_of_key;
        if (request_line.GetFilterKeyString(&list_of_key))
        {
            if (WebFilterWrapperFunctions::ProcessWebFilter(req_type, &list_of_key, response_error_code,
                    response_root, web_filter_response)) filtered_response = web_filter_response;
            else filtered_response = response_root;
        }
        else
        {
            filtered_response = response_root;
        }

        // Make Response body & status line
        response_body = MakeResponseObject(request_header, filtered_response, response_error_code,
                                           response_code, response_header);

        web_filter_response->Clear();
        delete web_filter_response;

        pSimpleObj->ReleaseRequestDict(request_root);
        pSimpleObj->ReleaseResponseDict(response_root);

        // Update the cache entry and add ETag header to the response
        WebCacheWrapperFunctions::UpdateCache(request_uri, req_type, is_etag_present,
                                              is_uri_present_in_cache, response_error_code, response_header);

        return response_body;
    }

    /**
       * Parse http request body to dictionary type from string type
       * @param[in] request_line : http request (type, uri, query parameter)
       * @param[in] request_header : http request header
       * @param[in] in_request_body : request body
       * @param[out] out_request_body : type casting to dictionary type
       * @return HTTPResponseType
       */
    HTTPResponseType ResourceRequestHandler::ParseRequestBody(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, const std::string &in_request_body,
            base::DictionaryValue **out_request_body)
    {
        // Check method
        const int request_method = request_line.GetHTTPMethod();
        if (web_util::HTTP_METHOD_GET == request_method  || web_util::HTTP_METHOD_DELETE == request_method)
        {
            // Get & Delete method does not need request body
            return RT_200_OK;
        }
        else if (web_util::HTTP_METHOD_PUT == request_method
                 || web_util::HTTP_METHOD_POST == request_method)
        {
            // Check string size
            if ("" == in_request_body || 0 == in_request_body.size())
            {
                return RT_400_REQUEST_BODY_SIZE_IS_ZERO;
            }

            CreatorHTTPBodyParser &factory = CreatorHTTPBodyParser::GetInstance();
            scoped_ptr<IHTTPBodyParser> pObj(factory.CreateHTTPBodyParser(request_header.GetContentType()));
            if (pObj.get())
            {
                // JSON, XML, ...
                if (pObj->ParseRequestBody(in_request_body, out_request_body)) return RT_200_OK;
                else return RT_400_INCORRECT_BODY;
            }
            else return RT_415_UNSUPPORTED_MEDIA_TYPE;
        }
        else return RT_405_METHOD_NOT_ALLOWED;
    }

    /**
       * Find object using uri
       * URI is similar with tree structure, so this is finding leaf of tree.
       * @param[in] uri_fields : uri which is -redefined to vector type
       * @param[out] id_deps : save id for identified object
       * @return BaseSimpleObject ptr
       */
    BaseSimpleObject *ResourceRequestHandler::FindSimpleObject(const std::vector<std::string>
            uri_fields, std::map<std::string, std::string> &id_deps, HTTPMessage *http)
    {
        base::AutoLock locked(lock_for_simple_object_);
        std::vector<std::string>::const_iterator it;
        BaseSimpleObject *pObj = root_object_ ? root_object_ : &(SimpleRoot::GetInstance());
        if (NULL == pObj) return NULL;

        for (it = uri_fields.begin(); it != uri_fields.end(); it++)
        {
            BaseSimpleObject *pChildObj = pObj->FindChildOfSimpleObject(*it, http);
            if (NULL != pChildObj)
            {
                pObj = pChildObj;
                if (enSimpleIdentifiedObject == pChildObj->GetSimpleObjectType())
                {
                    std::string name = pChildObj->GetObjectName() + "_id";
                    std::string id = *it;
                    id_deps.insert(std::pair<std::string, std::string>(name, id));
                    DLOG(INFO) << "IDs(" << name << ", " << id << ")";
                }
            }
            else
            {
                LOG(ERROR) << "Request URIs is incorrect : " << *it;
                return NULL;
            }
        }
        return pObj;
    }

    /**
       * Make http response data
       * @param[in] request_header : http request header, this need to negotiate content type
       * @param[in] in_response_body : response body which is defeined to Dictionary type
       * @param[in] response_error_code : internally error type
       * @param[out] response_status_code : http status code for response
       * @param[out] response_header : http response header
       * @return string : made response data for http response
       */
    std::string ResourceRequestHandler::MakeResponseObject(const HTTPRequestHeader &request_header,
            const base::DictionaryValue *const in_response_body,
            HTTPResponseType response_error_code, int *response_status_code,
            HTTPResponseHeader &response_header)
    {
        std::string out_response_body = "";

        // Negotiation content-type
        const std::string &accpet = request_header.GetAccept();
        HttpContentType content_type = HttpHeaderWrapperFunctions::NegotiateContentType(accpet);

        // Generate body
        CreatorHTTPBodyGenerator &factory = CreatorHTTPBodyGenerator::GetInstance();
        scoped_ptr<IHTTPBodyGenerator> p_response_body_object(factory.CreateObject(content_type));
        if (NULL == p_response_body_object.get())
        {
            // Exception Case, http body generator is not get
            HTTPStatusCode http_status_code(RT_406_NOT_ACCEPTABLE_ACCEPT);
            *response_status_code = http_status_code.GenerateHTTPStatusCode();
            // Set "Content-Type" for response data
            response_header.SetContentType(kHttpContentTypeTextPlain);
            out_response_body = kNotAcceptableAccept;
            return out_response_body;
        }

        // Parse status code
        HTTPStatusCode http_status_code(response_error_code);
        *response_status_code = http_status_code.GenerateHTTPStatusCode();
        if (http_status_code.IsSuccessfulStatus() && in_response_body != NULL)
        {
            // 200
            out_response_body = p_response_body_object->Generate(in_response_body);
        }
        else
        {
            // 400, 500, 300, 100
            base::DictionaryValue *error_response_body = new base::DictionaryValue;
            http_status_code.MakeHTTPStatusCodeAndDescription(&error_response_body);
            out_response_body = p_response_body_object->Generate(error_response_body);
            delete error_response_body;
        }

        // Set "Content-Type" for response data
        response_header.SetContentType(content_type);

        return out_response_body;
    }

    /**
       * Set supportable SHP version
       * This is for server(gateway/controllee), so this should be called only once in ResourceRequestHandler::GetInstance.
       * @param[in] p_this : this is static function, so we need own pointer.
       * @return void
       */
    void ResourceRequestHandler::SetSupportableSHPVersion(ResourceRequestHandler *p_this)
    {
        const std::string version_min = Resources::GetSupportableVersionMin();
        const std::string version_max = Resources::GetSupportableVersionMax();
        p_this->supportable_version_min_ = BaseVersion(version_min);
        p_this->supportable_version_max_ = BaseVersion(version_max);
    }

    /**
       * This is for checking the API version ( in the "X-API-VERSION" field in HTTP header data )
       * @param[in] client_version : this is client version which is get in "X-API-VERSION".
       * @param[out] response_header : this is to set shp version to http response header
       * @return bool
       */
    bool ResourceRequestHandler::IsVersionValidity(const std::string &client_version,
            HTTPResponseHeader &response_header)
    {
        BaseVersion api_version_of_client = BaseVersion(client_version);
        DLOG(INFO) << "Client BaseVersion : " << client_version;
        // Check client and min api
        if (api_version_of_client.CompareTo(supportable_version_min_) < 0)
        {
            LOG(ERROR) << "Client Version is older (Client : " << client_version
                       << ", Server : " << supportable_version_min_.GetString() << ")";
            return false;
        }
        // Check client and max api
        if (api_version_of_client.CompareTo(supportable_version_max_) > 0)
        {
            LOG(ERROR) << "Server Version is older (Client : " << client_version
                       << ", Server : " << supportable_version_max_.GetString() << ")";
            return false;
        }
        // Set "X-API-Version" for response data
        response_header.SetServerSHPVersion(client_version);
        return true;
    }

    /**
       * This is for checking session id ( in the "X-API-SessionID" field in HTTP header data )
       * @param[in] request_line : this use to set request method and request uri.
       * @param[in] session_id : this is for checking session id
       * @return bool
       */
    bool ResourceRequestHandler::IsSessionValidity(const HTTPRequestLine &request_line,
            const std::string &session_id)
    {
        // TODO returning true always
        // This needs to be implemented
        return true;
    }

// Process the http request by virtual
    std::string ResourceRequestHandler::VirtualProcessRequest(const int req_type,
            const std::string &req_uri, const std::string &session_id,
            const std::string &version, std::string *etag, int *response_code)
    {
        static std::string empty_string = "";
        std::string response;
        web_util::HttpHeaderData http_request_info, http_response_info;
        /*Initialize the header_info structures to default values*/
        for (int iter = 0; iter < web_util::HttpHeaderData::\
             NUM_HEADER_PARAMS; iter++)
        {
            http_request_info.param_name[iter].clear();
            http_request_info.param_value[iter].clear();
            http_request_info.flag_outheader_overwrite[iter] = 0;
            http_request_info.is_this_set[iter] = 0;
        }
        http_request_info.param_name[0] =  kSessionFieldName;
        http_request_info.param_value[0] =  session_id;
        http_request_info.flag_outheader_overwrite[0] = 0;
        http_request_info.is_this_set[0] = 1;

        http_request_info.param_name[1] =  kApiVersion;
        http_request_info.param_value[1] =  version;
        http_request_info.flag_outheader_overwrite[1] = 0;
        http_request_info.is_this_set[1] = 1;
#if defined(ENABLE_WEB_CACHE)
        // set Etag header
        if (etag)
        {
            http_request_info.param_name[2] =  kETagField;
            http_request_info.param_value[2] =  *etag;
            http_request_info.flag_outheader_overwrite[2] = 0;
            http_request_info.is_this_set[2] = 1;
        }
#endif
        web_util::RestResponse rest_response = ProcessRequest(req_type, req_uri, empty_string, empty_string,
                                               http_request_info,
                                               response_code, &http_response_info, &response);
#if defined(ENABLE_WEB_CACHE)
        if (etag)
        {
            if (*response_code >= web_util::kHttpStatusCodeSuccessOk &&
                *response_code < web_util::kHttpStatusCodeRedirectSpecialResponse)
            {
                // Update the 'ETag' value
                for (int iter = 0; iter < web_util::HttpHeaderData::NUM_HEADER_PARAMS; iter++)
                {
                    if (http_response_info.param_name[iter] == kETagField)
                    {
                        etag->clear();
                        *etag = http_response_info.param_value[iter];
                    }
                }
            }
        }
#endif
        return response;
    }

}
