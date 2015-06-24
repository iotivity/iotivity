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

#include "HueConnector.h"
#include <curl/curl.h>
#include <string.h>
#include <iostream>

using namespace OIC::Service;

HueConnector::HueConnector(){

}

HueConnector::~HueConnector(){

}

void HueConnector::connect(){

}

void HueConnector::disconnect(){

}

std::string HueConnector::transmit(std::string target, std::string payload){
    std::cout << "Transmitting to " << target << " "  << payload << endl;
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;                      /* http headers to send with request */
    /* set content type */
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    const char *cstr = payload.c_str();

    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, target.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cstr);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        /* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
       itself */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(cstr));

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return "";
}

static int writer(char* data, size_t size, size_t nmemb, std::string *buffer_in){
    buffer_in->append(data, size * nmemb);
    return size * nmemb;
}

std::string HueConnector::read(std::string target){
    std::cout << "Reading from to " << target << endl;
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;                      /* http headers to send with request */
    /* set content type */
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");


    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, target.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
        }
        else{
            cout << "Response is: " << response << endl;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return "";
}

