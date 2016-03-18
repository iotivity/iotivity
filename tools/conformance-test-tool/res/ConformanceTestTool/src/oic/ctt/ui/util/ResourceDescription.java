//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

package oic.ctt.ui.util;

import java.util.ArrayList;
import java.util.Map;

public class ResourceDescription {
    private String                         href;
    private String                         rt;
    private int                            dis;
    private int                            obs;
    private ArrayList<String>              ifList;
    private ArrayList<Boolean>             ccCsRUcUpD;
    private ArrayList<Map<String, String>> rep;
    private String                         payloadType;
    private String                         dutType;
    static StringBuilder                   errorMessage;

    /**
     * ResourceDescription Constructor
     */
    public ResourceDescription() {
        System.out.println("ResourceDescription constructor called.");

        errorMessage = new StringBuilder("");
    }

    /**
     * A method to get the Resource URI
     * 
     * @see setHref()
     * @return The Resource URI if available, empty otherwise
     * @note Please Set URI using setHref() method first.
     */
    public String getHref() {
        if (href == null)
            return "";

        return href;
    }

    /**
     * A method to set the Resource URI
     * 
     * @see getHref()
     * @param href
     *            Resource URI as String
     */
    public void setHref(String href) {
        if (href.isEmpty())
            errorMessage.append("Resource URI should not be empty.\n");
        else
            this.href = href;
    }

    /**
     * A method to get the Resource Type
     * 
     * @see setRt()
     * @return The Resource Type if available, empty otherwise
     * @note Please Set Resource Type using setRt() method first.
     */
    public String getRt() {
        if (rt == null)
            return "";

        return rt;
    }

    /**
     * A method to set the Resource Type
     * 
     * @see getRt()
     * @param rt
     *            Resource Type as String
     */
    public void setRt(String rt) {
        if (rt.isEmpty())
            errorMessage.append("Resource Type should not be empty.\n");
        else
            this.rt = rt;
    }

    /**
     * A method to get the Resource Discoverable status
     * 
     * @see setDis()
     * @return The Resource Discoverable status as integer
     * @note Please Set Resource Discoverable status using setDis() method
     *       first.
     */
    public int getDis() {
        return dis;
    }

    /**
     * A method to set the Resource Discoverable status
     * 
     * @see getDis()
     * @param dis
     *            Resource Discoverable status as integer
     */
    public void setDis(int dis) {
        if (dis == 0 || dis == 1) {
            this.dis = dis;
        } else {
            errorMessage.append("Discoverable : Must be 0 or 1\n");
        }
    }

    /**
     * A method to get the Resource Observable status
     * 
     * @see setObs()
     * @return The Resource Observable status as integer
     * @note Please Set Resource Observable status using setObs() method first.
     */
    public int getObs() {
        return obs;
    }

    /**
     * A method to set the Resource Observable status
     * 
     * @see getObs()
     * @param obs
     *            Resource Observable status as integer
     */
    public void setObs(int obs) {
        if (obs == 0 || obs == 1) {
            this.obs = obs;
        } else {
            errorMessage.append("Observable : Must be 0 or 1\n");
        }
    }

    /**
     * A method to get the Resource Interfaces
     * 
     * @see setIf()
     * @return Resource interface list if available, empty list otherwise
     * @note Please Set interface list using setIf() method first.
     */
    public ArrayList<String> getIfList() {
        if (ifList == null)
            return new ArrayList<>();

        return ifList;
    }

    /**
     * A method to set the Resource Interface list
     * 
     * @see getIfList()
     * @param interfaceList
     *            Resource Interface List
     */
    public void setIf(ArrayList<String> interfaceList) {
        ifList = new ArrayList<>();

        if (interfaceList == null) {
            errorMessage.append("Interface List should not be null.\n");
            return;
        }

        if (interfaceList.size() == 0) {
            errorMessage.append("Interface List should not be empty.\n");
        } else {
            int emptyStringSize = 0;

            for (String ifValue : interfaceList) {
                if (!ifValue.isEmpty()) {
                    ifList.add(ifValue);
                } else {
                    emptyStringSize++;
                }
            }

            if (emptyStringSize == interfaceList.size()) {
                errorMessage.append("Value of Interface List is empty.\n");
            }
        }
    }

    /**
     * A method to get the Resource Cc, Cs, R, Uc, Up D as list
     * 
     * @see setCcCsRUcUpD()
     * @return Resource Cc, Cs, R, Uc, Up D as list if available, empty list
     *         otherwise
     * @note Please Set ccCsRUcUpD using setCcCsRUcUpD() method first.
     */
    public ArrayList<Boolean> getCcCsRUcUpD() {
        if (ccCsRUcUpD == null)
            return new ArrayList<Boolean>();

        return ccCsRUcUpD;
    }

    /**
     * A method to set the Resource ccCsRUcUpD list
     * 
     * @see getCcCsRUcUpD()
     * @param ccCsRUcUpDList
     *            Resource Cc, Cs, R, Uc, Up, D as List
     */
    public void setCcCsRUcUpD(ArrayList<Boolean> ccCsRUcUpDList) {
        ccCsRUcUpD = new ArrayList<Boolean>();

        if (ccCsRUcUpDList == null) {
            errorMessage.append("CcCsRUcUpD List should not be null.\n");
        }

        if (ccCsRUcUpDList.size() == 0)
            errorMessage.append("CcCsRUcUpD List should not be empty.\n");
        else {
            ccCsRUcUpD = ccCsRUcUpDList;
        }
    }

    /**
     * A method to get Resource Representation
     * 
     * @see setRep()
     * @return Resource Representation list if available, empty list otherwise
     * @note Please Set Resource Representation using setRep() method first.
     */
    public ArrayList<Map<String, String>> getRep() {
        if (rep == null)
            return new ArrayList<Map<String, String>>();

        return rep;
    }

    /**
     * A method to set the Representation map list
     * 
     * @see getRep()
     * @param reps
     *            Resource Representation map list
     */
    public void setRep(ArrayList<Map<String, String>> reps) {
        rep = new ArrayList<Map<String, String>>();

        if (reps == null) {
            errorMessage.append("Representation List should not be null.\n");
            return;
        }

        if (reps.size() == 0) {
            errorMessage.append("Representation List should not be empty.\n");
        } else {
            int mapCount = 0;

            for (Map<String, String> map : reps) {
                if (map.containsKey(DUTDescriptorUtil.KEY)
                        && map.containsKey(DUTDescriptorUtil.TYPE)
                        && map.containsKey(DUTDescriptorUtil.TESTDATA)
                        && map.containsKey(DUTDescriptorUtil.TESTDATA_2)
                        && map.containsKey(DUTDescriptorUtil.ACCESSMODE)) {
                    if (map.get(DUTDescriptorUtil.KEY).isEmpty()
                            && map.get(DUTDescriptorUtil.TYPE).isEmpty()
                            && map.get(DUTDescriptorUtil.TESTDATA).isEmpty()
                            && map.get(DUTDescriptorUtil.TESTDATA_2).isEmpty()
                            && map.get(DUTDescriptorUtil.ACCESSMODE).isEmpty()) {
                        mapCount++;
                    } else if (map.get(DUTDescriptorUtil.KEY).isEmpty()
                            || map.get(DUTDescriptorUtil.TYPE).isEmpty()
                            || map.get(DUTDescriptorUtil.TESTDATA).isEmpty()
                            || map.get(DUTDescriptorUtil.TESTDATA_2).isEmpty()
                            || map.get(DUTDescriptorUtil.ACCESSMODE).isEmpty()) {
                        errorMessage
                                .append("Representation should not be empty.\n");
                        return;
                    } else {
                        if (!checkTestDataForDifferentType(
                                map.get(DUTDescriptorUtil.TYPE),
                                map.get(DUTDescriptorUtil.TESTDATA), false)) {
                            return;
                        }

                        if (!checkTestDataForDifferentType(
                                map.get(DUTDescriptorUtil.TYPE),
                                map.get(DUTDescriptorUtil.TESTDATA_2), true)) {
                            return;
                        }

                        if (map.get(DUTDescriptorUtil.ACCESSMODE).equals("R")
                                || map.get(DUTDescriptorUtil.ACCESSMODE)
                                        .equals("W")) {
                            rep.add(map);
                        } else {
                            errorMessage
                                    .append("Representation AccessMode should be R or W.\n");
                            return;
                        }
                    }
                }
            }

            if (reps.size() == mapCount) {
                errorMessage
                        .append("Representation List should not be empty String.\n");
            }
        }
    }

    private Boolean checkTestDataForDifferentType(String dataType,
            String testData, Boolean isTestData2) {
        String msg = "";

        if (isTestData2) {
            msg = "Representation testdata2 should be ";
        } else {
            msg = "Representation testdata should be ";
        }

        if (dataType.toLowerCase().contains("int")) {
            try {
                Integer.parseInt(testData);
            } catch (NumberFormatException e) {
                errorMessage.append(msg + "integer.\n");
                return false;
            }
        } else if (dataType.toLowerCase().contains("float")) {
            try {
                Float.parseFloat(testData);
            } catch (NumberFormatException e) {
                errorMessage.append(msg + "float.\n");
                return false;
            }
        } else if (dataType.toLowerCase().contains("double")) {
            try {
                Double.parseDouble(testData);
            } catch (NumberFormatException e) {
                errorMessage.append(msg + "double.\n");
                return false;
            }
        } else if (dataType.toLowerCase().contains("bool")) {
            if (!(testData.equals("true") || testData.equals("false"))) {
                errorMessage.append(msg + "bool.\n");
            }
        }

        return true;
    }

    /**
     * A method to get Payload Type
     * 
     * @see setPayloadType()
     * @return Payload Type if available, empty otherwise
     * @note Please Set Payload Type using setPayloadType() method first.
     */
    public String getPayloadType() {
        if (payloadType == null)
            return "";

        return payloadType;
    }

    /**
     * A method to set the Resource Payload Type
     * 
     * @see getPayloadType()
     * @param payload
     *            Resource Payload Type
     */
    public void setPayloadType(String payload) {
        if (payload == null) {
            errorMessage.append("PayloadType should not be null.\n");
        }

        if (payload.isEmpty())
            errorMessage.append("PayloadType should not be empty.\n");
        else
            payloadType = payload;
    }

    /**
     * A method to get DUT Type
     * 
     * @see setDutType()
     * @return DUT Type if available, empty otherwise
     * @note Please Set Payload Type using setDutType() method first.
     */
    public String getDutType() {
        if (dutType == null)
            return "";

        return dutType;
    }

    /**
     * A method to set the DUT Type
     * 
     * @see getDutType()
     * @param dut
     *            DUT Type
     */
    public void setDutType(String dut) {
        if (dut == null) {
            errorMessage.append("DutType should not be null.\n");
        }

        if (dut.isEmpty())
            errorMessage.append("DutType should not be empty.\n");
        else if (dut.equalsIgnoreCase("Server")
                || dut.equalsIgnoreCase("Client")) {
            dutType = dut;
        } else {
            errorMessage.append("DutType should be SERVER or CLIENT.\n");
        }
    }
}
