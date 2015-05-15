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

public class OcDeviceInfo {

    private String deviceName;
    private String hostName;
    private String deviceUuid;
    private String contentType;
    private String version;
    private String manufacturerName;
    private String manufacturerUrl;
    private String modelNumber;
    private String dateOfManufacture;
    private String platformVersion;
    private String firmwareVersion;
    private String supportUrl;

    public OcDeviceInfo() {
        deviceName = "";
        hostName = "";
        deviceUuid = "";
        contentType = "";
        version = "";
        manufacturerName = "";
        manufacturerUrl = "";
        modelNumber = "";
        dateOfManufacture = "";
        platformVersion = "";
        firmwareVersion = "";
        supportUrl = "";
    }

    public OcDeviceInfo(
            String deviceName,
            String hostName,
            String deviceUuid,
            String contentType,
            String version,
            String manufacturerName,
            String manufacturerUrl,
            String modelNumber,
            String dateOfManufacture,
            String platformVersion,
            String firmwareVersion,
            String supportUrl) {
        this.deviceName = deviceName;
        this.hostName = hostName;
        this.deviceUuid = deviceUuid;
        this.contentType = contentType;
        this.version = version;
        this.manufacturerName = manufacturerName;
        this.manufacturerUrl = manufacturerUrl;
        this.modelNumber = modelNumber;
        this.dateOfManufacture = dateOfManufacture;
        this.platformVersion = platformVersion;
        this.firmwareVersion = firmwareVersion;
        this.supportUrl = supportUrl;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public String getHostName() {
        return hostName;
    }

    public void setHostName(String hostName) {
        this.hostName = hostName;
    }

    public String getDeviceUuid() {
        return deviceUuid;
    }

    public void setDeviceUuid(String deviceUuid) {
        this.deviceUuid = deviceUuid;
    }

    public String getContentType() {
        return contentType;
    }

    public void setContentType(String contentType) {
        this.contentType = contentType;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
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
}
