//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//

#ifndef _CURLCLIENT_H_
#define _CURLCLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <stdexcept>
#include "mpmErrorCode.h"
#include "StringConstants.h"

namespace OC
{
    namespace Bridging
    {
        const char CURL_CONTENT_TYPE_JSON[] = "content-type: application/json";
        const char CURL_CONTENT_TYPE_URL_ENCODED[] = "content-type: application/x-www-form-urlencoded";
        const char CURL_HEADER_ACCEPT_JSON[] = "accept: application/json";

        const long INVALID_RESPONSE_CODE = 0;

        class CurlClient
        {

            public:
                enum class CurlMethod
                {
                    GET, PUT, POST, DELETE, HEAD
                };

                virtual ~CurlClient() { }

                long getLastResponseCode()
                {
                    return m_lastResponseCode;
                }

                CurlClient(CurlMethod method, const std::string &url)
                {
                    if (url.empty())
                    {
                        throw "Curl method or url is empty";
                    }

                    m_method = getCurlMethodString(method);
                    m_url = url;
                    m_useSsl = CURLUSESSL_TRY;
                }

                CurlClient &setRequestHeaders(std::vector<std::string> &requestHeaders)
                {
                    m_requestHeaders = requestHeaders;
                    return *this;
                }

                CurlClient &addRequestHeader(const std::string &header)
                {
                    m_requestHeaders.push_back(header);
                    return *this;
                }

                CurlClient &setUserName(const std::string &userName)
                {
                    m_username = userName;
                    return *this;
                }

                CurlClient &setRequestBody(std::string &requestBody)
                {
                    m_requestBody = requestBody;
                    return *this;
                }

                CurlClient &setUseSSLOption(curl_usessl sslOption)
                {
                    m_useSsl = sslOption;
                    return *this;
                }

                int send()
                {
                    return doInternalRequest(m_url, m_method, m_requestHeaders, m_requestBody, m_username, m_outHeaders,
                                             m_response);
                }

                std::string getResponseBody()
                {
                    return m_response;
                }

                std::vector<std::string> getResponseHeaders()
                {
                    return m_outHeaders;
                }


            private:

                std::string getCurlMethodString(CurlMethod method)
                {
                    if (method == CurlMethod::GET)          return OC::PlatformCommands::GET;
                    else if (method == CurlMethod::PUT)     return OC::PlatformCommands::PUT;
                    else if (method == CurlMethod::POST)    return OC::PlatformCommands::POST;
                    else if (method == CurlMethod::DELETE)  return OC::PlatformCommands::DELETE;
                    else if (method == CurlMethod::HEAD)    return "HEAD";

                    else throw std::runtime_error("Invalid CurlMethod");
                }

                std::string m_url;
                std::string m_method;
                std::vector<std::string> m_requestHeaders;
                std::string m_requestBody;
                std::string m_username;
                std::string m_response;
                std::vector<std::string> m_outHeaders;

                /// Indicates whether to use CURLOPT_USE_SSL option in doInternalRequest.
                /// Curl default is no SSL (CURLUSESSL_NONE). Specify one of the other CURL SSL options
                /// (for example, CURLUSESSL_TRY) if you need to perform SSL transactions.
                curl_usessl m_useSsl;

                static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

                // Represents contiguous memory to hold a HTTP response.
                typedef struct _MemoryChunk
                {
                    _MemoryChunk() : size(0)
                    {
                        memory = static_cast<char *>(malloc(1));
                    }

                    char *memory;
                    size_t size;

                } MemoryChunk;

                int decomposeHeader(const char *header, std::vector<std::string> &headers);


                int doInternalRequest(const std::string &url,
                                      const std::string &method,
                                      const std::vector<std::string> &inHeaders,
                                      const std::string &request,
                                      const std::string &username,
                                      std::vector<std::string> &outHeaders,
                                      std::string &response);

                long m_lastResponseCode;
        };
    } // namespace Bridging
}  // namespace OC
#endif // _CURLCLIENT_H_
