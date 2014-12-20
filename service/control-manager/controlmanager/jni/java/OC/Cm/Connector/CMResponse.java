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

package OC.Cm.Connector;

public class CMResponse extends CMMessage {

    public CMResponse(long nativeHandle) {
        super(nativeHandle);
    }

    public String getPayloadType() {
        return getPayloadType(mNativeHandle, false);
    }

    public String getPayload() {
        return getPayload(mNativeHandle, false);
    }

    public int getPayloadSize() {
        return getPayloadSize(mNativeHandle, false);
    }

    public HeadersList getHeaders() {
        return new HeadersList(getHeaders(mNativeHandle, false));
    }

    public String getResourcePath() {
        return getResourcePath(mNativeHandle, false);
    }

    public void setResourcePath(String resourcePath) {
        setResourcePath(mNativeHandle, resourcePath, false);
    }

    public String getMethod() {
        return getMethod(mNativeHandle, false);
    }

    public void setMethod(String resourcePath) {
        setMethod(mNativeHandle, resourcePath, false);
    }

    public String getQueryParam(String paramName) {
        return getQueryParam(mNativeHandle, paramName, false);
    }

    public void setQueryParam(String paramName, String paramValue) {
        setQueryParam(mNativeHandle, paramName, paramValue, false);
    }

    public int getResponseCode() {
        return getResponseCode(mNativeHandle);
    }

    public void setResponseCode(int responseCode) {
        setResponseCode(mNativeHandle, responseCode);
    }
}
