/*
 *******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.List;
import java.util.EnumSet;
import java.util.Map;
import java.util.ArrayList;



/**
 * OcCloudProvisioning class is responsible for providing methods for cloud
 * provisioning.
 */
public class  OcCloudProvisioning {

    private long    mNativeHandle;
    private String mIp;
    private int mPort;

   /**
    * API to construct the CloudProvisioning
    *
    * @param ip address of the cloud server
    * @param port port of the cloud server
    */
    public OcCloudProvisioning(String ip, int port) {
        this.mIp = ip;
        this.mPort = port;
    }

    public String getIP() {
        return mIp;
    }

    public int getPort() {
        return mPort;
    }

    private void setNativeHandle(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
    }

    public interface RequestCertificateListener {
        public void requestCertificateListener(int result);
    }

    public interface GetAclIdByDeviceListener {
        public void getAclIdByDeviceListener(int result, String aclId);
    }

    public interface CreateAclIdListener {
        public void createAclIdListener(int result, String aclId);
    }

    public interface GetIndividualAclInfoListener {
        public void getIndividualAclInfoListener(int result);
    }

    public interface GetCRLListener {
        public void getCRLListener(int result);
    }

    public interface PostCRLListener  {
        public void postCRLListener(int result);
    }

    public interface UpdateIndividualACLListener  {
        public void updateIndividualACLListener(int result);
    }

   /**
    * Method to Request a certificate from the cloud
    * @param certificateIssueRequestListener function called by the stack on completion of request.
    * @throws OcException Indicates failure requesting the certificate from the cloud.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public native void requestCertificate(
            RequestCertificateListener certificateIssueRequestListener) throws OcException;

   /**
    * Method to get ACL ID for the device
    * @param deviceId device ID for which the Acl ID is requested
    * @param cloudAclIdGetByDeviceHandler function called by the stack on completion of request.
    * @throws OcException Indicates failure getting ACL ID for the device.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public native void getAclIdByDevice(String deviceId,
            GetAclIdByDeviceListener cloudAclIdGetByDeviceHandler) throws OcException;

   /**
    * Method to create ACL ID
    * @param ownerId owner ID for which the Acl ID is created
    * @param deviceId device ID for which the Acl ID is requested
    * @param cloudcreateAclId function called by the stack on completion of request.
    * @throws OcException Indicates failure getting ACL ID for the device.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public native void createAclId(String ownerId, String deviceId,
            CreateAclIdListener cloudcreateAclId) throws OcException;

   /**
    * Method to get ACL information about the given Acl ID
    * @param aclId ACL ID for which the Acl information is requested
    * @param cloudAclIndividualGetInfoHandler function called by the stack on completion of request.
    * @throws OcException Indicates failure to get ACL information.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public native void getIndividualAclInfo(String aclId,
            GetIndividualAclInfoListener cloudAclIndividualGetInfoHandler) throws OcException;

    /**
    * Method to update Individual ACL info
    * @param aclId ACL ID
    * @param cloudAces List of cloud Aces for updation.
    * @param updateIndividualACLHandler function called by the stack on completion of request.
    * @throws OcException Indicates failure to get ACL information.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public void updateIndividualACL(String aclId, List<OicSecCloudAce> cloudAces,
            UpdateIndividualACLListener updateIndividualACLHandler) throws OcException
    {
        this.updateIndividualACL0(aclId,
                cloudAces.toArray(new OicSecCloudAce[cloudAces.size()]), updateIndividualACLHandler);
    }

    public native void updateIndividualACL0(String aclId, OicSecCloudAce[] aces,
            UpdateIndividualACLListener updateIndividualACLHandler) throws OcException;

   /**
    * Method to get certificate revocation list
    * @param cloudGetCRLHandler function called by the stack on completion of request.
    * @throws OcException Indicates failure to get certificate revocation list.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public native void getCRL(GetCRLListener cloudGetCRLHandler)
            throws OcException;

   /**
    * Method to post the  certificate revocation list to cloud
    * @param thisUpdate thisUpdate [mandatory param].
    * @param nextUpdate nextUpdate [mandatory param].
    * @param crl revocation list [optional].
    * @param serialNumbers [optional].
    * @param cloudPostCRLHandler function called by the stack on completion of request.
    * @throws OcException Indicates failure posting the certificate revocation list to cloud.
    *                     Use OcException.GetErrorCode() for more details.
    */
    public void postCRL(String thisUpdate, String nextUpdate, String crl, ArrayList<String> serialNumbers,
                                    PostCRLListener cloudPostCRLHandler) throws OcException
    {
        String[] serialNums = new String[serialNumbers.size()];
        serialNums = serialNumbers.toArray(serialNums);
        this.postCRL0(thisUpdate, nextUpdate, crl, serialNums, cloudPostCRLHandler);
    }
    public native void postCRL0(String thisUpdate, String nextUpdate, String crl, String[] serialNumbers,
                                    PostCRLListener cloudPostCRLHandler) throws OcException;
}

