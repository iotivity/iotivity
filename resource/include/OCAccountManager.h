/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef OC_ACCOUNT_MANAGER_H_
#define OC_ACCOUNT_MANAGER_H_

#include <map>

#include <OCApi.h>
#include <IClientWrapper.h>
#include <InProcClientWrapper.h>

namespace OC
{
    class OCAccountManager
    {
        friend class OCPlatform_impl;

    public:
        typedef std::shared_ptr<OCAccountManager> Ptr;

        OCAccountManager(OCAccountManager&&) = default;
        OCAccountManager(const OCAccountManager&) = delete;
        OCAccountManager& operator=(OCAccountManager&&) = delete;
        OCAccountManager& operator=(const OCAccountManager&) = delete;

        virtual ~OCAccountManager(void);

        /**
        * Function to get the host address of account server
        *
        * @return std::string host address
        */
        std::string host() const;

        /**
        * Function to get the connectivity type for account server
        *
        * @return enum connectivity type (flags and adapter)
        */
        OCConnectivityType connectivityType() const;

        /**
         * Function for account registration to account server
         *
         * @param authProvider Provider name used for authentication.
         * @param authCode The authorization code obtained by using an authorization server
         *                 as an intermediary between the client and resource owner.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult signUp(const std::string& authProvider,
                             const std::string& authCode,
                             PostCallback cloudConnectHandler);

        /**
         * Overload
         *
         * @param authProvider Provider name used for authentication.
         * @param authCode The authorization code obtained by using an authorization server
         *                 as an intermediary between the client and resource owner.
         * @param options The option values depends on auth provider.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult signUp(const std::string& authProvider,
                             const std::string& authCode,
                             const QueryParamsMap& options,
                             PostCallback cloudConnectHandler);

        /**
         * Function for sign-in to account server
         *
         * @param userUuid Identifier of the user obtained by account registration.
         * @param accessToken Identifier of the resource obtained by account registration.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult signIn(const std::string& userUuid,
                             const std::string& accessToken,
                             PostCallback cloudConnectHandler);

        /**
         * Function for sign-out to account server
         *
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult signOut(PostCallback cloudConnectHandler);

        /**
         * Function for refresh access token to account server
         *
         * @param userUuid Identifier of the user obtained by account registration.
         * @param refreshToken Refresh token used for access token refresh.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult refreshAccessToken(const std::string& userUuid,
                                         const std::string& refreshToken,
                                         PostCallback cloudConnectHandler);

        /**
         * Function to get information of the user to account server
         *
         * @param userUuid Identifier of the user to get information.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult searchUser(const std::string& userUuid,
                                 GetCallback cloudConnectHandler);

        /**
         * Overload
         *
         * @param queryParams Map which can have the query key and value for specific users.
         *                    account server can response information of more than one user.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult searchUser(const QueryParamsMap& queryParams,
                                 GetCallback cloudConnectHandler);

        /**
         * Function to delete the device registered on the account signed-in
         *
         * @param deviceId Device ID to delete.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult deleteDevice(const std::string& deviceId,
                                   DeleteCallback cloudConnectHandler);

        /**
         * Function to create a group on account server
         *
         * @param groupType Group type that can be used for referencing default group ACL creation.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult createGroup(AclGroupType groupType,
                                  PostCallback cloudConnectHandler);

        /**
         * Function to get a list of groups joined from account server
         *
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult getGroupList(GetCallback cloudConnectHandler);

        /**
         * Function to delete the group from account server
         *
         * @param groupId Group ID to delete.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult deleteGroup(const std::string& groupId,
                                  DeleteCallback cloudConnectHandler);

        /**
         * Function to join the group on account server
         *
         * @param groupId Group ID to join
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult joinGroup(const std::string& groupId,
                                PostCallback cloudConnectHandler);

        /**
         * Function to add devices to the group on account server
         *
         * @param groupId Group ID to add devices.
         * @param deviceId List of devices to add.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult addDeviceToGroup(const std::string& groupId,
                                       const std::vector<std::string>& deviceId,
                                       PostCallback cloudConnectHandler);

        /**
         * Function to get information of the group from account server
         *
         * @param groupId Group ID to get information.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult getGroupInfo(const std::string& groupId,
                                   GetCallback cloudConnectHandler);

        /**
         * Function to leave the group joined on account server
         *
         * @param groupId Group ID to leave.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult leaveGroup(const std::string& groupId,
                                 DeleteCallback cloudConnectHandler);

        /**
         * Function to delete devices from the group on account server
         *
         * @param groupId Group ID to delete devices.
         * @param deviceId List of devices to delete.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult deleteDeviceFromGroup(const std::string& groupId,
                                            const std::vector<std::string>& deviceId,
                                            DeleteCallback cloudConnectHandler);

        /**
         * Function to register observe to the group on account server
         * User can receive a notify when the group get changed (eg. new user/device added)
         *
         * @param groupId Group ID to observe.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult observeGroup(const std::string& groupId,
                                   ObserveCallback cloudConnectHandler);

        /**
         * Function to cancel observe to the group on account server
         *
         * @param groupId Group ID to observe.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult cancelObserveGroup(const std::string& groupId);

        /**
         * Function to register observe to invitation resource on account server
         * User can receive a invitation which is including group ID to join
         * Once receive a invitation, user should call 'deleteInvitation' to delete a invitation
         * on account server.
         *
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult observeInvitation(ObserveCallback cloudConnectHandler);

        /**
         * Function to cancel observe to invitation resource on account server
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult cancelObserveInvitation();

        /**
         * Function to send a invitation to invite a user into a group
         *
         * @param groupId Group ID for inviting.
         * @param userUuid Identifier of the user to invite.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult sendInvitation(const std::string& groupId,
                                     const std::string& userUuid,
                                     PostCallback cloudConnectHandler);

        /**
         * Function to cancel a invitation on account server that user has sent
         *
         * @param groupId Group ID to cancel a invitation.
         * @param userUuid Identifier of the user to cancel a invitation.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult cancelInvitation(const std::string& groupId,
                                       const std::string& userUuid,
                                       DeleteCallback cloudConnectHandler);

        /**
         * Function to delete a invitation on account server that user has received
         *
         * @param groupId Group ID to delete a invitation.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult deleteInvitation(const std::string& groupId,
                                       DeleteCallback cloudConnectHandler);

    private:
        std::weak_ptr<IClientWrapper> m_clientWrapper;
        std::string m_deviceID;
        std::string m_host;
        OCDoHandle m_invitationObserveHandle;
        mutable std::map<std::string, OCDoHandle> m_groupObserveHandles;
        OCConnectivityType m_connType;
        QualityOfService m_defaultQos;

    private:
        OCAccountManager(std::weak_ptr<IClientWrapper> clientWrapper,
                         const std::string& host,
                         OCConnectivityType connectivityType);

        OCStackResult signInOut(const std::string& userUuid,
                                const std::string& accessToken,
                                bool isSignIn,
                                PostCallback cloudConnectHandler);

        OCStackResult searchUser(const std::string& userUuid,
                                 const QueryParamsMap& queryParams,
                                 GetCallback cloudConnectHandler);
    };
} // namespace OC

#endif // OC_ACCOUNT_MANAGER_H_

