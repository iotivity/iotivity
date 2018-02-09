/*
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef EXAMPLE_OCF_LIGHT_RESOURCE_H_
#define EXAMPLE_OCF_LIGHT_RESOURCE_H_

#include "octypes.h"
#include "OCRepresentation.h"
#include "OCResourceRequest.h"

/**
* default Resource class, so that we have to define less variables/functions.
*/
class Resource
{
public:
    /**
     * destructor
     */
    virtual ~Resource() { };
protected:
    OCResourceHandle m_resourceHandle;
    OC::OCRepresentation m_rep;

    /**
     * Virtual entity handler for this resource. Child classes are responsible
     * for implementing this function.
     *
     * @param request the incoming request to handle
     *
     * @return OCEntityHandlerResult OC_EH_OK on success or other result indicating failure.
     */
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request)=0;
};

#endif /* EXAMPLE_OCF_LIGHT_RESOURCE_H_ */
