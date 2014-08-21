//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
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
