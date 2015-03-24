#include "light_resource.hpp"

namespace Intel { namespace OCDemo {

std::atomic<bool> LightResource::shutdown_flag(false);
std::thread LightResource::observe_thread;

void LightResource::setRepresentation(const OCRepresentation& rep)
{
 cout << "\t\t\t" << "Received representation: " << endl;

 rep.getValue("state", m_state);
 rep.getValue("power", m_power);

 cout << "\t\t\t\t" << "power: " << m_power << endl;
 cout << "\t\t\t\t" << "state: " << m_state << endl;
}

OCRepresentation LightResource::getRepresentation(void)
{
 m_rep.setValue("state", m_state);
 m_rep.setValue("power", m_power);
 return m_rep;
}

void LightResource::addType(const std::string& type) const
{
 OCStackResult result = OC::OCPlatform::bindTypeToResource(m_resourceHandle, type);

 if(OC_STACK_OK != result)
  cout << "Binding TypeName to Resource was unsuccessful, result was " << result << '\n';
}

void LightResource::addInterface(const std::string& interface) const
{
 OCStackResult result = OC::OCPlatform::bindInterfaceToResource(m_resourceHandle, interface);

 if(OC_STACK_OK != result)
  cout << "Binding TypeName to Resource was unsuccessful, result was " << result << '\n';
}

void LightResource::createResource(const unsigned int resource_number)
{
 string resourceURI      { make_URI(resource_number) };
 string resourceTypeName { "core.light" };

 cout << "registering resource: " << resourceURI << '\n';
 cout << "registering type name \"" << resourceTypeName << "\".\n";
 // This will internally create and register the resource, binding the current instance's method as a callback:
 OCStackResult result = OC::OCPlatform::registerResource(
                                            m_resourceHandle, resourceURI, resourceTypeName,
                                            DEFAULT_INTERFACE,
                                            std::bind(&LightResource::entityHandler, this, std::placeholders::_1),
                                            OC_DISCOVERABLE | OC_OBSERVABLE);
  if (OC_STACK_OK != result)
   std::cout << "Resource creation failed.\n";
}

void LightResource::observe_function()
{
 cerr << "Observation thread is spinning up.\n";

 while(!shutdown_flag)
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if(!m_observation)
     continue;

    m_power += 10;

    const auto result = OC::OCPlatform::notifyAllObservers(getHandle());

    // Stop notifications when there are no more observers:
    if(OC_STACK_NO_OBSERVERS == result)
     {
        m_observation = 0;
     }
  }

 cerr << "Observation thread is shutting down.\n";
}

void LightResource::unregisterResource()
{
    std::cout << "Unregistering light resource"<<std::endl;

    OCStackResult result = OC::OCPlatform::unregisterResource(m_resourceHandle);

    if(result == OC_STACK_OK)
    {
        std::cout << "Resource unregistered."<<std::endl;
    }
    else
    {
        cerr << "Unregister resource failed: "<<std::endl;
    }
}

// This is just a sample implementation of entity handler.
// Entity handler can be implemented in several ways by the manufacturer
OCEntityHandlerResult LightResource::entityHandler(std::shared_ptr<OCResourceRequest> request)
{
    if(!request)
     {
        cerr << "entityHandler(): Received invalid request object.\n";
        return OC_EH_ERROR;
     }

    switch(request->getRequestHandlerFlag())
    {
        default:
                cerr << "entityHandler(): invalid request flag\n";
                break;

        case RequestHandlerFlag::InitFlag:
                cerr << "entityHandler(): Initialization requested.\n";
                break;

        case RequestHandlerFlag::RequestFlag:
                dispatch_request(request->getRequestType(), request);
                break;

        case RequestHandlerFlag::ObserverFlag:
                handle_observe_event(request);
                break;
    }

    return OC_EH_OK;
}

void LightResource::dispatch_request(const std::string& request_type, std::shared_ptr<OCResourceRequest> request)
{
 std::cout << "dispatch_request(): " << request_type << '\n';

 if("GET" == request_type)
  return handle_get_request(request);

 if("PUT" == request_type)
  return handle_put_request(request);

 if("POST" == request_type)
  return handle_post_request(request);

 if("DELETE" == request_type)
  return handle_delete_request(request);

 cerr << "entityHandler(): Invalid request type \"" << request_type << "\".\n";
}

void LightResource::handle_get_request(std::shared_ptr<OCResourceRequest> request)
{
 cout << "handle_get_request():\n";

 const auto query_params_map = request->getQueryParameters();

 // ...do any processing of the query here...

 // Get a representation of the resource and send it back as a response:
 auto pResponse = std::make_shared<OC::OCResourceResponse>();
 pResponse->setRequestHandle(request->getRequestHandle());
 pResponse->setResourceHandle(request->getResourceHandle());
 pResponse->setErrorCode(200);
 pResponse->setResponseResult(OC_EH_OK);
 pResponse->setResourceRepresentation(getRepresentation());
 OCPlatform::sendResponse(pResponse);
}

void LightResource::handle_put_request(std::shared_ptr<OCResourceRequest> request)
{
 // Here's how you would get any query parameters:
 const auto query_params_map = request->getQueryParameters();
 // ...do something with the query parameters (if there were any)...

 auto rep = request->getResourceRepresentation();

 setRepresentation(rep);

 auto pResponse = std::make_shared<OC::OCResourceResponse>();
 pResponse->setRequestHandle(request->getRequestHandle());
 pResponse->setResourceHandle(request->getResourceHandle());
 pResponse->setErrorCode(200);
 pResponse->setResponseResult(OC_EH_OK);
 pResponse->setResourceRepresentation(getRepresentation());
 OCPlatform::sendResponse(pResponse);
}

void LightResource::handle_post_request(std::shared_ptr<OCResourceRequest> request)
{
 // ...demo-code...
}

void LightResource::handle_delete_request(std::shared_ptr<OCResourceRequest> request)
{
 // ...demo-code...
}

// Set up observation in a separate thread:
void LightResource::handle_observe_event(std::shared_ptr<OCResourceRequest> request)
{
 if(observe_thread.joinable())
  return;

 observe_thread = thread(bind(&LightResource::observe_function, this));
 observe_thread.detach();
}



}} // namespace Intel::OCDemo

