var request = require('request');

var intervalId,
    handleReceptacle = {},
    iotivity = require("../../iotivity-node/lowlevel");

iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT);

var map = new Object();
var responseflag = 0;

makeaddress = function(des){
    console.log()
    var address = "";
    for(var i=0; (i< des.addr.length) && (des.addr[i]!=0) ; i++ ){
        address = address + String.fromCharCode(des.addr[i]);
    }
    return address;
}

findresource = function(cap,res){
    findresourceHandler = function (handle, response) {
        console.log("Received response to DISCOVER request:");
        console.log(JSON.stringify(response, null, 4));
        var destination = response.addr,
        resources = response && response.payload && response.payload.resources;
        if(!resources)
            return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;

        var resourceCount = resources.length ? resources.length : 0;
        for (var index = 0; index < resourceCount; index++) {
            if(!(cap.resourceType === "all") && !(resources[ index ].types[0] === cap.resourceType))
                continue;

            var endpoint = "{\"address\":\"" + makeaddress(destination)
                           + "\",\"port\":\"" + destination.port + "\",\"uri\":\""
                           + resources[ index ].uri + "\"}";
            if(!map[endpoint])
            {
                var resource = new Object();
                resource.destination = destination;
                resource.uri = resources[ index ].uri;
                map[endpoint] = resource;
            }
            endpointList.push(endpoint);
        }
        return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
    }

    responseflag = 0;
    setTimeout(function(){
        res.status(200).json(JSON.parse("[" + endpointList + "]"));
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    },7000);

    var endpointList = [];

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
        findresourceHandler,
        // There are no header options
    null);
}

getresource = function(cap,res)
{
    getResponseHandler = function (handle, response) {
        if(responseflag==0){
            responseflag = 1;
            console.log("Received response to GET request:");
            console.log(JSON.stringify(response, null, 4));
            res.status(200).json(response.payload);
            return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
        }
    };
    specificResponseHandlerNget = function (handle, response) {
        console.log("Received response to DISCOVER request:");
        console.log(JSON.stringify(response, null, 4));
        destination = response.addr,
        resources = response && response.payload && response.payload.resources;
        if(resources){
            var endpoint = "oic://"+ makeaddress(destination) +":"+ destination.port + resources[0].uri;
            if(!map[endpoint])
            {
                var newresource = new Object();
                newresource.destination = destination;
                newresource.uri = resources[0].uri;
                map[endpoint] = newresource;
            }
            iotivity.OCDoResource(
                handleReceptacle,
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

    responseflag = 0;
    setTimeout(function(){
        if(responseflag==0){
            responseflag = 1;
            res.status(200).send("resouce is not exist");
            return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
        }
    },7000);

    var endpoint = "oic://"+ cap.params.address +":"+ cap.params.port + cap.params.uri;
    var resource = map[endpoint];
    if(resource == undefined)
    {
        console.log("resource is not exist");
        console.log("Find specific resource is not exist or not");
        var specificURI = "["+ cap.params.address + "]:" + cap.params.port +"/oic/res";

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
            handleReceptacle,
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

putresource = function(cap,res)
{
    putResponseHandler = function( handle, response ) {
        if(responseflag==0){
            responseflag = 1;
            console.log("Received response to PUT request:");
            console.log(JSON.stringify(response, null, 4));
            res.status(200).json(response.payload);
            return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
        }
    }
    specificResponseHandlerNput = function (handle, response) {
        console.log("Received response to DISCOVER request:");
        console.log(JSON.stringify(response, null, 4));
        destination = response.addr,
        resources = response && response.payload && response.payload.resources;
        if(resources){
            var endpoint = "oic://"+ makeaddress(destination) +":"+ destination.port + resources[0].uri;
            if(!map[endpoint])
            {
                var newresource = new Object();
                newresource.destination = destination;
                newresource.uri = resources[0].uri;
                map[endpoint] = newresource;
            }
            iotivity.OCDoResource(
                handleReceptacle,
                iotivity.OCMethod.OC_REST_PUT,
                resources[0].uri,
                destination,
                {
                    type: iotivity.OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                    values: cap.payload
                },
                iotivity.OCConnectivityType.CT_DEFAULT,
                iotivity.OCQualityOfService.OC_HIGH_QOS,
                putResponseHandler,
                null );
        }
        return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
    }

    responseflag = 0;
    setTimeout(function(){
      if(responseflag == 0){
          responseflag = 1;
          res.status(200).send("resouce is not exist");
          return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
      }
    },7000);

    var endpoint = "oic://"+ cap.params.address +":"+ cap.params.port + cap.params.uri;
    var resource = map[endpoint];
    if(resource == undefined)
    {
        console.log("resource is not exist");
        console.log("Find specific resource is not exist or not");
        //var specificURI = parseEndpoint(endpoint);
        var specificURI = "["+ cap.params.address + "]:" + cap.params.port +"/oic/res";

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
                values: cap.payload
            },
            iotivity.OCConnectivityType.CT_DEFAULT,
            iotivity.OCQualityOfService.OC_HIGH_QOS,
            putResponseHandler,
            null );
    }
}

observeresource = function(cap,res)
{
    obsResponseHandler = function( handle, response ) {
        console.log("Received response to OBSERVE request:");
        console.log(JSON.stringify(response, null, 4));
        var obsResult = JSON.stringify(response.payload);

        if(cap.chain!=null){
            console.log("Posting " + JSON.stringify(response) + "to " + cap.chain);
            var options = {
                url: cap.chain,
                json : true,
                method: 'POST',
                body: response.payload
            };
            console.log("JSON Body Sent = " + JSON.stringify(response.values));
            request(options, function (error, response, body) {
                console.log(error + "  - " + body);
            });
        }
        return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
    }
    specificResponseHandlerNput = function (handle, response) {
        console.log("Received response to DISCOVER request:");
        console.log(JSON.stringify(response, null, 4));
        destination = response.addr,
        resources = response && response.payload && response.payload.resources;
        if(resources){
            var endpoint = "oic://"+ makeaddress(destination) +":"+ destination.port + resources[0].uri;
            if(!map[endpoint])
            {
                var newresource = new Object();
                newresource.destination = destination;
                newresource.uri = resources[0].uri;
                map[endpoint] = newresource;
            }

            observeResult = iotivity.OCDoResource(
                handleReceptacle,
                iotivity.OCMethod.OC_REST_OBSERVE,
                resources[0].uri,
                destination,
                null,
                iotivity.OCConnectivityType.CT_DEFAULT,
                iotivity.OCQualityOfService.OC_HIGH_QOS,
                obsResponseHandler,
                null );
        }
        return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
    }

    var endpoint = "oic://"+ cap.params.address +":"+ port + cap.params.uri;
    var resource = map[endpoint];
    if(resource == undefined)
    {
        console.log("resource is not exist");
        console.log("Find specific resource is not exist or not");
        var specificURI = "["+ cap.params.address + "]:" + cap.params.port +"/oic/res";

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
        observeResult = iotivity.OCDoResource(
            observeHandleReceptacle,
            iotivity.OCMethod.OC_REST_OBSERVE,
            resource.uri,
            resource.destination,
            null,
            iotivity.OCConnectivityType.CT_DEFAULT,
            iotivity.OCQualityOfService.OC_HIGH_QOS,
            obsResponseHandler,
            null );
    }
    res.status(200).json("Started Observing " + uri + "@" + JSON.stringify(address));
    return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
}


module.exports = {
    init: function () {
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
                        "property" : "value"
                    },
                    "tags": [
                        "put reosurce properties and value"
                    ]
                },
                {
                  "cid": "org.iotivity.observeresource",
                  "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                  "endpointtype": "IOTIVITY",
                  "operation": "GET",
                  "resourceID" : "",
                  "chain" : ""
                }
            ]
        };
        return template;
    },
    request: function (cap, auth, res) {
        intervalId = setInterval(function () {
            iotivity.OCProcess();
        }, 1000);
        if(cap.cid == "org.iotivity.findresource"){
            findresource(cap,res);
        }
        else if(cap.cid == "org.iotivity.getresource"){
            getresource(cap,res);
        }
        else if(cap.cid == "org.iotivity.putresource"){
            putresource(cap,res);
        }
        else if(cap.cid == "org.iotivity.observeresource"){
            oberserveresource(cap,res);
        }
    }
}
