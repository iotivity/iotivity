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
var intervalId, initialResourceCount;
var iotivity = require("iotivity-node/lowlevel");
var StorageHandler = require("iotivity-node/lib/CustomStorageHandler");
var state = false;
var handleReceptacle = {};
var observerIds = [];
const SVR_SERVER = 'oic_svr_db_server.dat';
var subscriptionCallback;
var fs = require('fs');

module.exports.startServer = function (resourceCallback) {
    console.log("Starting OCF stack in server mode");

    iotivity.OCRegisterPersistentStorageHandler(StorageHandler(SVR_SERVER));

    // Start iotivity and set up the processing loop
    iotivity.OCInit(null, 0, iotivity.OCMode.OC_SERVER);

    intervalId = setInterval(function () {
        iotivity.OCProcess();
    }, 1000);

    var handle = iotivity.OCGetResourceHandleAtUri("/oic/d");
    iotivity.OCBindResourceTypeToResource(handle, "oic.d.light");

    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
        iotivity.OC_RSRVD_SPEC_VERSION, "ocf.2.0.0");
    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
        iotivity.OC_RSRVD_DATA_MODEL_VERSION, "ocf.res.1.3.0,ocf.sh.1.3.0");
    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
        iotivity.OC_RSRVD_DEVICE_NAME, "Binary Switch");

    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
        iotivity.OC_RSRVD_MFG_NAME, "iotivity-node");

    console.log("Server ready");

    var countReceptacle = {};
    var result = iotivity.OCGetNumberOfResources(countReceptacle);
    initialResourceCount = countReceptacle.count;
    updateServerStatus(resourceCallback);

};

module.exports.createResource = function (uri, types, observable, resourceCallback) {
    console.log("Registering resources");

    var properties = iotivity.OCResourceProperty.OC_DISCOVERABLE
        | iotivity.OCResourceProperty.OC_SECURE;

    if (observable) {
        properties = properties | iotivity.OCResourceProperty.OC_OBSERVABLE;
    }

    iotivity.OCCreateResource(

        // The bindings fill in this object
        handleReceptacle,
        types,
        iotivity.OC_RSRVD_INTERFACE_DEFAULT,
        uri,
        function (flag, request) {
            console.log("Entity handler called with flag = " + flag + " and the following request:");
            console.log(JSON.stringify(request, null, 4));

            if (request && request.method === iotivity.OCMethod.OC_REST_DELETE) {

                var result = iotivity.OCDeleteResource(handleReceptacle.handle);

                console.log("OCDeleteResource() has resulted in " + result);

                iotivity.OCDoResponse({
                    requestHandle: request.requestHandle,
                    resourceHandle: null,
                    ehResult: result ?
                        iotivity.OCEntityHandlerResult.OC_EH_ERROR :
                        iotivity.OCEntityHandlerResult.OC_EH_RESOURCE_DELETED,
                    payload: null,
                    resourceUri: uri,
                    sendVendorSpecificHeaderOptions: []
                });

                return iotivity.OCEntityHandlerResult.OC_EH_OK;
            }
            if (flag & iotivity.OCEntityHandlerFlag.OC_OBSERVE_FLAG) {
                if (request.obsInfo.obsId !== 0) {
                    if (request.obsInfo.action === iotivity.OCObserveAction.OC_OBSERVE_REGISTER) {

                        // Add new observer to list.
                        observerIds.push(request.obsInfo.obsId);
                    } else if (request.obsInfo.action ===
                        iotivity.OCObserveAction.OC_OBSERVE_DEREGISTER) {

                        // Remove requested observer from list.
                        observerIdIndex = observerIds.indexOf(request.obsInfo.obsId);
                        if (observerIdIndex >= 0) {
                            observerIds.splice(observerIdIndex, 1);
                        }
                    }
                }
            }
            if (request.requestHandle && request.method === iotivity.OCMethod.OC_REST_GET) {
                iotivity.OCDoResponse({
                    requestHandle: request.requestHandle,
                    resourceHandle: request.resource,
                    ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
                    payload: {
                        type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                        values: {
                            "value": state,
                            "rt": ["oic.r.switch.binary"],
                            "if": ["oic.if.baseline", "oic.if.a"]
                        }
                    },
                    resourceUri: uri,
                    sendVendorSpecificHeaderOptions: []
                });
            }

            if (request.requestHandle &&
                (request.method === iotivity.OCMethod.OC_REST_PUT || request.method === iotivity.OCMethod.OC_REST_POST)) {
                state = request.payload.values.value;
                iotivity.OCDoResponse({
                    requestHandle: request.requestHandle,
                    resourceHandle: request.resource,
                    ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
                    payload: {
                        type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                        values: {
                            "value": state,
                            "rt": ["oic.r.switch.binary"],
                            "if": ["oic.if.baseline", "oic.if.a"]
                        }
                    },
                    resourceUri: uri,
                    sendVendorSpecificHeaderOptions: []
                });
                notifyObservers();
            }

            // By default we error out
            return iotivity.OCEntityHandlerResult.OC_EH_OK;
        }, properties);

    iotivity.OCBindResourceInterfaceToResource(handleReceptacle.handle, "oic.if.a");
    console.log("/binaryswitch" + " resource ready");

    updateServerStatus(resourceCallback);
};

function updateServerStatus(resourceCallback) {
    var countReceptacle = {};
    var result = iotivity.OCGetNumberOfResources(countReceptacle);
    var resources = [];
    for (var index = initialResourceCount; index < countReceptacle.count; index++) {
        var resource = {};
        resource.types = [];
        resource.interfaces = [];

        var handle = iotivity.OCGetResourceHandle(index);
        resource.uri = iotivity.OCGetResourceUri(handle);

        var countRecep = {};
        result = iotivity.OCGetNumberOfResourceTypes(handle, countRecep);
        for (var i = 0; i < countRecep.count; i++) {
            resource.types[i] = iotivity.OCGetResourceTypeName(handle, i);
        }

        result = iotivity.OCGetNumberOfResourceInterfaces(handle, countRecep);
        for (i = 0; i < countRecep.count; i++) {
            resource.interfaces[i] = iotivity.OCGetResourceInterfaceName(handle, i);
        }

        resources[index - initialResourceCount] = resource;
    }
    resourceCallback(resources);
};

module.exports.stopServer = function () {
    // Tear down the processing loop and stop iotivity
    clearInterval(intervalId);
    iotivity.OCStop();
    state = false;
    console.log("=== server teardown ===");
};

module.exports.deleteResource = function (uri, resourceCallback) {
    var countReceptacle = {};
    var handle;

    var result = iotivity.OCGetNumberOfResources(countReceptacle);
    var count = countReceptacle.count;
    for (var index = initialResourceCount; index < count; index++) {
        handle = iotivity.OCGetResourceHandle(index);
        if (uri == null) {
            iotivity.OCDeleteResource(handle);
            index--;
            count--;
            continue;
        }

        if (uri == iotivity.OCGetResourceUri(handle)) {
            iotivity.OCDeleteResource(handle);
            break;
        }
    }

    updateServerStatus(resourceCallback);
};

function notifyObservers() {
    subscriptionCallback({ "value": state });
    if (observerIds.length > 0) {
        iotivity.OCNotifyListOfObservers(
            handleReceptacle.handle,
            observerIds,
            {
                type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                values: {
                    "value": state,
                    "rt": ["oic.r.switch.binary"],
                    "if": ["oic.if.baseline", "oic.if.a"]
                }
            },
            iotivity.OCQualityOfService.OC_HIGH_QOS);
    }
};

module.exports.setBinarySwitchValue = function (value) {
    console.log("updateResourceValue value:" + value);
    state = value;
    console.log("      state:" + state);
    notifyObservers();
};

module.exports.observeBinarySwitchValue = function (resourceCallback) {
    console.log("observeBinarySwitchValue");
    subscriptionCallback = resourceCallback;
};

module.exports.copyFile = function (mode, callback) {
    console.log("copyFile: " + mode);
    setTimeout(function () {
        if (mode == 'RFOTM') {
            fs.exists('oic_svr_db_server_rfotm.dat', function (exists) {
                if (exists) {
                    console.log('oic_svr_db_server_rfotm.dat exist!');
                    fs.createReadStream('oic_svr_db_server_rfotm.dat').pipe(fs.createWriteStream(SVR_SERVER));
                    console.log('renamed complete');
                    callback({ 'returnValue': true });
                } else {
                    callback({ 'returnValue': false });
                }
            });
        }
        else if (mode == 'RFNOP') {
            fs.exists('oic_svr_db_server_rfnop.dat', function (exists) {
                if (exists) {
                    console.log('oic_svr_db_server_rfnop.dat exist!');
                    fs.createReadStream('oic_svr_db_server_rfnop.dat').pipe(fs.createWriteStream(SVR_SERVER));
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
