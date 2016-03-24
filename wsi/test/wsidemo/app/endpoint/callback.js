var request = require('request');

var FSM = {
	NOTREADY : 0,
	RVICONNECT: 1,
	REGHOMECONTROL: 2,
	REGCALLBACK: 3,
	GETRVISERVICES: 4,
	GETHVAC : 5,
	GETLOCATION : 6,
	FINDOCFDEVICES: 7,
	MAKEOCFRVIDEVICE: 8,
	COMINGHOME : 9,
	SMARTHOMESTATUS : 10,
	SETHVAC : 11,
	UNREGHOMECONTROL: 12,
	UNREGCALLBACK: 13,
	READY: 14
};
var state = FSM.NOTREADY;
var rviuri = "http://localhost:8080/wsi/cap/org.genivi.rvi";
var ocfuri = "http://localhost:8080/wsi/cap/org.openinterconnect";

var rvimsg = [
      {
          "cid": "org.genivi.rvi.connect",
          "isauthrequired": "false",
          "description": "Connect to RVI.",
          "endpoint": "ws://localhost:9008/",
          "endpointtype": "rvi",
          "chain" : "http://localhost:8081/callback",
          "operation": "RVICONNECT",
          "params" : {}
      },
      {
          "cid": "org.genivi.rvi.send",
          "isauthrequired": "false",
          "description": "Send to RVI.",
          "endpoint": "ws://localhost:9008/",
          "endpointtype": "rvi",
          "operation": "RVISEND",
          "params" : {}
      }
];

var rvibody = [
	{//0
		"jsonrpc":"2.0",
		"id":"1",
		"method": "register_service",
		"params": {
			"timeout":1459388884,
			"service_name": "servicename"
		}
	},
 	{//1
		"jsonrpc":"2.0",
		"id":"1",
		"method": "get_available_services",
		"params" : {}
    },
	{//2
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
	        "parameters": {
                "target" : "GETHVAC"
	        }
		}
	},
	{//3
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
			"parameters":{
				"target" : "GETLOCATION"
			}
		}
	},
	{//4
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
			"parameters":{
				"target" : "SETHVAC",
				"function": "function",
				"newValue": "value"
			}
		}
	},
	{//5
		"jsonrpc":"2.0",
		"id":"1",
		"method": "unregister_service",
		"params": {
			"timeout":1459388884,
			"service_name": "servicename"
		}
	}
];

var ocfbody = [
   	{
   	    "cid": "org.openinterconnect.findresource",                    
   	    "endpointtype": "OCF",
   	    "operation": "GET",
   	    "resourceType" : "all"
   	},
    {
        "cid": "org.openinterconnect.createresource",
        "endpointtype": "OCF",
        "operation": "CREATE",
        "chain" : "notification URI",
        "params":
        {
            "uri": "/a/rvi",
            "type" : "core.rvi",
        },
        "payload": {},        
        "tags": [
            "create an OCF server with RVI resource"
        ]
    },
   	{
   	    "cid": "org.openinterconnect.getresource",
   	    "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
   	    "endpointtype": "OCF",
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
   	    "cid": "org.openinterconnect.putresource",
   	    "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
   	    "endpointtype": "OCF",
   	    "operation": "POST",
   	    "resourceID" : "",
   	    "params": {
   	          "address": "server ip address",
   	          "port": "server port",
   	          "uri": "server's uri"
   	     },
   	    "payload": {
   	         "property" : "value"
   	     },
   	    "tags": [
   	      "put reosurce properties and value"
   	    ]
   	},
    {
        "cid": "org.openinterconnect.updateresource",
        "endpointtype": "OCF",
        "operation": "UPDATE",
        "payload": {},        
        "tags": [
            "Update to a created OCF server resource"
        ]
    }    
];

var ocfdevices = [];


var wsiFSM = function(error, response, body) {
	if(!body || response.statusCode > 399){
        console.log("Failed Response = " + body);
		return;
	}
	var res;
	console.log("State " + state + " Response = " + JSON.stringify(body));
	switch(state)
    {
    	case FSM.RVICONNECT:{
    		console.log("RVI Connected.");
    		state = FSM.REGHOMECONTROL;
    		rvibody[0].params.service_name = "homecontrol";
    		rvimsg[1].params = rvibody[0];
    		res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.REGHOMECONTROL:{
    		console.log("RVI HomeControl Service Registered.");
    		state = FSM.REGCALLBACK;
    		rvibody[0].params.service_name = "callback";
    		rvimsg[1].params = rvibody[0];
    		res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.REGCALLBACK:{
    		console.log("RVI Callback Service Registered.");
    		state = FSM.GETRVISERVICES;
    		rvimsg[1].params = rvibody[1];
    		res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.GETRVISERVICES:{
    		console.log("RVI Service List Received.");
    		state = FSM.GETHVAC;
    		rvimsg[1].params = rvibody[2];
    		res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.FINDOCFDEVICES:{
    		console.log("OCF Device List Received.");
            var rcvbody = body.utf8Data;
            var params = rcvbody.params.parameters;
            console.log("Processing " + params.target);
            ocfdevices = params.status;
            state = FSM.MAKEOCFRVIDEVICE;
    		var res = post(ocfuri, ocfbody[1]);
    		break;
    	}
    	case FSM.MAKEOCFRVIDEVICE:
    	case FSM.SETHVAC:
    	case FSM.GETHVAC:
    	case FSM.GETLOCATION:
    	case FSM.READY:{
    		console.log("OCF RVI Device Created....Gateway Ready");
    		state = FSM.READY;
    		break;
    	}
    }
};

var post = function(uri, b){
    console.log("Posting " + JSON.stringify(b) + "to " + rviuri + " in state = " + state);
    var options = {
        url: rviuri ,
        json : true,
        method: 'POST',
        body: b
    };
    request(options, wsiFSM);        
};

var start = function(){
	state = FSM.RVICONNECT;//connect to rvi
	console.log("Callback Initialization");
	post(rviuri, rvimsg[0]);
};


module.exports = {
    init: function(app) {
        var template = {
                "sid":"org.wsidemo.callback",
                "handler" : "callback",
                "description":"The app to handle responses from WSI.",
                "platforms":[
                    {
                        "name":"Linux",
                        "minPlatformVersion":"3.0",
                        "minApiVersion":"0.1",
                        "appURL":"http://wsidemoapp.com",
                        "minAppVersion":"1.0"
                    }
                ],
                "capability":[
                    {
                        "cid":"org.wsi.ocfcallback",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Gets the current status of a resource and its properties.",
                        "endpoint":"http://localhost:8081/callback",
                        "endpointtype":"REST",
                        "operation":"OCFRVIUPDATE",
                        "params":{}
                    },
                    {
                        "cid":"org.genivi.rvicallback",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Callback invocations from OCF-RVI.",
                        "endpoint":"http://localhost:8081/callback",
                        "endpointtype":"REST",
                        "operation":"RVICALLBACK",
                        "params":{},
                    }
                ]
        };
        return template;
    },
    request: function (req, res) {
        console.log("Received RVI Callback : "+ JSON.stringify(req.body));
        res.sendStatus(200);
        
        console.log("Content Type = " + JSON.stringify(req.headers));
        
        var rcvbody;
        if(req.body.utf8Data instanceof Object)
        	rcvbody = req.body.utf8Data;
        else
        	rcvbody = JSON.parse(req.body.utf8Data);
        
    	var params = rcvbody.params.parameters;
        console.log("Processing " + params.target);
        
        if(params && params.target == "GETHVAC"){
    		state = FSM.GETLOCATION;
    		for(var key in params.status){
    			ocfbody[1].payload[key] = params.status[key];
    			console.log(key + " = " +  params.status[key]);
    		}
    		rvimsg[1].params = rvibody[3];
    		res = post(rviuri, rvimsg[1]);
        }
        if(params && params.target == "GETLOCATION"){
        	for(var key in params){
    			ocfbody[1].payload[key] = params[key];
    			console.log(key + " = " +  params[key]);
    		}
    		state = FSM.FINDOCFDEVICES;
        	var res = post(ocfuri, ocfbody[0]);
        }
        if(params && params.target == "UPDATEOCFRESOURCE"){
        	//send to RVI
        	console.log("Send Update to RVI." + params.status.function);
    		state = FSM.SETHVAC;
    		rvibody[4].params.parameters.function = params.status.function;
    		rvibody[4].params.parameters.newValue = params.status.newValue;
    		rvimsg[1].params = rvibody[4];
    		res = post(rviuri, rvimsg[1]);

        }
        if(params && params.target == "SMARTHOMESCENARIO"){
        	//series of OCF GET requests - SmartHome Status
        	//series of OCF POST requests - Coming Home

        }
    },
    start: start,
}
console.log("App initialized");


