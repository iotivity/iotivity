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
package oic.ctt.testhelper;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

import oic.ctt.formatter.CborManager;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.network.OICHelper;

/**
 * 
 * This is a helper class for the tester classes every test class extends this
 * class to share common property and method
 * 
 */

public class ConformanceTestHelper {

    public final static String dutID                          = "0685B960-736F-46F7-BEC0-9E6CBD61ADC2";
    public final static String dutPath                        = "DUTDescriptor.json";
    public final static String cborJsonDataPath               = "json/Sample.json";
    public final static String cborJsonDataPathWithoutPayload = "json/SampleWithoutPayload.json";
    public final static String matchStringFilePath            = "json/matchingstring";
    public final static String matchingStringforJsonFile      = "json/MatchingStringforJsonFile";
    public final static String invalidDUTID                   = "invalidDUTID";
    public final static String invalidDUTPath                 = "invalidDUTPath";
    public final static int    port                           = OICHelper.DEFAULT_COAP_PORT;
    public final static String payload                        = "a/test";
    public final static String uriPath                        = "oic/res";
    public final static String query                          = OICHelper.DEFAULT_QUERY;
    public final static String ipv4                           = OICHelper.DEFAULT_MULTICAST_IPv4;
    public final static String ipv6                           = OICHelper.DEFAULT_MULTICAST_IPv6;
    public final static String resourceType                   = "co.light";
    public String              matchingJsonString;
    public byte[]              byteDataOfJsonString;

    /**
     * This method read file data
     * 
     * @param query
     *            : file full path
     * @return file data as String
     */

    private String getStringFromFile(String filePath) {
        StringBuilder stringBuilder = new StringBuilder();
        String line = null;
        try (BufferedReader reader = new BufferedReader(
                new FileReader(filePath))) {

            while ((line = reader.readLine()) != null) {
                stringBuilder.append(line);
            }

        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return stringBuilder.toString();
    }

    /**
     * This method returns Json String
     * 
     * @param query
     *            : file full path
     * @return Json data as String
     */

    public String getMatchString(String filePath) throws IOException {
        try {
            matchingJsonString = getStringFromFile(filePath);
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
        }
        return matchingJsonString;
    }

    /**
     * This method returns Cbor byte data
     * 
     * @param query
     *            : OCPayloadType
     * @return byte[]
     */

    public byte[] getCborByteData(OCPayloadType payLoadType) throws IOException {
        byteDataOfJsonString = CborManager.convertToCbor(
                getMatchString(cborJsonDataPath), payLoadType);
        return byteDataOfJsonString;
    }

}
