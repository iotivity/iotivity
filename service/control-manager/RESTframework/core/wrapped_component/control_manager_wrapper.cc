/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "wrapped_component/control_manager_wrapper.h"

#if defined(OS_POSIX)
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#endif

#include "wrapped_component/response_listeners/control_manager_response_code.h"

#include "wrapped_component/response_listeners/access_point_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/access_points_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/alarm_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/alarms_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/configuration_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/detection_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/device_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/door_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/doors_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/energy_consumption_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/humidity_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/information_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/level_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/light_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/mode_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/movement_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/network_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/networks_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/operation_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/temperature_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/temperatures_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/thermostat_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/time_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/version_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/versions_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/wifi_rsc_resp_listener.h"

namespace webservice
{

// The controller device listens on the below port
    static const char *const kPort = "8098";
    static const char *const kUnknownRscname = "Unknown";
    const int kCMStartTimeout = 3000; //  in milliseconds (3 seconds)

    typedef Singleton<ControlManagerWrapper, LeakySingletonTraits<ControlManagerWrapper> >
    ControlManagerWrapperSingleton;
    ControlManagerWrapper *ControlManagerWrapper::control_manager_wrapper_instance_ = NULL;

#define DESTROY_RESP_LISTENER(type)             \
  type* type##_listener = type::GetInstance();  \
  if (type##_listener) {                        \
    type##_listener->DestroyInstance();         \
  }                                             \

    static void DeleteResourceInstance(ResourceType rsc_type, void *obj)
    {
        switch (rsc_type)
        {
            case kResourceTypeUnknown:
                {
                    LOG(ERROR) << "Invalid resource as input";
                    break;
                }
            case kResourceTypeAccessPoint:
                {
                    DESTROY_RESOURCE_OBJECT(AccessPoint, obj);
                    break;
                }
            case kResourceTypeAccessPoints:
                {
                    DESTROY_RESOURCE_OBJECT(AccessPoints, obj);
                    break;
                }
            case kResourceTypeAlarm:
                {
                    DESTROY_RESOURCE_OBJECT(Alarm, obj);
                    break;
                }
            case kResourceTypeAlarms:
                {
                    DESTROY_RESOURCE_OBJECT(Alarms, obj);
                    break;
                }
            case kResourceTypeConfiguration:
                {
                    DESTROY_RESOURCE_OBJECT(Configuration, obj);
                    break;
                }
            case kResourceTypeDetection:
                {
                    DESTROY_RESOURCE_OBJECT(Detection, obj);
                    break;
                }
            case kResourceTypeDevice:
                {
                    DESTROY_RESOURCE_OBJECT(Device, obj);
                    break;
                }
            case kResourceTypeDevices:
                {
                    DESTROY_RESOURCE_OBJECT(Devices, obj);
                    break;
                }
            case kResourceTypeDoor:
                {
                    DESTROY_RESOURCE_OBJECT(Door, obj);
                    break;
                }
            case kResourceTypeDoors:
                {
                    DESTROY_RESOURCE_OBJECT(Doors, obj);
                    break;
                }
            case kResourceTypeDRLC:
                {
                    DESTROY_RESOURCE_OBJECT(DRLC, obj);
                    break;
                }
            case kResourceTypeEnergyConsumption:
                {
                    DESTROY_RESOURCE_OBJECT(EnergyConsumption, obj);
                    break;
                }
            case kResourceTypeHumidity:
                {
                    DESTROY_RESOURCE_OBJECT(Humidity, obj);
                    break;
                }
            case kResourceTypeInformation:
                {
                    DESTROY_RESOURCE_OBJECT(Information, obj);
                    break;
                }
            case kResourceTypeLevel:
                {
                    DESTROY_RESOURCE_OBJECT(Level, obj);
                    break;
                }
            case kResourceTypeLight:
                {
                    DESTROY_RESOURCE_OBJECT(Light, obj);
                    break;
                }
            case kResourceTypeMessage:
                {
                    DESTROY_RESOURCE_OBJECT(Message, obj);
                    break;
                }
            case kResourceTypeMessages:
                {
                    DESTROY_RESOURCE_OBJECT(Messages, obj);
                    break;
                }
            case kResourceTypeMode:
                {
                    DESTROY_RESOURCE_OBJECT(Mode, obj);
                    break;
                }
            case kResourceTypeMovement:
                {
                    DESTROY_RESOURCE_OBJECT(Movement, obj);
                    break;
                }
            case kResourceTypeNetwork:
                {
                    DESTROY_RESOURCE_OBJECT(Network, obj);
                    break;
                }
            case kResourceTypeNetworks:
                {
                    DESTROY_RESOURCE_OBJECT(Networks, obj);
                    break;
                }
            case kResourceTypeOperation:
                {
                    DESTROY_RESOURCE_OBJECT(Operation, obj);
                    break;
                }
            case kResourceTypeRemote:
                {
                    DESTROY_RESOURCE_OBJECT(Remote, obj);
                    break;
                }
            case kResourceTypeTemperature:
                {
                    DESTROY_RESOURCE_OBJECT(Temperature, obj);
                    break;
                }
            case kResourceTypeTemperatures:
                {
                    DESTROY_RESOURCE_OBJECT(Temperatures, obj);
                    break;
                }
            case kResourceTypeThermostat:
                {
                    DESTROY_RESOURCE_OBJECT(Thermostat, obj);
                    break;
                }
            case kResourceTypeTime:
                {
                    DESTROY_RESOURCE_OBJECT(Time, obj);
                    break;
                }
            case kResourceTypeVersion:
                {
                    DESTROY_RESOURCE_OBJECT(Version, obj);
                    break;
                }
            case kResourceTypeVersions:
                {
                    DESTROY_RESOURCE_OBJECT(Versions, obj);
                    break;
                }
            case kResourceTypeWiFi:
                {
                    DESTROY_RESOURCE_OBJECT(WiFi, obj);
                    break;
                }
            default:
                {
                    LOG(ERROR) << "Invalid resource is given as input";
                    break;
                }
        };
    }
    CmCbData::CmCbData()
    {
        rsc_type = kResourceTypeUnknown;
        data = NULL;
        resp_status = RT_UNKNOWN;
    }

    CmCbData::~CmCbData()
    {
        if (data)
        {
            DeleteResourceInstance(rsc_type, data);
        }
        resp_status = RT_UNKNOWN;
    }

    void ControlManagerWrapper::tokenRequestNotification(std::string uuid)
    {
        LOG(ERROR) << "ControlManagerWrapper::tokenRequestNotification(): => " <<
                   "Sending setTokenGrantStatus() as TRUE...";
    }

    std::string ControlManagerWrapper::getMyDeviceToken(void)
    {
        LOG(ERROR) << "MySHPDevice::getMyDeviceToken(): => " <<
                   "Sending getMyDeviceToken() as SHP_DEVICE_TOKEN_123!";
        return "SHP_DEVICE_TOKEN_123";
    }

    void ControlManagerWrapper::onStarted(std::string address)
    {
        LOG(ERROR) << "ControlManagerWrapper::onStarted(): => " << "SHP Framework started with IP-Address "
                   << address;
        cm_start_condition.notify_all();
    }

    void ControlManagerWrapper::onStopped()
    {
        LOG(ERROR) << "ControlManagerWrapper::onStopped(): => " << "SHP Framework stopped successfully";
    }

    void ControlManagerWrapper::onError(OC::Cm::CMError error)
    {
        LOG(ERROR) << "ControlManagerWrapper::onError(): => " << "ERROR: Received Error from Framework";
    }

    void ControlManagerWrapper::updateUUIDAndTokenMap(std::string uuid, std::string deviceToken)
    {
        LOG(ERROR) << "ControlManagerWrapper::updateUUIDAndTokenMap(): => " <<
                   "Received request from Framework to update UUID and Token Map";
    }

    webservice::ControlManagerWrapper *ControlManagerWrapper::GetInstance(void)
    {
        DLOG(INFO) << " : Enter";
        if (!control_manager_wrapper_instance_)
        {
            control_manager_wrapper_instance_ = ControlManagerWrapperSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return control_manager_wrapper_instance_;
    }

    ControlManagerWrapper::ControlManagerWrapper()
    {
        my_device_ = NULL;
        control_manager_instance_ = NULL;
    }

    ControlManagerWrapper::~ControlManagerWrapper()
    {
        DLOG(INFO) << "~ControlManagerWrapper()";
        delete control_manager_instance_;
    }

    bool ControlManagerWrapper::Init()
    {
        DLOG(INFO) << "Init";
        return InitializeControlManager();
    }

    bool ControlManagerWrapper::DeInit()
    {
        LOG(ERROR) << "DeInit  CM state:: " << control_manager_instance_->getState();
        if (control_manager_instance_)
        {
            // TODO wait here till 'onStopped' callback comes from CM.
            // Currently CM doesn't provide any callback on stop.
            return control_manager_instance_->stop();
        }
        return false;
    }

    void ControlManagerWrapper::DestroyInstance(void)
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (control_manager_wrapper_instance_)
        {
            // delete all the resource response listeners
            control_manager_wrapper_instance_->DestroyRespListeners();
            delete control_manager_wrapper_instance_;
        }
    }

    OC::Cm::ControlManager *ControlManagerWrapper::GetControlManagerInstance()
    {
        return control_manager_instance_;
    }

    void ControlManagerWrapper::DestroyRespListeners(void)
    {
        DESTROY_RESP_LISTENER(AccessPointResourceResponseListener)
        DESTROY_RESP_LISTENER(AccessPointsResourceResponseListener)
        DESTROY_RESP_LISTENER(AlarmResourceResponseListener)
        DESTROY_RESP_LISTENER(AlarmsResourceResponseListener)
        DESTROY_RESP_LISTENER(ConfigurationResourceResponseListener)
        DESTROY_RESP_LISTENER(DetectionResourceResponseListener)
        DESTROY_RESP_LISTENER(DeviceResourceResponseListener)
        DESTROY_RESP_LISTENER(DoorResourceResponseListener)
        DESTROY_RESP_LISTENER(DoorsResourceResponseListener)
        DESTROY_RESP_LISTENER(EnergyConsumptionResourceResponseListener)
        DESTROY_RESP_LISTENER(HumidityResourceResponseListener)
        DESTROY_RESP_LISTENER(InformationResourceResponseListener)
        DESTROY_RESP_LISTENER(LevelResourceResponseListener)
        DESTROY_RESP_LISTENER(LightResourceResponseListener)
        DESTROY_RESP_LISTENER(ModeResourceResponseListener)
        DESTROY_RESP_LISTENER(MovementResourceResponseListener)
        DESTROY_RESP_LISTENER(NetworkResourceResponseListener)
        DESTROY_RESP_LISTENER(NetworksResourceResponseListener)
        DESTROY_RESP_LISTENER(OperationResourceResponseListener)
        DESTROY_RESP_LISTENER(TemperatureResourceResponseListener)
        DESTROY_RESP_LISTENER(TemperaturesResourceResponseListener)
        DESTROY_RESP_LISTENER(ThermostatResourceResponseListener)
        DESTROY_RESP_LISTENER(TimeResourceResponseListener)
        DESTROY_RESP_LISTENER(VersionResourceResponseListener)
        DESTROY_RESP_LISTENER(VersionsResourceResponseListener)
        DESTROY_RESP_LISTENER(WiFiResourceResponseListener)
    }

    bool ControlManagerWrapper::GetDeviceIP(std::string *ip_str)
    {
        ifaddrs *interfaces = NULL;
        if (getifaddrs(&interfaces) < 0 || !ip_str)
        {
            LOG(ERROR) << "getifaddrs failed or ip_str is null";
            return false;
        }
        *ip_str = "";

        // Enumerate the addresses assigned to network interfaces which are up.
        struct sockaddr_in *addr_in = NULL;
        struct sockaddr *addr = NULL;
        for (ifaddrs *nw_interface = interfaces; nw_interface != NULL;
             nw_interface = nw_interface->ifa_next)
        {
            // Skip loopback interfaces, and ones which are down.
            if (!(IFF_UP & nw_interface->ifa_flags))
                continue;
            if (IFF_LOOPBACK & nw_interface->ifa_flags)
                continue;
            // Skip interfaces with no address configured.
            addr = nw_interface->ifa_addr;
            if (!addr)
                continue;
            // Skip loopback addresses configured on non-loopback interfaces.
            if (addr->sa_family == AF_INET)
            {
                addr_in = reinterpret_cast<struct sockaddr_in *>(addr);
                if (addr_in->sin_addr.s_addr == INADDR_LOOPBACK)
                {
                    continue;
                }
                char buffer[16];
                int result = getnameinfo(addr, 16, buffer, sizeof(buffer), NULL, 0, NI_NUMERICHOST);
                *ip_str = std::string(buffer);
                if (result == 0)
                {
                    LOG(ERROR) << "IP Address: " << *ip_str;
                    break;
                }
                else
                {
                    LOG(ERROR) << "IP Address fetch failed";
                }
            }
            else
            {
                // Skip non-IP addresses.
                continue;
            }
        }
        freeifaddrs(interfaces);
        return true;
    }

    bool ControlManagerWrapper::InitializeControlManager()
    {
        LOG(ERROR) << "InitializeControlManager Enter";
        control_manager_instance_ = OC::Cm::ControlManager::getInstance();
        if (!control_manager_instance_)
        {
            LOG(ERROR) << "ControlManager GetInstance failed";
            return false;
        }
        control_manager_instance_->setLogLevel(OC::Cm::CMLogType::CM_LOG_TYPE_DEBUG);
        OC::Cm::Configuration *device_config_ = control_manager_instance_->getConfiguration();
        device_config_->setSubscriptionDbPath("SubDBPath"); // User needs to give actual DB Path
        control_manager_instance_->setConfiguration(device_config_);

        control_manager_instance_->setListener(*this);

        my_device_ = MyDevice::getInstance();
        if (!my_device_)
        {
            LOG(ERROR) << "getInstance of MyDevice failed";
            return false;
        }
        std::string ip = "";
        if (!GetDeviceIP(&ip))
        {
            LOG(ERROR) << "Get device ip failed";
            return false;
        }
        my_device_->setAddress(ip + ":" + kPort);
        // (TODO) Currently hard coding the device UUID and type
        my_device_->setUUID("E8113233-9A65-0000-0000-000000000000");
        my_device_->setDeviceType(OC::Cm::DeviceType_Unknown);

        if (false == control_manager_instance_->start(*my_device_))
        {
            LOG(ERROR) << "startFramework() => " << "ERROR: Failed to Start framework";
            return false;
        }
        // wait for 3 seconds, to get 'onStarted' callback from control manager
        boost::unique_lock<boost::mutex> lock(cm_start_lock);
        boost::system_time wait_time = boost::get_system_time() + boost::posix_time::milliseconds(
                                           kCMStartTimeout);
        if (!cm_start_condition.timed_wait(lock, wait_time))
        {
            // Timeout
            DLOG(ERROR) << "Timeout happened, Control manager didn't start as expected";
        }
        // check whether the CM is started properly or not
        if (control_manager_instance_->getState() != OC::Cm::CM_STARTED)
        {
            return false;
        }
        DLOG(INFO) << "CM started successfully";
        return true;
    }

    void ControlManagerWrapper::AddRequestToCBMap(int request_id, CmCbData *cb_data)
    {
        base::AutoLock auto_lock(lock_);
        cm_cb_data_map_.insert(std::pair<int, CmCbData *> (request_id, cb_data));
    }

    void ControlManagerWrapper::RemoveRequestFromCBMap(int request_id)
    {
        base::AutoLock auto_lock(lock_);
        CmCBDataMapIter it = cm_cb_data_map_.find(request_id);
        if (it != cm_cb_data_map_.end())
        {
            delete (*it).second;
            cm_cb_data_map_.erase(it);
        }
    }

    void ControlManagerWrapper::GetdataFromCBMap(int request_id, void **ptr,
            HTTPResponseType *resp_status)
    {
        base::AutoLock auto_lock(lock_);
        CmCBDataMapIter it = cm_cb_data_map_.find(request_id);
        if (it != cm_cb_data_map_.end())
        {
            DLOG(INFO) << "Response data found in map";
            *ptr = (*it).second->data;
            *resp_status = (*it).second->resp_status;
        }
    }

    void ControlManagerWrapper::DataReceivedForRequest(int request_id, void *ptr, int resp_status,
            const std::string &object_type)
    {
        base::AutoLock auto_lock(lock_);
        CmCBDataMapIter it = cm_cb_data_map_.find(request_id);
        if (it != cm_cb_data_map_.end())
        {
            (*it).second->data = ptr;
            (*it).second->resp_status = getResponseCode(resp_status);
            (*it).second->condition.notify_all();
        }
    }
}