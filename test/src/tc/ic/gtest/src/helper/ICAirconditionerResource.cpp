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

ICAirconditionerResource::ICAirconditionerResource(string uri, vector<string> rt, vector<string> itf): ICResource(uri, rt, itf)
{
    
}

OCEntityHandlerResult ICAirconditionerResource::entityHandler(shared_ptr<OCResourceRequest> request)
{
    cout << "\tIn Server Airconditioner entity handler:\n";
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
                string findRes = request->getQueryParameters().find("if")->second;
                if (findRes.compare(LINK_INTERFACE) == 0)
                {
					if (OC_STACK_OK == sendRepresentation(request))
                    {
						ehResult = OC_EH_OK;
                    }
                }
                else
                {
					ehResult = OC_EH_FORBIDDEN;
                }
            }
            else if (requestType == "PUT")
            {
				cout << "\t\t\trequestType : PUT\n";
            }
            else if (requestType == "POST")
            {
				// POST request operations
            }
            else if (requestType == "DELETE")
            {
				// DELETE request operations
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
			cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
		cout << "Request invalid" << endl;
    }

    return ehResult;
}
