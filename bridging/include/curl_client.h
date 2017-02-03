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

#ifndef __CURL_CLIENT_H__
#define __CURL_CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "gw_error_code.h"

#define CURL_CONTENT_TYPE_JSON         "content-type: application/json"
#define CURL_CONTENT_TYPE_URL_ENCODED  "content-type: application/x-www-form-urlencoded"
#define CURL_HEADER_ACCEPT_JSON        "accept: application/json"
#define INVALID_RESPONSE_CODE           0

#define CURL_GET      "GET"
#define CURL_PUT      "PUT"
#define CURL_POST     "POST"
#define CURL_DELETE   "DELETE"

class CurlClient
{
public:
    CurlClient();

    CurlClient(const curl_usessl useSsl);

    virtual ~CurlClient();

    long getLastResponseCode()
    {
      return m_lastResponseCode;
    }

    CurlClient(const std::string &method, const std::string &url)
    {
        if (method.empty() || url.empty())
        {
          throw "Curl method or url is empty";
        }

        m_method = method;
        m_url = url;
        m_useSsl = CURLUSESSL_TRY;
    }

    CurlClient& setRequestHeaders(std::vector<std::string> &requestHeaders)
    {
      m_requestHeaders = requestHeaders;
      return *this;
    }

    CurlClient& addRequestHeader(const std::string &header)
    {
      m_requestHeaders.push_back(header);
      return *this;
    }

    CurlClient& setUserName(const std::string &userName)
    {
      m_username = userName;
      return *this;
    }

    CurlClient& setRequestBody(std::string &requestBody)
    {
      m_requestBody = requestBody;
      return *this;
    }

    int send()
    {
      return doInternalRequest(m_url, m_method, m_requestHeaders, m_requestBody, m_username, m_outHeaders, m_response);
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

    std::string m_url;
    std::string m_method;
    std::vector<std::string> m_requestHeaders;
    std::string m_requestBody;
    std::string m_username;
    std::string m_response;
    std::vector<std::string> m_outHeaders;

    static size_t WriteCallback(void *contents,
                                size_t size,
                                size_t nmemb,
                                void *userp);

    // Represents contiguos memory to hold a HTTP response.
    typedef struct _MemoryChunk
    {
        _MemoryChunk() : size(0)
        {
            memory = reinterpret_cast<char *>(malloc(1));
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

    /// Indicates whether to use CURLOPT_USE_SSL option in doInternalRequest.
    /// Curl default is no SSL (CURLUSESSL_NONE). Specify one of the other CURL SSL options
    /// (for example, CURLUSESSL_TRY) if you need to perform SSL transactions.
    curl_usessl m_useSsl;
};

#endif
