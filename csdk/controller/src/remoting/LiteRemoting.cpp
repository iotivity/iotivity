//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

// ============================================================================
// Includes
// ============================================================================
#include "LiteRemoting.h"

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <stdexcept>

#ifdef DEBUG
#include <android/log.h>
#endif

// ============================================================================
// Namespace
// ============================================================================
namespace remoting {

  // ============================================================================
  // Macros
  // ============================================================================
  // TODO:  Need application ID for Remoting
  const std::string LiteRemoting::APPLICATION_UUID_STRING = "7A1B397B-B576-44C4-943F-000000000001";

// ============================================================================
// Macros
// ============================================================================
#ifdef DEBUG
  #define LOG_PRINT(X)       __android_log_print(ANDROID_LOG_VERBOSE, "LiteRemoting", (X));
#else
  #define LOG_PRINT(X)
#endif

// ============================================================================
// Constructor/Destructor/getInstance
// ============================================================================
  //---------------------------------------------------------------------------
  // Constructor
  // Called by getInstance method (Singleton patter)
  //---------------------------------------------------------------------------
	LiteRemoting::LiteRemoting(PrivateConstructKey key) {

	  apiResultReady_ = false;

//  public static final String  applicationId = "7A1B397B-B576-44C4-943F-000000000001";
//  public static final UUID    applicationUUID = UUID.fromString(applicationId);

	  // Begin CCF API interaction
	  pCCFService_ = API::Service::createService();
	  if (pCCFService_)
	  {
      pCCFService_->registerApplication();

      UUID_t applicationId;
      StringToUuid(APPLICATION_UUID_STRING.c_str(), &applicationId);

      pCCFContext_ = pCCFService_->createContext(applicationId);

      if (pCCFContext_)
      {
        doStartup();
      }
	  }
	}

  //---------------------------------------------------------------------------
  // Destructor
  //---------------------------------------------------------------------------
	LiteRemoting::~LiteRemoting() {
	  // Empty containers
	  sessionMap_.clear();
	  targetDeviceProxyMap_.clear();

	  // Turn off observer callbacks
	  pCCFContext_->setSessionObserver();
	  pCCFContext_->setUserObserver();

	  // CCF APP interaction
	  if (pCCFService_)
	  {
	    pCCFService_->deregisterApplication();
	  }
	}

  //---------------------------------------------------------------------------
  // getInstance (Singleton pattern
  //---------------------------------------------------------------------------
	// Get singleton instance of LiteRemoting
	boost::shared_ptr<LiteRemoting> LiteRemoting::getInstance() {
	  LOG_PRINT("LiteRemoting::getInstance()");

		static boost::shared_ptr<LiteRemoting> instance = boost::make_shared<LiteRemoting>(PrivateConstructKey());

		return instance;
	}

// ============================================================================
// Methods
// ============================================================================
	void LiteRemoting::doStartup() {

	  // Get the local session id
	  boost::unique_lock<boost::mutex> lock(mutex_);
	  queryMySessionId();
	  while (!apiResultReady_)
	  {
	    // TODO: maybe do cond_.timed_wait() ???
	    cond_.wait(lock);
	  }

	  // Get local session info
	  lock.lock();
	  queryMySessionInfo();
    while (!apiResultReady_)
    {
      // TODO: maybe do cond_.timed_wait() ???
      cond_.wait(lock);
    }

    // Now that we have local session info, set the session observer
    pCCFContext_->setSessionObserver(boost::bind(&LiteRemoting::sessionObserver, this, _1));

    // TODO:  set up connection observer

    // TODO: set up device observer

	}

	void LiteRemoting::addSessionToMap(UUID_t sessionId) {
    // Add session to the sessionMap
    auto iter = sessionMap_.find(sessionId);
    if (iter == sessionMap_.end() )
    {
      // sessionUUID not found, so add session
      sessionMap_[sessionId] = boost::make_shared<LiteSessionImpl>(pCCFContext_, sessionId);
    }
	}

	boost::shared_ptr<LiteSessionImpl> LiteRemoting::getSessionFromMap(UUID_t sessionId) {
	  boost::shared_ptr<LiteSessionImpl> pSession;
	  try {
	    pSession = sessionMap_.at(sessionId);
	  }
	  catch (const std::out_of_range& oor) {
      // sessionId not found, so add session
      sessionMap_[sessionId] = boost::make_shared<LiteSessionImpl>(pCCFContext_, sessionId);
	  }

	  return sessionMap_[sessionId];
	}

	void LiteRemoting::addTargetDeviceToMap(UUID_t targetDeviceId) {
    // A LiteTargetDeviceProxy with this targetDeviceId was not found, so add
    // Add target device to the targetDeviceProxyMap_
    auto iter = targetDeviceProxyMap_.find(targetDeviceId);
    if (iter == targetDeviceProxyMap_.end() )
    {
      // targetDeviceId not found, so add session
      targetDeviceProxyMap_[targetDeviceId] = boost::make_shared<LiteTargetDeviceProxy>(targetDeviceId);
    }

	}

	boost::shared_ptr<LiteTargetDeviceProxy> LiteRemoting::getTargetDeviceFromMap(UUID_t targetDeviceId) {
    boost::shared_ptr<LiteTargetDeviceProxy> pTargetDevice;
    try {
      pTargetDevice = targetDeviceProxyMap_.at(targetDeviceId);
    }
    catch (const std::out_of_range& oor) {
      // sessionId not found, so add session
      targetDeviceProxyMap_[targetDeviceId] = boost::make_shared<LiteTargetDeviceProxy>(targetDeviceId);
    }

    return targetDeviceProxyMap_[targetDeviceId];

	}

	void LiteRemoting::handleNewTargetDevice(UUID_t targetDeviceId) {
	  addTargetDeviceToMap(targetDeviceId);
	  //session.sendDeviceArrival(device);
	}

	// TODO: socket parameter is faked with an int for now until I know how the connection info will be conveyed
	void LiteRemoting::handleNewConnection(UUID_t sessionId, int socket) {
	  // Validate socket
	  if (socket == 0) {
	    return;
	  }

	  // Get the specified session and a new connection
	  boost::shared_ptr<LiteSessionImpl> pSession = getSessionFromMap(sessionId);
	  boost::shared_ptr<LiteConnection> pConnection = boost::make_shared<LiteConnection>(socket);
	  // TODO: add device info to connection

	  //    List<LiteDeviceProxy> deviceProxyList = new ArrayList<LiteDeviceProxy>();
	  //    synchronized (deviceProxySet) {
	  //      for(LiteDeviceProxy device : deviceProxySet) {
	  //        if (device.isLocal()) {
	  //          deviceProxyList.add(device);
	  //        }
	  //      }
	  //    }
	  //    for(LiteDeviceProxy device : deviceProxyList) {
	  //      try {
	  //        connection.sendDeviceArrival(device);
	  //      } catch (IOException e) {
	  //        e.printStackTrace();
	  //      }
	  //    }

	  // Add the new connection to the session
	  pSession->addConnection(pConnection);
	}

	void LiteRemoting::queryMySessionId() {
	  apiResultReady_ = false;

    pCCFContext_->querySessionId([this](const API::SessionIdResult& sessionIdResult) -> void {

      API::SessionIdResult::ResultType queryResult = sessionIdResult.getResult();
      boost::lock_guard<boost::mutex> lock(mutex_);
      switch (queryResult) {
        case API::SessionIdResult::ResultType::Success:
          LOG_PRINT("LiteRemoting::queryMySessionId() - queryResult == Success");
          mySessionId_ = sessionIdResult.getSessionId();
          apiResultReady_ = true;
          break;
        case API::SessionIdResult::ResultType::InternalError:
          LOG_PRINT("LiteRemoting::queryMySessionId() - queryResult == InternalError");
          break;
        case API::SessionIdResult::ResultType::InvalidLocalSession:
          LOG_PRINT("LiteRemoting::queryMySessionId() - queryResult == InvalidLocalSession");
          break;
        default:
          LOG_PRINT("LiteRemoting::queryMySessionId() - queryResult == unknown");
          break;
      }
      cond_.notify_one();
    });
	}

	void LiteRemoting::queryMySessionInfo()
	{
    apiResultReady_ = false;

//    pCCFContext_->querySession(sessionUUID, std::bind(&LiteRemoting::querySessionObserver, this, _1));
    pCCFContext_->querySession(mySessionId_, [this](const API::SessionQueryResult& sessionQueryResult) -> void {

      API::QueryResultType queryResult = sessionQueryResult.getResult();
      boost::lock_guard<boost::mutex> lock(mutex_);
      switch (queryResult) {
        case API::QueryResultType::Success:
          LOG_PRINT("LiteRemoting::queryMySessionInfo() - queryResult == Success");
          // TODO:  verify that CCF application is in the session application set?

          // Add my local session to the sessionMap
          addSessionToMap(mySessionId_);
          sessionMap_[mySessionId_]->setPublicAvatarId(sessionQueryResult.getPublicAvatarId());
          sessionMap_[mySessionId_]->setPrivateAvatarId(sessionQueryResult.getPrivateAvatarId());
          sessionMap_[mySessionId_]->setPublicAvatarId(sessionQueryResult.getPublicAvatarId());
          sessionMap_[mySessionId_]->setUserId(sessionQueryResult.getUserId());
          sessionMap_[mySessionId_]->setIssuer(sessionQueryResult.getIssuer());
          sessionMap_[mySessionId_]->setName(sessionQueryResult.getName());
          sessionMap_[mySessionId_]->setStatusText(sessionQueryResult.getStatusText());
          sessionMap_[mySessionId_]->setSecurityCode(sessionQueryResult.getSecurityCode());
          sessionMap_[mySessionId_]->setApplicationSet(sessionQueryResult.getApplicationSet());

          // TODO:  add more conditions before inviting to join session?
          if (!sessionMap_[mySessionId_]->isConnected() && !sessionMap_[mySessionId_]->isInvited()) {
            sessionMap_[mySessionId_]->invite();
          }

          apiResultReady_ = true;
          break;
        case API::QueryResultType::NotFound:
          LOG_PRINT("LiteRemoting::queryMySessionInfo() - queryResult == NotFound");
          break;
        default:
          LOG_PRINT("LiteRemoting::queryMySessionInfo() - queryResult == unknown");
          break;
      }
      cond_.notify_one();
    });
	}

// ============================================================================
// Observers
// ============================================================================
	void LiteRemoting::sessionObserver(const API::SessionEvent& sessionEvent)
	{

    UUID_t sessionId = sessionEvent.getSessionId();

    // TODO: Need to put in logic for seeing what's changed.  Added, dropped, paused?
//    int changeMask = sessionEvent.getWhatChanged();

    // See if we already have the session in the map.  If not, create and add
    addSessionToMap(sessionId);
	}

}
