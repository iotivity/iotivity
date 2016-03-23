var intervalId,
    handleReceptacle = {},
    resUri = "/a/rvi",
    chain = "http://localhost:8081/callback",
    iotivity = require("../../iotivity-node/lowlevel"),
    request = require('request'),
    map = new Object(),
    responseflag = 0,
    rviRep;

var updateresource = {
        "cid": "org.openinterconnect.updateresource",
        "endpointtype": "OCF",
        "operation": "UPDATE",
		"utf8Data": {
			"params":{
				"parameters":{
					"target" : "UPDATEOCFRESOURCE",
					"status" : {}
				}
			}
		},
        "tags": [
            "Update to a created OCF server resource"
        ]
    };

var findocfresponse = {
			"utf8Data": {
				"params":{
					"parameters":{
						"target" : "FINDOCFDEVICES",
						"status" : {}
					}
				}
			}
};


iotivity.OCInit(null, 0, iotivity.OCMode.OC_CLIENT_SERVER);
iotivity.OCSetDeviceInfo( { deviceName: "OCF-RVI Gateway" } );
iotivity.OCSetPlatformInfo( {
	platformID: "Linux",
	manufacturerName: "Samsung"
} );

makeaddress = function(des){
    console.log()
    var address = "";
    for(var i=0; (i< des.addr.length) && (des.addr[i]!=0) ; i++ ){
        address = address + String.fromCharCode(des.addr[i]);
    }
    return address;
}

function handleRequest( flag, req ) {
	console.log( "Entity handler called with flag = " + flag);
    if(req.method === iotivity.OCMethod.OC_REST_PUT) {
    	var rep = req.payload.values;
    	console.log(req.payload);

    	//choose the first one
    	var key = Object.keys(rep)[0];
    	var value = rep[key];

    	console.log("Key = " + key + " Value = " + value);
    	
    	//set the value
    	rviRep[key] = value;
    	
        iotivity.OCDoResponse( {
            requestHandle: req.requestHandle,
            resourceHandle: req.resource,
            ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
            payload: null,
            resourceUri: resUri,
            sendVendorSpecificHeaderOptions: []
        });

        console.log("Posting " + JSON.stringify(rviRep) + " to " + chain);
        updateresource.utf8Data.params.parameters.status = rviRep;
        updateresource.utf8Data.params.parameters.status.target = "UPDATEOCFRESOURCE";
        
        
        
        
        var options = {
            url: chain,
            json : true,
            method: 'POST',
            body: JSON.stringify(updateresource)
        };
        console.log("JSON Body Sent = " + JSON.stringify(updateresource));
        request(options, function (error, response, body) {
            console.log(error + "  - " + body);
        });
        
        return iotivity.OCEntityHandlerResult.OC_EH_OK;
    }
    if(req.method === iotivity.OCMethod.OC_REST_GET) {
        console.log(request.payload);
    	if(rviRep==null)  //simply echo back
    		request.payload;

    	iotivity.OCDoResponse( {
            requestHandle: request.requestHandle,
            resourceHandle: request.resource,
            ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
            payload: rviRep,
            resourceUri: resUri,
            sendVendorSpecificHeaderOptions: []
        } );
        return iotivity.OCEntityHandlerResult.OC_EH_OK;
    }
    return iotivity.OCEntityHandlerResult.OC_EH_ERROR
	
}

createresource = function(cap,res)
{
	console.log("Uri = " + cap.params.uri + " Type = " + cap.params.type);
	result = iotivity.OCCreateResource(
	        handleReceptacle,
	        cap.params.type,
	        iotivity.OC_RSRVD_INTERFACE_DEFAULT,
	        cap.params.uri,
	        handleRequest,
	        iotivity.OCResourceProperty.OC_DISCOVERABLE);
	
	rviRep = cap.payload;
	console.log("Created Resource " + JSON.stringify(rviRep));
    res.status(200).json("Created Resource " + cap.params.uri + " @ " + result);
}

findresource = function(cap,res){
    findresourceHandler = function (handle, response) {
        console.log("Received response to DISCOVER request:");
        console.log(JSON.stringify(response, null, 4));
        var destination = response.addr,
        resources = response && response.payload && response.payload.resources;
        if(!resources){
            console.log("No resources are found.");
            return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
        }

        var resourceCount = resources.length ? resources.length : 0;
        for (var index = 0; index < resourceCount; index++) {
            if(!(cap.resourceType === "all") && !(resources[ index ].types[0] === cap.resourceType))
                continue;

            var endpoint = "{\"address\":\"" + makeaddress(destination)
                           + "\",\"port\":\"" + destination.port + "\",\"uri\":\""
                           + resources[ index ].uri + "\"}";
            if(!map[endpoint])
            {
            	//filter out standard services.
            	if(resources[ index ].uri.indexOf("/oic") !== 0){
            		console.log("Is this a standard service. " + resources[ index ].uri.indexOf("/oic") + " URI = " + resources[ index ].uri);
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

    responseflag = 0;
    setTimeout(function(){
    	findocfresponse.utf8Data.params.parameters.status = endpointList;
        res.status(200).send(findocfresponse);
        return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
    },10000);

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
        console.log("Received response to GET request:");
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
        console.log("Received response to PUT request:");
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
        console.log("Received response to OBSERVE request:");
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

    var endpoint = "oic://"+ cap.params.address +":"+ cap.params.port + cap.params.uri;
    var resource = map[endpoint];
    if(resource == undefined)
    {
        console.log("resource is not exist");
        console.log("Find specific resource is not exist or not");
        var specificURI = "["+ cap.params.address + "]:" + cap.params.port +"/oic/res";

        iotivity.OCDoResource(
            handleReceptacle,
            iotivity.OCMethod.OC_REST_DISCOVER,
            specificURI,
            null,
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
    res.status(200).json("Started Observing " + cap.params.uri + "@" + JSON.stringify(cap.params.address));
    return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
}


module.exports = {
    init: function () {
        var template = {
            "handler": "ocf",
            "sid": "org.openinterconnect",
            "capability": [
                {
                    "cid": "org.openinterconnect.findresource",
                    "endpointtype": "OCF",
                    "operation": "GET",
                    "resourceType" : "resourceType or all"
                },
                {
                    "cid": "org.openinterconnect.getresource",
                    "endpoint": "oic://{{address}}:{{port}}{{uri}}",
                    "endpointtype": "OCF",
                    "operation": "GET",
                    "params":
                    {
                        "address": "server ip address",
                        "port": "server port",
                        "uri": "server's uri"
                    }
                },
                {
                    "cid": "org.openinterconnect.putresource",
                    "endpoint": "oic://{{address}}:{{port}}{{uri}}",
                    "endpointtype": "OCF",
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
                  "cid": "org.openinterconnect.observeresource",
                  "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                  "endpointtype": "OCF",
                  "operation": "GET",
                  "resourceID" : "",
                  "chain" : ""
                },
                {
                    "cid": "org.openinterconnect.createresource",
                    "endpointtype": "OCF",
                    "operation": "CREATE",
                    "chain" : "notification URI",
                    "params":
                    {
                        "uri": "server's uri",
                        "type" : "core.what",
                    },
                    "payload" : {},
                    "tags": [
                        "create an OCF server with resource"
                    ]
                }
            ]
        };
        return template;
    },
    request: function (cap, auth, res) {
        intervalId = setInterval(function () {
            iotivity.OCProcess();
        }, 1000);
        if(cap.cid == "org.openinterconnect.findresource"){
            findresource(cap,res);
        }
        else if(cap.cid == "org.openinterconnect.getresource"){
            getresource(cap,res);
        }
        else if(cap.cid == "org.openinterconnect.putresource"){
            putresource(cap,res);
        }
        else if(cap.cid == "org.openinterconnect.observeresource"){
            observeresource(cap,res);
        }
        else if(cap.cid == "org.openinterconnect.createresource"){
            createresource(cap,res);
        }

    }
}
