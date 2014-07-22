//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "OCResource.h"
#include "InProcClientWrapper.h"

#include "ocstack.h"

using namespace std;


namespace OC
{
    InProcClientWrapper::InProcClientWrapper(PlatformConfig cfg)
    {
        OCStackResult result = OCInit(cfg.ipAddress.c_str(), cfg.port, OC_CLIENT);

        if(OC_STACK_OK != result)
        {
            throw InitializeException("Error Initializing Stack", result);
        }

        m_threadRun = true;
        m_listeningThread = std::thread(&InProcClientWrapper::listeningFunc, this);
    }

    InProcClientWrapper::~InProcClientWrapper()
    {
        if(m_listeningThread.joinable())
        {
            m_threadRun = false;
            m_listeningThread.join();
        }

        OCStop();
    }

    void InProcClientWrapper::listeningFunc()
    {
        while(m_threadRun)
        {
			OCStackResult result;
			{
				std::lock_guard<std::mutex> lock(m_csdkLock);
				result = OCProcess();
			}

            if(result != OC_STACK_OK)
            {
                // TODO: @Erich do something with result if failed?
            }

            std::this_thread::yield();
            // To minimize CPU utilization we may wish to do this with sleep
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }


 
    std::string convertOCAddrToString(OCDevAddr* addr)
    {
	if(addr->size != 4) { return "NOT SUPPORTED ADDR;";}

	if(addr->size == 4) // IPV4
	{
		std::ostringstream address;
		address<<"coap://"<<addr->addr[0]<<"."<<addr->addr[1]<<"."<<addr->addr[2]<<"."<<addr->addr[3]<<"/";
		return address.str();
	}

	// TODO: @Erich Convert the device address to a valid string!
	return "";
    }


    OCStackApplicationResult listenCallback(void* ctx, OCClientResponse* clientResponse)
    {
	auto &callback =*(std::function <void(OCResource::Ptr)>*)ctx;
	std::stringstream requestStream;
	requestStream << clientResponse->resJSONPayload;


	boost::property_tree::ptree root;
	boost::property_tree::read_json(requestStream, root);
			
	boost::property_tree::ptree payload = root.get_child("oc.payload", boost::property_tree::ptree());
			
            
	for(auto payloadItr : payload)
	{
		try
		{
			std::string host = convertOCAddrToString(clientResponse->addr);
			OCResource::Ptr resource = std::make_shared<OCResource>(host, payloadItr.second);
		
			// Note: the call to detach allows the underlying thread to continue until completion 
			//  and allows us to destroy the exec object.  
			//  This is apparently NOT a memory leak, as the thread will apparently take care of itself.
			//  Additionally, the only parameter here is
			//  a shared ptr, so OCResource will be disposed of properly upon completion of the callback handler.
			std::thread exec(callback,resource);
			exec.detach();
		}
		catch(ResourceInitException)
		{
			// TODO: Do we want to handle this somehow?  Perhaps we need to log this?
		}
			
	}
	delete clientResponse;
	
	return OC_STACK_KEEP_TRANSACTION;
    } 

    int InProcClientWrapper::ListenForResource(const std::string& serviceUrl, const std::string& resourceType, std::function<void (OCResource::Ptr)>& callback)
    {
	OCStackResult result;

	OCCallbackData* cbdata = new OCCallbackData();
	cbdata->context = (void*)(&callback);
	cbdata->cb = &listenCallback;
	{
		std::lock_guard<std::mutex> lock(m_csdkLock);

		result = OCDoResource(OC_REST_GET, resourceType.c_str(), nullptr, nullptr, OC_NON_CONFIRMABLE, cbdata);

	}
		return result;
    }
   }
