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

/**
 * @file    SSMClient.h 
 * @brief   This file contains the declaration of classes and its members related to SSMClient.
 */

#ifndef RESOURCECLIENT_H_
#define RESOURCECLIENT_H_

#include <string>

#include "OCResource.h"
#include "OCPlatform.h"
#include "ISSMClientListener.h"

using namespace OC;

#if defined(WIN32)
class CSemaphore
{
private:
    HANDLE hSemaphore;
    int m_MaxTimeOut;

public:
    /**
     * This is CSemaphore constructor.
     */
    CSemaphore()
    {
        m_MaxTimeOut = 0;
    }
    /**
     *This is CSemaphore destructor.
     */
    ~CSemaphore()
    {
    }

    /**
     * This is a function to creat a semaphore
     * This is syncronus call.
     * @param maximumRequestTimeOut
     *          [IN] Request Time out.
     *
     * @return  None
     *   
     */
    void create(int maximumRequestTimeOut)
    {
        hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
        m_MaxTimeOut = maximumRequestTimeOut;
    }

    /**
     * This is a function to wait for semaphore
     *
     * @return  None
     *   
     */
    void wait()
    {
        WaitForSingleObject(hSemaphore, m_MaxTimeOut);
    }

    /**
     * This is a function to release  semaphore
     *
     * @return  None
     *   
     */
    void release()
    {
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }

};

#elif defined(LINUX)
#include <semaphore.h>

#define SEM_SUCCESS		0

class CSemaphore
{
private:
    sem_t hSemaphore;
    timespec m_MaxTimeOut;

public:
    /**
     * This is CSemaphore constructor.
     */
    CSemaphore()
    {
        m_MaxTimeOut.tv_sec = 0;
        m_MaxTimeOut.tv_nsec = 0;
    }
    /**
     *This is CSemaphore destructor.
     */
    ~CSemaphore()
    {
    }
    /**
     * This is a function to creat a semaphore
     * This is syncronus call.
     * @param maximumRequestTimeOut
     *          [IN] Request Time out.
     *
     * @return  None
     *   
     */
    void create(int maximumRequestTimeOut)
    {
        if ( sem_init(&hSemaphore, 1, 0) < SEM_SUCCESS )
        {
            perror("Error : sem_init.");
            exit(0);
        }
        m_MaxTimeOut.tv_sec = maximumRequestTimeOut;
    }

    void wait()
    {
        sem_wait( &hSemaphore );
    }

    /**
     * This is a function to release  semaphore
     *
     * @return  None
     *   
     */
    void release()
    {
        sem_post(&hSemaphore);
    }

};
#endif

/**
 *	@brief	SSMClient is a wrapper class to provide SoftSensorManager functionality to Application.
 *			Basically, SoftSensorManager is developed in Resource model (i.e. messaging with basic functions of put, get, and post).
 *			SSMClient abstracts the resource based operations from client applications and provides c++ style functions.
 */
class SSMClient
{
private:
    SSMReturn m_retResponse;
    OCPlatform *m_pPlatform;
    CSemaphore m_sem;
    /**
     * @brief SoftSensorManager Resource.
     */
    std::shared_ptr< OCResource > m_SSMResource;
    /**
     * @brief attribute map .
     */
    AttributeMap m_responseAttributeMap;
    /**
     * @brief query engine.
     */
    std::string m_queryEngineId;
    /**
     * @brief app listener
     */
    ISSMClientListener* m_appListener;

    /**
     * @brief internal find resource function
     */
    void _findResource(void);

    /**
     * @brief internal Query engine function
     */
    void _createQueryEngine(void);

    /**
     * @brief internal release query function
     */
    void _releaseQueryEngine(std::string queryEngineId);

public:
    /**
     * Constructor for SSMClient. 
     */
    SSMClient();
    ~SSMClient();

    /**
     * This API sends query strings of applications to SoftSensorManager on Iotivity Base messaging.
     * 
     * @param [in] queryString - A conditions query statement where the caller application specifies sensors for required data and conditions when the caller wants to get the data.
     * @param [in] listener - The address of listener class.  When an application which inherits the ISSMClientListener calls this operation, it sends its address for the listener so that
     *                          SSMClient can callback message to the application.
     * @param [out] cqid - A query id generated from SoftSensorManager service for the queryString request.
     * @return  SSMReturn
     
     */
    SSMReturn registerQuery(std::string queryString, ISSMClientListener* listener,
            std::string &cqid);
    /**
     * This API is to cancel the registered sends query strings of applications to SoftSensorManager on Iotivity Base messaging.
     * 
     * @param [in] listener - The address of listener class.  When an application which inherits the ISSMClientListener calls this operation, it sends its address for the listener so that
     *                          SSMClient can callback message to the application.
     * @param [in] cqid - A query id generated from SoftSensorManager service for the queryString request.
     * @return  SSMReturn
     */
    SSMReturn unregisterQuery(std::string cqid);

    // friend option. for callback from SSMResource
    void onFoundResource(std::shared_ptr< OCResource > resource);
    void onCreateQueryEngine(const OCRepresentation& rep, const int eCode);
    void onReleaseQueryEngine(const OCRepresentation& rep, const int eCode);
    void onRegisterQuery(const OCRepresentation& rep, const int eCode);
    void onUnregisterQuery(const OCRepresentation& rep, const int eCode);
    void onObserve(const OCRepresentation& rep, const int& eCode);
};

#endif /* RESOURCECLIENT_H_ */
