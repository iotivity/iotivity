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

#include "MntResource.h"

MntResource::MntResource(void)
{
    numberOf503ErrorResponse = 0;
    expect503ErrorRespond = false;
}

MntResource::MntResource(std::string resourceUri,
        std::string resourceTypeName,
        std::string resourceInterface,
        uint8_t resourceProperty)
         : SampleResource(resourceUri, resourceTypeName, resourceInterface, resourceProperty)
{
    numberOf503ErrorResponse = 0;
    expect503ErrorRespond = false;
}

MntResource::MntResource(std::string resourceUri,
        std::string resourceTypeName,
        std::string resourceInterface,
        uint8_t resourceProperty,
        OCRepresentation resourceRepresentation)
         : SampleResource(resourceUri, resourceTypeName, resourceInterface, resourceProperty, resourceRepresentation)
{
    numberOf503ErrorResponse = 0;
    expect503ErrorRespond = false;
}

MntResource::~MntResource(void)
{
}

void MntResource::handleGetRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCRepresentation rep = m_representation;
    string rbValue = rep.getValueToString(string(REBOOT_KEY));
    if (rbValue.compare("true") == 0)
    {
        cout << "resseting property 'rb' : " << rbValue << endl;
        rep.setValue(REBOOT_KEY, false);
    }

    if (expect503ErrorRespond && numberOf503ErrorResponse < 2)
    {
        numberOf503ErrorResponse++;
        rep.setValue(HTML_ERR_KEY, 503);
    }
    else if (numberOf503ErrorResponse >= 2)
    {
        numberOf503ErrorResponse = 0;
        expect503ErrorRespond = false;
    }

    SampleResource::handleGetRequest(queryParamsMap, request, response, rep);
}

void MntResource::handlePostRequest(QueryParamsMap &queryParamsMap,
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
                cout << "\t\t" << "property 'if' is readOnly "<< endl;
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
                cout << "\t\t" << "property 'rt' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(NAME_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "property 'n' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(HTML_ERR_KEY))
            {
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                cout << "\t\t" << "property 'err' is readOnly "<< endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(FACTORY_KEY))
            {
                expect503ErrorRespond = true;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

        try
        {
            if (incomingRepresentation.hasAttribute(REBOOT_KEY))
            {
                expect503ErrorRespond = true;
                updateRepresentation(REBOOT_KEY, incomingRepresentation, response);
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