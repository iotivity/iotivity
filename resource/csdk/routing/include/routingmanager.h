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

/**
 * @file routingmanager.h
 * @brief This file contains the APIs for routing manager.
 */
#ifndef ROUTING_MANAGER_H_
#define ROUTING_MANAGER_H_

#include "cacommon.h"
#include "cainterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initialize the Routing Manager.
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMInitialize(); // Should this be given ACL callback from RI ?

/**
 * @brief   Terminates the Routing Manager.
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMTerminate();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ROUTING_MANAGER_H_ */
