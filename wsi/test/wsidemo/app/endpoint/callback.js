var request = require('request');
var ocfdevices = [];
var rviuri = "http://localhost:8080/wsi/cap/org.genivi.rvi";
var ocfuri = "http://localhost:8080/wsi/cap/org.openinterconnect";
var smstatus, chstatus;
var smcount = 0, chcount = 0;
var toggle = 0;

var FSM = {
	NOTREADY : 0,
	RVICONNECT: 1,
	REGHOMECONTROL: 2,
	REGCALLBACK: 3,
	GETRVISERVICES: 4,
	GET_VEHICLE_HVAC : 5,
	GET_VEHICLE_LOCATION : 6,
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
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/callback",
			"parameters":{
				"target" : "SMARTHOMESCENARIO",
				"text": "scenario name",
				"result": "Success/Failure",
				"status" : {}
			}
		}
	},	
	{//6
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
   	{//0
   	    "cid": "org.openinterconnect.findresource",                    
   	    "endpointtype": "OCF",
   	    "operation": "GET",
   	    "resourceType" : "all"
   	},
    {//1
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
   	{//2
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
   	{//3
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
   	    "payload": {},
   	    "tags": [
   	      "put reosurce properties and value"
   	    ]
   	},
    {//4
        "cid": "org.openinterconnect.ocfupdate",
        "endpointtype": "OCF",
        "operation": "UPDATE",
        "payload": {},        
        "tags": [
            "RVI Resource updated by OCF"
        ]
    },    
    {//5
        "cid": "org.openinterconnect.rviupdate",
        "endpointtype": "OCF",
        "operation": "UPDATE",
        "params":
        {
            "uri": "/a/rvi",
            "type" : "core.rvi",
        },
        "payload": {},        
        "tags": [
            "RVI Resource updated by RVI"
        ]
    }    
];

var wsiFSM = function(error, response, body) {
	if(!body || response.statusCode > 399){
        console.log("Failed Response = " + body);
        if(state==FSM.COMINGHOME || state==FSM.SMARTHOMESTATUS)
    	{
        	chcount = -1;
        	rvibody[5].params.parameters.result = "Failure";
        	console.log("Coming Home Scenario Failed");
        }else{
        	state = FSM.READY;
    		return;
        }
	}
	var res;
	if(body)console.log("State " + state + " Response = " + JSON.stringify(body));
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
    		state = FSM.GET_VEHICLE_LOCATION;
    		rvimsg[1].params = rvibody[3];
    		res = post(rviuri, rvimsg[1]);
    		break;
    	}
    	case FSM.FINDOCFDEVICES:{
            var rcvbody = body.utf8Data;
            var params = rcvbody.params.parameters;
            ocfdevices = params.status;
    		console.log("OCF Device List Received." + ocfdevices);
            state = FSM.MAKEOCFRVIDEVICE;
            console.log("Making RVI device with " + JSON.stringify(ocfbody[1].payload));
    		var res = post(ocfuri, ocfbody[1]);
    		break;
    	}
    	case FSM.SMARTHOMESTATUS : {
    		smstatus += body;
    		console.log("SmartHome Status = " + smstatus);
    		if(smcount < ocfdevices.length && smcount>=0){
    			console.log("Checking " + ocfdevices[smcount]);
    			var device = JSON.parse(ocfdevices[smcount]);
    			ocfbody[2].params.address = device.address;	
    			ocfbody[2].params.port = device.port;
    			ocfbody[2].params.uri = device.uri;
    			smcount = smcount + 1;
    			post(ocfuri, ocfbody[2]);
    		}else{
    			if(chcount>0) {
            		rvibody[5].params.parameters.result = "Success";
    			}
    			state = FSM.READY;
    			smcount = 0;
    			smstatus = "";
        		rvimsg[1].params = rvibody[5];
        		post(rviuri, rvimsg[1]);
    		}
    		break;
    	}
    	case FSM.COMINGHOME : {
    		chstatus += body;
    		console.log("SmartHome Status = " + chstatus);
    		if(chcount < ocfdevices.length){
    			console.log("Checking " + ocfdevices[chcount]);
    			var device = JSON.parse(ocfdevices[chcount]);
    			ocfbody[3].params.address = device.address;	
    			ocfbody[3].params.port = device.port;
    			ocfbody[3].params.uri = device.uri;
    			if(toggle)
    				ocfbody[3].payload = {"state" : 1,"param" : 1};
    			else
    				ocfbody[3].payload = {"state" : 0,"param" : 0};
    			chcount = chcount + 1;
    			post(ocfuri, ocfbody[3]);
    		}else{
    			if(chcount>0) {
            		rvibody[5].params.parameters.result = "Success";
    				toggle = (toggle==0)?1:0;
    			}
    			state = FSM.READY;
    			chcount = 0;
    			chstatus = "";
        		rvimsg[1].params = rvibody[5];
        		post(rviuri, rvimsg[1]);
    		}
    	}
    	break;

    	case FSM.GET_VEHICLE_HVAC:{
    		console.log("HVAC Response Received.");
    		break;
    	}
    	case FSM.GET_VEHICLE_LOCATION:{
    		console.log("Location Response Received.");
    		break;
    	}
    	case FSM.MAKEOCFRVIDEVICE:
    	case FSM.SETHVAC:
    	case FSM.READY:{
    		console.log(".....................................Gateway Ready");
    		state = FSM.READY;
    		break;
    	}
    }
};

var post = function(uri, b){
    console.log("Posting " + JSON.stringify(b) + "to " + uri + " in state = " + state);
    var options = {
        url: uri ,
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
                        "cid":"/callback",
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
        
        if(state==FSM.COMINGHOME || state==FSM.SMARTHOMESTATUS)
    	{
        	console.log("Performing SmartHome Scenarion : Try Again");
        	return;
        }
        
        
        console.log("Content Type = " + JSON.stringify(req.headers));
        
        var rcvbody;
        if(req.body.utf8Data instanceof Object)
        	rcvbody = req.body.utf8Data;
        else
        	rcvbody = JSON.parse(req.body.utf8Data);
        
    	var params = rcvbody.params.parameters;
    	console.log("-------------------------------------------------------------------");
        if(params)
        	console.log("WSI Request Processing " + params.target);
        else
        	console.log("WHY AM I EVEN HERE");
        
        if(params && params.target == "GETLOCATION"){
    		state = FSM.GET_VEHICLE_HVAC;
        	for(var key in params){
    			ocfbody[1].payload[key] = params[key];
    			console.log(key + " = " +  params[key]);
    		}
    		rvimsg[1].params = rvibody[2];
    		res = post(rviuri, rvimsg[1]);
        }
        if(params && params.target == "GETHVAC"){
    		for(var key in params.status){
    			ocfbody[1].payload[key] = params.status[key];
    			console.log(key + " = " +  params.status[key]);
    		}
    		state = FSM.FINDOCFDEVICES;
        	var res = post(ocfuri, ocfbody[0]);
        }
        if(params && params.target == "HVACSTATUS"){
        	//Send an update to hosted resource
    		console.log("RVI OCF Device Update Received." + params.function + " = " + params.value + " by RVI.")
    		ocfbody[5].payload = {};
			ocfbody[5].payload[params.function] = params.value;
        	var res = post(ocfuri, ocfbody[5]);
        }
        if(params && params.target == "OCF_HVAC_UPDATE"){
        	//send to RVI
        	console.log("Send Update to RVI." + params.status.function);
    		state = FSM.SETHVAC;
    		rvibody[4].params.parameters.function = params.status.function;
    		rvibody[4].params.parameters.newValue = params.status.newValue;
    		rvimsg[1].params = rvibody[4];
    		res = post(rviuri, rvimsg[1]);
        }
        if(params && params.target == "SMARTHOMESCENARIO"){
        	if(params.text == "SmartHome Status"){
        		console.log("Scenario Name : " + params.text)
            	//series of OCF GET requests - SmartHome Status
        		state = FSM.SMARTHOMESTATUS;
        		rvibody[5].params.parameters.text = "Coming Home";
        		smstatus="";
        		smcount = 0;
        	}
        	if(params.text == "Coming Home"){
        		console.log("Scenario Name : " + params.text)
            	//series of OCF POST requests - Coming Home
        		state = FSM.COMINGHOME;
        		rvibody[5].params.parameters.text = "Coming Home";
        		chstatus="";
        		chcount = 0;
        	}
    		var response = {statusCode : 200};
    		var body = {};
    		wsiFSM(null, body, response);
        }
    	console.log("-------------------------------------------------------------------");
    },
    start: start,
}
console.log("App initialized");