//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <string>
#include "LiteSessionImpl.h"

#ifdef DEBUG
#include <android/log.h>
#endif

namespace remoting {

  LiteSessionImpl::LiteSessionImpl(API::Context::SharedPtr pCCFContext, UUID_t uuid) : LiteSession(pCCFContext, uuid)
  {
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_VERBOSE, tag, "LiteSessionImpl::LiteSessionImpl()");
#endif

    if (pCCFContext_)
    {
      // Set callbacks
      using namespace std::placeholders;

      pCCFContext_->querySession(sessionId_, std::bind(&LiteSessionImpl::querySessionObserver, this, _1));
    }
  }

  LiteSessionImpl::~LiteSessionImpl()
  {
    applicationSet_.clear();
  }


  void LiteSessionImpl::invite() {
    if (!isConnected_ && !isInvited_) {
      if (pCCFContext_) {
        pInvitationBuilder_ = pCCFContext_->createInvitationBuilder();
        isInvited_ = true;
      }
    }
  }

  void LiteSessionImpl::disconnect() {
    // TODO:  implement
  }

  void LiteSessionImpl::querySessionObserver(const API::SessionQueryResult& sessionQueryResult)
  {

  }
}

