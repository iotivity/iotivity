/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.List;
import java.util.EnumSet;
import java.util.Map;
import java.util.Arrays;
import java.util.ArrayList;

public class OcDirectPairDevice {


    private long mNativeHandle;
    private String mdeviceID;
    private String mHost;

    private OcDirectPairDevice(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
    }

    public OcDirectPairDevice(String deviceID)
    {
        this.mdeviceID = deviceID;
    }

    public String getDevId()
    {
        return this.mdeviceID;
    }

    public native String getHost();

    public List<Integer> getPairingMethodList() {
        int [] intList = this.getPairingMethods();
        List<Integer> prmList = new ArrayList<Integer>();
        for (int i = 0; i < intList.length; i++)
        {
            prmList.add(intList[i]);
        }
        return  prmList;
    }

    private native int[] getPairingMethods();

    /**
     * Method to get the connectivity type of this resource
     *
     * @return EnumSet<OcConnectivityType></OcConnectivityType> connectivity type set
     */
    public EnumSet<OcConnectivityType> getConnectivityTypeSet() {
        return OcConnectivityType.convertToEnumSet(
                this.getConnectivityTypeN()
        );
    }

    private native int getConnectivityTypeN();


    /**
     *  Method to get list of all paired devices for a given device.
     *
     *  @param GetDirectPairedListener Callback function, which will receive the list of direct paired devices.
     *  @throws OcException
     */

    public native void getDirectPairedDevices(GetDirectPairedListener getDirectPairedListener) throws OcException;

    public interface GetDirectPairedListener {
        public void getDirectPairedListener(List<String> ocPairedDeviceList);
    }

    /**
     *  Method to perform direct pairing between two devices.
     *
     *  @param DirectPairingListener Callback function, which will be called after
     *                                      completion of direct pairing.
     *  @throws OcException
     */
    public native void doDirectPairing(OcDirectPairDevice peer,
            OcPrmType pmSel, String pinNumber,
            DirectPairingListener directPairingListener) throws OcException;


    public interface DirectPairingListener {
        public void directPairingListener(String devId, int result);
    }

    @Override
    public String toString() {
        return this.mdeviceID;
    }
}
