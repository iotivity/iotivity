//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _IN_PROC_SERVER_WRAPPER_H_
#define _IN_PROC_SERVER_WRAPPER_H_

#include <thread>
#include <mutex>
#include <ocstack.h>
#include <OCApi.h>
#include <IServerWrapper.h>
#include <OCReflect.h>

using namespace OC::OCReflect;

namespace OC
{
    class InProcServerWrapper : public IServerWrapper
    {
    public:
        InProcServerWrapper(PlatformConfig cfg);
        virtual ~InProcServerWrapper();

        void registerResource(  const std::string& resourceURI, 
                                const std::string& resourceTypeName,
                                named_property_binding_vector& properties); 
								
	private:
		void processFunc();
		std::thread m_processThread;
        bool m_threadRun;
		std::mutex m_csdkLock;
    };
}

#endif
