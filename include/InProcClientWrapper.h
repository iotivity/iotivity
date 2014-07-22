//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _IN_PROC_CLIENT_WRAPPER_H_
#define _IN_PROC_CLIENT_WRAPPER_H_

#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <OCApi.h>
#include <ocstack.h>
#include <IClientWrapper.h>
#include <InitializeException.h>
#include <ResourceInitException.h>

namespace OC
{
    class InProcClientWrapper : public IClientWrapper
    {
    public:
        InProcClientWrapper(PlatformConfig cfg);
        virtual ~InProcClientWrapper();

        virtual int ListenForResource(const std::string& serviceUrl, const std::string& resourceType, std::function<void(OCResource::Ptr)>& callback);

    private:
        void listeningFunc();
        std::thread m_listeningThread;
        bool m_threadRun;
        std::mutex m_resourceListenerLock;
		std::mutex m_csdkLock;
        std::vector<std::function<void(OCClientResponse*)>> callbackList;
    };
}

#endif
