//******************************************************************
//
// Copyright 2018 Beechwoods Software All Rights Reserved.
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

#ifndef OC_ATOMICMEASUREMENT_H
#define OC_ATOMICMEASUREMENT_H

#include "ocstack.h"
#include "ocresourcehandler.h"

/**
 * This function is the default entity handler for the atomic measurement resource.
 * @param[in] flag Entity handler flag for the atomic measurement resource
 * @param[in] ehRequest Entity handler request received for the atomic measurement resource
 *
 * @return OCStackResult
 */
OCStackResult DefaultAtomicMeasurementEntityHandler (OCEntityHandlerFlag flag,
    OCEntityHandlerRequest *entityHandlerRequest);

/**
 * This function is the entity handler the atomic measurement substitutes to each of its
 * individual resources' entity handler, in order to correctly control and process
 * atomic measurement, and implement its restrictions.
 * @param[in] flag Entity handler flag for the individual resource
 * @param[in] ehRequest Entity handler request received for the individual resource
 * @param[in] ehCallbackParam Entity handler callback parameter for the individual resource
 *
 * @return OCEntityHandlerResult
 */
OCEntityHandlerResult DefaultAtomicMeasurementResourceEntityHandler(OCEntityHandlerFlag flag,
    OCEntityHandlerRequest *ehRequest, void *ehCallbackParam);

/**
 * This function notifies the atomic measurement that a new measurement is ready. This notification will
 * trigger the atomic measurement sending GET requests to all the individual resources composing it.
 * @param[in] amResourceHandle Handle of the atomic measurement resource to which the individual
 *            resource belongs
 *
 * @return OCStackResult
 */
OCStackResult NewAtomicMeasurementReadyNotificationHandler(const OCResourceHandle amResourceHandle);

/**
 * This function creates the RepPayloadArray for links parameter of an atomic measurement resource.
 * @param[in] resourceUri Resource URI (this should be an atomic measurement resource)
 * @param[in] isOCFContentFormat true if content is OCF (from OCEntityHandlerRequest)
 * @param[in] devAddr Structure pointing to the address. (from OCEntityHandlerRequest)
 * @param[in] insertSelfLink true if links array must contain a self link
 * @param[out] createdArraySize return value array size, Null is allowed if no need to know size
 * @note: The destroy of OCRepPayloadValue is not supported. Instead, use
 *        OCRepPayloadDestroy(...) to destroy RepPayload of the atomic measurement Resource
 *
 * @return linksRepPayloadArray The *RepPayload Array pointer for links parameter of collection
 * @see OCLinksPayloadArrayCreate API doxygen for API usage
 */
OCRepPayload** BuildAtomicMeasurementLinksPayloadArray(const char* resourceUri,
                    bool isOCFContentFormat, OCDevAddr* devAddr, bool insertSelfLink,
                    size_t* createdArraySize);

#endif //OC_ATOMICMEASUREMENT_H
