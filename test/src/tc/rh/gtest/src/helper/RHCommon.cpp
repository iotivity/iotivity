/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "RHCommon.h"

std::shared_ptr< OCResource > RHCommon::m_curResource = NULL;
std::shared_ptr< OCResource > RHCommon::m_curResource2 = NULL;
std::shared_ptr< OCResource > RHCommon::m_curResourceNonHostable = NULL;

RHCommon::RHCommon(){

    SUCCESS_RESPONSE = OC_STACK_OK;
    resourceUriOne = "/a/TempHumSensor";
    resourceUriTwo = "/a/TempHumSensor2";
    resourceOneTemp = 0;
    resourceTwoTemp = 0;
    resourceOneHum = 0;
    resourceTwoHum = 0;

}

void RHCommon::initialize(){
    static int counter = 0;
    if(counter == 0){
        counter = 1;
        CommonUtil::launchApp("./rh_provider");
        CommonUtil::waitInSecond(2);
    }
}

bool RHCommon::startOCResourceHost(){
    return true;
}

bool RHCommon::stopOCResourceHost(){
    return true;
}
void RHCommon::onObserve(const HeaderOptions &headerOption , const OCRepresentation &rep , const int &eCode,
               const int &sequenceNumber)
{
    std::cout << "onObserve" << std::endl;

    if (eCode <= OC_STACK_OK)
    {
    	if (rep.getUri().find("/a/TempHumSensor3") != std::string::npos) {
    	    std::cout << "observing non-hostable resource...." << '\n';
    	}
    	else if (rep.getUri().find("/a/TempHumSensor2") != std::string::npos) {
            resourceTwoTemp = rep.getValue<int>("temperature");
            resourceTwoHum = rep.getValue<int>("humidity");
    	}
    	else if (rep.getUri().find("/a/TempHumSensor") != std::string::npos) {
    		resourceOneTemp = rep.getValue<int>("temperature");
			resourceOneHum = rep.getValue<int>("humidity");
    	}

        std::cout << std::endl;
        std::cout << "========================================================" << std::endl;
        std::cout << "Receive OBSERVE RESULT:" << std::endl;
        std::cout << "\tUri: " << rep.getUri() << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << std::endl;
        std::cout << "\tTemperature : " << rep.getValue<int>("temperature") << std::endl;
        std::cout << "\tHumidity : " << rep.getValue<int>("humidity") << std::endl;
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// callback handler on GET request
void RHCommon::onGet(const HeaderOptions &headerOption , const OCRepresentation &rep , const int eCode)
{
    std::cout << "GET request was successful1" << std::endl;
    if (eCode == SUCCESS_RESPONSE)
    {
        if (rep.getUri().find("/a/TempHumSensor3") != std::string::npos) {
			std::cout << "observing non-hostable resource...." << '\n';
		}
		else if (rep.getUri().find("/a/TempHumSensor2") != std::string::npos) {
			resourceTwoTemp = rep.getValue<int>("temperature");
			resourceTwoHum = rep.getValue<int>("humidity");
		}
		else if (rep.getUri().find("/a/TempHumSensor") != std::string::npos) {
			resourceOneTemp = rep.getValue<int>("temperature");
			resourceOneHum = rep.getValue<int>("humidity");
		}

        std::cout << "GET request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri().c_str() << std::endl;
        std::cout << "\tTemperature : " << rep.getValue<int>("temperature") << std::endl;
        std::cout << "\tHumidity : " << rep.getValue<int>("humidity") << std::endl;
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

//callback hadnler on DELETE request
void RHCommon::onDelete(const HeaderOptions &headerOption , const int eCode)
{
    try
    {
        if (eCode == OC_STACK_RESOURCE_DELETED)
        {
            std::cout << "DELETE request was successful" << std::endl;
        }
        else
        {
            std::cout << "onDelete Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onDelete" << std::endl;
    }
}

void RHCommon::foundResource(std::shared_ptr< OCResource > resource)
{
    std::string resourceURI;
    std::string hostAddress;

    std::cout << "foundResource" << std::endl;
    //m_resourceCount++;

    try
    {
        if (resource)
        {
            std::cout << "========================================================" << std::endl;
			std::cout << "DISCOVERED Resource(Consumer):" << std::endl;

			resourceURI = resource->uri();
			std::cout << "\tURI of the resource: " << resourceURI << std::endl;

			hostAddress = resource->host();
			std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            if (resourceURI.find("/a/TempHumSensor3") != std::string::npos) {
            	std::cout << std::endl;
				std::cout << "**** Find TempHumSensor3 ****" << std::endl;

                m_curResourceNonHostable = resource;
			}
			else if (resourceURI.find("/a/TempHumSensor2") != std::string::npos) {
            	std::cout << std::endl;
				std::cout << "**** Find TempHumSensor2 ****" << std::endl;

                m_curResource2 = resource;
			}
			else if (resourceURI.find("/a/TempHumSensor") != std::string::npos) {
				std::cout << std::endl;
				std::cout << "**** Find TempHumSensor ****" << std::endl;

				m_curResource = resource;
			}
        }
        else
        {
            std::cout << "Resource is invalid" << std::endl;
        }
    }
    catch (std::exception &e)
    {
    }
}

void RHCommon::onPut(const HeaderOptions &headerOption, const OCRepresentation &rep, const int eCode)
{
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "PUT request was successful" << std::endl;
            int humidity;
            int temperature;
            rep.getValue("temperature", temperature);
            rep.getValue("humidity", humidity);


            std::cout << "\t temperature: " << temperature << std::endl;
            std::cout << "\t humidity: " << humidity << std::endl;
        }
        else
        {
            std::cout << "onPut Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
    }
}

void RHCommon::onPost(const HeaderOptions &headerOption, const OCRepresentation &rep, const int eCode)
{
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "POST request was successful" << std::endl;
            int humidity;
            int temperature;
            rep.getValue("temperature", temperature);
            rep.getValue("humidity", humidity);


            std::cout << "\t temperature: " << temperature << std::endl;
            std::cout << "\t humidity: " << humidity << std::endl;
        }
        else
        {
            std::cout << "onPost Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onPost" << std::endl;
    }
}

bool RHCommon::verifySensor(std::shared_ptr< OCResource > resource, std::string uri, std::string &errorMessage){

    if(resource == NULL){
        errorMessage = uri + " not found";
        return false;
    }
    if(resource->uri() != uri){
        errorMessage = "expected resource : " + uri + " actual resource : " + resource->uri();
        return false;
    }
    return true;
}

bool RHCommon::verifyTempHumSensorOne(std::string &errorMessage){

    if(m_curResource == NULL){
        errorMessage = "TempHumSensor resource not found";
        return false;
    }

    if (m_curResource->uri().find("/a/TempHumSensor") != std::string::npos) {
		return true;
	}
    else {
    	errorMessage = "resource with uri /a/TempHumSensor not found";
    	return false;
    }
}

bool RHCommon::verifyTempHumSensorTwo(std::string &errorMessage){

    if(m_curResource2 == NULL){
        errorMessage = "TempHumSensor2 resource not found";
        return false;
    }

    if (m_curResource2->uri().find("/a/TempHumSensor2") != std::string::npos) {
		return true;
	}
    else {
    	errorMessage = "resource with uri /a/TempHumSensor2 not found";
    	return false;
    }
}

bool RHCommon::verifyObserveDataFromSensorOne(std::string &errorMessage, int expectedTemperature, int expectedHumidity){
    if(resourceOneTemp != expectedTemperature){
        errorMessage = "TempHumSensor Temperature expected = " + std::to_string(expectedTemperature) + " : actual = " + std::to_string(resourceOneTemp);
        return false;
    }
    if(resourceOneHum != expectedHumidity){
        errorMessage = "TempHumSensor Temperature expected = " + std::to_string(expectedHumidity) + " : actual = " + std::to_string(resourceOneHum);
        return false;
    }
    return true;

}

bool RHCommon::verifyObserveDataFromSensorTwo(std::string &errorMessage, int expectedTemperature, int expectedHumidity){
    if(resourceTwoTemp != expectedTemperature){
        errorMessage = "TempHumSensor Temperature expected = " + std::to_string(expectedTemperature) + " : actual = " + std::to_string(resourceTwoTemp);
        return false;
    }
    if(resourceTwoHum != expectedHumidity){
        errorMessage = "TempHumSensor Temperature expected = " + std::to_string(expectedHumidity) + " : actual = " + std::to_string(resourceTwoHum);
        return false;
    }
    return true;
}
