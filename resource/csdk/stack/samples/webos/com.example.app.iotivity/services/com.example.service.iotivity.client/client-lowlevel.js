/* ******************************************************************
 *
 * Copyright (c) 2018 LG Electronics, Inc.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
var intervalId, handleReceptacle = {}, iotivity = require('iotivity-node/lowlevel');
var StorageHandler = require('iotivity-node/lib/CustomStorageHandler');
var observeHandles = [];
var observeCount = 0;
const SVR_CLIENT = 'oic_svr_db_client.dat';
var discoverHandle = {};
var discoveredDevices = [];
var fs = require('fs');

function setDiscoveryTimeOut(callback, time) {
    setTimeout(function () {
        if (discoveredDevices.length === 0)
            console.log('Resource not found');
        callback(discoveredDevices);
        discoveredDevices = [];
        if (discoverHandle.handle)
            iotivity.OCCancel(discoverHandle.handle, iotivity.OCQualityOfService.OC_HIGH_QOS, null);
        discoverHandle = {};
    }, time, null);
};

var isActive = false;
module.exports.startClient = function () {
    if (!isActive) {
        isActive = true;
        iotivity.OCRegisterPersistentStorageHandler(StorageHandler(SVR_CLIENT));
        iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT_SERVER);

        var handle = iotivity.OCGetResourceHandleAtUri("/oic/d");
        iotivity.OCBindResourceTypeToResource(handle, "oic.wk.d");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
            iotivity.OC_RSRVD_SPEC_VERSION, "ocf.2.0.0");
        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
            iotivity.OC_RSRVD_DATA_MODEL_VERSION, "ocf.res.1.3.0");
        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
            iotivity.OC_RSRVD_DEVICE_NAME, "client");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_PLATFORM_ID, "0A3E0D6F-DBF5-404E-8719-D6880042463A");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_MFG_NAME, "myName");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_MFG_URL, "https://www.iotivity.org");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_MODEL_NUM, "myModelNumber");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_MFG_DATE, "2016-01-15");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_PLATFORM_VERSION, "myPlatformVersion");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_OS_VERSION, "myOS");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_HARDWARE_VERSION, "myHardwareVersion");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_FIRMWARE_VERSION, "myFirmwareVersion");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_SUPPORT_URL, "https://www.iotivity.org");

        iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
            iotivity.OC_RSRVD_SYSTEM_TIME, "2017-12-01T12:00:00.52Z");

        intervalId = setInterval(function () {
            iotivity.OCProcess();
        }, 1000);
    }
};

function stopClient() {
    if (isActive) {
        console.log('stopClient called');
        if (discoverHandle.handle)
            iotivity.OCCancel(discoverHandle.handle, iotivity.OCQualityOfService.OC_HIGH_QOS, null);
        discoverHandle = {};
        if (observeCount > 0) {
            for (var index = 0; index < observeHandles.size(); index++) {
                console.log('Cancel observation');
                iotivity.OCCancel(observeHandles[index].handle, iotivity.OCQualityOfService.OC_HIGH_QOS, null);
            }
            observeHandles.length = 0;
            observeCount = 0;
        }
        clearInterval(intervalId);
        iotivity.OCStop();
        isActive = false;
    }
}

module.exports.stopClient = function () {
    console.log('stopClient start');
    stopClient();
};

function assembleRequestUrl(eps, path) {
    var endpoint;
    var endpointIndex;
    var result;
    for (endpointIndex in eps) {
        endpoint = eps[endpointIndex];
        if (endpoint.tps === 'coaps') {
            result = (endpoint.tps + '://' +
                (endpoint.family & iotivity.OCTransportFlags.OC_IP_USE_V6 ? '[' : '') +
                endpoint.addr.replace(/[%].*$/, '') +
                (endpoint.family & iotivity.OCTransportFlags.OC_IP_USE_V6 ? ']' : '') +
                ':' + endpoint.port) + path;
            console.log('GET request to ' + result);
            return result;
        }
    }
    throw new Error('No secure endpoint found!');
}

module.exports.startDiscovery = function (callback) {
    console.log('Issuing discovery request');
    // Discover resources and list them

    if (discoverHandle.handle) {
        console.log('reset discovery');
        discoveredDevices = [];
        iotivity.OCCancel(discoverHandle.handle, iotivity.OCQualityOfService.OC_HIGH_QOS, null);
    }

    iotivity.OCDoResource(
        discoverHandle,
        iotivity.OCMethod.OC_REST_DISCOVER,
        iotivity.OC_MULTICAST_DISCOVERY_URI,
        null,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        function (handle, response) {
            console.log('Discovery response: ' + JSON.stringify(response, null, 4));
            discoveredDevices.push(response);
            return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
        },
        // There are no header options
        null
    );

    setDiscoveryTimeOut(callback, 5000);
};

module.exports.getResource = function (uri, eps, destination, callback) {
    console.log('Sending GET request');
    var getHandleReceptacle = {};
    var getResourceHandler = function (handle, response) {
        console.log('Received response to GET request:');
        console.log(JSON.stringify(response, null, 4));
        callback(response);
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    };

    iotivity.OCDoResource(
        getHandleReceptacle,
        iotivity.OCMethod.OC_REST_GET,
        eps && eps[0].tps === 'coaps' ? assembleRequestUrl(eps, uri) : uri,
        destination,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        getResourceHandler,
        null);
};

module.exports.putResource = function (uri, eps, destination, key, value, callback) {
    console.log('Sending PUT request');
    console.log('uri: ' + uri);
    var getHandleReceptacle = {};
    var payload = {};
    var values = {};
    values[key] = value;
    payload['values'] = values;
    payload['type'] = iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION;
    var putResourceHandler = function (handle, response) {
        console.log('Received response to PUT request:');
        console.log(JSON.stringify(response, null, 4));
        callback(response);
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    };

    iotivity.OCDoResource(
        getHandleReceptacle,
        iotivity.OCMethod.OC_REST_PUT,
        eps && eps[0].tps === 'coaps' ? assembleRequestUrl(eps, uri) : uri,
        destination,
        payload,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        putResourceHandler,
        null
    );
};

module.exports.postResource = function (uri, eps, destination, key, value, callback) {
    console.log('Sending POST request');
    console.log('uri: ' + uri);
    var getHandleReceptacle = {};
    var payload = {};
    var values = {};
    values[key] = value;
    payload['values'] = values;
    payload['type'] = iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION;
    var putResourceHandler = function (handle, response) {
        console.log('Received response to POST request:');
        console.log(JSON.stringify(response, null, 4));
        callback(response);
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    };

    iotivity.OCDoResource(
        getHandleReceptacle,
        iotivity.OCMethod.OC_REST_POST,
        eps && eps[0].tps === 'coaps' ? assembleRequestUrl(eps, uri) : uri,
        destination,
        payload,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        putResourceHandler,
        null
    );
};

module.exports.deleteResource = function (uri, eps, destination, callback) {
    console.log('Sending DELETE request');
    var deleteHandleReceptacle = {};
    var deleteResponseHandler = function (handle, response) {
        console.log('Received response to DELETE request:');
        console.log(JSON.stringify(response, null, 4));
        callback(response);
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    };

    iotivity.OCDoResource(
        deleteHandleReceptacle,
        iotivity.OCMethod.OC_REST_DELETE,
        eps && eps[0].tps === 'coaps' ? assembleRequestUrl(eps, uri) : uri,
        destination,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        deleteResponseHandler,
        null
    );
};

module.exports.observeResource = function (uri, eps, destination, token, callback) {
    console.log('Observing ' + uri);
    var observeHandleReceptacle = {};
    var observeResponseHandler = function (handle, response) {
        console.log('Received response to OBSERVE request:');
        console.log(JSON.stringify(response, null, 4));

        for (var index = 0; index < observeHandles.length; index++) {
            if (observeHandles[index].handle === handle) {
                observeHandles[index].callback(response);
            }
        }

        return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
    };

    iotivity.OCDoResource(
        observeHandleReceptacle,
        iotivity.OCMethod.OC_REST_OBSERVE,
        eps && eps[0].tps === 'coaps' ? assembleRequestUrl(eps, uri) : uri,
        destination,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        observeResponseHandler,
        null
    );

    observeHandles[observeCount] = {
        handle: observeHandleReceptacle.handle,
        callback: callback,
        messageToken: token
    };
    observeCount++;
};

module.exports.cancelObservation = function (token) {
    console.log('Cancel observation start');
    for (var index = 0; index < observeCount; index++) {
        if (observeHandles[index].messageToken === token) {
            iotivity.OCCancel(observeHandles[index].handle, iotivity.OCQualityOfService.OC_HIGH_QOS, null);
            observeCount--;
            observeHandles.splice(index, 1);
            break;
        }
    }
};

module.exports.copyFile = function (mode, callback) {
    console.log("copyFile");
    setTimeout(function () {
        if (mode == 'RFOTM') {
            fs.exists('oic_svr_db_client_rfotm.dat', function (exists) {
                if (exists) {
                    console.log('oic_svr_db_client_rfotm.dat exist!');
                    fs.createReadStream('oic_svr_db_client_rfotm.dat').pipe(fs.createWriteStream(SVR_CLIENT));
                    console.log('renamed complete');
                    callback({ 'returnValue': true });
                } else {
                    callback({ 'returnValue': false });
                }
            });
        }
        else if (mode == 'RFNOP') {
            fs.exists('oic_svr_db_client_rfnop.dat', function (exists) {
                if (exists) {
                    console.log('oic_svr_db_client_rfnop.dat exist!');
                    fs.createReadStream('oic_svr_db_client_rfnop.dat').pipe(fs.createWriteStream(SVR_CLIENT));
                    console.log('renamed complete');
                    callback({ 'returnValue': true });
                } else {
                    callback({ 'returnValue': false });
                }
            });
        }
        else {
            callback({ 'returnValue': false });
        }
    }, 3000, null);
};

//Exit gracefully when node service is killed
process.on('exit', function () {
    // Tear down the processing loop and stop iotivity
    stopClient();
    console.log('=== client teardown ===');
});