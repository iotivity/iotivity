/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
package org.iotivity.service.pm;

import android.content.Context;

import org.iotivity.service.testapp.framework.Base;

import android.util.Log;

import org.iotivity.service.pm.PMHelper;
import org.iotivity.service.pm.PMConstants;

import java.io.File;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.LinkedList;
import java.util.List;

import org.iotivity.base.CredType;
import org.iotivity.base.KeySize;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcDirectPairDevice;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPrmType;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OicSecPdAcl;
import org.iotivity.base.OxmType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

public class Action extends Base {

    public static PMHelper mPMHelper = new PMHelper();
    public static OCPlatformHelper mOCPlatformHelper = new OCPlatformHelper();
    public static OcDirectPairDeviceHelper mOcDirectPairDeviceHelper = new OcDirectPairDeviceHelper();
    int unownedDevCount = PMConstants.NUMBER_ZERO;
    List<Light> lights = new LinkedList<>();

    public Action(Context context1) {
        Log.i(PMConstants.TAG, "Action Contructor called.");
    }

    public void createNewLightResource(String resourceUri, String resourceName) {

        Light light = new Light(resourceUri, // URI
                resourceName, // name
                false, // state
                0 // power
        );

        light.setContext(getContext());

        try {
            light.registerResource();
        } catch (OcException e) {
            Log.e(PMConstants.TAG, e.toString());

        }
        lights.add(light);
    }

    public void startJustWorksServer1() {
        showOutPut("Start JustWork Server 1");

        mPMHelper.copyCborFromAsset(getContext(),
                PMConstants.OIC_JWSERVER_CBOR_DB_FILE_1);

        PMHelper.mFilePath = getContext().getFilesDir().getPath() + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(getContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW,
                PMHelper.mFilePath + PMConstants.OIC_JWSERVER_CBOR_DB_FILE_1);
        OcPlatform.Configure(cfg);

        PMHelper.mSqlDbPath = getContext().getFilesDir().getAbsolutePath()
                .replace("files", "databases")
                + File.separator;
        mPMHelper.initOICStack(PMHelper.mSqlDbPath,
                PMConstants.OIC_JWSERVER_CBOR_DB_FILE_1);

    }

    public void startJustWorksServer2() {
        showOutPut("Start JustWork Server 2");

        mPMHelper.copyCborFromAsset(getContext(),
                PMConstants.OIC_JWSERVER_CBOR_DB_FILE_2);

        // create platform config
        PMHelper.mFilePath = getContext().getFilesDir().getPath() + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(getContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW,
                PMHelper.mFilePath + PMConstants.OIC_JWSERVER_CBOR_DB_FILE_2);
        OcPlatform.Configure(cfg);

        PMHelper.mSqlDbPath = getContext().getFilesDir().getAbsolutePath()
                .replace("files", "databases")
                + File.separator;
        mPMHelper.initOICStack(PMHelper.mSqlDbPath,
                PMConstants.OIC_JWSERVER_CBOR_DB_FILE_2);
    }

    public void startRandomPinServer() {
        showOutPut("Start JustWork Server 2");

        mPMHelper.copyCborFromAsset(getContext(),
                PMConstants.OIC_RANDOMPIN_SERVER_CBOR_DB_FILE);

        // create platform config
        PMHelper.mFilePath = getContext().getFilesDir().getPath() + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(getContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW,
                PMHelper.mFilePath + PMConstants.OIC_RANDOMPIN_SERVER_CBOR_DB_FILE);
        OcPlatform.Configure(cfg);

        PMHelper.mSqlDbPath = getContext().getFilesDir().getAbsolutePath()
                .replace("files", "databases")
                + File.separator;
        mPMHelper.initOICStack(PMHelper.mSqlDbPath,
                PMConstants.OIC_RANDOMPIN_SERVER_CBOR_DB_FILE);
        mPMHelper.setDisplayPinListener(PMHelper.displayPinListener);
    }

    public void startPmClient() {
        showOutPut("Start PM Client");

        showOutPut("start Client");

        mPMHelper.copyCborFromAsset(getContext(),
                PMConstants.OIC_CLIENT_CBOR_DB_FILE);

        // create platform config
        PMHelper.mFilePath = getContext().getFilesDir().getPath() + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(getContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW,
                PMHelper.mFilePath + PMConstants.OIC_CLIENT_CBOR_DB_FILE);
        OcPlatform.Configure(cfg);

        PMHelper.mSqlDbPath = getContext().getFilesDir().getAbsolutePath()
                .replace("files", "databases")
                + File.separator;
        mPMHelper.initOICStack(PMHelper.mSqlDbPath, PMConstants.OIC_SQL_DB_FILE);
    }

    // For Debug Purpose
    public void discoverUnowendDev() {
        showOutPut("Discovering Owned Dev");
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
    }

    // For Debug Purpose
    public void discoverOwendDev() {
        showOutPut("Discovering Unowned Dev");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
    }

    public void justworksOwnerShipTransfer() {
        showOutPut("JustWorks Ownership Transfer");
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
        mPMHelper.setOwnershipTransferCBdata(OxmType.OIC_JUST_WORKS, null);
        mPMHelper.doOwnershipTransfer(mPMHelper);
    }

    public void randomPinOwnerShipTransfer() {
        showOutPut("RandomPin Ownership Transfer");
        mPMHelper.discoverUnownedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
        mPMHelper.setOwnershipTransferCBdata(OxmType.OIC_RANDOM_DEVICE_PIN,
                PMHelper.pinCallbackListener);
        mPMHelper.doOwnershipTransfer(mPMHelper);
    }

    public void provisionACL() {
        showOutPut("Provisioning ACL");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        List<OicSecAcl> aclArrayList = PMHelper.createAcl(ownedDevice01);

        mPMHelper.provisionACL(ownedDevice01, aclArrayList.get(0),
                PMHelper.provisionAclListener);
    }

    public void provisionCredential() {
        showOutPut("Provisioning Credential");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        mPMHelper.provisionCredentials(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, ownedDevice02,
                PMHelper.provisionCredentialsListener);
    }

    public void provisionDirectPairing() {
        showOutPut("Provisioning DP");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);

        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        List<OcPrmType> prmTypes = new ArrayList<OcPrmType>();
        prmTypes.add(OcPrmType.DP_PRE_CONFIGURED);
        boolean edp = true;

        List<String> resources = new ArrayList<String>();
        List<String> periods = new ArrayList<String>();
        List<String> recurrences = new ArrayList<String>();
        resources.add(PMConstants.DEFAULT_RESOURCES);
        OicSecPdAcl pdAcl = new OicSecPdAcl(recurrences, periods,
                PMConstants.DEFAULT_PERMISSION, resources);
        OicSecPdAcl[] oicSecPdAcls = new OicSecPdAcl[1];
        oicSecPdAcls[0] = pdAcl;

        mPMHelper.doProvisionDirectPairing(ownedDevice01,
                PMConstants.DP_PRE_CONFIGURED_PIN, oicSecPdAcls, prmTypes, edp,
                mPMHelper);
    }

    public void provisionPairwiseDev() {
        showOutPut("Provisioning Pairwise Devs");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        List<OicSecAcl> aclArrayList = PMHelper.createAcl(ownedDevice01,
                ownedDevice02);
        mPMHelper.provisionPairwiseDevices(
                EnumSet.of(CredType.SYMMETRIC_PAIR_WISE_KEY),
                KeySize.OWNER_PSK_LENGTH_128, ownedDevice01, aclArrayList.get(0),
                ownedDevice02, aclArrayList.get(1), mPMHelper);
    }

    public void getLinkedDev() {
        showOutPut("Get Linked Devices");
        mPMHelper.discoverUnownedDevices(10);
        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        mPMHelper.getLinkedDevices(ownedDevice01);
    }

    public void unlinkDev() {
        showOutPut("Unlink Devices");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);

        mPMHelper.unlinkDevices(ownedDevice01, ownedDevice02,
                PMHelper.unlinkDevicesListener);
    }

    public void removeDev() {
        showOutPut("Removing Device");
        mPMHelper.discoverOwnedDevices(PMConstants.DISCOVERY_TIMEOUT_10);
        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        mPMHelper.removeDevice(ownedDevice01, PMConstants.DISCOVERY_TIMEOUT_10,
                PMHelper.removeDeviceListener);
    }

    public void startDpClient() {
        showOutPut("Start DP Client");
        mOCPlatformHelper.copyCborFromAsset(getContext(),
                PMConstants.OIC_DP_CLIENT_CBOR_DB_FILE);

        // create platform config
        OCPlatformHelper.mFilePath = getContext().getFilesDir().getPath() + "/"; // data/data/<package>/files/
        PlatformConfig cfg = new PlatformConfig(getContext(), ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW,
                PMHelper.mFilePath + PMConstants.OIC_DP_CLIENT_CBOR_DB_FILE);
        OcPlatform.Configure(cfg);

        OCPlatformHelper.mSqlDbPath = getContext().getFilesDir().getAbsolutePath()
                .replace("files", "databases")
                + File.separator;
        mOCPlatformHelper.initOICStack(PMHelper.mSqlDbPath,
                PMConstants.OIC_DP_CLIENT_CBOR_DB_FILE);
    }

    public void discoverDpDevs() {
        showOutPut("Discover DP Devices");
        mOCPlatformHelper.findDirectPairingDevices(
                PMConstants.DISCOVERY_TIMEOUT_10, OCPlatformHelper.finddirectPairing);
    }

    public void doDirectPairing() {
        showOutPut("Do Direct Pairing");

        OcDirectPairDevice peer = OCPlatformHelper.discoveredPairableDevices
                .get(PMConstants.DEVICE_INDEX_ZERO);
        mOCPlatformHelper.doDirectPairing(peer, OcPrmType.DP_PRE_CONFIGURED,
                PMConstants.DP_PRE_CONFIGURED_PIN, OCPlatformHelper.doDPListener);
    }

    public void getDirectPairedDevices() {
        showOutPut("Get Direct Paired Devices");
        OcDirectPairDevice peer = OCPlatformHelper.discoveredPairableDevices
                .get(PMConstants.DEVICE_INDEX_ZERO);
        mOcDirectPairDeviceHelper.getDirectPairedDevices(peer,
                mOcDirectPairDeviceHelper);
    }

    public void clearLog() {
        showOutPut("Clearing Log");
        clearOutPut();
    }
}