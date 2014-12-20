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

public class SSLConfiguration implements IConnectorConfiguration {

    private long mNativeHandle;

    private native void addCACertificate(long config, String caCertificatePath)
            throws Exception;

    private native void enableDefaultCACertificates(long config)
            throws Exception;

    private native void setSelfCertificate(long config, String certificatePath,
            String privateKeyFilePath, String password) throws Exception;

    private native void setSelfCertificateWithRSAPrivateKey(long config,
            String certificatePath, String privateKeyFilePath, String password)
            throws Exception;

    private native void setTempDHParamFilePath(long config, String tmpDHFile)
            throws Exception;

    private native void enablePeerVerification(long config) throws Exception;

    private native void setProxyServer(long config, String address, String Port);

    private native void setCipherSuites(long config, String cipherSuite)
            throws Exception;

    private native String[] getCACertificatePath(long config) throws Exception;

    private native String getSelfCertificatePath(long config) throws Exception;

    private native String getPrivateKeyFilePath(long config) throws Exception;

    private native String getPrivateKeyPassword(long config) throws Exception;

    private native String getTemporaryDHParametersFile(long config)
            throws Exception;

    private native boolean getEnableDefaultCACertificates(long config)
            throws Exception;

    private native boolean getEnablePeerVerification(long config)
            throws Exception;

    private native boolean IsRSAformatPrivateKey(long config) throws Exception;

    private native String getProxyAddress(long config);

    private native String getProxyPort(long config);

    private native String[] getProxyUsageList(long config) throws Exception;

    private native void setProxyUsageList(long config, String[] proxyUsageList)
            throws Exception;

    private native boolean loadFromFile(long config, String configPath)
            throws Exception;

    public SSLConfiguration(long nativeHandle) {
        mNativeHandle = nativeHandle;
    }

    public boolean loadFromFile(String sslConfigPath) throws Exception {
        return this.loadFromFile(mNativeHandle, sslConfigPath);
    }

    public void addCACertificate(String caCertificatePath) throws Exception {
        this.addCACertificate(mNativeHandle, caCertificatePath);
    }

    public void setCipherSuites(String cipherSuite) throws Exception {
        this.setCipherSuites(mNativeHandle, cipherSuite);
    }

    public void enableDefaultCACertificates() throws Exception {
        this.enableDefaultCACertificates(mNativeHandle);
    }

    public void setSelfCertificate(String certificatePath,
            String privateKeyFilePath, String password) throws Exception {
        this.setSelfCertificate(mNativeHandle, certificatePath,
                privateKeyFilePath, password);
    }

    public void setSelfCertificateWithRSAPrivate(String certificatePath,
            String privateKeyFilePath, String password) throws Exception {
        this.setSelfCertificateWithRSAPrivateKey(mNativeHandle,
                certificatePath, privateKeyFilePath, password);
    }

    public void setTempDHParamFilePath(String tmpDHFile) throws Exception {
        this.setTempDHParamFilePath(mNativeHandle, tmpDHFile);
    }

    public void enablePeerVerification() throws Exception {
        this.enablePeerVerification(mNativeHandle);
    }

    public void setProxyServer(String Address, String Port) throws Exception {
        this.setProxyServer(mNativeHandle, Address, Port);
    }

    public String[] getCACertificatePath() throws Exception {
        return this.getCACertificatePath(mNativeHandle);
    }

    public String getSelfCertificatePath() throws Exception {
        return this.getSelfCertificatePath(mNativeHandle);
    }

    public String getPrivateKeyFilePath() throws Exception {
        return this.getPrivateKeyFilePath(mNativeHandle);
    }

    public String getPrivateKeyPassword(int m_ssl_conn_type) throws Exception {
        return this.getPrivateKeyPassword(mNativeHandle);
    }

    public String gettemporaryDHParametersFile(int m_ssl_conn_type)
            throws Exception {
        return this.getTemporaryDHParametersFile(mNativeHandle);
    }

    public boolean getEnableDefaultCACertificates() throws Exception {
        return this.getEnableDefaultCACertificates(mNativeHandle);
    }

    public boolean getEnablePeerVerification() throws Exception {
        return this.getEnablePeerVerification(mNativeHandle);
    }

    public boolean IsRSAformatPrivateKey() throws Exception {
        return this.IsRSAformatPrivateKey(mNativeHandle);
    }

    public String getProxyAddress() throws Exception {
        return this.getProxyAddress(mNativeHandle);
    }

    public String getProxyPort() throws Exception {
        return this.getProxyPort(mNativeHandle);
    }

    public void setProxyUsageList(String[] proxyUsageList) throws Exception {
        this.setProxyUsageList(mNativeHandle, proxyUsageList);
    }

    public String[] getProxyUsageList() throws Exception {
        return this.getProxyUsageList(mNativeHandle);
    }

}
