var FacebookStrategy = require('passport-facebook').Strategy;
var servicedb = require('../models/service');

module.exports = {

    init: function() {
        var template = {
            "handler": "google",
            "sid": "com.google",
            "capability": [
                {
                    "cid": "com.google.search",
                    "isauthrequired": "false",
                    "endpoint": "https://www.googleapis.com/customsearch/v1?key={{key}}&cx={{cx}}&q={{q}}",
                    "endpointtype": "REST",
                    "operation": "GET",
                    "params": {
                        "key": "AIzaSyBGXrtIyGsrOOT9soXavjGwj9lxmJYOli4",
                        "cx" : "017576662512468239146:omuauf_lfve",
                        "q": "Topic"
                    },
                    "tags": [
                        "search",
                        "keyword"
                    ]
                }
            ]
        };
        return template;
    },
    auth :  function(app, authcred, passport, serviceid) {
        console.log("Client ID = " + authcred.clientID);
        console.log("Before Use = " + authcred.callbackURL);
        console.log("google auth initialized");
    }
}
console.log("google  initialized");
