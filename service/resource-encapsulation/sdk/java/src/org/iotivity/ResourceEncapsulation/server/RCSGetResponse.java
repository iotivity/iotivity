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
 * @file    RCSGetResponse.java
 *
 *    This file contains the class for creating a Get response
 *             for the received Get request.
 *
 */

package org.iotivity.ResourceEncapsulation.server;

import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;
import org.iotivity.ResourceEncapsulation.utils.EntityHandlerResult;

/**
 * This class provides methods to create the response for a received get
 * request.
 *
 * {@link RCSResourceObject}
 */
public class RCSGetResponse {

    private int                   m_typeOfGetResponse;
    private int                   m_handlerResult;
    private int                   m_errorCode;
    private RCSResourceAttributes m_attributes;

    private final static int      DEFAULT_RESPONSE                 = 0;
    private final static int      CREATE_WITH_RESULT               = 1;
    private final static int      CREATE_WITH_ATTRIBUTE            = 2;
    private final static int      CREATE_WITH_ATTRIBUTE_AND_RESULT = 3;

    /**
     * Creates a default RCSGetResponse. The response will have OC_EH_OK for the
     * result and 200 for the errorCode. The attributes of RCSResourceObject
     * will be set as the result attributes.
     *
     * @return RCSGetResponse
     */
    public static RCSGetResponse defaultAction() {
        return new RCSGetResponse();
    }

    /**
     * Creates a RCSGetResponse with a result and error code passed. The
     * attributes of the RCSResourceObject will be set as the result attributes.
     *
     * @param result
     *            The response result.
     * @param errorCode
     *            The error code to set in response.
     *
     * @return RCSGetResponse
     *
     *         {@link EntityHandlerResult}
     *
     */
    public static RCSGetResponse create(EntityHandlerResult result,
            int errorCode) {
        return new RCSGetResponse(result.ordinal(), errorCode);

    }

    /**
     * Creates a RCSGetResponse with custom attributes. This sends the passed
     * attributes as the result attributes instead of the one the
     * RCSResourceObject holds.
     *
     * @param attributes
     *            The attributes to send in response.
     *
     * @return RCSGetResponse
     *
     *         {@link RCSResourceAttributes}
     *
     */
    public static RCSGetResponse create(RCSResourceAttributes attributes) {
        return new RCSGetResponse(attributes);
    }

    /**
     * Creates a RCSGetResponse with a result and error code passed. This sends
     * the passed attributes as the result attributes instead of one the
     * RCSResourceObject holds.
     *
     * @param attributes
     *            The attributes to send in response.
     * @param result
     *            The response result.
     * @param errorCode
     *            The error code for response.
     *
     *            {@link RCSResourceAttributes}
     *
     * @return RCSGetResponse
     */
    public static RCSGetResponse create(RCSResourceAttributes attributes,
            EntityHandlerResult result, int errorCode) {
        return new RCSGetResponse(attributes, result.ordinal(), errorCode);
    }

    // ******* JNI UTILTY FUNCTIONS ******* //

    private RCSGetResponse() {
        m_typeOfGetResponse = DEFAULT_RESPONSE;
    }

    private RCSGetResponse(int handlerResult, int errorCode) {
        m_typeOfGetResponse = CREATE_WITH_RESULT;
        this.m_handlerResult = handlerResult;
        this.m_errorCode = errorCode;
    }

    private RCSGetResponse(RCSResourceAttributes attributes) {
        m_typeOfGetResponse = CREATE_WITH_ATTRIBUTE;
        this.m_attributes = attributes;
    }

    private RCSGetResponse(RCSResourceAttributes attributes, int handlerResult,
            int errorCode) {
        m_typeOfGetResponse = CREATE_WITH_ATTRIBUTE_AND_RESULT;
        this.m_attributes = attributes;
        this.m_handlerResult = handlerResult;
        this.m_errorCode = errorCode;
    }

    @SuppressWarnings("unused")
    private int getTypeOfGetResponse() {
        return m_typeOfGetResponse;
    }

    @SuppressWarnings("unused")
    private int getResult() {
        return m_handlerResult;
    }

    @SuppressWarnings("unused")
    private int getErrorCode() {
        return m_errorCode;
    }

    @SuppressWarnings("unused")
    private RCSResourceAttributes getAttributes() {
        return m_attributes;
    }
}
