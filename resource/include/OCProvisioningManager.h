//****************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef OC_PROVISIONINGMANAGER_CXX_H_
#define OC_PROVISIONINGMANAGER_CXX_H_

#include <thread>

#include "pinoxmcommon.h"
#include "ocprovisioningmanager.h"
#include "OCApi.h"
#include "OCPlatform_impl.h"

namespace OC
{
    class OCSecureResource;

    typedef std::vector<std::shared_ptr<OCSecureResource>> DeviceList_t;
    typedef std::vector<OicUuid_t> UuidList_t;
    typedef std::vector<OCProvisionResult_t> PMResultList_t;
    typedef std::function<void(PMResultList_t *result, int hasError)> ResultCallBack;

    struct ProvisionContext
    {
        ResultCallBack callback;
        ProvisionContext(ResultCallBack cb) : callback(cb){}
    };

    /**
     * This class is for credential's to be set to devices.
     * The types supported are
     *              0:  no security mode
     *              1:  symmetric pair-wise key
     *              2:  symmetric group key
     *              4:  asymmetric key
     *              8:  signed asymmetric key (aka certificate)
     *              16: PIN /password
     */
    class Credential
    {
            OicSecCredType_t type;
            size_t keySize;
        public:
            Credential() = default;
            Credential(OicSecCredType_t type, size_t size) : type(type), keySize(size)
            {}

            /**
             * API to get credential type of device.
             * @return credential type of device.
             */
            OicSecCredType_t getCredentialType() const
            {
                return type;
            }

            /**
             * API to get size of credential key type.
             * @return size of credential key type.
             */
            size_t getCredentialKeySize() const
            {
                return keySize;
            }

            /**
             * API to set credential type of device.
             * Device can have following credential types
             *  - symmetric pair-wise key
             *  - symmetric group key
             *  - asymmetric key
             *  - signed asymmetric key (aka certificate)
             *  - PIN /password
             * @param type credential type.
             */
            void setCredentialType(OicSecCredType_t type)
            {
                this->type = type;
            }

            /**
             * API to set size of credential key type.
             * @param keySize credential key size.
             * @note can be either 128 or 256 for symmetric pair-wise key
             */
            void setCredentialKeySize(size_t keySize)
            {
                this->keySize = keySize;
            }
    };

    class OCSecure
    {
        public:
            /**
             * The API is responsible for initialization of the provisioning manager. It will load
             * provisioning database which have owned device's list and their linked status.
             *
             * @param dbPath file path of the sqlite3 database.
             *
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            static OCStackResult provisionInit(const std::string& dbPath);

            /**
             * API is responsible for discovery of devices in it's subnet. It will list
             * all the device in subnet which are not yet owned.
             *
             * @param timeout Timeout in seconds, time until which function will listen to
             *                    responses from server before returning the list of devices.
             * @param list List of candidate devices to be provisioned.
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            static OCStackResult discoverUnownedDevices(unsigned short timeout,
                    DeviceList_t &list);

            /**
             * API is responsible for discovery of devices in it's subnet. It will list
             * all the device in subnet which are already owned by calling provisioning client.
             *
             * @param timeout Timeout in seconds, time until which function will listen to
             *                    responses from server before returning the list of devices.
             * @param list List of owned devices.
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            static OCStackResult discoverOwnedDevices(unsigned short timeout,
                    DeviceList_t &list);

            /**
             * API for registering Ownership transfer methods for a particular transfer Type.
             *
             * @param oxm Ownership transfer method.
             * @param callbackData CallbackData Methods for ownership transfer.
             * @param inputPin Callback method to input pin for verification.
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm,
                    OTMCallbackData_t* callbackData, InputPinCallback inputPin);

            /**
             * API to get status of all the devices in current subnet. The status include endpoint
             * information and doxm information which can be extracted during owned and unowned
             * discovery. Along with this information, API will provide information about
             * devices' status.
             * Device can have following states
             *  - ON/OFF: Device is switched on or off.
             *
             * @param timeout Wait time for the API.
             * @param ownedDevList  List of owned devices.
             * @param unownedDevList  List of unowned devices.
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            static OCStackResult getDevInfoFromNetwork(unsigned short timeout,
                    DeviceList_t &ownedDevList,
                    DeviceList_t &unownedDevList);
            /**
             * Server API to register callback to display stack generated PIN.
             *
             * @param displayPin Callback Method to Display generated PIN.
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            static OCStackResult setDisplayPinCB(GeneratePinCallback displayPin);
    };

    /**
     * This class represents a secure virtual device, which can be provisioned by the
     * provisioning client.
     */
    class OCSecureResource
    {
        private:
            std::weak_ptr<std::recursive_mutex> m_csdkLock;
            OCProvisionDev_t *devPtr;   // pointer to device.

        public:
            OCSecureResource();
            OCSecureResource(std::weak_ptr<std::recursive_mutex> csdkLock, OCProvisionDev_t *dPtr);

            ~OCSecureResource();

            /**
             * API to provision credentials between two devices and ACLs for the devices who
             * act as a server.
             *
             * @param cred  Type of credentials & key size to be provisioned to the device.
             * @param acl1  ACL for device 1. If this is not required set NULL.
             * @param device2  Second device to be provisioned.
             * @param acl2  ACL for device 2. If this is not required set NULL.
             * @param resultCallback Callback will be called when provisioning request receives
             *                           a response from first resource server.
             * @return  ::OC_STACK_OK in case of success and other value otherwise.
             */
            OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1,
                    const OCSecureResource &device2, const OicSecAcl_t* acl2,
                    ResultCallBack resultCallback);

            /**
             * API to do ownership transfer for un-owned device.
             *
             * @param resultCallback Result callback function to be invoked when
             *                           ownership transfer finished.
             * @return ::OC_STACK_OK in case of success and other value otherwise.
             */
            OCStackResult doOwnershipTransfer(ResultCallBack resultCallback);

            /**
             * API to send ACL information to resource.
             *
             * @param acl ACL to provision.
             * @param resultCallback Callback will be called when provisioning request
             *                           receives a response from resource server.
             * @return  ::OC_STACK_OK in case of success and other value otherwise.
             */
            OCStackResult provisionACL(const OicSecAcl_t* acl,
                    ResultCallBack resultCallback);

            /**
             * API to provision credential to devices.
             *
             * @param cred Type of credentials to be provisioned to the device.
             * @param device2 Second device' instance, representing resource to be provisioned.
             * @param resultCallback Callback will be called when provisioning request receives
             *                           a response from first resource server.
             * @return  ::OC_STACK_OK in case of success and other value otherwise.
             */
            OCStackResult provisionCredentials(const Credential &cred,
                    const OCSecureResource &device2,
                    ResultCallBack resultCallback);

           /**
            * API to remove the credential & relationship between the two devices.
            *
            * @param device2 Second device information to be unlinked.
            * @param resultCallback Callback provided by API user, callback will be called when
            *            device unlink is finished.
            * @return  ::OC_STACK_OK in case of success and other value otherwise.
            */
            OCStackResult unlinkDevices(const OCSecureResource &device2,
                    ResultCallBack resultCallback);

            /**
             * API to remove device credential from all devices in subnet.
             *
             * @param resultCallback Callback provided by API user, callback will be called when
             *            credential revocation is finished.
             * @param waitTimeForOwnedDeviceDiscovery Maximum wait time for owned device
             *            discovery in seconds.
             * @return  ::OC_STACK_OK in case of success and other value otherwise.
             */
            OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery,
                    ResultCallBack resultCallback);

            /**
             * This method is used to get linked devices' IDs.
             *
             * @param uuidList Information about the list of linked devices uuids.
             * @return  ::OC_STACK_OK in case of success and other value otherwise.
             */
            OCStackResult getLinkedDevices(UuidList_t &uuidList);

            /**
             * API to get the device ID of this resource.
             * @return device ID.
             */
            std::string getDeviceID();

            /**
             * API to get the information of device for provisioning.
             * @return  @ref OCProvisionDev_t Reference provides information of device for provisioning.
             */
            OCProvisionDev_t* getDevPtr()const;

            /**
             * This function returns the device's IP address.
             * @return device address.
             */
            std::string getDevAddr();

            /**
             * This function returns the device's Status.
             * @return Device status (1 = ON and 2 = OFF).
             */
            int getDeviceStatus();

            /**
             * This function provides the owned status of the device.
             * @return Device owned status.
             */
            bool getOwnedStatus();

        private:
            /**
             * Common callback wrapper, which will be called from OC-APIs.
             */
            static void callbackWrapper(void* ctx, int nOfRes,
                    OCProvisionResult_t *arr, bool hasError);

            void validateSecureResource();
    };

}
#endif // OC_PROVISIONINGMANAGER_CXX_H_
