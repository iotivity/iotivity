

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;


// Forward declaring the entityHandler of bookmark
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);

class PlayerResource
{
public:
    //light Members of Bookmark
    std::string m_PlayerUri;
    std::string m_PlayerType;
    std::string m_PlayerInterface;
    std::string m_State;
    OCResourceHandle m_PlayerHandler;

public:
    /// Constructor
    PlayerResource()
    {
        m_State = "off";
        
        m_PlayerUri = "/core/music_player"; // URI of the resource
        m_PlayerType = "core.music_player"; // resource type name. In this case, it is light

        m_PlayerInterface = DEFAULT_INTERFACE; // resource interface.
        m_PlayerHandler = 0;
    }

    /// This function internally calls registerResource API.
    void createResources(OC::OCPlatform& platform)
    {
        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_PlayerHandler, m_PlayerUri, m_PlayerType,
                                    m_PlayerInterface, entityHandler,
                                    OC_DISCOVERABLE | OC_OBSERVABLE );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (musicplayer) was unsuccessful\n";
        }
    }

    void setRepresentation(OCRepresentation& rep)
    {
        AttributeMap attributeMap = rep.getAttributeMap();

        if(attributeMap.find("powerstate") != attributeMap.end())
        {
            std::cout << attributeMap["powerstate"][0] << endl;
            if( attributeMap["powerstate"][0] == "on" )
            {
                cout << "\n\n\n\n MUSIC PLAYER ON \n\n\n\n" << endl;

                // do somethings.
            } 
            else 
            {
                cout << "\n\n\n\n MUSIC PLAYER OFF \n\n\n\n" << endl;
            }
        }
    }

    OCRepresentation getRepresentation()
    {
        OCRepresentation rep;

        AttributeMap attributeMap;

        AttributeValues att_powerstate;
        att_powerstate.push_back(m_State);
        attributeMap["powerstate"] = att_powerstate;

        rep.setAttributeMap(attributeMap);

        return rep;
    }
};


// Create the instance ofpowerstate resource class (in this case instance of class 'BookmarkResource').
PlayerResource    myPlayerResource;


void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();                                    
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if(requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";
                
                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                OCRepresentation rep;
                rep = myPlayerResource.getRepresentation();

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }

            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                // Get the representation from the request
                OCRepresentation rep = request->getResourceRepresentation();

                myPlayerResource.setRepresentation(rep);

                // Do related operations related to PUT request
                rep = myPlayerResource.getRepresentation();

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }

            }
            else if(requestType == "POST")
            {
                // POST request operations
            }
            else if(requestType == "DELETE")
            {
                // DELETE request operations
            }
        }
        else if(requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}



int main()
{
    // Create PlatformConfig object

    PlatformConfig cfg(
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "192.168.20.130",
        0,
        OC::QualityOfService::NonConfirmable );
    //cfg.ipAddress = "192.168.20.110";
    //cfg.port = 0;
    //cfg.mode = ModeType::Server;
    //cfg.serviceType = ServiceType::InProc;

    try
    {
        // Create a OCPlatform instance.
        // Note: Platform creation is synchronous call.
        OCPlatform platform(cfg);

        // Invoke createResource function of class light.

        myPlayerResource.createResources(platform);

        // Perform app tasks
        while(true)
        {
            // some tasks
        }
    }
    catch(OCException e)
    {
        std::cout << "Exception in main: " << e.what();
    }
}
