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

package OC.Cm;

public class StreamSocket {

    private long nativeHandle;

    private native long constructNative(String peerID, String streamName);

    private native long constructNative(int connID, String streamName);

    private native void deleteNative(long socket);

    private native int connectForStreaming(long nativeHandle) throws Exception;

    private native void closeStreaming(long nativeHandle) throws Exception;

    private native boolean isConnected(long nativeHandle) throws Exception;

    private native boolean sendStream(long nativeHandle, final byte[] stream)
            throws Exception;

    private native int receiveStream(long nativeHandle, byte[] stream)
            throws Exception;

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeHandle != 0) {
            this.deleteNative(nativeHandle);
            nativeHandle = 0;
        }
    }

    public void onNativeDelete() {
        nativeHandle = 0;
    }

    public StreamSocket(String peerID, String streamName) {
        nativeHandle = constructNative(peerID, streamName);
    }

    public StreamSocket(int connID, String streamName) {
        nativeHandle = constructNative(connID, streamName);
    }

    public int connectForStreaming() throws Exception {
        return connectForStreaming(nativeHandle);
    }

    public void closeStreaming() throws Exception {
        closeStreaming(nativeHandle);
    }

    public boolean isConnected() throws Exception {
        return isConnected(nativeHandle);
    }

    public boolean sendStream(final byte[] stream) throws Exception {
        return sendStream(nativeHandle, stream);
    }

    public int receiveStream(byte[] stream) throws Exception {
        return receiveStream(nativeHandle, stream);
    }
}