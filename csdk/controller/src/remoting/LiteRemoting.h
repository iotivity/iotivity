//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LITEREMOTING_H_
#define LITEREMOTING_H_

// ============================================================================
// Includes
// ============================================================================
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <uuid.h>
#include <map>
#include <set>
#include "api.h"
using namespace Intel::STC;

#include "LiteSessionImpl.h"
#include "LiteUUIDLess.hpp"
#include "LiteTargetDevice.hpp"
#include "LiteTargetDeviceProxy.h"

// ============================================================================
// Namespace
// ============================================================================
namespace remoting {

  // ==========================================================================
  // Class
  // ==========================================================================
  class LiteRemoting {
    // ============================================================
    // Constants
    // ============================================================
    public:
      static const std::string APPLICATION_UUID_STRING;

    // ============================================================
    // Type Definition
    // ============================================================
    public:
      typedef boost::shared_ptr<LiteRemoting> Ptr;

    // ============================================================
    // Singleton
    // ============================================================
    public:
      static boost::shared_ptr<LiteRemoting> getInstance();

    // ============================================================
    // Private key for Singleton with shared_ptr
    // LiteRemoting ctor must be public for make_ptr.
	  // But since we are a singleton, we don't want to allow clients
    // to call the ctor.  Since client can't access
    // PrivateConstructKey, it can't call the public ctor.
    // ============================================================
    private:
      struct PrivateConstructKey {};

    // ============================================================
    // Constructor and Destructor
    // ============================================================
    public:
      LiteRemoting(PrivateConstructKey key);
      virtual ~LiteRemoting();

    // ============================================================
    // Public Method(s)
    // ============================================================
    public:


    // ============================================================
    // Private copy constructor for Singleton
    // ============================================================
    private:
      LiteRemoting(const LiteRemoting&);

    // ============================================================
    // Private Attributes
    // ============================================================
    private:
      // CCF API Interface
      API::Service::SharedPtr pCCFService_;
      API::Context::SharedPtr pCCFContext_;

      UUID_t mySessionId_;
      std::map <UUID_t, boost::shared_ptr<LiteSessionImpl>, LiteUUIDLess> sessionMap_;
      std::map <UUID_t, boost::shared_ptr<LiteTargetDeviceProxy>, LiteUUIDLess> targetDeviceProxyMap_;
      boost::condition_variable cond_;
      boost::mutex mutex_;
      bool apiResultReady_;

    // ============================================================
    // Private Methods
    // ============================================================
    private:
      void doStartup();
      void queryMySessionId();
      void queryMySessionInfo();
      void addSessionToMap(UUID_t sessionId);
      boost::shared_ptr<LiteSessionImpl> getSessionFromMap(UUID_t sessionId);
      void addTargetDeviceToMap(UUID_t targetDeviceId);
      boost::shared_ptr<LiteTargetDeviceProxy> getTargetDeviceFromMap(UUID_t targetDeviceId);
      void handleNewConnection(UUID_t sessionId, int socket);
      void handleNewTargetDevice(UUID_t targetDeviceId);

    // ============================================================
    // Observers
    // ============================================================
    private:
      void sessionObserver(const API::SessionEvent& sessionEvent);

	};
}
#endif /* LITEREMOTING_H_ */
