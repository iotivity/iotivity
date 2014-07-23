//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef LITESESSION_HPP_
#define LITESESSION_HPP_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <uuid.h>
#include <set>
#include <boost/shared_ptr.hpp>
#include "api.h"
using namespace Intel::STC;

#include "LiteConnection.h"

// ============================================================================
// Namespace
// ============================================================================
namespace remoting {

  class LiteSession
  {

  public:
    LiteSession(API::Context::SharedPtr pCCFContext, UUID_t uuid) : pCCFContext_(pCCFContext), sessionId_(uuid) {};
    virtual ~LiteSession() { connectionSet_.clear(); };

    virtual void invite() = 0;
    virtual void disconnect() = 0;

  public:
    bool isInvited() {
      return isInvited_;
    }

    bool isConnected() {
      return isConnected_;
    }

    void setPublicAvatarId(const UUID_t& avatarId) {
      publicAvatarId_ = avatarId;
    }

    void setPrivateAvatarId(const UUID_t& avatarId) {
      privateAvatarId_ = avatarId;
    }

    void setUserId(const UUID_t& userId) {
      userId_ = userId;
    }

    void setIssuer(const std::string& issuer) {
      issuer_ = issuer;
    }

    void setName(const std::string& name) {
      sessionName_ = name;
    }

    void setStatusText(const std::string& text) {
      statusText_ = text;
    }

    void setSecurityCode(const std::string& code) {
      securityCode_ = code;
    }

    void setApplicationSet(const std::set<UUID_t>& applicationSet) {
      applicationSet_ = applicationSet;
    }

    const UUID_t& getPublicAvatarId() {
      return publicAvatarId_;
    }

    const UUID_t& getPrivateAvatarId() {
      return privateAvatarId_;
    }

    const UUID_t& getUserId() {
      return userId_;
    }

    const std::string& getIssuer() {
      return issuer_;
    }

    const std::string& getName() {
      return sessionName_;
    }

    const std::string& getStatusText() {
      return statusText_;
    }

    const std::string& getSecurityCode() {
      return securityCode_;
    }
    const std::set<UUID_t>& getApplicationSet() {
      return applicationSet_;
    }

    void addConnection(boost::shared_ptr<LiteConnection> connection) {
      connectionSet_.insert(connection);
    }

    void removeConnection(boost::shared_ptr<LiteConnection> connection) {
      connectionSet_.erase(connection);
    }

  protected:
    bool onLocalNetwork_;
    bool onCloudNetwork_;
    bool isController_;
    bool isInvited_;
    bool isConnected_;

    API::Context::SharedPtr pCCFContext_;
    API::InvitationBuilder::SharedPtr pInvitationBuilder_;

    UUID_t            sessionId_;
    UUID_t            publicAvatarId_;
    UUID_t            privateAvatarId_;
    UUID_t            userId_;
    std::string       issuer_;
    std::string       sessionName_;
    std::string       statusText_;
    std::string       securityCode_;
    std::set <UUID_t> applicationSet_;

    std::set<boost::shared_ptr<LiteConnection>> connectionSet_;


  };

}


#endif /* LITESESSION_HPP_ */
