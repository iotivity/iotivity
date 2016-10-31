#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>

#include "ocstack.h"
#include "ocpayload.h"
#include "rd_client.h"
#include "rd_server.h"
#include "rd_database.h"

#include <OCApi.h>
#include <OCPlatform.h>

#include "ICHelper.h"

using namespace OC;
using namespace std;

ICTemperatureResource::ICTemperatureResource(string uri, vector<string> rt, vector<string> itf): ICResource(uri, rt, itf)
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

        if (rep.getValue("temperature", temperature) && rep.getValue("range", range) && rep.getValue("units", units))
        {
        m_temperature = temperature;
        m_range = range;
        m_units = units;
        m_representation.setValue("temperature", m_temperature);
        m_representation.setValue("range", m_range);
        m_representation.setValue("units", m_units);
        cout << "\t\t\t\t" << "temperature: " << m_temperature << endl;
        cout << "\t\t\t\t" << "range: " << m_range << endl;
        cout << "\t\t\t\t" << "units: " << m_units << endl;

        propagate();
    }
}

OCEntityHandlerResult ICTemperatureResource::entityHandler(shared_ptr<OCResourceRequest> request)
{
	cout << "\tIn Server Temperature entity handler:\n";
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    if (request)
    {
		// Get the request type and request flag
		string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
			cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
				cout << "\t\t\trequestType : GET\n";
                if (OC_STACK_OK == sendRepresentation(request))
                {
					ehResult = OC_EH_OK;
                 }
            }
            else if (requestType == "PUT")
            {
				cout << "\t\t\trequestType : PUT\n";
                // PUT requeist operations
            }
            else if (requestType == "POST")
            {
				cout << "\t\t\trequestType : POST\n";
                // POST request operations
                OCRepresentation    rep = request->getResourceRepresentation();
                setTemperatureRepresentation(rep);

                if (OC_STACK_OK == sendRepresentation(request))
                {
					ehResult = OC_EH_OK;
                }
            }
            else if (requestType == "DELETE")
            {
				cout << "\t\t\trequestType : DELETE\n";
                // DELETE request operations
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
			cout << "\t\trequestFlag : Observer\n";

            ObservationInfo observationInfo = request->getObservationInfo();
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
				m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if (ObserveAction::ObserveUnregister == observationInfo.action)
            {
				m_interestedObservers.erase(remove(m_interestedObservers.begin(),
                                                   m_interestedObservers.end(),
                                                   observationInfo.obsId),
                                                   m_interestedObservers.end());
            }
        }
    }
    else
    {
		cout << "Request invalid" << endl;
    }

    return ehResult;
}
