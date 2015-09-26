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
 * This class provides methods to create the response for a received get
 * request.
 *
 * @see RcsResourceObject
 * @see RcsSetResponse
 */
public class RcsGetResponse extends RcsResponse {

    /**
     * Creates a default RcsResourceObject. The response will have
     * RCSResponse.DEFAULT_ERROR_CODE for the errorCode. The attributes of
     * RCSResourceObject will be set as the result attributes.
     *
     */
    public static RcsGetResponse defaultAction() {
        return new RcsGetResponse();
    }

    /**
     * Creates a RcsResourceObject with error code passed. The
     * attributes of the RCSResourceObject will be set as the result attributes.
     *
     * @param errorCode
     *            The error code to set in response.
     *
     */
    public static RcsGetResponse create(int errorCode) {
        return new RcsGetResponse(errorCode);

    }

    /**
     * Creates a RcsResourceObject with custom attributes. This sends the passed
     * attributes as the result attributes instead of the one the
     * RCSResourceObject holds.
     *
     * @param attributes
     *            The attributes to send in response.
     *
     */
    public static RcsGetResponse create(RcsResourceAttributes attributes) {
        return new RcsGetResponse(attributes);
    }

    /**
     * Creates a RcsResourceObject with error code passed. This sends
     * the passed attributes as the result attributes instead of one the
     * RCSResourceObject holds.
     *
     * @param attributes
     *            The attributes to send in response.
     * @param errorCode
     *            The error code for response.
     *
     */
    public static RcsGetResponse create(RcsResourceAttributes attributes,
            int errorCode) {
        return new RcsGetResponse(attributes, errorCode);
    }

    private RcsGetResponse() {
        super();
    }

    private RcsGetResponse(int errorCode) {
        super(errorCode);
    }

    private RcsGetResponse(RcsResourceAttributes attrs) {
        super(attrs);
    }

    private RcsGetResponse(RcsResourceAttributes attrs, int errorCode) {
        super(attrs, errorCode);
    }
}
