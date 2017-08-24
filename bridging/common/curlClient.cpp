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

#include "curlClient.h"
#include <iostream>
#include "experimental/logger.h"

using namespace std;
using namespace OC::Bridging;

#define TAG "CURL_CLIENT"

#define DEFAULT_CURL_TIMEOUT_SECONDS     60L


size_t CurlClient::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    MemoryChunk *mem = static_cast<MemoryChunk *>(userp);

    mem->memory = static_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
    if (mem->memory == NULL)
    {
        OIC_LOG(ERROR, TAG, "not enough memory!");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int CurlClient::decomposeHeader(const char *header, std::vector<std::string> &headers)
{
    size_t npos = 0;
    if (NULL == header)
    {
        return MPM_RESULT_INVALID_PARAMETER;
    }

    std::string header_s = header;

    npos = header_s.find("\r\n");
    while (npos != std::string::npos)
    {
        std::string s = header_s.substr(0, npos);
        headers.push_back(s);
        header_s = header_s.substr(npos + 2);
        npos = header_s.find("\r\n");
    }

    return MPM_RESULT_OK;
}

int CurlClient::doInternalRequest(const std::string &url,
                                  const std::string &method,
                                  const std::vector<std::string> &inHeaders,
                                  const std::string &request,
                                  const std::string &username,
                                  std::vector<std::string> &outHeaders,
                                  std::string &response)
{
    int result = MPM_RESULT_OK;
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;
    struct curl_slist *headers = NULL;
    MemoryChunk rsp_body;
    MemoryChunk rsp_header;
    m_lastResponseCode = INVALID_RESPONSE_CODE; //initialize recorded code value in case of
    //early return

    curl = curl_easy_init();
    if (curl != NULL)
    {
        curl_easy_reset(curl);

        for (unsigned int i = 0; i < inHeaders.size(); i++)
        {
            headers = curl_slist_append(headers, inHeaders[i].c_str());
            if (NULL == headers)
            {
                OIC_LOG(ERROR, TAG, "curl_slist_append failed");
                result = MPM_RESULT_OUT_OF_MEMORY;
                goto CLEANUP;
            }
        }

        // Expect the transfer to complete within DEFAULT_CURL_TIMEOUT seconds
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, DEFAULT_CURL_TIMEOUT_SECONDS);

        // Set CURLOPT_VERBOSE to 1L below to see detailed debugging
        // information on curl operations.
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rsp_body);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &rsp_header);
        if (CURLUSESSL_NONE != m_useSsl)
        {
            curl_easy_setopt(curl, CURLOPT_USE_SSL, m_useSsl);
        }

        if (!username.empty())
        {
            curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        }

        if (!method.empty())
        {
            // NOTE: The documentation for CURLOPT_CUSTOMREQUEST only lists HTTP, FTP, IMAP, POP3, and SMTP
            //       as valid options, although it says all this option does is change the string used in
            //       the request. (Basically, don't know whether this option has any effect as currently
            //       used?

            /// only required for GET, PUT, DELETE
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        }

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            OIC_LOG_V(ERROR, TAG, "curl_easy_perform failed with %lu", (unsigned long) res);
            result = MPM_RESULT_NETWORK_ERROR;
            goto CLEANUP;
        }

        if (CURLE_OK != curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &m_lastResponseCode))
        {
            OIC_LOG(WARNING, TAG, "curl_easy_getinfo(CURLINFO_RESPONSE_CODE) failed.");
            m_lastResponseCode = INVALID_RESPONSE_CODE;
        }

        response = rsp_body.memory;

        decomposeHeader(rsp_header.memory, outHeaders);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "curl_easy_init failed");
        result = MPM_RESULT_INTERNAL_ERROR;
    }

CLEANUP:
    if (NULL != headers)
    {
        curl_slist_free_all(headers);
    }

    free(rsp_body.memory);

    free(rsp_header.memory);

    if (NULL != curl)
    {
        curl_easy_cleanup(curl);
    }

    return result;
}
