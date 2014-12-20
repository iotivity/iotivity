//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#if !defined(_CM_H)
#define _CM_H

#include "map"

#include "Configuration.h"
#include "DeviceType.h"
#include "IControlManagerListener.h"
#include "CMLogType.h"

#include "CMDeviceFinder.h"
#include "CMClientResource.h"
#include "CMSerialization.h"
#include "Notification/INotificationListener.h"
#include "../../core/src/subscriptionstore/include/ISubscriptionDB.h"

#include "CMStates.h"
#include "CMModes.h"

namespace OC
{
    namespace Cm
    {

        /**
        *  @addtogroup ControlManager
        *  @{
        */
        /**
         * @class       ControlManager
         * @brief       This class acts as interface to Control Manager framework
         *              This class provides methods to instantiate interfaces using which other Control Manager functionalities can be accessed
         *
         * @since       1.0
         */

        class CMAPI ControlManager
        {

            public:

                /**
                 * Default destructor of this class
                 */
                ~ControlManager();


                static ControlManager *getInstance();

                /**
                 * This API returns an instance of Control Manager which belongs to particular context
                 *
                 * @param[in] pContext  Represents the instance of CMContext on which Control Manager is running.
                 *
                 * @return instance of Control Manager class
                 */
                static ControlManager *getInstance(OC::Cm::IContext *pContext);

                /**
                 * This is the API to start Control Manager on a particular device. Before starting Control Manager one needs to make sure that it is being started
                 * in the right context, valid configuration like Database path etc. is set and the device on which Control Manager needs to start
                 * is configured with the correct details like UUID, address etc. This is non-blocking API, so whenever framework started successfully
                 * application can get onStarted() callback from the framework. Hence application has to wait for onStarted() callback.
                 *
                 * Framework will accept this API only when Control Manager State is CM_STOPPED / CM_FAILED
                 *
                 * @param[in] myDevice  Represents device object which contains the details of current device like UUID, address etc
                 *
                 * @return  returns @c true in case Control Manager start initiated successfully (it doesn'r mean Control Manager started completely) @n
                 *                  @c false in case of any error while initiate Control Manager start.
                 */
                bool start(OC::Cm::Device &myDevice);

                /**
                 * This is the API to stop Control Manager on a particular device. Stopping Control Manager will stop Device Finder, Subscription Manager, Control Manager Engine
                 * and Remote Access Channel if supports before stopping Control Manager.
                 * If application want to stop only internal server then application can use this API by passing true as argument,
                 * so that framework will not stop completely, framework just restart the internal server and clienets.
                 * Internal Stop is used when we just need to stop the server and not Control Manager entirely.
                 * This API is used in certain scenarios like easy setup process, registration process etc.
                 *
                 * This is blocking API. So application can get true once Control Manager stopped successfully.
                 * Framework will accept this API only when Control Manager State is CM_STARTED / CM_FAILED
                 *
                 * @param[in] isInternalStop    Represents internal STOP or not. Default is false.
                 *
                 * @return  Returns @c true in case Control Manager stopped successfully @n
                 *                  @c false in case of any error
                 */
                bool stop(bool isInternalStop = false);

                /**
                 * This API checks the Control Manager state and determines whether Control Manager is started or not
                 *
                 * @return  @c true in case of Control Manager state is CM_STARTED @n
                 *          @c false in case of Control Manager state is other than CM_STARTED
                 */
                bool isStarted();

                /**
                 * This API returns the current Control Manager State.
                 * Control Manager framework maintain few states in the lifetime. i.e., CM_STARTING, CM_STARTED, CM_STOPPING, CM_STOPPED, CM_FAILED etc.
                 * When Control Manager is either starting or stopping, Control Manager framework will be having one of this state.
                 *
                 * @return  Returns the current state of Control Manager in a particular device
                 */
                OC::Cm::CMStates getState();

                /**
                 * This API will configure Control Manager on a device. Control Manager should be configured before it is started
                 *
                 * @param[in] config    Represents the object which holds configuration details of Control Manager
                 *
                 * @return  @c true in case Control Manager is configured correctly  @n
                 *          @c false in case of any error
                 */
                bool setConfiguration(OC::Cm::Configuration *config);

                /**
                 * This API will register a listener with Control Manager to receive framework related events as defined by IControlManagerListener interface
                 *
                 * @param[in] listener  Represents listener object which is registered to listen to framework related events
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                bool setListener(OC::Cm::IControlManagerListener &listener);

                /**
                 * This API will remove previously registered listener
                 *
                 * @param[in] listener  Represents listener object which will be removed from listening to framework related events
                 */
                void removeListener(OC::Cm::IControlManagerListener &listener);

                /**
                 * This API returns an interface which can be used for device finder functionality.
                 *
                 * @return  Returns instance of IDeviceFinder interface.
                 */
                OC::Cm::DeviceFinder::IDeviceFinder *getDeviceFinder();

                /**
                 * This API returns current configuration of Control Manager
                 *
                 * @return  Returns current configuration of Control Manager
                 */
                OC::Cm::Configuration *getConfiguration();

                /**
                 *  Adds a Notification listener. Application can receive notification through this listener
                 *
                 *  @param[in]  notificationListener    instance of notification listener
                 *
                 * @return Return @c true in case of success @n@c false in case of any failure
                 */
                bool addNotificationListener ( OC::Cm::Notification::INotificationListener &notificationListener );

                /**
                 *  removes previously added notification listener
                 *
                 *  @param[in]  notificationListener    instance of notification listener
                 */
                void removeNotificationListener ( OC::Cm::Notification::INotificationListener
                                                  &notificationListener );


                /**
                 * This API returns context of the current framework instance. Control Manager Framework stores the information
                 * related to particular instance of framework in IContext.
                 *
                 * @return  Returns instance of IContext
                 */
                IContext *getContext() ;

                /**
                 * Function to set Control Manager-Log Level at run time. Logs are of four types : Error, Warning, Info and Debug
                 * defined by CMLogType enumeration. It is up to the developer to set the log level according to his implementation
                 *
                 * @param[in] logType   CMLogType enumeration value
                 *
                 * @return  @c true in case of success @n
                 *          @c false in case of any error
                 */
                bool setLogLevel(OC::Cm::CMLogType logType);


            private:

                static ControlManager *s_cmInstance ;

                OC::Cm::Configuration  *m_pConfiguration
                ;                  /**< Represents instance of Control Manager Configuration */

                OC::Cm::IContext           *p_context
                ;                         /**< Represents instance of Control Manager Context */

                void
                *p_shpMutex;                         /**< Represents instance of Mutex which can be used for some critical sections with in Control Manager */

                OC::Cm::CMModes
                m_cmMode;                           /**< Represents state of Control Manager framework */

                OC::Cm::Notification::ISubscriptionDB *m_pSubDBStore;

                OC::Cm::DeviceFinder::DeviceFactory        *m_pControlDeviceFactory;

                OC::Cm::Serialization::SerializableDataFactory      *m_pCMSerializationFactory;

                OC::Cm::Server::ResourceHandler::ResourceHandlerFactory    *m_pCMResourceHandlerFactory;


                /**
                 * Default constructor of this class
                 */
                ControlManager();

                /**
                 * This API validates the Control Manager Configuration
                 *
                 * @return  @c true if configuration is valid @n
                 *          @c false in case of any error
                 */
                bool validateConfiguration(OC::Cm::Configuration *pConfig);

                /**
                 * This API validates the given device details
                 *
                 * @return  @c true if current device is valid @n
                 *          @c false in case of any error
                 */
                bool validateMyDevice( OC::Cm::Device *pDevice );
        };
        /** @} */
    }
}

#endif  //_CM_H
