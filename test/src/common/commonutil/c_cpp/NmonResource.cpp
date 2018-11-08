/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "NmonResource.h"

NmonResource::NmonResource(void)
{
}

NmonResource::NmonResource(std::string resourceUri,
        std::string resourceTypeName,
        std::string resourceInterface,
        uint8_t resourceProperty)
         : SampleResource(resourceUri, resourceTypeName, resourceInterface, resourceProperty)
{
}

NmonResource::NmonResource(std::string resourceUri,
        std::string resourceTypeName,
        std::string resourceInterface,
        uint8_t resourceProperty,
        OCRepresentation resourceRepresentation)
         : SampleResource(resourceUri, resourceTypeName, resourceInterface, resourceProperty, resourceRepresentation)
{
}

NmonResource::~NmonResource(void)
{
}

void NmonResource::handleGetRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCRepresentation rep = m_representation;
    string resetValue = rep.getValueToString(string(RESET_KEY));
    if (resetValue.compare("true") == 0)
    {
        cout << "reset : " << resetValue << endl;
        rep.setValue(RESET_KEY, false);
    }
    SampleResource::handleGetRequest(queryParamsMap, request, response, rep);
}

void NmonResource::handlePostRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCStackResult result = OC_STACK_ERROR;
    bool shouldChange = true;
    cout << "Inside handlePostRequest... " << endl;
    cout << "THe POST request comprises of the following representation:" << endl;
    p_resourceHelper->printRepresentation(incomingRepresentation);

    // handle POST request
    string targetValue = "";
    string uriValue = "";
    OCRepresentation rep = getRepresentation();
    m_pResponse = response;
    response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);

    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            if (key.compare(INTERFACE_KEY) == 0)
            {
                string value = eachQuery.second;
                if ((value.compare("oic.if.r") == 0) || (value.compare("oic.if.s") == 0))
                {
                    cout << "Update request received via interface: " << value
                            << " . This interface is not authorized to update resource!!" << endl;
                    response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                    shouldChange = false;
                    break;
                }
            }

        }
    }

    if (shouldChange)
    {
        try
        {
            if (incomingRepresentation.hasAttribute(INTERFACE_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'if' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(RESOURCE_TYPE_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'rt' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(IANA_NETWORK_CONNECTION_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'ianaifType' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(TRANSMITTED_KB_FROM_COL_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'tx' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(RECEIVED_KB_FROM_COL_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'rx' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(MAX_TRANSMITTED_MSG_BYTES_IN_COL_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'mmstx' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(AVG_TRANSMITTED_MSG_BYTES_IN_COL_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'amstx' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(MAX_RECEIVED_MSG_BYTES_IN_COL_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'mmsrx' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(AVG_RECEIVED_MSG_BYTES_IN_COL_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'amsrx' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }



        try
        {
            if (incomingRepresentation.hasAttribute(COLLECTING_VALUES) &&
                incomingRepresentation.hasAttribute(RESET_KEY))
            {
                updateRepresentation(COLLECTING_VALUES, incomingRepresentation, response);
                notifyObservers(this);
                updateRepresentation(RESET_KEY, incomingRepresentation, response);
                notifyObservers(this);
            }
            else if (incomingRepresentation.hasAttribute(COLLECTING_VALUES) ||
                     incomingRepresentation.hasAttribute(RESET_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "Property 'reset' and 'col' have to be both inside POST request"<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(RESET_KEY))
            {
                updateRepresentation(RESET_KEY, incomingRepresentation, response);
                notifyObservers(this);
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }
    }

    try
    {
        result = OCPlatform::sendResponse(response);
    }
    catch (exception& e)
    {
        cout << "Exception occurred while sending response. Exception is: " << e.what() << endl;
    }
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for POST Request" << endl;
    }
}