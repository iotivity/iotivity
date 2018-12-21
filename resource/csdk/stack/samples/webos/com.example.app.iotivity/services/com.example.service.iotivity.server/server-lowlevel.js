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

module.exports.startServer = function(resourceCallback) {
    console.log("Starting OCF stack in server mode");

    iotivity.OCRegisterPersistentStorageHandler(StorageHandler("oic_svr_db_server.dat"));

    // Start iotivity and set up the processing loop
    iotivity.OCInit(null, 0, iotivity.OCMode.OC_SERVER);

    intervalId = setInterval(function() {
        iotivity.OCProcess();
    }, 1000);

    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
                    iotivity.OC_RSRVD_SPEC_VERSION, "res.1.3.0");
    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
                    iotivity.OC_RSRVD_DATA_MODEL_VERSION, "webosose.1.0.0");
    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_DEVICE,
                    iotivity.OC_RSRVD_DEVICE_NAME, "server.example");
    iotivity.OCSetPropertyValue(iotivity.OCPayloadType.PAYLOAD_TYPE_PLATFORM,
                    iotivity.OC_RSRVD_MFG_NAME, "lge-iotivity-node");

    console.log("Server ready");

    var countReceptacle = {};
    var result = iotivity.OCGetNumberOfResources(countReceptacle);
    initialResourceCount = countReceptacle.count;
    updateServerStatus(resourceCallback);

};

module.exports.createResource = function(uri, types, q, a, observable, resourceCallback) {
    console.log("Registering resources");

    var handleReceptacle = {};
    var observerIds = [];
    var properties = iotivity.OCResourceProperty.OC_DISCOVERABLE
        | iotivity.OCResourceProperty.OC_SECURE;

    if(observable){
        properties= properties|iotivity.OCResourceProperty.OC_OBSERVABLE;
    }

    var sensor = require( "./mock-sensor" )()
        .on( "change", function( data ) {
            iotivity.OCNotifyListOfObservers(
                handleReceptacle.handle,
                observerIds,
                {
                    type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                    values: data
                },
                iotivity.OCQualityOfService.OC_HIGH_QOS );
        }
    );

    iotivity.OCCreateResource(

        // The bindings fill in this object
        handleReceptacle,
        types,
        iotivity.OC_RSRVD_INTERFACE_DEFAULT,
        uri,
        function( flag, request ) {
            console.log( "Entity handler called with flag = " + flag + " and the following request:" );
            console.log( JSON.stringify( request, null, 4 ) );

            // If we find the magic question in the request, we return the magic answer
            if ( request && request.payload && request.payload.values &&
                request.payload.values.question===q) {
                iotivity.OCDoResponse( {
                    requestHandle: request.requestHandle,
                    resourceHandle: request.resource,
                    ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
                    payload: {
                        type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                        values: {
                            "answer": a
                        }
                    },
                    resourceUri: uri,
                    sendVendorSpecificHeaderOptions: []
                } );

                return iotivity.OCEntityHandlerResult.OC_EH_OK;
            }
            if ( request && request.method === iotivity.OCMethod.OC_REST_DELETE ) {

                var result = iotivity.OCDeleteResource( handleReceptacle.handle );

                console.log( "OCDeleteResource() has resulted in " + result );

                iotivity.OCDoResponse( {
                    requestHandle: request.requestHandle,
                    resourceHandle: null,
                    ehResult: result ?
                        iotivity.OCEntityHandlerResult.OC_EH_ERROR :
                        iotivity.OCEntityHandlerResult.OC_EH_RESOURCE_DELETED,
                    payload: null,
                    resourceUri: uri,
                    sendVendorSpecificHeaderOptions: []
                } );

                return iotivity.OCEntityHandlerResult.OC_EH_OK;
            }
            if ( flag & iotivity.OCEntityHandlerFlag.OC_OBSERVE_FLAG ) {
                if ( request.obsInfo.obsId !== 0 ) {
                    if ( request.obsInfo.action === iotivity.OCObserveAction.OC_OBSERVE_REGISTER ) {

                        // Add new observer to list.
                        observerIds.push( request.obsInfo.obsId );
                    } else if ( request.obsInfo.action ===
                            iotivity.OCObserveAction.OC_OBSERVE_DEREGISTER ) {

                        // Remove requested observer from list.
                        observerIdIndex = observerIds.indexOf( request.obsInfo.obsId );
                        if ( observerIdIndex >= 0 ) {
                            observerIds.splice( observerIdIndex, 1 );
                        }
                    }
                }
            }
            if ( request.requestHandle ) {
                iotivity.OCDoResponse( {
                    requestHandle: request.requestHandle,
                    resourceHandle: request.resource,
                    ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
                    payload: {
                        type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                        values: sensor.currentData()
                    },
                    resourceUri: uri,
                    sendVendorSpecificHeaderOptions: []
                } );
            }

        // By default we error out
        return iotivity.OCEntityHandlerResult.OC_EH_OK;
    }, properties);

    console.log(uri + " resource ready");

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

module.exports.stopServer = function() {
    // Tear down the processing loop and stop iotivity
    clearInterval(intervalId);
    iotivity.OCStop();
    console.log("=== server teardown ===");
};

module.exports.deleteResource = function(uri, resourceCallback) {
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