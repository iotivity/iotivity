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
                    "endpoint": "http://api.openweathermap.org/data/2.5/weather?q={{q}}&appid={{appid}}",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "q": "cityname",  // Replace with specific cityname
                        "appid": "appid"  // Replace with specific API key
                    },
                    "tags": [
                        "weather by name"
                    ]
                },
                {
                    "cid": "org.owm.getbylocation",
                    "isauthrequired": false,
                    "endpoint": "http://api.openweathermap.org/data/2.5/weather?lat={{lat}}&lon={{lon}}&appid={{appid}}",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "lat": "135",
                        "lon": "135",
                        "appid": "appid"  // Replace with specific API key
                    },
                    "tags": [
                        "weather by name"
                    ]
                },
                {
                    "cid": "org.owm.getbycode",
                    "isauthrequired": false,
                    "endpoint": "http://api.openweathermap.org/data/2.5/weather?zip={{zip}},{{cc}}&appid={{appid}}",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "zip": "zip",  // Replace with specific zip code
                        "cc": "country code",  // Replace with specific country code
                        "appid": "appid"  // Replace with specific API key
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
