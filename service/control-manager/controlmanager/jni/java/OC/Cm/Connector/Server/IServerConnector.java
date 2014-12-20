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

import OC.Cm.Connector.IConnectorConfiguration;

public abstract class IServerConnector {

    private native long constructNativeConnector();

    private native void deleteNativeConnector(long nativeHandle);

    protected native String getProtocol(long nativeHandle);

    protected native long getConnectorConfiguration(long nativeHandle);

    protected long mNativeHandle;

    public IServerConnector() {
        this.mNativeHandle = constructNativeConnector();
    }

    public void destroyConnector() {
        if (mNativeHandle != 0) {
            deleteNativeConnector(mNativeHandle);
            mNativeHandle = 0;
        }
    }

    public long getNativeHandle() {
        return mNativeHandle;
    }

    public abstract boolean init(int serverId);

    public abstract boolean deinit();

    public abstract boolean start();

    public abstract boolean stop();

    public abstract void addServerlistener(
            OC.Cm.Connector.Server.IServerListener listener);

    public abstract boolean sendResponse(
            OC.Cm.Connector.Server.ServerSession session);

    public abstract void clearSession(
            OC.Cm.Connector.Server.ServerSession session);

    public abstract String getProtocol();

    public abstract IConnectorConfiguration getConnectorConfiguration();
}
