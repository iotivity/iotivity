//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef __CONTROLLEEMANGER__
#define __CONTROLLEEMANGER__

#include "OCDefines.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "MyDevice.h"
#include "ResourceTypeEnum.h"
#include "Configuration.h"
#include <list>
#include <string>
#include <map>

namespace OC
{
    class ControlleeManager
    {
        public:
            /**
             * ControlleeManager - Default constructor for this class. The default values to initialise the Manager are used. Use parametised constructor to specialise
             *@param - none
             *@return - none
             **/
            ControlleeManager();

            /**
             * ~ControlleeManager - Default Destructor for this class.
             *@param - none
             *@return - none
             **/
            ~ControlleeManager();
            /**
             *ControlleeManager - Controllee Manager's parameterised contructor - The application will exit if the input cannot be validated
             * @param serviceType - The serviceType desired. The serviceTypes are listed in OCDefines.h
             * @param modeType - The mode type desired. The modes are listed in OCDefines.h
             * @param ipAddress - The IPaddress of the underlying framework service. Note that this is not the device IP address
             * @param port - The port number for the undelying framework service. Note that this is not the port number of the device service
             * @param qualityOfService - The quality of service desired. The possible values for the qulaity of service are listed in OCDefines.h
             * @return - none
             */
            ControlleeManager(const OC::PServiceType &serviceType,
                              const OC::PModeType &modeType, const std::string &ipAddress,
                              const uint16_t &port,
                              const OC::PQualityOfService &qualityOfService);

            /**
             *ControlleeManager - Controllee Manager's parameterised constructor - The application will exit if the input cannot be validated
             *@param ipAddress - The IP address of the underlying framework service. Note that this is not the device IP address.
             *@return - none
             */
            ControlleeManager(const std::string &ipAddress);

            /**
             *Donot use - Deprecated
             */
            static ControlleeManager *getInstance(OC::Cm::IContext *);

            /**
             * init - Invoked by the App to initialize any dynamic data on the ControlleeManager
             * @param -none
             * @return type:bool - true if init succeeded else false.
             */
            bool init();

            /**
             * start - Invoked by the App to begin controllee specific processing of an instantiated controlleeManager. Starts any needed
             * threads and instantiates and initializes the subscriptionManager
             * @param -none
             * @return type:bool- true if init succeeded else false.
             */
            bool start();

            /**
             * stop - Invoked by the App to shutdown controllee specific processing of an instantiated controlleeManager. Stops any
             * threads and de-initializes the subscriptionManager
             * @param -none
             * @return bool- true if start succeeded else false.
             */
            bool stop();

            /**
             * getSubscriptionManager -  Returns the SubscriptionManager instance
             * @param - none
             * @return type:SubscriptionManager
             */
            OC::Cm::Notification::ISubscriptionManager *getSubscriptionManager();

            /**
             * setSubscriptionManager -Available to allow the App to create and set an instance of a SubscriptionManager
             * @param subscription manager
             * @return bool- true if setSubscriptionManager succeeded else false.
             */
            bool setSubscriptionManager(
                OC::Cm::Notification::ISubscriptionManager &subscriptionManager);

            /**
             * setConfiguration- Available to allow the App to create and set an instance of a Configuration
             * @param configuration
             * @return bool- true if setConfiguration succeeded else false.
             */
            bool setConfiguration(OC::Cm::Configuration *config);

            /**
             * addResources - The controllee manager starts with a device already present. This method helps to add the resources to the device.
             *@param - list<OC::ResourceTypeEnum> - The list of resource types to add. The list of the supported resources is available at ResourceTypeEnums.h
             *@return - ControlleeStatus - The controllee return value. All the return values are in OCDefines.h
             */
            ControlleeStatus addResources(
                const std::list<ResourceTypeEnum> &resourceTypes, int &posOfError);

            /**
             *getPlatform - Get the platform into the
             *@param - none
             *@return - OCPlatform*  - The pointer to the platform
             */
            //OC::OCPlatform* getPlatform();

            /**
             *getContext - Get the context to this instance of the device
             *@param - none
             *@return - IContext*  - The context to this instance.
             */
            OC::Cm::IContext *getContext();

            /**
             *getConfiguration - Get the configuration of this device
             *@param - none
             *@return - Configuration*  -Configuration of the current device
             */
            OC::Cm::Configuration *getConfiguration();

            /**
             *getMyDevice - Get the device object associated with the manager
             *@param - none
             *@return - MyDevice*  - The current device
             */
            ::MyDevice *getMyDevice();

        private:
            MyDevice *_controllee;  /**< The device object.*/
            OC::ControlleeStatus _status;/**< The status of this object.*/
            std::map<string, OCResourceHandle> _uriHandleMap;/**< Represents the OCPlatform class instance which is under the OIC base. This can be used for OC layer operations.*/
            OC::Cm::Configuration *_configuration; /**< Configuration class need for App populated values */
            OC::Cm::IContext *_context; /**< Represents instance of Runtime Context data */
            /**
             * validateOCParams - Internal validation for OC platform validation
             * @param serviceType - The serviceType desired. The serviceTypes are listed in OCDefines.h
             * @param modeType - The mode type desired. The modes are listed in OCDefines.h
             * @param ipAddress - The IPaddress of the underlying framework service. Note that this is not the device IP address
             * @param port - The port number for the undelying framework service. Note that this is not the port number of the device service
             * @param qualityOfService - The quality of service desired. The possible values for the qulaity of service are listed in OCDefines.h
             * @return type:bool - true if valid false if not
             */
            bool validateOCParams(const OC::PServiceType &serviceType,
                                  const OC::PModeType &modeType, const std::string &ipAddress,
                                  const int &port, const OC::PQualityOfService &qualityOfService);

            /**
             * getURI - Get an OIC uri for a resource Type
             * @param resourceString - The resoruce in string format
             * @return type:string - the URI
             */
            std::string getURI(const std::string &resourceString);

            /**
             * getoicTypes - Get an OIC resourceType for a resource string
             * @param resourceString - The resource in string format
             * @return -type:bool - the OIC type
             */
            std::string getoicTypes(const std::string &resourceString);

            /**
             * isValidIPAddress - Validate the format of a provided IP address
             * @param address - The IP address in string format
             * @return - type:bool - true if the address is valid, else false.
             */
            bool isValidIPAddress(std::string address);

            /**
             * init_private - Private helper function to init some components of the framework.
             * @return - type:bool - False on an error
             */
            bool init_private();
    };
}

#endif // __CONTROLLEEMANGER__
