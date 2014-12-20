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

#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include "rest_request_handler.h"
#include "http_header.h"

void ProcessHttpRequest(int accept_fd);
void *AcceptConnections(void *ptr);
bool InitRestFw();
bool DeInitRestFw();
bool DestroyRestFw();
static bool is_stopped = false;
static const int portno = 3115;

const int SIZE = 1024;
const char *kSpace = " ";
const char *kCrlf = "\r\n";
const char *kDoubleCrlf = "\r\n\r\n";

int main(int argc , char *argv[])
{
    // initialize the thread to accept requests

    std::cout << "\nType \"stop\" to exit the execution\n";
    std::string stop = "run";
    pthread_t http_thread;
    if ( pthread_create( &http_thread , NULL , AcceptConnections , (void *)NULL) < 0)
    {
        std::cout << "Thread creation failed" << "\n";
        return 1;
    }
    while (0 != stop.compare("stop"))
    {
        std::cin >> stop;
    }
    is_stopped = true;
    int *resp = NULL;
    pthread_join(http_thread, (void **)&resp);
    return 0;
}

void *AcceptConnections(void *ptr)
{
    int sock_fd, accept_fd, clilen;
    struct sockaddr_in server, client;

    std::cout << "AcceptConnections Entry" << std::endl;
    sock_fd  = socket(AF_INET, SOCK_STREAM, 0);
    // set SO_REUSEADDR on a socket to true
    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portno);

    bind(sock_fd, (struct sockaddr *)&server, sizeof(server));

    listen(sock_fd, 1024);
    // initialize rest framework
    if (!InitRestFw())
    {
        std::cout << "InitRestFw failed" << std::endl;
        return 0;
    }

    while (!is_stopped)
    {
        clilen = sizeof(client);
        accept_fd = accept(sock_fd, (struct sockaddr *)&client, (socklen_t *)&clilen);
        std::cout << "Request accepted" << std::endl;
        ProcessHttpRequest(accept_fd);
        close(accept_fd);
    }
    // un-initialize rest framework
    if (!DeInitRestFw())
    {
        std::cout << "DeInitRestFw failed" << std::endl;
        return 0;
    }
    // destroy rest framework
    if (!DestroyRestFw())
    {
        std::cout << "DestroyRestFw failed" << std::endl;
        return 0;
    }
    close(sock_fd);
    return 0;
}

std::string CreateResponseString(int response_code, const web_util::HttpHeaderData &resp_hdr,
                                 const std::string &response, std::string version)
{
    std::string resp = "";

    for (int header_iter = 0; header_iter < resp_hdr.NUM_HEADER_PARAMS; header_iter++)
    {
        if (resp_hdr.is_this_set[header_iter])
        {
            if (resp_hdr.flag_outheader_overwrite[header_iter] == 0)
            {
                resp = resp + resp_hdr.param_name[header_iter] + ": " + resp_hdr.param_value[header_iter] + kCrlf;
            }
        }
    }

    // resp = resp + "Content-Length : " + std::to_string(response.length()) + kCrlf;

    std::string status_message = "";
    // populate the response string
    switch (response_code)
    {
        case 200: status_message = "OK";
            break;

        case 204: status_message = "No Content";
            break;

        case 400: status_message = "Bad Request";
            break;

        case 404: status_message = "Not Found";
            break;

        case 405: status_message = "Method Not Allowed";
            break;

        case 500: status_message = "Server Error";
            break;
    }

    std::string code = std::to_string(response_code);
    resp = version + " " + code + " " + status_message + kCrlf + resp;

    // Incase of PUT request skip the response body
    if (!response.empty() && response_code != 204)
    {
        resp = resp + kCrlf + response + kCrlf;
    }
    else
    {
        resp = resp + kCrlf;
    }
    return resp;
}

void ProcessHttpRequest(int accept_fd)
{
    int read_size;
    char buffer[SIZE];

    bzero(buffer, SIZE);
    read_size = recv(accept_fd, buffer, SIZE, 0);

    std::string request_data = std::string(buffer);

    if ( request_data.at(0) == '\0' )
    {
        std::cout << "invalid request data received";
        return;
    }
    // populate the request parameters from request body
    web_util::HttpMethod req_method = web_util::HTTP_METHOD_UNSET;
    std::string resource = "";
    std::string req_body = "";
    std::string version = "";
    int length_request_body, length_content;

    // parse the input method
    std::size_t found_space_method = request_data.find(kSpace);
    std::string method = request_data.substr(0, found_space_method);

    //parse the resource
    request_data  = request_data.substr(found_space_method + 1);
    std::size_t found_space_resource = request_data.find(kSpace);
    resource = std::string(request_data.substr(1, found_space_resource - 1));

    // parse the version details
    request_data  = request_data.substr(found_space_resource + 1);
    std::size_t found_end_version = request_data.find(kCrlf);
    version = std::string(request_data.substr(0, found_end_version));

    std::cout << "method: " << method.length() << "  resource: " << resource << "  version: " << version
              << "\n";

    // parse the content-length
    std::size_t found_expr = request_data.find("Content-Length");

    //parse the request body
    if (found_expr != std::string::npos)
    {
        request_data = request_data.substr(found_expr);

        found_expr = request_data.find(":");
        std::size_t found_end = request_data.find(kCrlf);

        std::string str = request_data.substr(found_expr + 1, (found_end - found_expr));

        length_content = atoi(str.c_str());

        found_expr = request_data.find(kDoubleCrlf);
        request_data = request_data.substr(found_expr + strlen(kDoubleCrlf));

        if (!request_data.empty())
        {
            std::cout << "request_body_content:" << request_data << std::endl;
            length_request_body = request_data.length();
        }
        req_body = std::string(request_data);
    }
    else
    {
        request_data = "";
        req_body = std::string("");
    }
    // find the appropriate http method
    std::cout << "http_method: " << method << "\n";

    if (!method.compare("GET"))
    {
        req_method = web_util::HTTP_METHOD_GET;
    }
    else if (!method.compare("POST"))
    {
        req_method = web_util::HTTP_METHOD_POST;
    }
    else if (!method.compare("PUT"))
    {
        req_method = web_util::HTTP_METHOD_PUT;
    }
    else if (!method.compare("DELETE"))
    {
        req_method = web_util::HTTP_METHOD_DELETE;
    }

    web_util::HttpHeaderData req_hdr;
    int response_code = 500;
    web_util::HttpHeaderData resp_hdr;
    std::string response = "";

    // Send request to rest framework and get response back
    webservice::RestRequestHandler *rest_engine = webservice::RestRequestHandler::GetInstance();
    web_util::RestResponse rest_resp = rest_engine->ProcessRequest(req_method, resource, "", req_body,
                                       req_hdr,
                                       &response_code, &resp_hdr, &response);
    if (rest_resp == web_util::kRestFwProcessRequstSuccess)
    {
        std::cout << "Successfully processed the request" << std::endl;
    }
    else if (rest_resp == web_util::kRestFwUnInitializedError)
    {
        std::cout << "Rest framework is not initialized" << std::endl;
    }
    else if (rest_resp == web_util::kRestFwProcessRequstFailed)
    {
        std::cout << "Failed to process the request" << std::endl;
    }
    else
    {
        std::cout << "Unknown error occurred" << std::endl;
    }

    // Send the response back to the client
    std::string response_to_send = CreateResponseString(response_code, resp_hdr, response, version);
    if (send(accept_fd, response_to_send.c_str(), response_to_send.length(), 0))
        std::cout << "success" << "\n";
    else
        std::cout << "error";
}

bool InitRestFw()
{
    webservice::RestRequestHandler *rest_engine = webservice::RestRequestHandler::GetInstance();
    if (rest_engine)
    {
        // initialize the Rest framework
        if (!rest_engine->Init())
        {
            std::cout << "Rest framework Initialize failed" << std::endl;
            return false;
        }
        return true;
    }
    return false;
}

bool DeInitRestFw()
{
    webservice::RestRequestHandler *rest_engine = webservice::RestRequestHandler::GetInstance();
    if (rest_engine)
    {
        // Un-initialize the Rest framework
        if (!rest_engine->DeInit())
        {
            std::cout << "Rest framework Initialize failed";
            return false;
        }
    }
    return true;
}

bool DestroyRestFw()
{
    webservice::RestRequestHandler *rest_engine = webservice::RestRequestHandler::GetInstance();
    if (rest_engine)
    {
        // destroy the Rest framework
        rest_engine->DestroyInstance();
        return true;
    }
    return false;
}