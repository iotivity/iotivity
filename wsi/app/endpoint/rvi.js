var WebSocketClient = require('websocket').client;
var servicedb = require('../models/service');
var mustache = require('mustache');
var request = require('request');

module.exports = {

    init: function(app, passport) {
        var template = {
            "handler": "rvi",
            "sid": "org.genici",
            "description": "RVI WebSocket API.",
            "platforms": [
            {
                "name": "linux",
                "appURL": "https://genivi.org"
            }
            ],
            "auth": [
            {
                "type": "none",
            },
            ],
            "capability": [
            {
                "cid": "org.genivi.rvi.register",
                "isauthrequired": "false",
                "description": "Register a service with RVI.",
                "endpoint": "ws://",
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
                "endpoint": "ws://",
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
                "endpoint": "ws://",
                "endpointtype": "rvi",
                "operation": "RVIDISCOVER",
                "params": {
                    "message": "None"
                },
                "tags": [
                    "command"
                ]
            }
            ]
        };
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
                console.log('echo-protocol Connection Closed');
            });
            connection.on('message', function(message) {
                if (message.type === 'utf8') {
                    console.log("Received: '" + message.utf8Data + "'");
                }
            });

            function sendNumber() {
                if (connection.connected) {
                    var number = Math.round(Math.random() * 0xFFFFFF);
                    connection.sendUTF(number.toString());
                    setTimeout(sendNumber, 1000);
                }
            }
            sendNumber();
        });
        client.connect('ws://localhost:9008/', 'echo-protocol');
        console.log("RVI description found ");
        return template;
    },
    request: function (cap, auth, res) {
        console.log("RVI Module : "+ JSON.stringify(cap));
        if (cap.operation == "POSTTORVI") {
            console.log("Making a POST WebSocket Request");
            var params = {
                "message": cap.params.message
            };
            var uri = mustache.render(cap.endpoint, params);
            console.log("Final URL = " + uri + " " + cap.params);
        }
    }    
}
console.log("RVI initialized");
