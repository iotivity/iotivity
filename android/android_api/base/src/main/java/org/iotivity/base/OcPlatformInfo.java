/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
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

import java.lang.Error;

public class OcPlatformInfo {

    private String platformID;
    private String manufacturerName;
    private String manufacturerUrl;
    private String modelNumber;
    private String dateOfManufacture;
    private String platformVersion;
    private String operatingSystemVersion;
    private String hardwareVersion;
    private String firmwareVersion;
    private String supportUrl;
    private String systemTime;

    // construct OcPlatformInfo with mandatory fields which cannot be null
    // manufacturerName cannot be > 16 chars
    // manufacturerUrl cannot be > 32 chars
    protected OcPlatformInfo(String platformID, String manufacturerName,
                             String manufacturerUrl) throws OcException {
        ErrorCode result = validatePlatformInfo(platformID, manufacturerName, manufacturerUrl);
        if (ErrorCode.OK == result) {
            this.platformID = platformID;
            this.manufacturerName = manufacturerName;
            this.manufacturerUrl = manufacturerUrl;
        } else {
            throw new OcException(result, result.getDescription());
        }
    }

    public ErrorCode validatePlatformInfo(String platformID, String manufacturerName,
                                          String manufacturerUrl) {
        // checks to see if the mandatory fields have non-null values or not
        if (platformID == null || platformID.isEmpty()) return ErrorCode.INVALID_PLATFORM_INFO_PLATFORMID;
        if (manufacturerName == null || manufacturerName.isEmpty() ||
                manufacturerName.length() > OcStackConfig.MAX_MANUFACTURER_NAME_LENGTH)
            return ErrorCode.INVALID_PLATFORM_INFO_MANUFACTURER_NAME;
        if (manufacturerUrl == null || manufacturerUrl.isEmpty() ||
                manufacturerUrl.length() > OcStackConfig.MAX_MANUFACTURER_URL_LENGTH)
            return ErrorCode.INVALID_PLATFORM_INFO_PLATFORMID_MANUFACTURER_URL;
        return ErrorCode.OK;
    }

    public String getPlatformID() {
        return platformID;
    }

    public void setPlatformID(String platformID) {
        this.platformID = platformID;
    }

    public String getManufacturerName() {
        return manufacturerName;
    }

    public void setManufacturerName(String manufacturerName) {
        this.manufacturerName = manufacturerName;
    }

    public String getManufacturerUrl() {
        return manufacturerUrl;
    }

    public void setManufacturerUrl(String manufacturerUrl) {
        this.manufacturerUrl = manufacturerUrl;
    }

    public String getModelNumber() {
        return modelNumber;
    }

    public void setModelNumber(String modelNumber) {
        this.modelNumber = modelNumber;
    }

    public String getDateOfManufacture() {
        return dateOfManufacture;
    }

    public void setDateOfManufacture(String dateOfManufacture) {
        this.dateOfManufacture = dateOfManufacture;
    }

    public String getPlatformVersion() {
        return platformVersion;
    }

    public void setPlatformVersion(String platformVersion) {
        this.platformVersion = platformVersion;
    }

    public String getOperatingSystemVersion() {
        return operatingSystemVersion;
    }

    public void setOperatingSystemVersion(String operatingSystemVersion) {
        this.operatingSystemVersion = operatingSystemVersion;
    }

    public String getHardwareVersion() {
        return hardwareVersion;
    }

    public void setHardwareVersion(String hardwareVersion) {
        this.hardwareVersion = hardwareVersion;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public String getSupportUrl() {
        return supportUrl;
    }

    public void setSupportUrl(String supportUrl) {
        this.supportUrl = supportUrl;
    }

    public String getSystemTime() {
        return systemTime;
    }

    public void setSystemTime(String systemTime) {
        this.systemTime = systemTime;
    }
}
