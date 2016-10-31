/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************/

package org.iotivity.service.pm.test.btc;

import java.util.ArrayList;

import android.test.InstrumentationTestCase;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcCloudProvisioning;
import org.iotivity.service.pm.test.helper.OcCloudProvisioningHelper;
import org.iotivity.service.pm.test.helper.OcDirectPairDeviceHelper;
import org.iotivity.service.pm.test.helper.PMConstants;

public class PMOcCloudProvisioningTest extends InstrumentationTestCase {

    public OcCloudProvisioningHelper mOcCloudProvisioningHelper = new OcCloudProvisioningHelper();
    public OcCloudProvisioning mOcCloudProvisioning = new OcCloudProvisioning(
            OcCloudProvisioningHelper.DEFAULT_HOST_IP,
            OcCloudProvisioningHelper.DEFAULT_PORT);

    protected void setUp() throws Exception {
        super.setUp();

        PMConstants.mErrorMessage = PMConstants.EMPTY_STRING;

        // create platform config
        mOcCloudProvisioningHelper.copyCborFromAsset(
                getInstrumentation().getContext(),
                PMConstants.OIC_CLOUD_CLIENT);
        OcCloudProvisioningHelper.mFilePath = getInstrumentation().getContext()
                .getFilesDir().getPath() + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind to all available
                // interfaces
                0, QualityOfService.LOW, OcDirectPairDeviceHelper.mFilePath
                + PMConstants.OIC_CLOUD_CLIENT);

        OcPlatform.Configure(cfg);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call requestCertificate negatively with
     *            certificateIssueRequestListener as null
     * @target public native void requestCertificate(RequestCertificateListener
     *         certificateIssueRequestListener) throws OcException
     * @test_data certificateIssueRequestListener as null
     * @pre_condition none
     * @procedure Call requestCertificate() API
     * @post_condition none
     * @expected API should throw INVALID_CALLBACK error
     */
    public void testRequestCertificateCb_NV_N() {
        assertFalse(PMConstants.mErrorMessage, mOcCloudProvisioningHelper
                .requestCertificate(mOcCloudProvisioning, null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call getIndividualAclInfo negatively with
     *            cloudAclIndividualGetInfoHandler as null
     * @target public native void getIndividualAclInfo(String aclId,
     *         GetIndividualAclInfoListener cloudAclIndividualGetInfoHandler)
     *         throws OcException
     * @test_data cloudAclIndividualGetInfoHandler as null
     * @pre_condition none
     * @procedure Call getIndividualAclInfo() API
     * @post_condition none
     * @expected API should throw INVALID_CALLBACK error
     */
    public void testGetIndividualAclInfoCb_NV_N() {
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.getIndividualAclInfo(
                        mOcCloudProvisioning,
                        OcCloudProvisioningHelper.DEFAULT_ACL_ID, null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call getIndividualAclInfo negatively with aclId as null
     * @target public native void getIndividualAclInfo(String aclId,
     *         GetIndividualAclInfoListener cloudAclIndividualGetInfoHandler)
     *         throws OcException
     * @test_data aclId as null
     * @pre_condition none
     * @procedure Call getIndividualAclInfo() API
     * @post_condition none
     * @expected API should throw INVALID_PARAM error
     */
    public void testGetIndividualAclInfoAclId_NV_N() {
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.getIndividualAclInfo(
                        mOcCloudProvisioning, null,
                        mOcCloudProvisioningHelper));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call getAclIdByDevice negatively with
     *            cloudAclIdGetByDeviceHandler as null
     * @target public native void getAclIdByDevice(String deviceId,
     *         GetAclIdByDeviceListener cloudAclIdGetByDeviceHandler) throws
     *         OcException
     * @test_data cloudAclIdGetByDeviceHandler as null
     * @pre_condition none
     * @procedure Call getAclIdByDevice() API
     * @post_condition none
     * @expected API should throw INVALID_CALLBACK error
     */
    public void testGetAclIdByDeviceCb_NV_N() {
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.getAclIdByDevice(
                        mOcCloudProvisioning,
                        OcCloudProvisioningHelper.DEFAULT_DEVICE_ID, null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call getAclIdByDevice negatively with deviceId as null
     * @target public native void getAclIdByDevice(String deviceId,
     *         GetAclIdByDeviceListener cloudAclIdGetByDeviceHandler) throws
     *         OcException
     * @test_data deviceId as null
     * @pre_condition none
     * @procedure Call getAclIdByDevice() API
     * @post_condition none
     * @expected API should throw INVALID_PARAM error
     */
    public void testGetAclIdByDeviceDev_NV_N() {
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.getAclIdByDevice(
                        mOcCloudProvisioning, null,
                        mOcCloudProvisioningHelper));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_PARAM,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call getCRL negatively with cloudGetCRLHandler as null
     * @target public native void getCRL(GetCRLListener cloudGetCRLHandler)
     *         throws OcException
     * @test_data cloudGetCRLHandler as null
     * @pre_condition none
     * @procedure Call getCRL() API
     * @post_condition none
     * @expected API should throw INVALID_CALLBACK error
     */
    public void testGetCrlCb_NV_N() {
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.getCRL(mOcCloudProvisioning, null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call postCRL negatively with cloudPostCRLHandler as null
     * @target public void postCRL(String thisUpdate, String nextUpdate, String
     *         crl, ArrayList<String> serialNumbers, PostCRLListener
     *         cloudPostCRLHandler) throws OcException
     * @test_data cloudPostCRLHandler as null
     * @pre_condition none
     * @procedure Call postCRL() API
     * @post_condition none
     * @expected API should throw INVALID_CALLBACK error
     */
    public void testPostCrlCb_NV_N() {
        ArrayList<String> serialNumbers = new ArrayList<String>();
        serialNumbers.add(PMConstants.CERT_SERIAL_ONE);
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.postCRL(mOcCloudProvisioning,
                        OcCloudProvisioningHelper.thisUpdate,
                        OcCloudProvisioningHelper.nextUpdate,
                        PMConstants.DEFAULT_CRL, serialNumbers, null));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_CALLBACK,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call postCRL negatively with thisUpdate as null
     * @target public void postCRL(String thisUpdate, String nextUpdate, String
     *         crl, ArrayList<String> serialNumbers, PostCRLListener
     *         cloudPostCRLHandler) throws OcException
     * @test_data thisUpdate as null
     * @pre_condition none
     * @procedure Call postCRL() API
     * @post_condition none
     * @expected API should throw INVALID_PARAM error
     */
    public void testPostCrlThisUpdate_NV_N() {
        ArrayList<String> serialNumbers = new ArrayList<String>();
        serialNumbers.add(PMConstants.CERT_SERIAL_ONE);
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.postCRL(mOcCloudProvisioning, null,
                        OcCloudProvisioningHelper.nextUpdate,
                        PMConstants.DEFAULT_CRL, serialNumbers,
                        mOcCloudProvisioningHelper));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_PARAM,
                PMConstants.mErrorMessage);
    }

    /**
     * @since 2016-10-20
     * @see none
     * @objective Call postCRL negatively with nextUpdate as null
     * @target public void postCRL(String thisUpdate, String nextUpdate, String
     *         crl, ArrayList<String> serialNumbers, PostCRLListener
     *         cloudPostCRLHandler) throws OcException
     * @test_data nextUpdate as null
     * @pre_condition none
     * @procedure Call postCRL() API
     * @post_condition none
     * @expected API should throw INVALID_PARAM error
     */
    public void testPostCrlNextUpdate_NV_N() {
        ArrayList<String> serialNumbers = new ArrayList<String>();
        serialNumbers.add(PMConstants.CERT_SERIAL_ONE);
        assertFalse(PMConstants.mErrorMessage,
                mOcCloudProvisioningHelper.postCRL(mOcCloudProvisioning,
                        OcCloudProvisioningHelper.thisUpdate, null,
                        PMConstants.DEFAULT_CRL, serialNumbers,
                        mOcCloudProvisioningHelper));
        assertEquals(PMConstants.WRONG_ERROR_CODE, PMConstants.ERROR_INVALID_PARAM,
                PMConstants.mErrorMessage);
    }

}
