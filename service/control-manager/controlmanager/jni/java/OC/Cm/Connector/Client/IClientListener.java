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

public class IClientListener {
    private long mNativeHandle;

    private native void onResponseReceived(long nativeHandle, long sesion);

    private native void onErrorRecieved(long nativeHandle, long sesion,
            int errorCode);

    public IClientListener(long nativehandle) {
        mNativeHandle = nativehandle;
    }

    void onResponseReceived(OC.Cm.Connector.Client.ClientSession client_sessioin) {
        onResponseReceived(mNativeHandle, client_sessioin.getNativeHandle());
    }

    void onErrorRecieved(OC.Cm.CMErrorCode error,
            OC.Cm.Connector.Client.ClientSession client_sessioin) {
        onErrorRecieved(mNativeHandle, client_sessioin.getNativeHandle(),
                error.getValue());
    }
}
