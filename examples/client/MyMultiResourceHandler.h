//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "ocapi.h"
#include "OCResourceHandler.h"
#include "MyObserverHandler.h"

namespace OC {

class MyMultiResourceHandler :
	public OCResourceHandler
{
public:
	void onFoundResource(OCResourceResult *update, void *params);
	void onCompleted();
	void onFailed();

private:
	MyObserverHandler myObserverHandler;

	/// This is a private function to perform operations related to dimmer resource 
	void performDimmerOperations(OCResource myResource, OCResourceResult *update);

	/// This is a private function to perform operations related to door resource
	void performDoorOperations(OCResource myResource, OCResourceResult *update);
};

} // namespace OC
