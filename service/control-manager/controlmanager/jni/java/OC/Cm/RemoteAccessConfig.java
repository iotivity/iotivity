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

public class RemoteAccessConfig {
    private long nativeHandle;

    private native void setAuthorizationType(long handle, int authType)
            throws Exception;

    private native OC.Cm.AuthorizationType getAuthorizationType(long handle)
            throws Exception;

    private native void setNetworkType(long handle, int netType)
            throws Exception;

    private native int getNetworkType(long handle) throws Exception;

    private native void setEmail(long handle, final String email)
            throws Exception;

    private native String getEmail(long handle) throws Exception;

    private native void setPassword(long handle, final String pwd)
            throws Exception;

    private native String getPassword(long handle) throws Exception;

    private native void setAuthAccountServerAddress(long handle,
            final String address) throws Exception;

    private native String getAuthAccountServerAddress(long handle)
            throws Exception;

    private native void setApiAccountServerAddress(long handle,
            final String address) throws Exception;

    private native String getApiAccountServerAddress(long handle)
            throws Exception;

    private native void setAccountServerPort(long handle, final String port)
            throws Exception;

    private native String getAccountServerPort(long handle, final String port)
            throws Exception;

    private native void setServiceServerAddress(long handle,
            final String address) throws Exception;

    private native String getServiceServerAddress(long handle) throws Exception;

    private native void setServiceServerPort(long handle, final String port)
            throws Exception;

    private native String getServiceServerPort(long handle) throws Exception;

    private native void setRemoteServicePort(long handle, final String port)
            throws Exception;

    private native String getRemoteServicePort(long handle) throws Exception;

    private native void setInstanceId(long handle, final String id)
            throws Exception;

    private native String getInstanceId(long handle) throws Exception;

    private native void setAppId(long handle, final String id) throws Exception;

    private native String getAppId(long handle) throws Exception;

    private native void setAppSecret(long handle, final String secret)
            throws Exception;

    private native String getAppSecret(long handle) throws Exception;

    private native void setAuthCode(long handle, final String code)
            throws Exception;

    private native String getAuthCode(long handle) throws Exception;

    private native void setAccessToken(long handle, final String token)
            throws Exception;

    private native String getAccessToken(long handle) throws Exception;

    private native void setRefreshToken(long handle, final String token)
            throws Exception;

    private native String getRefreshToken(long handle) throws Exception;

    private native void setUserID(long handle, final String id)
            throws Exception;

    private native String getUserID(long handle) throws Exception;

    private native void setRemoteConfigPath(long handle, final String path)
            throws Exception;

    private native String getRemoteConfigPath(long handle) throws Exception;

    private native void setSCSLogFilePath(long handle, final String path)
            throws Exception;

    private native String getSCSLogFilePath(long handle) throws Exception;

    private native void setSCSLogLevel(long handle, final String level)
            throws Exception;

    private native int getSCSLogLevel(long handle) throws Exception;

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {

    }

    public RemoteAccessConfig() {

    }

    public RemoteAccessConfig(long handle) {
        nativeHandle = handle;
    }

    public void setNativeHandle(long handle) {
        nativeHandle = handle;
    }

    public long getNativeHandle() {
        return nativeHandle;
    }

    public void setAuthorizationType(int authType) throws Exception {
        this.setAuthorizationType(nativeHandle, authType);
    }

    public OC.Cm.AuthorizationType getAuthorizationType() throws Exception {
        return this.getAuthorizationType(nativeHandle);
    }

    public void setNetworkType(int netType) throws Exception {
        this.setNetworkType(nativeHandle, netType);
    }

    public int getNetworkType() throws Exception {
        return this.getNetworkType(nativeHandle);
    }

    public void setEmail(final String email) throws Exception {
        this.setEmail(nativeHandle, email);
    }

    public String getEmail() throws Exception {
        return this.getEmail(nativeHandle);
    }

    public void setPassword(final String pwd) throws Exception {
        this.setPassword(nativeHandle, pwd);
    }

    public String getPassword() throws Exception {
        return this.getPassword(nativeHandle);
    }

    public void setAuthAccountServerAddress(final String address)
            throws Exception {
        this.setAuthAccountServerAddress(nativeHandle, address);
    }

    public String getAuthAccountServerAddress() throws Exception {
        return this.getAuthAccountServerAddress(nativeHandle);
    }

    public void setApiAccountServerAddress(final String address)
            throws Exception {
        this.setApiAccountServerAddress(nativeHandle, address);
    }

    public String getApiAccountServerAddress() throws Exception {
        return this.getApiAccountServerAddress(nativeHandle);
    }

    public void setAccountServerPort(final String port) throws Exception {
        this.setAccountServerPort(nativeHandle, port);
    }

    public void getAccountServerPort(final String port) throws Exception {
        this.getAccountServerPort(nativeHandle, port);
    }

    public void setServiceServerAddress(final String address) throws Exception {
        this.setServiceServerAddress(nativeHandle, address);
    }

    public String getServiceServerAddress() throws Exception {
        return this.getServiceServerAddress(nativeHandle);
    }

    public void setServiceServerPort(final String port) throws Exception {
        this.setServiceServerPort(nativeHandle, port);
    }

    public String getServiceServerPort() throws Exception {
        return this.getServiceServerPort(nativeHandle);
    }

    public void setRemoteServicePort(final String port) throws Exception {
        this.setRemoteServicePort(nativeHandle, port);
    }

    public String getRemoteServicePort() throws Exception {
        return this.getRemoteServicePort(nativeHandle);
    }

    public void setInstanceId(final String id) throws Exception {
        this.setInstanceId(nativeHandle, id);
    }

    public String getInstanceId() throws Exception {
        return this.getInstanceId(nativeHandle);
    }

    public void setAppId(final String id) throws Exception {
        this.setAppId(nativeHandle, id);
    }

    public String getAppId() throws Exception {
        return this.getAppId(nativeHandle);
    }

    public void setAppSecret(final String secret) throws Exception {
        this.setAppSecret(nativeHandle, secret);
    }

    public String getAppSecret() throws Exception {
        return this.getAppSecret(nativeHandle);
    }

    public void setAuthCode(final String code) throws Exception {
        this.setAuthCode(nativeHandle, code);
    }

    public String getAuthCode() throws Exception {
        return this.getAuthCode(nativeHandle);
    }

    public void setAccessToken(final String token) throws Exception {
        this.setAccessToken(nativeHandle, token);
    }

    public String getAccessToken() throws Exception {
        return this.getAccessToken(nativeHandle);
    }

    public void setRefreshToken(final String token) throws Exception {
        this.setRefreshToken(nativeHandle, token);
    }

    public String getRefreshToken() throws Exception {
        return this.getRefreshToken(nativeHandle);
    }

    public void setUserID(final String id) throws Exception {
        this.setUserID(nativeHandle, id);
    }

    public String getUserID() throws Exception {
        return this.getUserID(nativeHandle);
    }

    public void setRemoteConfigPath(final String path) throws Exception {
        this.setRemoteConfigPath(nativeHandle, path);
    }

    public String getRemoteConfigPath() throws Exception {
        return this.getRemoteConfigPath(nativeHandle);
    }

    public void setSCSLogFilePath(final String path) throws Exception {
        this.setSCSLogFilePath(nativeHandle, path);
    }

    public String getSCSLogFilePath() throws Exception {
        return this.getSCSLogFilePath(nativeHandle);
    }

    public void setSCSLogLevel(final String path) throws Exception {
        this.setSCSLogLevel(nativeHandle, path);
    }

    public int getSCSLogLevel() throws Exception {
        return this.getSCSLogLevel(nativeHandle);
    }
}
