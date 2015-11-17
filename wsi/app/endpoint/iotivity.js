var intervalId,
        handleReceptacle = {},
        iotivity = require("../../iotivity-node/lowlevel");

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
                  "resourceType" : "all"
                },
                {
                  "cid": "org.iotivity.getresource",
                  "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                  "endpointtype": "IOTIVITY",
                  "operation": "GET",
                  "resourceID" : "",
                  "params": 
                   {
                        "address": "server ip address",
                        "port": "server port",
                        "uri": "server's uri"
                   }
                },
                {
                  "cid": "org.iotivity.putresource",
                  "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                  "endpointtype": "IOTIVITY",
                  "operation": "POST",
                  "resourceID" : "",
                  "params": {
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
                }
              ]
        };
        return template;
    },
    request: function (cap, auth, res) {
        intervalId = setInterval(function () {
            iotivity.OCProcess();
        }, 1000);        

        var resourceID = [];        

        if(cap.cid == "org.iotivity.findresource"){
            setTimeout(function(){          
              res.status(200).json(resourceID);
              return iotivity.OCStackApplicationResult.OC_STACK_DELETE_TRANSACTION;
            },7000);
            if( cap.resourceType == "all"){                
                /*console.log("findresource resource : " + cap.resourceType);*/
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
                function (handle, response) {
                    console.log("Received response to DISCOVER request:");
                    console.log(JSON.stringify(response, null, 4));
                    var index,
                    destination = response.addr,
                    getHandleReceptacle = {},
                    resources = response && response.payload && response.payload.resources;
                    if(resources){
                        resourceCount = resources.length ? resources.length : 0;                                                       
                        for (index = 0; index < resourceCount; index++) {                                                    
                            var destString = JSON.stringify(destination); 
                            resourceID.push(destString + ";"+ resources[ index ].uri);
                        }                        
                    }
                    return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
                },
                // There are no header options
                null);                
            }else{
                console.log("findresource resource : " + cap.resourceType);
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
                function (handle, response) {
                    console.log("Received response to DISCOVER request:");
                    console.log(JSON.stringify(response, null, 4));          
                    var index,
                    destination = response.addr,
                    getHandleReceptacle = {},                                    
                    resources = response && response.payload && response.payload.resources;
                    if(resources){
                        var resourceCount = resources.length ? resources.length : 0;
                        console.log("resourceCount URI : " + resourceCount);        

                        // If the sample URI is among the resources, issue the GET request to it
                        for (index = 0; index < resourceCount; index++) {
                            console.log("Resource Type : ");                        
                            console.log(resources[ index ].types[0]);                            

                            if (resources[ index ].types[0] === cap.resourceType) {                                                                              
                               var destString = JSON.stringify(destination);                   
                               resourceID.push(destString + ";"+ resources[ index ].uri);
                            }                     
                        }                        
                    }
                    return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
                },
                // There are no header options
                null);                                
            }
        }
        else if(cap.cid == "org.iotivity.getresource"){         
            var resourceID = cap.resourceID.split(";");
            console.log("Resource ID " + resourceID)
            
            var address = JSON.parse(resourceID[0]);
            var uri = resourceID[1];
            console.log("uri " + uri)

            getResponseHandler = function (handle, response) {
                console.log("Received response to GET request:");
                console.log(JSON.stringify(response, null, 4));                                    
                var getResult = JSON.stringify(response.payload);                
                res.writeHead(200, {"Content-Type": "application/json"});                
                res.end(getResult);
                return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
            };
            var getHandleReceptacle = {};

            iotivity.OCDoResource(
            getHandleReceptacle,
            iotivity.OCMethod.OC_REST_GET,
            uri,
            address,
            null,
            iotivity.OCConnectivityType.CT_DEFAULT,
            iotivity.OCQualityOfService.OC_HIGH_QOS,
            getResponseHandler,
            null);
        }
        else if(cap.cid == "org.iotivity.putresource"){
            var resourceID = cap.resourceID.split(";");         
            var address = JSON.parse(resourceID[0]);
            var uri = resourceID[1];

            putResponseHandler = function( handle, response ) {
                console.log("Received response to PUT request:");
                console.log("TEST 2" + JSON.stringify(response, null, 4));                                    
                var putResult = JSON.stringify(response.payload);
                console.log("TEST 3" + JSON.stringify(response.payload));                                    
                res.status(200).json(putResult);
                return iotivity.OCStackApplicationResult.OC_STACK_KEEP_TRANSACTION;
            }
            
            var payload = cap.payload;
            
            console.log("Payload = " + JSON.stringify(payload));
            
            
            iotivity.OCDoResource(
            handleReceptacle,
            iotivity.OCMethod.OC_REST_PUT,
            uri,
            address,
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
