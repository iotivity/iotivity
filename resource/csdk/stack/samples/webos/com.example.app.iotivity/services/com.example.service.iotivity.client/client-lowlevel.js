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
var intervalId, handleReceptacle = {}, iotivity = require("iotivity-node/lowlevel");
var StorageHandler = require("iotivity-node/lib/CustomStorageHandler");
var observeHandles = [];
var observeCount = 0;

returnNoResponse = function(callback) {
    setTimeout(function() {
        console.log("No response");
        callback({});
        clearInterval( intervalId );
        iotivity.OCStop();
    }, 5000, null);
};

function assembleRequestUrl( eps, path ) {
    var endpoint;
    var endpointIndex;
    var result;
    for ( endpointIndex in eps ) {
        endpoint = eps[ endpointIndex ];
        if ( endpoint.tps  === "coaps" ) {
            result = ( endpoint.tps + "://" +
            ( endpoint.family & iotivity.OCTransportFlags.OC_IP_USE_V6 ? "[" : "" ) +
            endpoint.addr.replace( /[%].*$/, "" ) +
            ( endpoint.family & iotivity.OCTransportFlags.OC_IP_USE_V6 ? "]" : "" ) +
            ":" + endpoint.port ) + path;
            console.log( "GET request to " + result );
            return result;
        }
    }
    throw new Error( "No secure endpoint found!" );
}

module.exports.startDiscovery = function(callback) {

    console.log("Issuing discovery request");

    iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT );
    clearInterval( intervalId );
    intervalId = setInterval(function() {
        iotivity.OCProcess();
    }, 1000);

    // Discover resources and list them
    iotivity.OCDoResource(

        // The bindings fill in this object
        handleReceptacle,

        iotivity.OCMethod.OC_REST_DISCOVER,

        // Standard path for discovering resources
        iotivity.OC_MULTICAST_DISCOVERY_URI,

        // There is no destination
        null,

        // There is no payload
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        function(handle, response) {
            console.log("Discovery response: "+ JSON.stringify(response, null, 4));
            callback(response);
            return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
        },

        // There are no header options
        null
    );

    returnNoResponse(callback);
};

module.exports.getResource = function(uri, destination, question, callback) {

    console.log("Sending GET request");
    var getHandleReceptacle = {};
    var payload = {
        "type":iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
        "values":{"question":question}
    };

    var getResourceHandler = function( handle, response ) {
        var resources = response && response.payload && response.payload.resources,
            resourceCount = resources ? resources.length : 0;

        for (var index = 0; index < resourceCount; index++ ) {
            if ( resources[ index ].uri === uri ) {
                iotivity.OCDoResource(
                    getHandleReceptacle,
                    iotivity.OCMethod.OC_REST_GET,
                    assembleRequestUrl( resources[ index ].eps, uri),
                    destination,
                    payload,
                    iotivity.OCConnectivityType.CT_DEFAULT,
                    iotivity.OCQualityOfService.OC_HIGH_QOS,
                    function( handle, response ){
                        console.log( "Received response to GET request:" );
                        console.log( JSON.stringify( response, null, 4 ) );
                        callback( response );
                        setTimeout(function() {
                            clearInterval( intervalId );
                            iotivity.OCStop();
                        }, 1000, null);
                        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
                    },
                    null
                );
            }
        }
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    };

    iotivity.OCRegisterPersistentStorageHandler(StorageHandler("oic_svr_db_client.dat"));
    iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT_SERVER );
    clearInterval( intervalId );
    intervalId = setInterval(function() {
        iotivity.OCProcess();
    }, 1000);

    iotivity.OCDoResource(
        getHandleReceptacle,
        iotivity.OCMethod.OC_REST_DISCOVER,
        iotivity.OC_MULTICAST_DISCOVERY_URI,
        null,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        getResourceHandler,
        null
    );

    returnNoResponse(callback);
};

module.exports.deleteResource = function(uri, destination, callback) {

    console.log("Sending DELETE request");
    var deleteHandleReceptacle = {};
    var deleteResponseHandler = function( handle, response ) {
        console.log( "Received response to DELETE request:" );
        console.log( JSON.stringify( response, null, 4 ) );
        callback( response );
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    };

    iotivity.OCRegisterPersistentStorageHandler(StorageHandler("oic_svr_db_client.dat"));
    iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT_SERVER );
    clearInterval( intervalId );
    intervalId = setInterval(function() {
        iotivity.OCProcess();
    }, 1000);

    iotivity.OCDoResource(
        deleteHandleReceptacle,
        iotivity.OCMethod.OC_REST_DELETE,
        uri,
        destination,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        deleteResponseHandler,
        null
    );

    returnNoResponse(callback);
};

module.exports.observeResource = function(uri, destination, callback) {

    console.log( "Observing " + uri );
    var observeHandleReceptacle = {};
    var observeResponseHandler = function( handle, response ) {
        console.log( "Received response to OBSERVE request:" );
        console.log( JSON.stringify( response, null, 4 ) );
        callback(response);
        return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
    };

    iotivity.OCRegisterPersistentStorageHandler(StorageHandler("oic_svr_db_client.dat"));
    iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT_SERVER );
    clearInterval( intervalId );
    intervalId = setInterval(function() {
        iotivity.OCProcess();
    }, 1000);

    iotivity.OCDoResource(
        observeHandleReceptacle,
        iotivity.OCMethod.OC_REST_OBSERVE,
        uri,
        destination,
        null,
        iotivity.OCConnectivityType.CT_DEFAULT,
        iotivity.OCQualityOfService.OC_HIGH_QOS,
        observeResponseHandler,
        null
    );

    observeHandles[observeCount] = {
        uri: uri,
        handle: observeHandleReceptacle.handle
    };
    observeCount++;
};

module.exports.cancelObservation = function(uri) {
    console.log( "Cancel observation " + uri );

    for (var index = 0; index < observeCount; index++){
        if (observeHandles[index].uri === uri){
            iotivity.OCCancel(observeHandles[index].handle, iotivity.OCQualityOfService.OC_HIGH_QOS, null );
            observeCount--;
            observeHandles.splice(index, 1);
            break;
        }
    }

    clearInterval( intervalId );
    iotivity.OCStop();
};

//Exit gracefully when node service is killed
process.on( "exit", function() {
    // Tear down the processing loop and stop iotivity
    clearInterval( intervalId );
    iotivity.OCStop();
    console.log("=== client teardown ===");
} );