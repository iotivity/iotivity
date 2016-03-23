var request = require('request');
var intervalId,
handleReceptacle = {},
iotivity = require("../../iotivity-node/lowlevel");


var map = new Object();
iotivity.OCInit(null, 0, iotivity.OCMode.OC_SERVER);

intervalId = setInterval(function () {
			iotivity.OCProcess();
        }, 1000);

function handleRequest( flag, request ) {
    if(request.method === iotivity.OCMethod.OC_REST_PUT) {
        console.log(request.payload);

        iotivity.OCDoResponse( {
            requestHandle: request.requestHandle,
            resourceHandle: request.resource,
            ehResult: iotivity.OCEntityHandlerResult.OC_EH_OK,
            payload: null,
            resourceUri: resUri,
            sendVendorSpecificHeaderOptions: []
        } );
        return iotivity.OCEntityHandlerResult.OC_EH_OK;
    }
    return iotivity.OCEntityHandlerResult.OC_EH_ERROR;
}

createresource = function(cap,res)
{
	iotivity.OCCreateResource(
	        handleReceptacle,
	        cap.params.type,
	        iotivity.OC_RSRVD_INTERFACE_DEFAULT,
	        cap.params.uri,
	        handleRequest,
	        iotivity.OCResourceProperty.OC_DISCOVERABLE );
	resUri = cap.params.uri;
    res.status(200).json("Created Resource" + cap.params.uri + "@" + JSON.stringify(cap.params.address));
}



module.exports = {
    init: function () {
        var template = {
            "handler": "ocfserver",
            "sid": "org.openinterconnect",
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
