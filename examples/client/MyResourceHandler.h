//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "ocapi.h"
#include "OCResourceHandler.h"
#include "MyObserverHandler.h"

using namespace OC;

class MyResourceHandler :
	public OCResourceHandler
{
public:
	MyResourceHandler(void);
	virtual ~MyResourceHandler(void);
	void onFoundResource(OCResourceResult *update, void *params);
	void onCompleted();
	void onFailed();

private:
	MyObserverHandler myObserverHandler;

};

