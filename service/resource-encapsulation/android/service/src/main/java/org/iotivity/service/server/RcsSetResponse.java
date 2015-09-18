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

package org.iotivity.service.server;

import org.iotivity.service.RcsResourceAttributes;

/**
 * This class provides methods to create the response for a received set
 * request.
 *
 * @see RcsResourceObject
 * @see RcsSetResponse
 */
public final class RcsSetResponse extends RcsResponse {
    /**
     * Options for handling a set request.
     *
     * This overrides RcsResourceObject#SetRequestHandlerPolicy.
     *
     * @see RcsResourceObject#SetRequestHandlerPolicy
     *
     */
    public enum AcceptanceMethod {
        /**
         * Follow RcsResourceObject#SetRequestHandlerPolicy.
         */
        DEFAULT,

        /**
         * Accept the request attributes even if there is an unknown key or
         * mismatched type.
         */
        ACCEPT,

        /**
         * Ignore the request attributes.
         */
        IGNORE
    };

    private AcceptanceMethod mAcceptanceMethod = AcceptanceMethod.DEFAULT;

    /**
     * Creates a default RCSSetResponse with DEFAULT acceptance method. The
     * response will have RCSResponse.DEFAULT_ERROR_CODE for the errorCode. The
     * attributes of RCSResourceObject will be set as the result attributes.
     *
     */
    public static RcsSetResponse defaultAction() {
        return new RcsSetResponse();
    }

    /**
     * Creates a default RCSSetResponse with ACCEPT acceptance method. The
     * response will have RCSResponse.DEFAULT_ERROR_CODE for the errorCode. The
     * attributes of RCSResourceObject will be set as the result attributes.
     *
     */
    public static RcsSetResponse accept() {
        return new RcsSetResponse()
                .setAcceptanceMethod(AcceptanceMethod.ACCEPT);
    }

    /**
     * Creates a RCSSetResponse with ACCEPT acceptance method and error code
     * passed.
     * The attributes of the RCSResourceObject will be set as the result
     * attributes.
     *
     * @param errorCode
     *            The error code to set in response.
     *
     */
    public static RcsSetResponse accept(int errorCode) {
        return new RcsSetResponse(errorCode)
                .setAcceptanceMethod(AcceptanceMethod.ACCEPT);
    }

    /**
     * Creates a default RCSSetResponse with IGNORE acceptance method. The
     * response will have RCSResponse.DEFAULT_ERROR_CODE for the errorCode. The
     * attributes of RCSResourceObject will be set as the result attributes.
     *
     */
    public static RcsSetResponse ignore() {
        return new RcsSetResponse()
                .setAcceptanceMethod(AcceptanceMethod.IGNORE);
    }

    /**
     * Creates a RCSSetResponse with IGNORE acceptance method and
     * error code passed. The attributes of the RCSResourceObject will be set as
     * the result attributes.
     *
     * @param errorCode
     *            The error code to set in response.
     *
     */
    public static RcsSetResponse ignore(int errorCode) {
        return new RcsSetResponse(errorCode)
                .setAcceptanceMethod(AcceptanceMethod.IGNORE);
    }

    /**
     * Creates a RCSSetResponse with error code passed and has
     * DEFAULT acceptance method. The attributes of the RCSResourceObject will
     * be set as the result attributes.
     *
     * @param errorCode
     *            The error code to set in response.
     *
     */
    public static RcsSetResponse create(int errorCode) {
        return new RcsSetResponse(errorCode);
    }

    /**
     * Creates a RCSSetResponse with custom attributes and has DEFAULT
     * acceptance method. This sends the passed attributes as the result
     * attributes instead of one the RCSResourceObject holds.
     *
     * @param attributes
     *            The attributes to send in response.
     *
     */
    public static RcsSetResponse create(RcsResourceAttributes attributes) {
        return new RcsSetResponse(attributes);
    }

    /**
     * Returns the acceptance method.
     *
     */
    public AcceptanceMethod getAcceptanceMethod() {
        return mAcceptanceMethod;
    }

    /**
     * Sets the acceptance method for the RCSSetResponse.
     *
     * @param method
     *            AcceptanceMethod value to set
     *
     * @return The reference to this RCSSetResponse
     *
     */
    public RcsSetResponse setAcceptanceMethod(AcceptanceMethod method) {
        mAcceptanceMethod = method;
        return this;
    }

    /**
     * Creates a RCSSetResponse with error code passed. This sends
     * the passed attributes as the result attributes instead of one the
     * RCSResourceObject holds.
     *
     * @param attributes
     *            The attributes to send in response.
     * @param errorCode
     *            The error code for response.
     *
     */
    public static RcsSetResponse create(RcsResourceAttributes attributes,
            int errorCode) {
        return new RcsSetResponse(attributes, errorCode);
    }

    private RcsSetResponse() {
        super();
    }

    private RcsSetResponse(int errorCode) {
        super(errorCode);
    }

    private RcsSetResponse(RcsResourceAttributes attrs) {
        super(attrs);
    }

    private RcsSetResponse(RcsResourceAttributes attrs, int errorCode) {
        super(attrs, errorCode);
    }

}
