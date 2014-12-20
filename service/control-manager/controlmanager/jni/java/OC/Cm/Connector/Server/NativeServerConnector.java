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

public class NativeServerConnector extends IServerConnector {

    public NativeServerConnector(long nativeHandle) {
        mNativeHandle = nativeHandle;
    }

    @Override
    public IConnectorConfiguration getConnectorConfiguration() {
        return null;
    }

    @Override
    public String getProtocol() {
        return getProtocol(mNativeHandle);
    }

    // Currently no need to implement below methods as these methods are not
    // used from java..
    @Override
    public boolean init(int serverId) {
        return false;
    }

    @Override
    public boolean deinit() {
        return false;
    }

    @Override
    public boolean start() {
        return false;
    }

    @Override
    public boolean stop() {
        return false;
    }

    @Override
    public void addServerlistener(IServerListener listener) {
    }

    @Override
    public boolean sendResponse(ServerSession session) {
        return false;
    }

    @Override
    public void clearSession(ServerSession session) {
    }
}
