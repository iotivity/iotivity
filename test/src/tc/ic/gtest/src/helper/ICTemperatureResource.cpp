/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "ICHelper.h"
#include "ICTemperatureResource.h"

ICTemperatureResource::ICTemperatureResource(string uri, vector< string > rt, vector< string > itf) :
        ICResource(uri, rt, itf)
{
    m_temperature = 0;
    m_range = "";
    m_units = "";
    m_representation.setValue("temperature", m_temperature);
    m_representation.setValue("range", m_range);
    m_representation.setValue("units", m_units);
}

void ICTemperatureResource::setTemperatureRepresentation(OCRepresentation &rep)
{
    int temperature;
    string range;
    int units;

    if (rep.getValue("temperature", temperature) && rep.getValue("range", range)
            && rep.getValue("units", units))
    {
        m_temperature = temperature;
        m_range = range;
        m_units = units;
        m_representation.setValue("temperature", m_temperature);
        m_representation.setValue("range", m_range);
        m_representation.setValue("units", m_units);
        IOTIVITYTEST_LOG(INFO, "\t\t\t\t temperature: %d", m_temperature);
        IOTIVITYTEST_LOG(INFO, "\t\t\t\t range: %s", m_range.c_str());
        IOTIVITYTEST_LOG(INFO, "\t\t\t\t units: %s", m_units.c_str());
        propagate();
    }
}

OCEntityHandlerResult ICTemperatureResource::entityHandler(shared_ptr< OCResourceRequest > request)
{
    IOTIVITYTEST_LOG(INFO, "\tIn Server Temperature entity handler:\n");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    if (request)
    {
        // Get the request type and request flag
        string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Request\n");

            // If the request type is GET
            if (requestType == "GET")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : GET\n");
                if (OC_STACK_OK == sendRepresentation(request))
                {
                    ehResult = OC_EH_OK;
                }
            }
            else if (requestType == "PUT")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : PUT\n");
                // PUT requeist operations
            }
            else if (requestType == "POST")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : POST\n");
                // POST request operations
                OCRepresentation rep = request->getResourceRepresentation();
                setTemperatureRepresentation(rep);

                if (OC_STACK_OK == sendRepresentation(request))
                {
                    ehResult = OC_EH_OK;
                }
            }
            else if (requestType == "DELETE")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : DELETE\n");
                // DELETE request operations
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Observer\n");

            ObservationInfo observationInfo = request->getObservationInfo();
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if (ObserveAction::ObserveUnregister == observationInfo.action)
            {
                m_interestedObservers.erase(
                        remove(m_interestedObservers.begin(), m_interestedObservers.end(),
                                observationInfo.obsId), m_interestedObservers.end());
            }
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "Request invalid\n");
    }

    return ehResult;
}
