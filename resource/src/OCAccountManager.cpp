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

#include "OCAccountManager.h"
#include "OCUtilities.h"

#include "ocstack.h"

namespace OC {

using OC::result_guard;
using OC::checked_guard;

OCAccountManager::OCAccountManager(std::weak_ptr<IClientWrapper> clientWrapper,
                                   const std::string& host,
                                   OCConnectivityType connectivityType)
 : m_clientWrapper(clientWrapper), m_host(host), m_connType(connectivityType)
{
    if (m_host.empty() || m_clientWrapper.expired())
    {
        throw OCException(OC::Exception::INVALID_PARAM);
    }

    const char* di = OCGetServerInstanceIDString();
    if (!di)
    {
        oclog() << "The mode should be Server or Both to generate UUID" << std::flush;
        throw OCException(OC::Exception::INVALID_PARAM);
    }

    m_deviceID.append(di);
    checked_guard(m_clientWrapper.lock(), &IClientWrapper::GetDefaultQos, m_defaultQos);
}

OCAccountManager::~OCAccountManager()
{
}

std::string OCAccountManager::host() const
{
    return m_host;
}

OCConnectivityType OCAccountManager::connectivityType() const
{
    return m_connType;
}

OCStackResult OCAccountManager::signUp(const std::string& authProvider,
                                       const std::string& authCode,
                                       PostCallback cloudConnectHandler)
{
    return result_guard(signUp(authProvider, authCode, QueryParamsMap(), cloudConnectHandler));
}

OCStackResult OCAccountManager::signUp(const std::string& authProvider,
                                       const std::string& authCode,
                                       const QueryParamsMap& options,
                                       PostCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACCOUNT_URI;

    OCRepresentation rep;
    rep.setValue(OC_RSRVD_DEVICE_ID, m_deviceID);
    rep.setValue(OC_RSRVD_AUTHPROVIDER, authProvider);
    rep.setValue(OC_RSRVD_AUTHCODE, authCode);

    if (!options.empty())
    {
        OCRepresentation optionsRep;
        for (auto iter : options)
        {
            optionsRep[iter.first] = iter.second;
        }
        rep.setValue(OC_RSRVD_OPTIONS, optionsRep);
    }

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::signIn(const std::string& userId,
                                       const std::string& accessToken,
                                       PostCallback cloudConnectHandler)
{
    return result_guard(signInOut(userId, accessToken, true, cloudConnectHandler));
}

OCStackResult OCAccountManager::signOut(const std::string& userId,
                                        const std::string& accessToken,
                                        PostCallback cloudConnectHandler)
{
    return result_guard(signInOut(userId, accessToken, false, cloudConnectHandler));
}

OCStackResult OCAccountManager::signInOut(const std::string& userId,
                                          const std::string& accessToken,
                                          bool isSignIn,
                                          PostCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACCOUNT_SESSION_URI;

    OCRepresentation rep;
    rep.setValue(OC_RSRVD_USER_ID, userId);
    rep.setValue(OC_RSRVD_DEVICE_ID, m_deviceID);
    rep.setValue(OC_RSRVD_ACCESS_TOKEN, accessToken);
    rep.setValue(OC_RSRVD_LOGIN, isSignIn);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::refreshAccessToken(const std::string& userId,
                                                   const std::string& refreshToken,
                                                   PostCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACCOUNT_TOKEN_REFRESH_URI;

    OCRepresentation rep;
    rep.setValue(OC_RSRVD_USER_ID, userId);
    rep.setValue(OC_RSRVD_DEVICE_ID, m_deviceID);
    rep.setValue(OC_RSRVD_GRANT_TYPE, OC_RSRVD_GRANT_TYPE_REFRESH_TOKEN);
    rep.setValue(OC_RSRVD_REFRESH_TOKEN, refreshToken);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}
} // namespace OC
