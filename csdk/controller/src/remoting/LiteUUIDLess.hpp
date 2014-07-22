//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LITEUUIDLESS_HPP_
#define LITEUUIDLESS_HPP_

#include <uuid.h>

namespace remoting {

  class LiteUUIDLess
  {
  public:
     bool operator()(const UUID_t &uuid1, const UUID_t &uuid2) const
     {
        return memcmp(&uuid1, &uuid2, sizeof(uuid1))<0;
     }
  };

}


#endif /* LITEUUIDLESS_HPP_ */
