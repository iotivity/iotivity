//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LITETARGETDEVICE_HPP_
#define LITETARGETDEVICE_HPP_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <uuid.h>


// ============================================================================
// Namespace
// ============================================================================
namespace remoting {

  // ==========================================================================
  // Class
  // ==========================================================================
  class LiteTargetDevice
  {
  public:
    LiteTargetDevice(UUID_t uuid) : deviceId_(uuid), name_("Unknown target device"), isLocal_(false) {};
    virtual ~LiteTargetDevice() {};

    const UUID_t& getDeviceUUID() {
      return deviceId_;
    }

    const std::string& getName() {
      return name_;
    }

    bool isLocal() {
      return isLocal_;
    }

    void setName(const std::string& name) {
      name_ = name;
    }

    void setLocal(bool isLocal) {
      isLocal_ = isLocal;
    }

  protected:
    UUID_t deviceId_;
    std::string name_;
    bool isLocal_;
  };

}

#endif /* LITETARGETDEVICE_HPP_ */
