//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LITESESSIONIMPL_H_
#define LITESESSIONIMPL_H_

#include "LiteSession.hpp"
#include "api.h"
using namespace Intel::STC;

namespace remoting {

  class LiteSessionImpl : public LiteSession
  {
  public:
    LiteSessionImpl(API::Context::SharedPtr pCCFContext, UUID_t uuid);
    virtual ~LiteSessionImpl();

    virtual void invite();
    virtual void disconnect();

  private:
    // Listeners
    void querySessionObserver(const API::SessionQueryResult& sessionQueryResult);
  };
}


#endif /* LITESESSIONIMPL_H_ */

