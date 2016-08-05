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
         * @param userId Identifier of the user obtained by account registration.
         * @param accessToken Identifier of the resource obtained by account registration.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult signIn(const std::string& userId,
                             const std::string& accessToken,
                             PostCallback cloudConnectHandler);

        /**
         * Function for sign-out to account server
         *
         * @param userId Identifier of the user obtained by account registration.
         * @param accessToken Identifier of the resource obtained by account registration.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult signOut(const std::string& userId,
                              const std::string& accessToken,
                              PostCallback cloudConnectHandler);

        /**
         * Function for refresh access token to account server
         *
         * @param userId Identifier of the user obtained by account registration.
         * @param refreshToken Refresh token used for access token refresh.
         * @param cloudConnectHandler Callback function that will get the result of the operation.
         *
         * @return Returns ::OC_STACK_OK if success
         */
        OCStackResult refreshAccessToken(const std::string& userId,
                                         const std::string& refreshToken,
                                         PostCallback cloudConnectHandler);

    private:
        std::weak_ptr<IClientWrapper> m_clientWrapper;
        std::string m_deviceID;
        std::string m_host;
        OCConnectivityType m_connType;
        QualityOfService m_defaultQos;

    private:
        OCAccountManager(std::weak_ptr<IClientWrapper> clientWrapper,
                         const std::string& host,
                         OCConnectivityType connectivityType);

        OCStackResult signInOut(const std::string& userId,
                                const std::string& accessToken,
                                bool isSignIn,
                                PostCallback cloudConnectHandler);
    };
} // namespace OC

#endif // OC_ACCOUNT_MANAGER_H_

