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

public class IServerListener {
    private long mNativeHandle;

    private native void onServerStarted(long nativehandle, int serverId,
            String address);

    private native void onServerStopped(long nativehandle, int serverId);

    private native void onServerError(long nativehandle, int serverId,
            int errorCode);

    private native void onServerSessionStarted(long nativehandle, int serverId,
            long session);

    private native void onServerSessionCompleted(long nativehandle,
            int serverId, long session);

    private native boolean handleRequest(long nativehandle, long session);

    public IServerListener(long nativehandle) {
        mNativeHandle = nativehandle;
    }

    public void onServerStarted(int serverId, String address) {
        onServerStarted(mNativeHandle, serverId, address);
    }

    public void onServerStopped(int serverId) {
        onServerStopped(mNativeHandle, serverId);
    }

    public void onServerError(int serverId, OC.Cm.CMErrorCode errorCode) {
        onServerError(mNativeHandle, serverId, errorCode.getValue());
    }

    public void onServerSessionStarted(int serverId,
            OC.Cm.Connector.Server.ServerSession session) {
        onServerSessionStarted(mNativeHandle, serverId, session.nativeHandle);
    }

    public void onServerSessionCompleted(int serverId,
            OC.Cm.Connector.Server.ServerSession session) {
        onServerSessionCompleted(mNativeHandle, serverId, session.nativeHandle);
    }

    public boolean handleRequest(OC.Cm.Connector.Server.ServerSession session) {
        return handleRequest(mNativeHandle, session.nativeHandle);
    }
}
