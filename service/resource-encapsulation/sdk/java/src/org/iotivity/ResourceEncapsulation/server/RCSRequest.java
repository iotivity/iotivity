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
 * This file provides RCSRequest class which will describes the request.
 */

package org.iotivity.ResourceEncapsulation.server;

/**
 * This class describes the request.
 */
public class RCSRequest {

    private String m_resourceUri;

    private RCSRequest(String resourceUri) {
        m_resourceUri = resourceUri;
    }

    /**
     * Returns the URI of the request.
     *
     * @return Uri of the request in string form
     */
    public String getResourceUri() {
        return m_resourceUri;
    }
}
