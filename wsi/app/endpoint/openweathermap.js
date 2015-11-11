var servicedb = require('../models/service');

module.exports = {

    init: function(app, passport) {
        var template = {
            "handler": "openweathermap",
            "sid": "org.openweathermap",
            "capability": [
                {
                    "cid": "org.owm.getbycity",
                    "isauthrequired": false,
                    "endpoint": "http://api.openweathermap.org/data/2.5/weather",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "q": "cityname",
                        "appid": "appid"                            
                    },
                    "tags": [
                        "weather by name"
                    ]
                },
                {
                    "cid": "org.owm.getbylocation",
                    "isauthrequired": false,
                    "endpoint": "http://api.openweathermap.org/data/2.5/weather",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "lat": "135",
                        "long": "135",
                        "appid": "appid"
                    },
                    "tags": [
                        "weather by name"
                    ]
                },
                {
                    "cid": "org.owm.getbycode",
                    "isauthrequired": false,
                    "endpoint": "http://api.openweathermap.org/data/2.5/weather",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "zip": "zip",
                        "cc": "country code",
                        "appid": "appid"
                    },
                    "tags": [
                        "weather by name"
                    ]
                }
            ]
        };
        return template;
    },
    auth :  function(app, authcred, passport, serviceid) {
        console.log("Client ID = " + authcred.clientID);
        console.log("Before Use = " + authcred.callbackURL);
        console.log("openweathermap auth initialized");
    }
}
console.log("openweathermap  initialized");




