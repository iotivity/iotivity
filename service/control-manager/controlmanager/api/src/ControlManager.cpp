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



#include "ControlManager.h"
#include "CMContext.h"
#include "CMCommon.h"

#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "CMNotification.h"
#include "SubscriptionManager.h"
#include "SqliteSubscriptionDatabase.h"

#include "ControlDeviceFactory.h"
#include "Serialization/CMSerializationFactory.h"
#include "Server/CMResourceHandlerFactory.h"


#define MODULE_ID   CM_API

namespace OC
{
    namespace Cm
    {

        ControlManager *OC::Cm::ControlManager::s_cmInstance ;

        ControlManager *
        ControlManager::getInstance()
        {
            if (NULL == s_cmInstance)
            {
                s_cmInstance = new ControlManager();
            }

            return s_cmInstance ;
        }

        ControlManager *
        ControlManager::getInstance(OC::Cm::IContext *pContext)
        {
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)pContext;
            TryReturn(pCMContext != NULL, NULL, "Failed to get Control Manager Context Class Instance");
            return pCMContext->p_shp ;
        }

        ControlManager::ControlManager()
        {
            m_pConfiguration = new Configuration();
            m_pControlDeviceFactory = new ControlDeviceFactory();
            m_pCMSerializationFactory = new CMSerializationFactory();
            m_pCMResourceHandlerFactory = new CMResourceHandlerFactory();


            p_context = new OC::Cm::Core::CMContext(this) ;
            TryReturnVoid(p_context != NULL, "Failed to get CM Context Class Instance");

            /* Configure Subscription Manager and Subscription database */
            m_pSubDBStore = new OC::Cm::Notification::SqliteSubscriptionDatabase();
            TryReturnVoid(m_pSubDBStore != NULL,
                          "Failed to create Notification::SqliteSubscriptionDatabase Class Instance");

            OC::Cm::Notification::SubscriptionManager *pSub = new OC::Cm::Notification::SubscriptionManager(
                m_pSubDBStore);
            TryReturnVoid(pSub != NULL, "Failed to create Notification::SubscriptionManager Class Instance");

            ((OC::Cm::Core::CMContext *)p_context)->p_subscriptionManager = pSub;

            /* Configure Factories */
            m_pConfiguration->setDeviceFactory(m_pControlDeviceFactory);
            m_pConfiguration->setSerializableDataFactory(m_pCMSerializationFactory);
            m_pConfiguration->setResourceHandlerFactory(m_pCMResourceHandlerFactory);

            p_shpMutex = new OC::Cm::Platform::Thread::Mutex();

            m_cmMode = NORMAL_MODE;
        }

        ControlManager::~ControlManager()
        {
            CM_LOG_DEBUG_A(CM_API, "%s", "Called");
            this->stop();

            if (NULL != m_pControlDeviceFactory)
            {
                delete m_pControlDeviceFactory ;
                m_pControlDeviceFactory = NULL ;
            }
            if (NULL != m_pCMSerializationFactory)
            {
                delete m_pCMSerializationFactory ;
                m_pCMSerializationFactory = NULL ;
            }
            if (NULL != m_pCMResourceHandlerFactory)
            {
                delete m_pCMResourceHandlerFactory ;
                m_pCMResourceHandlerFactory = NULL ;
            }
            if (NULL != m_pConfiguration)
            {
                delete m_pConfiguration ;
                m_pConfiguration = NULL ;
            }

            if (NULL != p_context)
            {
                OC::Cm::Notification::ISubscriptionManager *subMgr = p_context->getSubscriptionManager();
                if (subMgr != NULL)
                {
                    delete subMgr;
                }

                delete p_context ;
                p_context = NULL ;
            }

            if (m_pSubDBStore != NULL)
            {
                delete m_pSubDBStore;
                m_pSubDBStore = NULL;
            }



            /*
             * if somebody holds shpMutex, then Control Manager destructor will wait for that mutex to be unlocked and then destroy.
             */
            OC::Cm::Platform::Thread::Mutex *shpMutex = (OC::Cm::Platform::Thread::Mutex *)p_shpMutex;
            if (shpMutex != NULL)
            {
                shpMutex->lock();
                shpMutex->unlock();
                delete shpMutex; shpMutex = NULL;
            }
        }

        bool
        ControlManager::start(OC::Cm::Device &myDevice)
        {
            OC::Cm::Platform::Thread::Mutex *shpMutex = (OC::Cm::Platform::Thread::Mutex *)p_shpMutex;
            shpMutex->lock();
            OC::Cm::CMStates state = getState();
            CM_LOG_DEBUG_A(CM_API, "Control Manager State [%d]", state);
            bool bRet = false ;

            if (state == CM_STARTING)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Control Manager is in starting process already, Hence returning") ;
                shpMutex->unlock();
                return false;
            }
            else if (state == CM_STARTED)
            {
                CM_LOG_DEBUG_A(CM_API, "%s", "Control Manager is already started, Hence returning") ;
                shpMutex->unlock();
                return true;
            }
            else if (state == CM_STOPPING)
            {
                CM_LOG_ERROR_A(CM_API, "%s",
                               "Control Manager is not stopped completely to start again, Hence returning") ;
                shpMutex->unlock();
                return false;
            }
            else if (state == CM_FAILED)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Control Manager is failed to start earlier, Hence stop first") ;
                shpMutex->unlock();
                return false;
            }

            if (state == CM_INTERNAL_STOPPED)
            {
                CM_LOG_ERROR_A(CM_API, "%s",
                               "Control Manager is internally stopped. Internal stop should always followed by setControl ManagerMode() only")
                ;
                shpMutex->unlock();
                return false;
            }

            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
            TryReturnEx(pCMContext != NULL, false, shpMutex->unlock(),
                        "Failed to get Control Manager Context Class Instance");

            CM_LOG_DEBUG_A(CM_API, "%s", "Control Manager Framework Going To Start Now");
            pCMContext->m_engine.setCMEngineState(CM_STARTING);

            bRet = validateConfiguration(m_pConfiguration) ;
            if (bRet != true)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Invalid Configuration") ;
                pCMContext->m_engine.setCMEngineState(CM_FAILED);
                shpMutex->unlock();
                return false;
            }

            bRet = validateMyDevice(&myDevice) ;
            if (bRet != true)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Invalid Device") ;
                pCMContext->m_engine.setCMEngineState(CM_FAILED);
                shpMutex->unlock();
                return false;
            }

            CM_LOG_INFO_A(CM_API, "Self Device Address:[%s]", myDevice.getAddress().c_str());

            if (pCMContext->p_subscriptionManager == NULL)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Subscription Manager is not configured") ;
                pCMContext->m_engine.setCMEngineState(CM_FAILED);
                shpMutex->unlock();
                return false;
            }

            // Initialize Subscription Manager
            std::string dbPath = m_pConfiguration->getSubscriptionDbPath();
            bRet = pCMContext->p_subscriptionManager->init(p_context, dbPath );
            if (bRet != true)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Failed to initialize Subscription Manager") ;
                pCMContext->m_engine.setCMEngineState(CM_FAILED);
                shpMutex->unlock();
                return false;
            }

            // Start Control Manager Engine
            std::string address = myDevice.getAddress();
            pCMContext->m_engine.setDeviceAddress(address) ;
            pCMContext->m_engine.setDeviceType(myDevice.getDeviceType());
            pCMContext->m_engine.setUUID(myDevice.getUUID());
            bRet = pCMContext->m_engine.init(m_pConfiguration);
            if (bRet != true)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Failed to initialize Control Manager Engine") ;
                pCMContext->m_engine.setCMEngineState(CM_FAILED);
                shpMutex->unlock();
                return false;
            }

            bRet = pCMContext->m_engine.start();
            if (bRet != true)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Failed to start Control Manager Engine") ;
                pCMContext->m_engine.setCMEngineState(CM_FAILED);
                shpMutex->unlock();
                return false;
            }

            shpMutex->unlock();
            return true ;
        }

        bool
        ControlManager::stop(bool isInternalStop)
        {
            OC::Cm::Platform::Thread::Mutex *shpMutex = (OC::Cm::Platform::Thread::Mutex *)p_shpMutex;
            shpMutex->lock();
            OC::Cm::CMStates state = getState();
            CM_LOG_DEBUG_A(CM_API, "Control Manager State [%d] / InternalStop[%s]", state,
                           isInternalStop ? string("true").c_str() : string("false").c_str());

            if (state == CM_UNKNOWN)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Control Manager has't started, Hence returning") ;
                shpMutex->unlock();
                return false;
            }
            else if (state == CM_STOPPING)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "Control Manager is in stopping process already, Hence returning") ;
                shpMutex->unlock();
                return false;
            }
            else if (state == CM_STOPPED)
            {
                CM_LOG_DEBUG_A(CM_API, "%s", "Control Manager is already stopped, Hence returning") ;
                shpMutex->unlock();
                return true;
            }
            else if (state == CM_STARTING)
            {
                CM_LOG_ERROR_A(CM_API, "%s",
                               "Control Manager is not started completely to stop again, Hence returning") ;
                shpMutex->unlock();
                return false;
            }

            if (isInternalStop)
            {
                if (state == CM_INTERNAL_STOPPED)
                {
                    CM_LOG_DEBUG_A(CM_API, "%s", "Control Manager is already stopped internally, Hence returning") ;
                    shpMutex->unlock();
                    return true;
                }
            }

            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
            TryReturnEx(pCMContext != NULL, false, shpMutex->unlock(),
                        "Failed to get Control Manager Context Class Instance");
            pCMContext->m_engine.setCMEngineState(CM_STOPPING);

            bool bRet = false ;

            CM_LOG_DEBUG_A(CM_API, "%s", "Control Manager Framework Going To Stop Now");

            // TODO: move DeviceFinder Stop and Deinit to ENGINE
            bRet = pCMContext->m_deviceFinderImpl.stop();
            TryLog( bRet == true, "Failed to Stop DeviceFinder" );

            bRet = pCMContext->m_deviceFinderImpl.deinit();
            TryLog( bRet == true, "Failed to deinit DeviceFinder" );

            if (pCMContext->p_subscriptionManager)
            {
                bRet = pCMContext->p_subscriptionManager->deinit();
                TryLog( bRet == true, "Failed to deinit Subscription Manager" );
            }

            bRet = pCMContext->m_engine.stop();
            TryLog( bRet == true, "Failed to Stop Control Manager Engine" );

            state = getState();
            CM_LOG_DEBUG_A(CM_API, "Control Manager State [%d]", state);

            if (bRet)
            {
                while (getState() != CM_STOPPED)
                {
                    CM_LOG_DEBUG_A(CM_API, "%s", "Waiting for Control Manager Engine to Stop");
                    OC::Cm::Platform::Thread::Thread::sleep(1000000);
                }
            }

            bRet = pCMContext->m_engine.deinit();
            TryReturnEx( bRet == true, false, shpMutex->unlock(),
                         "Failed to de-initialize Control Manager Engine" ) ;

            CM_LOG_DEBUG_A(CM_API, "%s", "Control Manager Framework Stopped Successfully");

            shpMutex->unlock();
            return true ;
        }

        bool
        ControlManager::isStarted()
        {
            OC::Cm::CMStates state = getState();
            CM_LOG_DEBUG_A(CM_API, "Control Manager State [%d]", state);
            if (state == CM_STARTED) return true;

            return false;
        }

        OC::Cm::CMStates
        ControlManager::getState()
        {
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
            if (pCMContext == NULL) { return CM_STOPPED; }
            return pCMContext->m_engine.getCMEngineState();
        }

        bool
        ControlManager::setConfiguration(OC::Cm::Configuration *config)
        {
            if (false == validateConfiguration(config))
            {
                return false ;
            }

            if ((m_pConfiguration != NULL) && (m_pConfiguration != config))
            {
                delete m_pConfiguration;
                m_pConfiguration = NULL ;
            }

            m_pConfiguration = config ;

            return true ;
        }

        bool
        ControlManager::setListener(OC::Cm::IControlManagerListener &listener)
        {
            if ((&listener) == NULL) { return false; }
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
            TryReturn(pCMContext != NULL, false, "Failed to get Control Manager Context Class Instance");
            pCMContext->m_engine.setControlManagerListener(listener) ;
            return true;
        }

        void
        ControlManager::removeListener(OC::Cm::IControlManagerListener &listener)
        {
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context ;
            TryReturnVoid(pCMContext != NULL, "Failed to get Control Manager Context Class Instance");
            pCMContext->m_engine.removeControlManagerListener(listener);
        }

        OC::Cm::DeviceFinder::IDeviceFinder *
        ControlManager::getDeviceFinder()
        {
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context ;
            TryReturn(pCMContext != NULL, NULL, "Failed to get Control Manager Context Class Instance");
            return &(pCMContext->m_deviceFinderImpl) ;
        }

        OC::Cm::Configuration *
        ControlManager::getConfiguration()
        {
            return m_pConfiguration ;
        }


        bool
        ControlManager::addNotificationListener ( OC::Cm::Notification::INotificationListener
                &notificationListener )
        {
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
            TryReturn(pCMContext != NULL, NULL, "Failed to get Control Manager Context Class Instance");
            return pCMContext->p_subscriptionManager->addNotificationListener(notificationListener);
        }

        void ControlManager::removeNotificationListener ( OC::Cm::Notification::INotificationListener
                &notificationListener )
        {
            OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
            TryReturnVoid(pCMContext != NULL, "Failed to get Control Manager Context Class Instance");
            pCMContext->p_subscriptionManager->removeNotificationListener(notificationListener) ;
        }

        bool
        ControlManager::validateConfiguration(OC::Cm::Configuration *config)
        {
            if (config == NULL)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "config is NULL");
                return false ;
            }

            if (config->getDeviceFactory() == NULL)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "config->getDeviceFactory() is NULL");
                return false ;
            }

            if (config->getResourceHandlerFactory() == NULL)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "config->getResourceHandlerFactory() is NULL");
                return false ;
            }

            config->getResourceHandlerFactory()->setContext(p_context) ;

            if (config->getSerializableDataFactory() == NULL)
            {
                CM_LOG_ERROR_A(CM_API, "%s", "config->getSerializableDataFactory() is NULL");
                return false ;
            }

            if (config->getSubscriptionDbPath().empty())
            {
                CM_LOG_ERROR_A(CM_API, "%s", "config->getSubscriptionDbPath() is empty");
                return false ;
            }

            return true;
        }

        bool
        ControlManager::validateMyDevice( OC::Cm::Device *pDevice )
        {
            if (pDevice == NULL)
            {
                return false ;
            }

            TryReturn(pDevice->getAddress().empty() == false, false, "Invalid Device Address") ;

            TryReturn(pDevice->getUUID().empty() == false, false, "Invalid UUID is passed from application") ;

            return true;
        }

        IContext *
        ControlManager::getContext()
        {
            return p_context ;
        }

        /**
        * @fn           : setCMLogLevel()
        * @brief        : Function to set Control Manager-Log Level at run time
        * @Assumptions  :
        * @input        : Control Manager-Log Type, allowed are -2, -1, 0, and 1
        * @output       : bool
        * @logic        :
        * @return       : true -> On SUCCESS, false -> On Failure
        * @since        : 16/05/2014
        *
        * @author       : Bharat Kumar P
        *
        * @version      : 1
        **/
        bool
        ControlManager::setLogLevel(OC::Cm::CMLogType logType)
        {
            if ((logType > CM_LOG_TYPE_DEBUG) || (logType < CM_LOG_TYPE_SILENT))
            {
                return false;
            }

            OC::Cm::Log::Log::setLogLevel((OC::Cm::Log::log_level)logType);
            return true;
        } // end of setCMLogLevel(

    }
}
