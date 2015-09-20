/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 * @file
 * This file provides EntityHandlerResult Enum & utility function.
 */

package org.iotivity.ResourceEncapsulation.utils;

import org.iotivity.ResourceEncapsulation.server.*;

/**
 * Represents the EntityHandlerResult, that application can send for a Get or a
 * Set request.
 *
 * {@link RCSResourceObject} {@link RCSGetResponse} {@link RCSSetResponse}
 */
public enum EntityHandlerResult {

    OC_EH_OK, OC_EH_ERROR, OC_EH_RESOURCE_CREATED, OC_EH_RESOURCE_DELETED, OC_EH_SLOW, OC_EH_FORBIDDEN, OC_EH_RESOURCE_NOT_FOUND;
}
