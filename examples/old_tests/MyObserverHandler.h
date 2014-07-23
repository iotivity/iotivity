//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "OCApi.h"
#include "OCObserverHandler.h"

namespace OC {

class MyObserverHandler :
	public OC::OCObserverHandler
{
public:
	void onObserverUpdate(std::string propertyName, void *value);
};

} // namespace OC
