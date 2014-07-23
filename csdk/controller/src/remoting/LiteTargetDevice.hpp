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
