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

package OC.Cm.Connector.Client;

import OC.Cm.Connector.IConnectorConfiguration;

public abstract class IClientConnector {

    private native long constructNativeConnector();

    private native void deleteNativeConnector(long nativeHandle);

    protected native long getConnectorConfiguration(long nativeHandle);

    protected native String getProtocol(long nativeHandle);

    protected long mNativeHandle;

    public IClientConnector() {
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

    public abstract boolean init(int clientId);

    public abstract boolean deinit();

    public abstract boolean start();

    public abstract boolean stop();

    public abstract void addClientListener(
            OC.Cm.Connector.Client.IClientListener listener);

    public abstract int sendRequest(
            OC.Cm.Connector.Client.ClientSession client_session);

    public abstract OC.Cm.Connector.Client.ClientSession createNewSession(
            String deviceAddress);

    public abstract String getProtocol();

    /**
     * returns instance of Sec::Cm::Connector::IConnectorConfiguration
     * interface. Application needs to typecast it to corresponding connector
     * specific configuration class before using it
     * 
     * @return instance of Sec::Cm::Connector::IConnectorConfiguration object
     */
    public abstract IConnectorConfiguration getConnectorConfiguration();
}
