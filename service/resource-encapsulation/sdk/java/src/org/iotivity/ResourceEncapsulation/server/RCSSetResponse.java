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
 * @file    RCSSetResponse.java
 *
 *    This file contains the class for creating a Set response
 *             for the received Set request.
 *
 */

package org.iotivity.ResourceEncapsulation.server;

import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;
import org.iotivity.ResourceEncapsulation.utils.EntityHandlerResult;

/**
 * This class provides methods to create the response for a received set
 * request.
 *
 * {@link RCSResourceObject}
 */
public class RCSSetResponse {

    private int                   m_typeOfSetResponse;
    private int                   m_handlerResult;
    private int                   m_errorCode;
    private RCSResourceAttributes m_attributes;

    private final static int      DEFAULT_RESPONSE                 = 0;
    private final static int      CREATE_WITH_RESULT               = 1;
    private final static int      CREATE_WITH_ATTRIBUTE            = 2;
    private final static int      CREATE_WITH_ATTRIBUTE_AND_RESULT = 3;
    private final static int      ACCEPT                           = 4;
    private final static int      ACCEPT_WITH_RESULT               = 5;
    private final static int      IGNORE                           = 6;
    private final static int      IGNORE_WIH_RESULT                = 7;

    /**
     * Creates a default RCSSetResponse with DEFAULT acceptance method. The
     * response will have OC_EH_OK for the result and 200 for the errorCode. The
     * attributes of RCSResourceObject will be set as the result attributes.
     *
     * <p>
     * DEFAULT : Follow SetRequestHandlerPolicy of the RCSResourceObject.
     *
     * @return RCSSetResponse
     *
     *         {@link SetRequestHandlerPolicy}
     */
    public static RCSSetResponse defaultAction() {
        return new RCSSetResponse(DEFAULT_RESPONSE);
    }

    /**
     * Creates a default RCSSetResponse with ACCEPT acceptance method. The
     * response will have OC_EH_OK for the result and 200 for the errorCode. The
     * attributes of RCSResourceObject will be set as the result attributes.
     *
     * <p>
     * ACCEPT : Accept the request attributes even if there is an unknown key or
     * mismatched type.
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse accept() {
        return new RCSSetResponse(ACCEPT);
    }

    /**
     * Creates a RCSSetResponse with ACCEPT acceptance method. with a result and
     * error code passed. The attributes of the RCSResourceObject will be set as
     * the result attributes.
     *
     * @param result
     *            The response result.
     * @param errorCode
     *            The error code to set in response.
     *
     *            <p>
     *            ACCEPT : Accept the request attributes even if there is an
     *            unknown key or mismatched type.
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse accept(EntityHandlerResult result,
            int errorCode) {
        return new RCSSetResponse(result.ordinal(), errorCode,
                ACCEPT_WITH_RESULT);
    }

    /**
     * Creates a default RCSSetResponse with IGNORE acceptance method. The
     * response will have OC_EH_OK for the result and 200 for the errorCode. The
     * attributes of RCSResourceObject will be set as the result attributes.
     *
     * <p>
     * IGNORE : Ignore the request attributes.
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse ignore() {
        return new RCSSetResponse(IGNORE);
    }

    /**
     * Creates a RCSSetResponse with IGNORE acceptance method. with a result and
     * error code passed. The attributes of the RCSResourceObject will be set as
     * the result attributes.
     *
     * @param result
     *            The response result.
     * @param errorCode
     *            The error code to set in response.
     *
     *            <p>
     *            IGNORE : Ignore the request attributes.
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse ignore(EntityHandlerResult result,
            int errorCode) {
        return new RCSSetResponse(result.ordinal(), errorCode,
                IGNORE_WIH_RESULT);
    }

    /**
     * Creates a RCSSetResponse with a result and error code passed and has
     * DEFAULT acceptance method. The attributes of the RCSResourceObject will
     * be set as the result attributes.
     *
     * <p>
     * DEFAULT : Follow SetRequestHandlerPolicy of the RCSResourceObject.
     *
     * @param result
     *            The response result.
     * @param errorCode
     *            The error code to set in response.
     *
     *            {@link EntityHandlerResult}
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse create(EntityHandlerResult result,
            int errorCode) {
        return new RCSSetResponse(result.ordinal(), errorCode,
                CREATE_WITH_RESULT);
    }

    /**
     * Creates a RCSSetResponse with custom attributes and has DEFAULT
     * acceptance method. This sends the passed attributes as the result
     * attributes instead of one the RCSResourceObject holds.
     *
     * <p>
     * DEFAULT : Follow SetRequestHandlerPolicy of the RCSResourceObject.
     *
     * @param attributes
     *            The attributes to send in response.
     *
     *            {@link RCSResourceAttributes}
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse create(RCSResourceAttributes attributes) {
        return new RCSSetResponse(attributes);
    }

    /**
     * Creates a RCSSetResponse with a result and error code passed. This sends
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
     *            <p>
     *            RCSResourceAttributes
     *
     * @return RCSSetResponse
     */
    public static RCSSetResponse create(RCSResourceAttributes attributes,
            EntityHandlerResult result, int errorCode) {
        return new RCSSetResponse(attributes, result.ordinal(), errorCode);
    }

    // ******* JNI UTILTY FUNCTIONS ******* //

    private RCSSetResponse(int typeOfSetResponse) {
        this.m_typeOfSetResponse = typeOfSetResponse;
    }

    private RCSSetResponse(int handlerResult, int errorCode, int typeOfRequest) {
        m_typeOfSetResponse = typeOfRequest;
        m_handlerResult = handlerResult;
        m_errorCode = errorCode;
    }

    private RCSSetResponse(RCSResourceAttributes attributes) {
        m_typeOfSetResponse = CREATE_WITH_ATTRIBUTE;
        this.m_attributes = attributes;
    }

    private RCSSetResponse(RCSResourceAttributes attributes, int handlerResult,
            int errorCode) {
        m_typeOfSetResponse = CREATE_WITH_ATTRIBUTE_AND_RESULT;
        m_attributes = attributes;
        m_handlerResult = handlerResult;
        m_errorCode = errorCode;
    }

    private int getTypeOfGetResponse() {
        return m_typeOfSetResponse;
    }

    private int getResult() {
        return m_handlerResult;
    }

    private int getErrorCode() {
        return m_errorCode;
    }

    private RCSResourceAttributes getAttributes() {
        return m_attributes;
    }
}
