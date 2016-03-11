/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.base.protocols.coap;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;

public class CoapRequest extends CoapMessage {
    public CoapRequest(CoapMethod method) {
        this.code = method.getCode();
    }

    public CoapMethod getRequestMethod() {
        return CoapMethod.valueOf(code);
    }

    public void setUriPath(String path) {
        if (uri_path == null) {
            uri_path = new ArrayList<byte[]>();
        }
        clearUriPath();

        String[] pathSegments = path.split("/");
        for (String pathSegment : pathSegments) {
            if (pathSegment.length() == 0)
                continue;
            uri_path.add(pathSegment.getBytes(StandardCharsets.UTF_8));
        }
    }

    public String getUriPath() {
        if (uri_path == null) {
            return null;
        }
        StringBuilder path = new StringBuilder();
        int nItem = uri_path.size();
        for (byte[] pathSegment : uri_path) {
            path.append(new String(pathSegment, StandardCharsets.UTF_8));
            if (--nItem > 0) {
                path.append('/');
            }
        }
        return path.toString();
    }

    public List<String> getUriPathSegments() {
        if (uri_path == null) {
            return null;
        }
        List<String> segments = new ArrayList<String>();
        for (byte[] pathSegment : uri_path) {
            segments.add(new String(pathSegment, StandardCharsets.UTF_8));
        }
        return segments;
    }

    public void setUriQuery(String query) {
        if (uri_query == null) {
            uri_query = new ArrayList<byte[]>();
        }
        String[] querySegments = query.split("&");
        for (String querySegment : querySegments) {
            uri_query.add(querySegment.getBytes(StandardCharsets.UTF_8));
        }
    }

    public String getUriQuery() {
        if (uri_query == null) {
            return null;
        }
        StringBuilder uriQuery = new StringBuilder();
        int nItem = uri_query.size();
        for (byte[] querySegment : uri_query) {
            uriQuery.append(new String(querySegment, StandardCharsets.UTF_8));
            if (--nItem > 0) {
                uriQuery.append('&');
            }
        }
        return uriQuery.toString();
    }

    public List<String> getUriQuerySegments() {
        if (uri_query == null) {
            return null;
        }
        List<String> segments = new ArrayList<String>();
        for (byte[] querySegment : uri_query) {
            segments.add(new String(querySegment, StandardCharsets.UTF_8));
        }
        return segments;
    }

    public void clearUriPath() {
        if (uri_path != null) {
            uri_path.clear();
        }
    }
}