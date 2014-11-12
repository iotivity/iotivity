//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file    TGMClient.cpp
///  @brief   

#include "TGMClient.h"
#include <algorithm>
#include <thread>

using namespace OC;

std::map< std::vector< std::string >, CandidateCallback > candidateRequest;
std::map< std::vector< std::string >, CandidateCallback > candidateRequestForTimer;
std::map< std::string, std::map< std::string, std::shared_ptr< OCResource > > > rtForResourceList;
std::vector< std::string > allFoundResourceTypes;

template< typename T >
bool IsSubset(std::vector< T > full, std::vector< T > sub)
{
    std::sort(full.begin(), full.end());
    std::sort(sub.begin(), sub.end());
    return std::includes(full.begin(), full.end(), sub.begin(), sub.end());
}

void TGMClient::onFoundResource(std::shared_ptr< OCResource > resource, int waitsec)
{

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {

            std::cout << "DISCOVERED Resource:" << std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;

            hostAddress.append(resourceURI);

            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;

                if (std::find(allFoundResourceTypes.begin(), allFoundResourceTypes.end(),
                        resourceTypes) == allFoundResourceTypes.end())
                {
                    allFoundResourceTypes.push_back(resourceTypes);
                }

                rtForResourceList[resourceTypes][hostAddress] = resource;
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if (waitsec == -1)
            {
                findPreparedRequest(candidateRequest);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch (std::exception& e)
    {
        //log(e.what());
    }
}

TGMClient::TGMClient(void)
{
    ;
}

/**
 * Virtual destructor
 */
TGMClient::~TGMClient(void)
{
    candidateRequest.clear();
    candidateRequestForTimer.clear();
    rtForResourceList.clear();
    allFoundResourceTypes.clear();
}

void TGMClient::findPreparedRequest(
        std::map< std::vector< std::string >, CandidateCallback > &request)
{
    std::vector < std::shared_ptr < OCResource >> resources;

    for (auto it = request.begin(); it != request.end();)
    {

        if (IsSubset(allFoundResourceTypes, it->first))
        {
            //std::cout << "IS SUBSET !!! \n";

            for (unsigned int i = 0; i < it->first.size(); ++i)
            {

                for (auto secondIt = rtForResourceList[it->first.at(i)].begin();
                        secondIt != rtForResourceList[it->first.at(i)].end(); ++secondIt)
                {
                    resources.push_back(secondIt->second);
                }
            }

            it->second(resources);

            //TODO : decide policy - callback only once 
            request.erase(it++);
        }
        else
        {
            ++it;
        }

    }

}

void TGMClient::lazyCallback(int second)
{
    sleep(second);
    findPreparedRequest(candidateRequestForTimer);

}
OCStackResult TGMClient::findCandidateResources(std::vector< std::string > resourceTypes,
        CandidateCallback callback, int waitsec)
{
    if (resourceTypes.size() < 1)
    {
        return OC_STACK_ERROR;
    }

    std::sort(resourceTypes.begin(), resourceTypes.end());
    resourceTypes.erase(std::unique(resourceTypes.begin(), resourceTypes.end()),
            resourceTypes.end());

    if (waitsec != -1)
    {
        candidateRequestForTimer.insert(std::make_pair(resourceTypes, callback));
    }
    else
    {
        candidateRequest.insert(std::make_pair(resourceTypes, callback));
    }

    for (unsigned int i = 0; i < resourceTypes.size(); ++i)
    {
        std::cout << "resourceTypes : " << resourceTypes.at(i) << std::endl;
        std::string query = "coap://224.0.1.187/oc/core?rt=";
        query.append(resourceTypes.at(i));
        OCPlatform::findResource("", query.c_str(),
                std::function < void(std::shared_ptr < OCResource > resource)
                        > (std::bind(&TGMClient::onFoundResource, this, std::placeholders::_1,
                                waitsec)));
    }

    if (waitsec != -1)
    {
        std::thread exec(
                std::function< void(int second) >(
                        std::bind(&TGMClient::lazyCallback, this, std::placeholders::_1)), waitsec);
        exec.detach();
    }

    return OC_STACK_OK;
}
