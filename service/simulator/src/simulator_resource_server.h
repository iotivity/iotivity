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
 * @file   simulator_resource_server.h
 *
 * @brief   This file contains a class which represents a simulator resource that provides a set
 *             of functions for operating a resource and performing automation on attribute values.
 */

#ifndef SIMULATOR_RESOURCE_SERVER_H_
#define SIMULATOR_RESOURCE_SERVER_H_

#include "simulator_resource_model.h"
#include "simulator_attribute_automation.h"
#include "simulator_error_codes.h"

class ResourceManager;
/**
 * @class   SimulatorResourceServer
 * @brief   This class provides a set of functions for operating and automating a resource.
 */
class SimulatorResourceServer
{
        friend class ResourceManager;

    public:
        typedef std::function<void (const std::string &uri, const SimulatorResourceModel &resModel)>
        ResourceModelChangedCB;

        SimulatorResourceServer();

        /**
         * This method is used to set the resource URI.
         *
         * @param uri - Resource URI
         *
         * @return SimulatorResult
         */
        SimulatorResult setURI(const std::string &uri);

        /**
         * This method is used to get the resource URI.
         *
         * @return Resource URI
         */
        std::string getURI() const;

        /**
         * This method is used to set the resource type.
         *
         * @param resourceType - Resource Type
         *
         * @return SimulatorResult
         */
        SimulatorResult setResourceType(const std::string &resourceType);

        /**
         * This method is used to get the resource URI.
         *
         * @return Resource Type
         */
        std::string getResourceType() const;

        /**
         * This method is used to set the interface type of the resource.
         *
         * @param interfaceType - Interface Type of the resource
         *
         * @return SimulatorResult
         */
        SimulatorResult setInterfaceType(const std::string &interfaceType);

        /**
         * This method is used to get the interface type of the resource.
         *
         * @return Interface type of the resource
         */
        std::string getInterfaceType() const;

        /**
         * This method is used to set the name of the resource.
         *
         * @param name - Name of the resource
         *
         * @return void
         */
        void setName(const std::string &name);

        /**
         * This method is used to get the name of the resource.
         *
         * @return Resource name
         */
        std::string getName() const;

        /**
         * This method is used to set whether resource can be observable or not.
         *
         * @param state - true for resource observable, otherwise false.
         *
         * @return SimulatorResult
         */
        SimulatorResult setObservable(bool state);

        /**
         * This method is used to get the observable state of resource.
         *
         * @return bool - true if resource is observable, otherwise false.
         */
        bool isObservable() const;

        /**
         * This method is used to start the attribute value automation for all attributes.
         * Once started, values for the attributes will be selected randomly from their allowed range
         * and the updated values will be notified to all the observers of the resource.
         *
         * @param id - Identifier for automation will be returned
         *
         * @return SimulatorResult
         */
        SimulatorResult startUpdateAutomation(AutomationType type,
                                              updateCompleteCallback callback, int &id);

        /**
         * This method is used to start the attribute value automation for a specific attribute.
         * Once started, values for the attribute will be selected randomly from its allowed range
         * and the updated value will be notified to all the observers of the resource.
         *
         * @param attrName - Name of the attribute to be automated
         * @param id - Identifier for automation will be returned
         *
         * @return SimulatorResult
         */
        SimulatorResult startUpdateAutomation(const std::string &attrName, AutomationType type,
                                              updateCompleteCallback callback, int &id);

        /**
         * This method is used to get the Ids of all ongoing resource update automation .
         *
         * @return Ids as vector of int
         */
        std::vector<int> getResourceAutomationIds();

        /**
         * This method is used to get the Ids of all ongoing attribute update automation .
         *
         * @return Ids as vector of int
         */
        std::vector<int> getAttributeAutomationIds();

        /**
        * This method is used to stop the automation.
        *
        * @param id - Identifier for automation
        */
        void stopUpdateAutomation(const int id);

        /**
         * This method is used to add a new attribute to the resource model.
         *
         * @param attrName - Name of the attribute
         * @param attrValue - Value of the attribute
         *
         * @return void
         */
        template <typename T>
        void addAttribute(const std::string &attrName, const T &attrValue)
        {
            m_resModel.addAttribute(attrName, attrValue);
            notifyListOfObservers();
        }

        /**
         * This method is used to set the value range of an attribute.
         * This method is intended to be used for attributes whose values are numbers only.
         *
         * @param attrName - Name of the attribute
         * @param min - Minimum value of the range
         * @param max - Maximum value of the range
         *
         * @return void
         */
        void setRange(const std::string &attrName, const int min, const int max);

        /**
         * This method is used to set the allowed values of an attribute.
         *
         * @param attrName - Name of the attribute
         * @param values - Allowed values
         *
         * @return void
         */
        template <typename T>
        void setAllowedValues(const std::string &attrName, const std::vector<T> &values)
        {
            m_resModel.setAllowedValues(attrName, values);
        }

        /**
         * This method is used to set the update interval time for automation.
         *
         * @param attrName - Name of the attribute
         * @param interval - Interval time in miliseconds for attribute value update automation
         *
         * @return void
         */
        void setUpdateInterval(const std::string &attrName, int interval);

        /**
         * This method is used to update the value of an attribute.
         *
         * @param attrName - Name of the attribute
         * @param value - Value of the attribute
         *
         * @return void
         */
        template <typename T>
        void updateAttribute(const std::string &attrName, const T &value)
        {
            m_resModel.updateAttribute(attrName, value);
            notifyListOfObservers();
        }

        /**
         * This method is used to update the attribute's value by taking the index of the value
         * in the allowed values range.
         *
         * @param attrName - Name of the attribute
         * @param allowedValueIndex - Index of the value in the allowed values range
         *
         * @return void
         */
        void updateAttributeFromAllowedValues(const std::string &attrName, const int allowedValueIndex);

        /**
          * This method is used to remove an attribute from the resource model.
          *
          * @param attName - Name of the attribute to be removed
          *
          * @return void
          */
        void removeAttribute(const std::string &attName);

        /**
         * This method is used to get the object of SimulatorResourceModel.
         * Attributes of the resource are accessed using this object.
         *
         * @return SimulatorResourceModel - Resource model of the resource
         */
        SimulatorResourceModel getModel() const;

        /**
         * This method is used to set the callback for receiving the notifications when the
         * resource model changes.
         *
         * @param callback - Callback to be set for receiving the notifications.
         *
         * @return SimulatorResourceModel - Resource model of the resource
         */
        void setModelChangeCallback(ResourceModelChangedCB callback);

    private:
        SimulatorResult start();
        SimulatorResult stop();
        OC::OCRepresentation getOCRepresentation();
        bool modifyResourceModel(OC::OCRepresentation &ocRep);
        OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request);
        void notifyListOfObservers ();

        SimulatorResourceModel m_resModel;
        std::string m_uri;
        std::string m_resourceType;
        std::string m_interfaceType;
        std::string m_name;
        ResourceModelChangedCB m_callback;
        UpdateAutomationManager m_updateAutomationMgr;

        OCResourceHandle m_resourceHandle;
        OCResourceProperty m_property;
        OC::ObservationIds m_interestedObservers;
};

typedef std::shared_ptr<SimulatorResourceServer> SimulatorResourceServerPtr;

#endif
