/*
 *******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
import java.util.Arrays;
import java.util.Map;
import java.util.EnumSet;

/**
 * OcProvisionig represents functions corresponding to the provisioing of
 * resources.
 */
public class OcProvisioning {

    /**
     * Method to Initialize Provisioning Manager.This will load the provisioning
     * Manager's persistent database.
     *
     * @param dbPath     dbPath file path of the sqlite3 db.
     *
     * @throws OcException Indicate initialization failure.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static native void provisionInit(String dbPath) throws OcException;

    /**
     * Method is used by provisioning manager to close provisioning database.
     *
     * @throws OcException Indicate failure.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static native void provisionClose() throws OcException;

    /**
     * Method to Discover un-owned devices in its subnet.Un-owned devices need
     * to be owned by calling ownershipTransferCBdata.
     *
     * @param timeout     Timeout in sec.Time to listen for responses before
     *                    returining the Array.
     *
     * @return            Array of OcSecureResource class objects.
     *
     * @throws OcException Indicate discovery failure.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public  static List<OcSecureResource> discoverUnownedDevices(int timeout) throws OcException {
        return Arrays.asList(OcProvisioning.discoverUnownedDevices1(timeout));
    }
    private static native OcSecureResource[] discoverUnownedDevices1(int timeout) throws OcException;

    /**
     * Method to Discover owned devices in its subnet.
     *
     * @param timeout     Timeout in sec.Time to listen for responses before
     *                    returining the Array.
     *
     * @return            Array of OcSecureResource class objects.
     *
     * @throws OcException Indicate discovery failure.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static List<OcSecureResource> discoverOwnedDevices(int timeout) throws OcException {
        return Arrays.asList(OcProvisioning.discoverOwnedDevices1(timeout));
    }
    private static native OcSecureResource[] discoverOwnedDevices1(int timeout) throws OcException;

    /**
     *  API for registering Ownership transfer methods for a particular
     *  transfer Type
     *
     * @param type     OxmType ownership transfer type.
     * @param pinCallbackListener Pin callback Listener.
     * @deprecated use {@link #setOwnershipTransferCBdata} instead.
     * @throws OcException Indicate failure registering ownership transfer.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static void SetownershipTransferCBdata(OxmType type,
            PinCallbackListener pinCallbackListener) throws OcException {
        OcProvisioning.ownershipTransferCBdata(type.getValue(), pinCallbackListener);
    }

    /**
     *  API for registering Ownership transfer methods for a particular
     *  transfer Type
     *
     * @param type     OxmType ownership transfer type.
     * @param pinCallbackListener Pin callback Listener.
     *
     * @throws OcException Indicate failure registering ownership transfer.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static void setOwnershipTransferCBdata(OxmType type,
            PinCallbackListener pinCallbackListener) throws OcException {
        OcProvisioning.ownershipTransferCBdata(type.getValue(), pinCallbackListener);
    }
    private  static native void ownershipTransferCBdata(int oxmType,  PinCallbackListener pinCallbackListener);

    public static interface PinCallbackListener {
        public String pinCallbackListener();
    }


    /**
     * Method to Discover devices which are MOT (Multiple owner transfer) enabled
     * and unowned.
     *
     * @param timeout     Timeout in sec.Time to listen for responses before
     *                    returining the Array.
     * @return            Array of OcSecureResource class objects.
     * @throws OcException Indicate failure discovering devices which are MOT enabled
     */
    public static List<OcSecureResource> discoverMOTEnabledDevices(int timeout)
        throws OcException {
        return Arrays.asList(OcProvisioning.discoverMOTEnabledDevices1(timeout));
    }
    private static native OcSecureResource[] discoverMOTEnabledDevices1(int timeout)
        throws OcException;

    /**
     * Method to Discover devices which are MOT (Multiple owner transfer) enabled and Owned.
     *
     * @param timeout     Timeout in sec.Time to listen for responses before
     *                    returining the Array.
     * @return            Array of OcSecureResource class objects.
     * @throws OcException Indicate failure discovering MOT enabled owned devices
     */
    public static List<OcSecureResource> discoverMOTEnabledOwnedDevices(int timeout)
        throws OcException {
        return Arrays.asList(OcProvisioning.discoverMOTEnabledOwnedDevices1(timeout));
    }
    private static native OcSecureResource[] discoverMOTEnabledOwnedDevices1(int timeout)
        throws OcException;

    /**
     * Server API to set Callback for Displaying stack generated PIN.
     *
     * @param displayPinListener Pin callback Listener to be registered.
     *
     * @throws OcException Indicate failure setting the pin listener callback.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static native void setDisplayPinListener(DisplayPinListener displayPinListener)
        throws OcException;

    public static interface DisplayPinListener {
        public void displayPinListener(String Pin);
    }

    /**
     * API to Set callback for displaying verifNum in verified Just-Works.
     *
     * @param displayNumListener callback Listener to be registered for
     *                          displaying VerifyNUm.
     *
     * @throws OcException Indicate failure setting the DisplayNumListener.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static native void setDisplayNumListener(
            DisplayNumListener displayNumListener) throws OcException;

    public static interface DisplayNumListener {
        public int displayNumListener(String verifyNum);
    }

    /**
     * API to unregister DisplayNumListener Listener
     *
     *@return 0 on success, 1 on failure
     *
     *@throws OcException Indicate failure to unregister the DisplayNumListener Listener.
     *                    Use OcException.GetErrorCode() for more details.
     */
    public static native int unsetDisplayNumListener() throws OcException;

    /**
     * API to Set callback for getting user confirmation in verified
     * Just-Works
     *
     *@param confirmNumListener callback Listener to be registered for getting user confirmation.
     *
     *@throws OcException Indicate failure setting the confirmation number Just-Works listener.
     *                    Use OcException.GetErrorCode() for more details.
     */
    public static native void setConfirmNumListener(ConfirmNumListener
            confirmNumListener) throws OcException;

    public static interface ConfirmNumListener {
        public int confirmNumListener();
    }

    /**
     * API to unregister ConfirmMutualVerifyNum Listener
     *
     *@return  0 on success, 1 on failure
     *
     *@throws OcException Indicate failure unregistering the ConfirmMutualVerifyNum Listener
     *                    Use OcException.GetErrorCode() for more details.
     */
    public static native int unsetConfirmNumListener() throws OcException;

    /**
     * API to set options for Mutual Verified Just-works
     * Default is  for both screen PIN display and get user confirmation.
     *
     * @param optionMask Verification Method Options
     *
     * @return 0 on success
     *
     * @throws OcException Indicate failure to set the Mutual Verified Just-works options.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static int setMVJustWorksOptions(EnumSet<MVJustWorksOptionMask> optionMask) throws OcException {

        int optionMaskInt = 0;

        for (MVJustWorksOptionMask ops : MVJustWorksOptionMask.values()) {
            if (optionMask.contains(ops))
                optionMaskInt |= ops.getValue();
        }
        return setMVJustWorksOptions0(optionMaskInt);
    }

    private static native int setMVJustWorksOptions0(int optionsMask) throws OcException;

    /**
     * Method to get Array of owned and un-owned devices in the current subnet.
     *
     * @param timeout    timeout in sec for the API to return.
     * @return           Array of OcSecureResource class objects.
     *                   be provisioned.
     * @throws OcException Indicate failure to get array of devices.
     *                     Use OcException.GetErrorCode() for more details.
     * 
     */
    public static List<OcSecureResource> getDeviceStatusList(int timeout) throws OcException {
        return Arrays.asList(OcProvisioning.getDeviceStatusList1(timeout));
    }
    private static native OcSecureResource[] getDeviceStatusList1(int timeout) throws OcException;

    /**
     * Method to save the Trust certificate chain to SVR.
     *
     * @param trustCertChain Trust certificate chain to be saved in Cred of SVR.
     * @param encodingType Encoding type of trust certificate chain to be saved in Cred of SVR
     *
     * @return CredId of saved trust certificate chain in Cred of SVR.
     *
     * @throws OcException Indicate failure to save the trust certificate chain.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public static int saveTrustCertChain(byte[] trustCertChain, EncodingType encodingType) throws OcException {
        return saveTrustCertChain1(trustCertChain, encodingType.getValue());
    }
    private static native int saveTrustCertChain1(byte[] trustCertChain, int encodingType)
        throws OcException;

    /**
     *  Method to save pin type.
     *
     *  @param pinSize Byte Len of Random pin.
     *  @param pinType Enumset of pin, see PinType for enums
     *  @throws OcException Indicate failure saving pin type
     */
    public static void setPinType(int pinSize, EnumSet<PinType>  pinType) throws OcException {

        int pinTypeInt = 0;

        for (PinType ops : PinType.values()) {
            if (pinType.contains(ops))
                pinTypeInt |= ops.getValue();
        }
        setPinType0(pinSize, pinTypeInt);
    }
    private static native void setPinType0(int pinSize, int pinType) throws OcException;

    /**
     * API to save ACL, having multiple ACE's
     *
     *@param acl object
     *@throws OcException Indicate failure saving the ACL
     */
    public static native void saveACL(Object acl) throws OcException;

    /**
     * API to do self ownership transfer.
     *
     *@throws OcException Indicate failure doing the self ownership transfer
     */
    public static native void doSelfOwnershiptransfer() throws OcException;
    
    /**
     *  Method to save the seed value to generate device UUID
     *
     *  @param seed   buffer of seed value
     *  @throws OcException Indicate falure setting the device UUID seed
     *  @return -1 if TLS not enabled return 0 otherwise
     */
    public static int setDeviceIdSeed(byte[] seed) throws OcException {
        return setDeviceIdSeed1(seed);
    }
    private static native int setDeviceIdSeed1(byte[] seed)
        throws OcException;
}
