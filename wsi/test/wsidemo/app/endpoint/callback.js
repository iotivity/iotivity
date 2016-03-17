var request = require('request');

var FSM = {
	NOTREADY : 0,
	RVICONNECT: 1,
	REGHOMECONTROL: 2,
	REGCALLBACK: 3,
	GETRVISERVICES: 4,
	FINDOCFDEVICES: 5,
	OBSERVEHVAC: 6,
	GETHVAC : 7,
	SETHVAC : 8,
	GETLOCATION : 9,
	COMINGHOME : 10,
	SMARTHOMESTATUS : 11,
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
          "description": "Connect to RVI.",
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
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
	        "parameters": {
                "target" : "GETHVAC"
	        }
		}
	},
    {//2
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
		"method": "get_available_services",
		"params" : {}
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
   	    "endpointtype": "OCFCLIENT",
   	    "operation": "GET",
   	    "resourceType" : "all"
   	},
   	{
   	    "cid": "org.openinterconnect.getresource",
   	    "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
   	    "endpointtype": "OCFCLIENT",
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
   	    "endpointtype": "OCFCLIENT",
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
   	    "cid": "org.openinterconnect.observeresource",
   	    "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
   	    "endpointtype": "OCFCLIENT",
   	    "operation": "GET",
   	    "chain" : "http://localhost:8081/callback",
   	    "params": 
   	    {
   	        "address": "server ip address",
   	        "port": "server port",
   	        "uri": "server's uri"
   	    }            
   	}
];

var ocfdevices = [];


var wsiFSM = function(error, response, body) {
	if(!body || response.statusCode > 399){
        console.log("Failed Response = " + body);
		return;
	}
	console.log("State " + state + " Response = " + JSON.stringify(body));
	switch(state)
    {
    	case FSM.RVICONNECT:{
    		console.log("RVI Connected.");
    		state = FSM.REGHOMECONTROL;
    		rvibody[0].params.service_name = "homecontrol";
    		rvimsg[1].params = rvibody[0];
    		var res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.REGHOMECONTROL:{
    		console.log("RVI HomeControl Service Registered.");
    		state = FSM.REGCALLBACK;
    		rvibody[0].params.service_name = "callback";
    		rvimsg[1].params = rvibody[0];
    		var res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.REGCALLBACK:{
    		console.log("RVI Callback Service Registered.");
    		state = FSM.GETRVISERVICES;
    		rvimsg[1].params = rvibody[4];
    		var res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.GETRVISERVICES:{
    		console.log("RVI Service List Received.");
    		state = FSM.FINDOCFDEVICES;
    		var res = post(ocfuri, ocfbody[0]);
    		break;
    	}
    	case FSM.FINDOCFDEVICES:{
    		console.log("OCF Device List Received.");
    		ocfdevices = body;
    		state = FSM.OBSERVEHVAC;
    		var len = ocfdevices.length;
    		for (var i = 0; i < len; i++) {
    		    if(ocfdevices[i].uri.indexOf("/rvi/hvac")==0){
    		    	ocfbody[3].params = ocfdevices[i];
    	    		var res = post(ocfuri, ocfbody[3]);
    	    		console.log("Start Observing Vehicle HVAC Changes");
    	    		break;
    		    }
    		}    		
    	}
    	case FSM.OBSERVEHVAC:{
    		console.log("OCF Device List Received.");
    		//var res = post(ocfuri, ocfbody[0]);
    		//state = FSM.OBSERVE;
    		break;
    	}
    }
    
//    rvibody[0].register_service.params.service_name = "homecontrol";
//    rvimsg[1].params = rvibody[0];
//    var res = $http.post(uri, rvimsg[1]); //register services
    
};

var post = function(uri, b){
	console.log("Connecting to RVI " + rviuri);
    //console.log("Posting " + b + "to " + rviuri );
    var options = {
        url: rviuri ,
        json : true,
        method: 'POST',
        body: b
    };
    request(options, wsiFSM);        
    //console.log("JSON Body Sent = " + JSON.stringify(b));
};

var start = function(){
	state = FSM.RVICONNECT;//connect to rvi
	console.log("Callback Initialization");
	post(rviuri, rvimsg[0]);
};

/*
executeRVICap : function(body){
    var uri = "http://localhost:8080/wsi/cap/org.genivi.rvi";
    console.log("Making a POST HTTP Request " + uri);
    var res = $http.post(uri, document.getElementById('rvicap').value);
    //res.success = wsi_success;
    //res.error = wsi_error;
},
executeIoTivityCap : function(body) {
    var uri = "http://localhost:8080/wsi/cap/org.openinterconnect";
    console.log("Making a POST HTTP Request " + uri);
    reqbody = JSON.parse(body);
    
    var res = $http.post(uri, reqbody);
    res.success(function(data, status, headers, config) {
        console.log("Success Response = " + JSON.stringify(data));

        var addresses = JSON.parse(JSON.stringify(data));
        
        if(addresses instanceof Array){
            for(var i = 0; i<addresses.length; i++)
            {
                var obj = addresses[i];
                console.log("Checking : " + obj.uri);
                if(obj.uri == "/a/wsilight")
                {
                    getresource.params.address = obj.address;
                    getresource.params.port = obj.port;
                    getresource.params.uri = obj.uri;

                    putresource.params.address = obj.address;
                    putresource.params.port = obj.port;
                    putresource.params.uri = obj.uri;

                }
            }
        }
        $scope.scene1updates.push({title: 'Success', content: data});
    });
    res.error(function(data, status, headers, config) {
        console.log("Failed Response = " + data );
        $scope.scene1updates.push({title: 'Failure', content: data});
    });
},

*/

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
                        "cid":"org.wsidemo.observeresult",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Gets the current status of a resource and its properties.",
                        "endpoint":"http://localhost:8081/callback",
                        "endpointtype":"REST",
                        "operation":"OBSERVERESULT",
                        "params":{},
                        "tags":[
                            "iotivity",
                            "observe"
                        ]
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
        var rvibody = req.body;
        res.sendStatus(200);

        // Send notify to Controller.

        
        if(rvibody.parameters && rvibody.parameters.target == "LOCATIONSTATUS"){
        }
        if(rvibody.parameters && rvibody.parameters.target == "HVACSTATUS"){
        }
        if(rvibody.parameters && rvibody.parameters.target == "SETHVAC"){
        }
        if(rvibody.parameters && rvibody.parameters.target == "GETHVAC"){
        }
        if(rvibody.parameters && rvibody.parameters.target == "SMARTHOMESCENARIO"){
        }
    },
    start: start,
}
console.log("App initialized");


