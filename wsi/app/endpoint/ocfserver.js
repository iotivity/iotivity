var request = require('request');
var intervalId,
handleReceptacle = {},
iotivity = require("../../iotivity-node/lowlevel");

makeaddress = function(des){
    console.log()
    var address = "";
    for(var i=0; (i< des.addr.length) && (des.addr[i]!=0) ; i++ ){
        address = address + String.fromCharCode(des.addr[i]);
    }
    return address;
}

var map = new Object();
iotivity.OCInit(null, 0, iotivity.OCMode.OC_SERVER);
iotivity.OCSetDeviceInfo( { deviceName: "RVI" } );
iotivity.OCSetPlatformInfo( {
	platformID: "Linux",
	manufacturerName: "JLR"
} );

intervalId = setInterval( function() {
	iotivity.OCProcess();
}, 1000 );

console.log("Server mode initialized");

function handleRequest( flag, request ) {
	console.log( "Entity handler called with flag = " + flag + " and the following request:" );
	console.log( JSON.stringify( request, null, 4 ) );
	
	// If we find the magic question in the request, we return the magic answer
	if ( request && request.payload && request.payload.values &&
			request.payload.values.question ===
			"How many angels can dance on the head of a pin?" ) {

		iotivity.OCDoResponse( {
			requestHandle: request.requestHandle,
			resourceHandle: request.resource,
			ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
			payload: {
				type: 4,
				values: {
					"answer": "As many as wanting."
				}
			},
			resourceUri: sampleUri,
			sendVendorSpecificHeaderOptions: []
		} );

		return iotivity.OCEntityHandlerResult.OC_EH_OK;
	}	
	
	
	
//    if(request.method === iotivity.OCMethod.OC_REST_PUT) {
//        console.log(request.payload);
//        iotivity.OCDoResponse( {
//            requestHandle: request.requestHandle,
//            resourceHandle: request.resource,
//            ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
//            payload: null,
//            resourceUri: resUri,
//            sendVendorSpecificHeaderOptions: []
//        } );
//        return iotivity.OCEntityHandlerResult.OC_EH_OK;
//    }
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
	
	console.log("result " + result);
    res.status(200).json("Created Resource " + cap.params.uri + " @ " + result);
}



module.exports = {
    init: function () {
        var template = {
            "handler": "ocfserver",
            "sid": "org.openinterconnect.ocfserver",
            "capability": [
                {
                    "cid": "org.openinterconnect.createresource",
                    "endpointtype": "OCFSERVER",
                    "operation": "CREATE",
                    "chain" : "notification URI",
                    "params":
                    {
                        "uri": "server's uri",
                        "type" : "core.what",
                    },
                    "tags": [
                        "create an OCF server with resource"
                    ]
                },
                {
                    "cid": "org.openinterconnect.putresource",
                    "endpoint": "oic://{{address}}:{{port}}{{uri}}",
                    "endpointtype": "OCFSERVER",
                    "operation": "PUT",
                    "params":
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
        if(cap.cid == "org.openinterconnect.createresource"){
            createresource(cap,res);
        }
    }
}
