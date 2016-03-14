var WebSocketClient = require('websocket').client;
var mustache = require('mustache');
var client = new WebSocketClient();
var rvicon = null;
var resobj = null;

rviconnect = function(cap,res){
	client.on('connectFailed', function(error) {
	    console.log('Connect Error: ' + error.toString());
	});

	client.on('connect', function(connection) {
	    connection.on('error', function(error) {
	        console.log("Connection Error: " + error.toString());
	    });
	    connection.on('close', function() {
	        console.log('Connection Closed');
	    });
	    connection.on('message', function(message) {
	    	console.log("Received: '" + JSON.stringify(message) + "'");
	    	if(resobj)resobj.send(JSON.stringify(message));
	        return;
	    });
	    
	    rvicon = connection;
	    console.log('WebSocket Client Connected');
		res.status(200).send("Connected to RVI");
	});
	console.log('Trying to connect to RVI');
	client.connect(cap.endpoint);
}

rvisend = function(cap, res) {
	if(rvicon.connected){
		console.log("Sending Data to RVI " + JSON.stringify(cap.params));
		resobj = res;
		rvicon.send(JSON.stringify(cap.params));
	}
}

module.exports = {

	init: function(app, passport) {
        var template = {
            "handler": "rvi",
            "sid": "org.genivi.rvi",
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
	                "cid": "org.genivi.rvi.connect",
	                "isauthrequired": "false",
	                "description": "Connect to RVI.",
	                "endpoint": "{{rviurl}}",
	                "endpointtype": "rvi",
	                "operation": "RVICONNECT"
        	    },
	            {
	                "cid": "org.genivi.rvi.send",
	                "isauthrequired": "false",
	                "description": "Post Message to RVI.",
	                "endpoint": "{{rviurl}}",
	                "endpointtype": "rvi",
	                "operation": "RVISEND",
	                "params": {
	                    "message": "None"
	                }
	            },
	        ]
        };
        console.log("RVI description found ");
        return template;
    },
    request: function (cap, auth, res) {
        console.log("RVI Module : "+ JSON.stringify(cap));
        if (cap.operation == "RVICONNECT") {
            console.log("Making RVICONNECT Request");
            var uri = cap.endpoint;
            console.log("Final URL = " + uri + " " + cap.params);
        	if(rvicon && rvicon.connected){
        		console.log("Already Connected ");
        		res.status(200).send("Already Connected");
        	}else{
            	rviconnect(cap, res);
        	}
        }
        if (cap.operation == "RVISEND") {
            console.log("Making RVISEND Request");
            var uri = cap.endpoint;
            console.log("Final URL = " + uri + " " + cap.params);
            rvisend(cap, res);
        }
    }    
}
console.log("RVI initialized");



