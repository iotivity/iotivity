var WebSocketClient = require('websocket').client;
var servicedb = require('../models/service');
var mustache = require('mustache');

var rviparams = [
	{//0
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/right_temp",
			"parameters":{"a":"b"}
		}
	},
	{//1
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/left_temp",
			"parameters":{"a":"b"}
		}
	},
	{//2
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/fan_speed",
			"parameters":{"a":"b"}
		}
	},
	{//3
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/defroster",
			"parameters":{"a":"b"}
		}
	},
	{//4
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/subscribe_hvac",
			'callback_service':'genivi.org/oic_gw/callback/report_hvac',
			"parameters":{"a":"b"}
		}
	},
	{//5
		"jsonrpc":"2.0",
		"id":"1",
		"method": "register_service",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/smarthome/control",
			"parameters":{"command":"PredefinedString"}
		}
	},
	{//6
		"jsonrpc":"2.0",
		"id":"1",
		"method": "get_available_services",
		"params": {}
	}
]; 

var client = new WebSocketClient();

client.on('connectFailed', function(error) {
    console.log('Connect Error: ' + error.toString());
});

client.on('connect', function(connection) {
    console.log('WebSocket Client Connected');
    connection.on('error', function(error) {
        console.log("Connection Error: " + error.toString());
    });
    connection.on('close', function() {
        console.log('Connection Closed');
    });
    connection.on('message', function(message) {
    	console.log("Received: '" + JSON.stringify(message) + "'");
    });

    function sendToRVI(params) {
        if (connection.connected) {
        	console.log("Sending data " + JSON.stringify(params));
            connection.send(JSON.stringify(params));
        }
    }
    sendToRVI(rviparams[5]);
});
client.connect('ws://localhost:9008/');

module.exports = {

    init: function(app, passport) {
        var template = {
            "handler": "rvi",
            "sid": "org.genivi",
            "description": "RVI WebSocket API.",
            "platforms": [
	            {
	                "name": "linux",
	                "appURL": "https://genivi.org"
	            }
            ],
            "auth": [{"type": "none"}],
            "capability": [
	            {
	                "cid": "org.genivi.rvi.register",
	                "isauthrequired": "false",
	                "description": "Register a service with RVI.",
	                "endpoint": "ws://localhost:9008/",
	                "endpointtype": "rvi",
	                "operation": "RVIREG",
	                "params": {
	                    "message": "Message to post"
	                },
	                "tags": [
	                    "command"
	                ]
	            },
	            {
	                "cid": "org.genivi.rvi.invoke",
	                "isauthrequired": "false",
	                "description": "Invoke a remote RVI Service",
	                "endpoint": "ws://localhost:9008/",
	                "endpointtype": "rvi",
	                "operation": "RVIINVOKE",
	                "params": {
	                    "message": ""
	                },
	                "tags": [
	                    "command"
	                ]
	            },
	            {
	                "cid": "org.genivi.rvi.discover",
	                "isauthrequired": "false",
	                "description": "Post Message to RVI.",
	                "endpoint": "ws://localhost:9008/",
	                "endpointtype": "rvi",
	                "operation": "RVIDISCOVER",
	                "params": {
	                    "message": "None"
	                },
	                "tags": [
	                    "command"
	                ]
	            },
	            {
	                "cid": "org.genivi.rvi.invoke",
	                "isauthrequired": "false",
	                "description": "Invoke a remote RVI Service",
	                "endpoint": "ws://localhost:9008/",
	                "endpointtype": "rvi",
	                "operation": "RVIINVOKE",
	                "params": {
	                    "message": ""
	                },
	                "tags": [
	                    "command"
	                ]
	            }
	        ]
        };
        console.log("RVI description found ");
        return template;
    },
    request: function (cap, auth, res) {
        console.log("RVI Module : "+ JSON.stringify(cap));
        if (cap.operation == "RVIDISCOVER") {
            console.log("Making RVIDISCOVER WebSocket Request");
            var uri = mustache.render(cap.endpoint, params);
            console.log("Final URL = " + uri + " " + cap.params);
        }
    }    
}
console.log("RVI initialized");
