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

package OC.Cm.Connector.Server;

import OC.Cm.Connector.CMRequest;
import OC.Cm.Connector.CMResponse;
import OC.Cm.Connector.Session;

public class ServerSession extends Session {
    public long  nativeHandle;
    private long mContext;

    private native long constructNativeSession(int serverId, long context,
            String address);

    private native String getClientAddress(long sHandle);

    private native OC.Cm.Device getClientDevice(long sHandle);

    private native int getServerId(long sHandle);

    private native boolean setResponseHeader(long sHandle, String name,
            String value);

    private native String getTemplateParameter(long handle, String name);

    public ServerSession(long nativeHandle, long context) {
        this.nativeHandle = nativeHandle;
        this.mContext = context;
    }

    public ServerSession(long context, int serverId, String address) {
        this.nativeHandle = constructNativeSession(serverId, context, address);
        this.mContext = context;
    }

    public long getContext() {
        return mContext;
    }

    public int getServerId() {
        return this.getServerId(nativeHandle);
    }

    public String getClientAddress() {
        return this.getClientAddress(nativeHandle);
    }

    public OC.Cm.Device getClientDevice() {
        return this.getClientDevice(nativeHandle);
    }

    public int getSessionId() {
        return this.getSessionId(nativeHandle);
    }

    public void setResponseHeader(String name, String value) {
        this.setResponseHeader(nativeHandle, name, value);
    }

    public String getTemplateParameter(String name) {
        return this.getTemplateParameter(nativeHandle, name);
    }

    public OC.Cm.Connector.CMRequest getRequest() {
        return new CMRequest(getRequest(nativeHandle));
    }

    public OC.Cm.Connector.CMResponse getResponse() {
        return new CMResponse(getResponse(nativeHandle));
    }
}
