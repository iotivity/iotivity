/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/

/**
 * @file   simulator_single_resource.h
 *
 * @brief   This file contains a class which represents a simulator resource that provides a set
 *             of functions for operating a resource and performing automation on attribute values.
 */

#ifndef SIMULATOR_SINGLE_RESOURCE_H_
#define SIMULATOR_SINGLE_RESOURCE_H_

#include "simulator_resource.h"

/**
 * @class   SimulatorSingleResource
 * @brief   This class provides a set of APIs for handling simulated resource.
 */
class SimulatorSingleResource : public SimulatorResource
{
    public:

        /**
         * API to get attribute from resource's resource model.
         *
         * @param attrName - Attribute's name.
         * @param attribute - A attribute of resource's resource model.
         *
         * @return bool - true on success, otherwise false.
         */
        virtual bool getAttribute(const std::string &attrName,
                                  SimulatorResourceModel::Attribute &attribute) = 0;

        /**
         * API to add a new attribute to the resource model.
         *
         * @param attribute - Attribute to be add to model.
         * @param notify - If value is true then notification will be send to observers on success.
         *                             This flag is set to true by default.
         *
         * NOTE: API throws @SimulatorException exceptions on failure.
         */
        virtual void addAttribute(const SimulatorResourceModel::Attribute &attribute,
                                  bool notify = true) = 0;

        /**
         * API to get property of attribute's value.
         *
         * @param attrName - Attribute's name.
         * @param property - Property which was set to attribute's value.
         *
         * @return bool - true on success, otherwise false.
         */
        virtual bool getAttributeProperty(const std::string &attrName,
                                          SimulatorResourceModel::AttributeProperty &property) = 0;

        /**
         * API to set property of attribute's value.
         *
         * @param attrName - Attribute's name.
         * @param property - Property to be set for attribute's value.
         *
         * @return bool - true on success, otherwise false.
         */
        virtual bool setAttributeProperty(const std::string &attrName,
                                          const SimulatorResourceModel::AttributeProperty &property) = 0;

        /**
         * API to remove an attribute from the resource model.
         *
         * @param attrName - Name of the attribute to be removed.
         * @param notify - If value is true then notification will be send to observers on success.
         *                             This flag is set to true by default.
         *
         * @return bool - true on success, otherwise false.
         */
        virtual bool removeAttribute(const std::string &attrName, bool notify = true) = 0;

        /**
         * API to update the value of an attribute.
         *
         * @param attrName - Name of the attribute.
         * @param value - Value of the attribute.
         * @param notify - If value is true then notification will be send to observers on success.
         *                             This flag is set to true by default.
         */
        template <typename T>
        bool updateAttributeValue(const std::string &attrName, const T &value, bool notify = true)
        {
            SimulatorResourceModel::Attribute attribute(attrName);
            attribute.setValue(value);
            return updateAttributeValue(attribute, notify);
        }

        /**
         * API to update the value of an attribute.
         *
         * @param attribute - A resource model attribute.
         * @param notify - If value is true then notification will be send to observers on success.
         *                             This flag is set to true by default.
         *
         * @return bool - true on success, otherwise false.
         */
        virtual bool updateAttributeValue(const SimulatorResourceModel::Attribute &attribute,
                                          bool notify = true) = 0;

        /**
         * API to start the attribute value update automation for all attributes.
         * Values for the attributes will be selected from their allowed range
         * and the updated resource model will be notified to all the observers of the resource.
         *
         * @param type - Automation type.
         * @param updateInterval - Time in milliseconds to be set as interval between updating
         *                                              attribute values.
         * @param callback - Callback to get notifiy when update automation is finished.
         *
         * @return ID representing update automation session.
         * NOTE: API throws @InvalidArgsException, @SimulatorException exceptions.
         */
        virtual int startResourceUpdation(AutomationType type, int updateInterval,
                                          updateCompleteCallback callback) = 0;

        /**
         * This method is used to start the attribute value update automation for
         * specific attribute. Values for the attribute will be selected from its allowed range
         * and the updated resource model will be notified to all the observers of the resource.
         *
         * @param attrName - Name of the attribute to be automated.
         * @param type - Automation type.
         * @param updateInterval - Time in milliseconds to be set as interval between updating
         *                                              attribute values.
         * @param callback - Callback to get notifiy when update automation is finished.
         *
         * @return ID representing update automation session.
         * NOTE: API throws @InvalidArgsException, @SimulatorException exceptions.
         */
        virtual int startAttributeUpdation(const std::string &attrName, AutomationType type,
                                           int updateInterval, updateCompleteCallback callback) = 0;

        /**
         * API to get the Ids of all ongoing resource update automation .
         *
         * @return vector of resource automation ids.
         */
        virtual std::vector<int> getResourceUpdationIds() = 0;

        /**
         * API to get the Ids of all ongoing attribute update automation .
         *
         * @return vector of attribute automation ids.
         */
        virtual std::vector<int> getAttributeUpdationIds() = 0;

        /**
         * API to stop the resource/attribute automation.
         *
         * @param id - Identifier for automation.
         */
        virtual void stopUpdation(int id) = 0;
};

typedef std::shared_ptr<SimulatorSingleResource> SimulatorSingleResourceSP;

#endif
