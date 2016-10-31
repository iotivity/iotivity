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

ICBinarySwitchResource::ICBinarySwitchResource(string uri, vector<string> rt, vector<string> itf): ICResource(uri, rt, itf)
{
	m_value = false;
	m_representation.setValue("value", m_value);
}

void ICBinarySwitchResource::setBinarySwitchRepresentation(OCRepresentation &rep)
{
	bool value;
    if (rep.getValue("value", value))
    {
		m_value = value;
        m_representation.setValue("value", m_value);
        cout << "\t\t\t\t" << "value: " << m_value << endl;

        propagate();
    }
}

OCEntityHandlerResult ICBinarySwitchResource::entityHandler(shared_ptr<OCResourceRequest> request)
{
	cout << "\tIn Server Binaryswitch entity handler:\n";
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
                // PUT request operations
            }
            else if (requestType == "POST")
            {
				cout << "\t\t\trequestType : POST\n";
                // POST request operations
                OCRepresentation    rep = request->getResourceRepresentation();
                setBinarySwitchRepresentation(rep);

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
