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

#ifndef EXAMPLE_OCF_LIGHT_BINARYSWITCHRESOURCE_H_
#define EXAMPLE_OCF_LIGHT_BINARYSWITCHRESOURCE_H_

#include <vector>
#include <string>
#include "Resource.h"

#include "OCApi.h"
#include "OCRepresentation.h"

/**
 * Class implements the OCF oic.r.switch.binary resource.
 *
 * This class is responsible for handling GET and POST requests sent from
 * a client
 */
class BinarySwitchResource : public Resource
{
public:
    /**
     * constructor
     *
     * @param resourceUri the URI the resource will be register with
     */
    BinarySwitchResource(std::string resourceUri = "/binaryswitch");

    /**
     * destructor
     */
    virtual ~BinarySwitchResource(void);

    /**
     * Register the resource with the server
     *
     * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
     * setting resourceProperty as OC_OBSERVABLE will allow observation
     * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
     * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
     * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
     * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
     *
     * @param resourceProperty indicates the property of the resource. Defined in octypes.h.
     */
    OCStackResult registerResource(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

    /**
     * get the value of the binary switch
     *
     * @return value of the binary switch
     */
    bool getValue(void);

    /**
     * set the value of the binary switch
     *
     * @param newValue new binaryswitch value
     */
    void setValue(bool newValue);

    /**
     * Attempt to send out notifications to observing clients.
     * If no value on the device has been changed the notification
     * may not be sent.
     *
     * @return OC_STACK_OK on success
     */
    OCStackResult sendNotification(void);

private:

    /**
     * Make the payload for the retrieve function (e.g. GET)
     *
     * @param queries the query parameters for this call
     */
    OC::OCRepresentation get(OC::QueryParamsMap queries);

    /**
     * Parse the payload for the update function (e.g. POST)
     *
     * @param queries the query parameters for this call
     * @param rep the response to get the property values from
     *
     * @return OCEntityHandlerResult OC_EH_OK on success or other result indicating failure.
     */
    OCEntityHandlerResult post(OC::QueryParamsMap queries, const OC::OCRepresentation& rep);

    // resource types and interfaces as array..
    std::string m_resourceUri;
    std::string m_RESOURCE_TYPE[1]; // rt value (as an array)
    std::string m_RESOURCE_INTERFACE[2]; // interface if (as an array)
    std::string m_IF_UPDATE[3]; // updateble interfaces
    OC::ObservationIds m_interestedObservers;

    // member variables for path: /binaryswitch
    std::vector<std::string>  m_var_value_rt;
    std::string m_var_name_rt; // the name for the attribute

    std::string m_var_value_n; // the value for the attribute
    std::string m_var_name_n; // the name for the attribute


    std::vector<std::string>  m_var_value_if;
    std::string m_var_name_if; // the name for the attribute

    bool m_var_value_value; // the value for the attribute
    std::string m_var_name_value; // the name for the attribute

protected:
    /**
     * Check if the interface is an updatable interface.
     *
     * @param  interface_name the interface name used during the request
     *
     * @return true: updatable interface
     */
    bool in_updatable_interfaces(std::string interface_name)
    {
        for (size_t i = 0; i < (sizeof(m_IF_UPDATE)/sizeof(m_IF_UPDATE[0])); i++)
        {
            if (m_IF_UPDATE[i].compare(interface_name) == 0)
                return true;
        }
        return false;
    }

    /**
     * The entity handler for this resource
     *
     * @param request the incoming request to handle
     *
     * @return OCEntityHandlerResult OC_EH_OK on success or other result indicating failure.
     */
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
};

#endif /* EXAMPLE_OCF_LIGHT_BINARYSWITCHRESOURCE_H_ */
