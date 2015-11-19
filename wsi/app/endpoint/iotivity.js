var spawn = require("child_process").spawn;
var intervalId,
        handleReceptacle = {},
        iotivity = require("../../iotivity-node/lowlevel");

var map = new Object();

function parseEndpoint(endpoint){
    var endpointarray = endpoint.split("/");
    var addressport = endpointarray[2].split(":");
    var specificURI = "[" + addressport[0] + ":" + addressport[1] + ":"
                          + addressport[2] + ":" + addressport[3] + ":"
                          + addressport[4] + ":" + addressport[5]
                          + "]" + ":" + addressport[6] + "/oic/res";
    return specificURI;
}

module.exports = {
    init: function () {
    // Start iotivity and set up the processing loop
        iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT);
        var template = {
            "handler": "iotivity",
            "sid": "org.iotivity",
            "capability": [
                {
                    "cid": "org.iotivity.findresource",
                    "endpointtype": "IOTIVITY",
                    "operation": "GET",
                    "resourceType" : "resourceType or all"
                },
                {
                    "cid": "org.iotivity.getresource",
                    "endpoint": "oic://{{address}}:{{port}}{{uri}}",
                    "endpointtype": "IOTIVITY",
                    "operation": "GET",
                    "params":
                    {
                        "address": "server ip address",
                        "port": "server port",
                        "uri": "server's uri"
                    }
                },
                {
                    "cid": "org.iotivity.putresource",
                    "endpoint": "oic://{{address}}:{{port}}{{uri}}",
                    "endpointtype": "IOTIVITY",
                    "operation": "PUT",
                    "params":
                    {
                        "address": "server ip address",
                        "port": "server port",
                        "uri": "server's uri"
                    },
                    "payload":
                    {

                    }
                }
            ]
        };
        return template;
    },
    request: function (cap, res) {
        intervalId = setInterval(function () {
            iotivity.OCProcess();
        }, 1000);

        var endpointList = [];
        var responseflag = 0;


        findresourceAllResponseHandler = function (handle, response) {
            console.log("Received response to DISCOVER request:");
            console.log(JSON.stringify(response, null, 4));
            var index,
            destination = response.addr,
            getHandleReceptacle = {},
            resources = response && response.payload && response.payload.resources;
            if(resources){
                var resourceCount = resources.length ? resources.length : 0;
                for (index = 0; index < resourceCount; index++) {
                    var destinationAddress="";
                    for(var i=0; (i< destination.addr.length) && (destination.addr[i]!=0) ; i++ ){
                        destinationAddress = destinationAddress + String.fromCharCode(destination.addr[i]);
                    }
                    var endpoint = "{\"address\":\"" + destinationAddress + "\",\"port\":\"" + destination.port + "\",\"uri\":\"" + resources[ index ].uri + "\"}";
                    //var endpoint = "oic://"+ destinationAddress +":"+ destination.port + resources[ index ].uri;
                    var resource = new Object();
                    resource.destination = destination;
                    resource.uri = resources[ index ].uri;
                    map[endpoint] = resource;
                    endpointList.push(endpoint);
                }
            }
            return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
        };


        findresourceResourceTypeResponseHandler = function (handle, response) {
            console.log("Received response to DISCOVER request:");
            console.log(JSON.stringify(response, null, 4));
            var index,
            destination = response.addr,
            getHandleReceptacle = {},
            resources = response && response.payload && response.payload.resources;
            if(resources){
                var resourceCount = resources.length ? resources.length : 0;
                for (index = 0; index < resourceCount; index++) {
                    if (resources[ index ].types[0] === cap.resourceType) {
                        var destinationAddress="";
                        for(var i=0; (i< destination.addr.length) && (destination.addr[i]!=0) ; i++ ){
                            destinationAddress = destinationAddress + String.fromCharCode(destination.addr[i]);
                        }
                        var endpoint = "{\"address\":\"" + destinationAddress + "\",\"port\":\"" + destination.port + "\",\"uri\":\"" + resources[ index ].uri + "\"}";
                        //var endpoint = "oic://"+ destinationAddress +":"+ destination.port + resources[ index ].uri;
                        var resource = new Object();
                        resource.destination = destination;
                        resource.uri = resources[ index ].uri;
                        map[endpoint] = resource;
                        endpointList.push(endpoint);
                    }
                }
            }
            return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
        }

        if(cap.cid == "org.iotivity.findresource"){
            setTimeout(function(){
                res.writeHead(200, {"Content-Type": "application/json"});
                /*var json = JSON.stringify({
                "endpointList": endpointList
                });*/
                var json = "\"response\" : [" + endpointList + "]";
                res.end(json);
                return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
            },7000);

            if( cap.resourceType == "all"){
                handler = findresourceAllResponseHandler;
            }else{
                handler = findresourceResourceTypeResponseHandler;
            }
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
                handler,
                // There are no header options
            null);
        }
        else if(cap.cid == "org.iotivity.getresource"){
            getResponseHandler = function (handle, response) {
                responseflag = 1;
                console.log("Received response to GET request:");
                console.log(JSON.stringify(response, null, 4));
                var getResult = JSON.stringify(response.payload);
                res.writeHead(200, {"Content-Type": "application/json"});
                res.end(getResult);
                return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
            };
            var getHandleReceptacle = {};
            responseflag = 0;

            setTimeout(function(){
                if(responseflag==0){
                    res.writeHead(200, {"Content-Type": "text/plain"});
                    res.end("resouce is not exist");
                    return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
                }
            },7000);
            //var endpoint = cap.endpoint;
            console.log("1");
            var address = cap.params.address;
            var port = cap.params.port;
            var uri = cap.params.uri;
            var endpoint = "oic://"+ address +":"+ port + uri;
            var resource = map[endpoint];
            if(resource == undefined)
            {
                console.log("resource is not exist");
                console.log("Find specific resource is not exist or not");
                //var specificURI = parseEndpoint(endpoint);
                var specificURI = "["+ address + "]:" + port +"/oic/res";
                specificResponseHandlerNget = function (handle, response) {
                    console.log("Received response to DISCOVER request:");
                    console.log(JSON.stringify(response, null, 4));
                    destination = response.addr,
                    resources = response && response.payload && response.payload.resources;
                    if(resources){
                        var destinationAddress="";
                        for(var i=0; (i< destination.addr.length) && (destination.addr[i]!=0) ; i++ ){
                            destinationAddress = destinationAddress + String.fromCharCode(destination.addr[i]);
                        }
                        var endpoint = "oic://"+ destinationAddress +":"+ destination.port + resources[0].uri;
                        var newresource = new Object();
                        newresource.destination = destination;
                        newresource.uri = resources[0].uri;
                        map[endpoint] = newresource;
                        endpointList.push(endpoint);

                        iotivity.OCDoResource(
                            getHandleReceptacle,
                            iotivity.OCMethod.OC_REST_GET,
                            resources[0].uri,
                            destination,
                            null,
                            iotivity.OCConnectivityType.CT_DEFAULT,
                            iotivity.OCQualityOfService.OC_HIGH_QOS,
                            getResponseHandler,
                            null);
                    }
                    return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
                }
                iotivity.OCDoResource(
                    // The bindings fill in this object
                    handleReceptacle,
                    iotivity.OCMethod.OC_REST_DISCOVER,
                    // Standard path for discovering resources
                    specificURI,
                    // There is no destination
                    null,
                    // There is no payload
                    null,
                    iotivity.OCConnectivityType.CT_DEFAULT,
                    iotivity.OCQualityOfService.OC_HIGH_QOS,
                    specificResponseHandlerNget,
                    null);
            }
            else{
                iotivity.OCDoResource(
                    getHandleReceptacle,
                    iotivity.OCMethod.OC_REST_GET,
                    resource.uri,
                    resource.destination,
                    null,
                    iotivity.OCConnectivityType.CT_DEFAULT,
                    iotivity.OCQualityOfService.OC_HIGH_QOS,
                    getResponseHandler,
                    null);
            }
        }
        else if(cap.cid == "org.iotivity.putresource"){
            putResponseHandler = function( handle, response ) {
                responseflag = 1;
                console.log("Received response to PUT request:");
                console.log(JSON.stringify(response, null, 4));
                var putResult = JSON.stringify(response.payload);
                res.writeHead(200, {"Content-Type": "application/json"});
                res.end(putResult);
                return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
            }
            responseflag = 0;
            //var endpoint = cap.endpoint;
            var address = cap.params.address;
            var port = cap.params.port;
            var uri = cap.params.uri;
            var payload = cap.payload;
            var endpoint = "oic://"+ address +":"+ port + uri;
            var resource = map[endpoint];

            setTimeout(function(){
              if(responseflag == 0){
                  res.writeHead(200, {"Content-Type": "text/plain"});
                  res.end("resouce is not exist");
                  return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
              }
            },7000);

            if(resource == undefined)
            {
                console.log("resource is not exist");
                console.log("Find specific resource is not exist or not");
                //var specificURI = parseEndpoint(endpoint);
                var specificURI = "["+ address + "]:" + port +"/oic/res";
                specificResponseHandlerNput = function (handle, response) {
                    console.log("Received response to DISCOVER request:");
                    console.log(JSON.stringify(response, null, 4));
                    destination = response.addr,
                    resources = response && response.payload && response.payload.resources;
                    if(resources){
                        var destinationAddress="";
                        for(var i=0; (i< destination.addr.length) && (destination.addr[i]!=0) ; i++ ){
                            destinationAddress = destinationAddress + String.fromCharCode(destination.addr[i]);
                        }
                        var endpoint = "oic://"+ destinationAddress +":"+ destination.port + resources[0].uri;
                        var newresource = new Object();
                        newresource.destination = destination;
                        newresource.uri = resources[0].uri;
                        map[endpoint] = newresource;
                        endpointList.push(endpoint);

                        iotivity.OCDoResource(
                            handleReceptacle,
                            iotivity.OCMethod.OC_REST_PUT,
                            resources[0].uri,
                            destination,
                            {
                                type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                                values: payload
                            },
                            iotivity.OCConnectivityType.CT_DEFAULT,
                            iotivity.OCQualityOfService.OC_HIGH_QOS,
                            putResponseHandler,
                            null );
                    }
                    return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
                }
                iotivity.OCDoResource(
                    // The bindings fill in this object
                    handleReceptacle,
                    iotivity.OCMethod.OC_REST_DISCOVER,
                    // Standard path for discovering resources
                    specificURI,
                    // There is no destination
                    null,
                    // There is no payload
                    null,
                    iotivity.OCConnectivityType.CT_DEFAULT,
                    iotivity.OCQualityOfService.OC_HIGH_QOS,
                    specificResponseHandlerNput,
                    null);
            }
            else{
                iotivity.OCDoResource(
                    handleReceptacle,
                    iotivity.OCMethod.OC_REST_PUT,
                    resource.uri,
                    resource.destination,
                    {
                        type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                        values: payload
                    },
                    iotivity.OCConnectivityType.CT_DEFAULT,
                    iotivity.OCQualityOfService.OC_HIGH_QOS,
                    putResponseHandler,
                    null );
            }
        }
    }
}
