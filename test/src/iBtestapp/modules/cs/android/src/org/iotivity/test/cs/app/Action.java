/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.test.cs.app;

import android.content.Context;
import android.util.Log;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcCloudProvisioning;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.cloud.OcAccountManagerHelper;
import org.iotivity.configuration.IConfiguration;
import org.iotivity.test.cs.app.CSConstants;
import org.iotivity.test.cs.app.OcCloudProvisioningHelper;
import org.iotivity.service.testapp.framework.Base;

import java.io.File;
import java.util.ArrayList;

public class Action extends Base implements IConfiguration {

    private OcAccountManagerHelper    mCloudHelper               = new OcAccountManagerHelper();
    private OcCloudProvisioningHelper mOcCloudProvisioningHelper = new OcCloudProvisioningHelper();
    public OcCloudProvisioning        mOcCloudProvisioning       = new OcCloudProvisioning(
            OcCloudProvisioningHelper.DEFAULT_HOST_IP,
            OcCloudProvisioningHelper.DEFAULT_PORT);
    private OcAccountManager          mAccountManager;
    private Context                   context;

    public Action(Context context1) {
        Log.i(CSConstants.TAG, "Action Contructor called.");
    }

    public void startCloudControlee() {
        showOutPut("Start CS Client");

        context = getContext();
        // create platform config
        mOcCloudProvisioningHelper.copyCborFromAsset(context,
                CSConstants.OIC_CLIENT_CBOR_DB_FILE);
        OcCloudProvisioningHelper.mFilePath = context.getFilesDir().getPath()
                + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(context, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind to all available
                // interfaces
                0, QualityOfService.LOW, OcCloudProvisioningHelper.mFilePath
                        + CSConstants.OIC_CLOUD_CLIENT);

        OcPlatform.Configure(cfg);
        mAccountManager = OcAccountManagerHelper
                .getAccountMangerInstance(TLS.DISABLED);

        OcAccountManagerHelper.init(context);
    }

    public void signUp() {
        showOutPut("signUp");
        mCloudHelper.singUp(mAccountManager, DEFAULT_AUTH_PROVIDER,
                OcAccountManagerHelper.authCode, mCloudHelper);
    }

    public void signIn() {
        showOutPut("signIn");
        mCloudHelper.singIn(mAccountManager, OcAccountManagerHelper.s_CloudUid,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper);
    }

    public void signOut() {
        showOutPut("signOut");
        mCloudHelper.singOut(mAccountManager,
                OcAccountManagerHelper.s_CloudAccesstoken, mCloudHelper);
    }

    public void cloudCertificateIssueRequest() {
        showOutPut("cloudCertificateIssueRequest");
        mOcCloudProvisioningHelper.requestCertificate(mOcCloudProvisioning,
                mOcCloudProvisioningHelper);
    }

    public void cloudGetCRL() {
        showOutPut("cloudGetCRL");
        mOcCloudProvisioningHelper.getCRL(mOcCloudProvisioning,
                mOcCloudProvisioningHelper);
    }

    public void cloudPostCRL() {
        showOutPut("cloudPostCRL");
        ArrayList<String> serialNumbers = new ArrayList<String>();
        serialNumbers.add(CSConstants.CERT_SERIAL_ONE);
        mOcCloudProvisioningHelper.postCRL(mOcCloudProvisioning,
                OcCloudProvisioningHelper.thisUpdate,
                OcCloudProvisioningHelper.nextUpdate, CSConstants.DEFAULT_CRL,
                serialNumbers, mOcCloudProvisioningHelper);
    }

    public void cloudGetAclIdByDevice() {
        showOutPut("cloudGetAclIdByDevice");
        mOcCloudProvisioningHelper.getAclIdByDevice(mOcCloudProvisioning,
                OcCloudProvisioningHelper.DEFAULT_DEVICE_ID,
                mOcCloudProvisioningHelper);
    }

    public void cloudAclIndividualGetInfo() {
        showOutPut("cloudAclIndividualGetInfo");
        mOcCloudProvisioningHelper.getIndividualAclInfo(mOcCloudProvisioning,
                OcCloudProvisioningHelper.DEFAULT_ACL_ID,
                mOcCloudProvisioningHelper);
    }

    public void clearLog() {
        showOutPut("Clearing Log");
        clearOutPut();
    }
}