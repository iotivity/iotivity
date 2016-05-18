var UberStrategy = require('passport-uber').Strategy;
var servicedb = require('../models/service');
var Uber = require('node-uber');
var mustache = require('mustache');
var request = require('request');

module.exports = {

    init: function(app, passport) {
        var template = {
            "handler": "uber",
            "sid": "com.uber",
            "description": "Uber RESTful API.",
            "platforms": [
                {
                    "name": "linux",
                    "appURL": "https://uber.com"
                }
            ],
            "auth": [
                {
                "type": "oauth2.0",
                "clientID": "",    // Fill with specific client ID while reigster the app on uber 
                "clientSecret": "", // Fill with specific clientSecret while reigster the app on uber
                "server_token": "Fill", // // Replace with specific server_token while reigster the app on uber 
                "endpoint": "/wsi/uber/auth",
                "callbackURL": "/wsi/uber/auth/callback",
                "userid": "",
                "token": "",
                "secret": "",
                "firstName": "",
                "lastName": "",
                "email": "",
                "scope" : ['profile', 'history', 'history_lite', 'places']
                },
            ],
            "capability": [
                {
                    "cid": "com.uber.getProfile",
                    "isauthrequired": "true",
                    "description": "Get user profile",
                    "endpoint": "https://api.uber.com/v1/me?access_token={{access_token}}",
                    "endpointtype": "uber",
                    "operation": "GETPROFILE",
                    "params": {
                        
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
                },
                {
                    "cid": "com.uber.getPrice",
                    "isauthrequired": "true",
                    "description": "Get estimate price",
                    "endpoint": "https://api.uber.com//v1/estimates/price?server_token={{server_token}}&start_latitude={{start_latitude}}&start_longitude={{start_longitude}&end_latitude={{end_latitude}}&end_longitude={{end_longitude}}",
                    "endpointtype": "uber",
                    "operation": "GETPRICE",
                    "params": {
                        "start_latitude": "3.1357",        // Replace with your specific latitude for start point
                        "start_longitude" : "101.6880",    // Replace with your specific latitude for start point
                        "end_latitude" : "3.0833",         // Replace with your specific latitude for end point
                        "end_longitude" : "101.6500"       // Replace with your specific latitude for end point
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
                }
                
            ]
        };
        console.log("Uber description found " + template.auth[0].endpoint+ " " + template.auth[0].callbackURL);
        app.get(template.auth[0].endpoint, passport.authenticate(template.handler));
        app.get(template.auth[0].callbackURL, 
            passport.authenticate(template.handler, { successRedirect: '/', failureRedirect: '/' }),function(req, res) {}); 
            
        return template;

    },
    auth :  function(authcred, passport, serviceid) {
        console.log("Client ID = " + authcred.clientID);
        console.log("Before Use = " + authcred.callbackURL);
        console.log("Uber auth initialized");

        return passport.use(new UberStrategy({
                                        clientID    : authcred.clientID,
                                        clientSecret : authcred.clientSecret,
                                        callbackURL    : authcred.callbackURL
                                },
                                function(token, secret, profile, done) {
                                    process.nextTick(function() {
                                        console.log('Next Tick : ' +  profile.id);
                                        servicedb.findOne({ sid : serviceid}, function(err, service) {
                                            if (err)
                                                return done(err);

                                            console.log(JSON.stringify(profile));
                                            console.log("Token  : " + token);
                                            console.log("Secret: " + secret);
                                            service.auth[0].userid    = profile.id;
                                            service.auth[0].token = token;
                                            service.auth[0].secret = secret;
                                            service.markModified('');
                                            
                                            servicedb.update({sid:service.sid, 'auth.type':'oauth2.0'},
                                                                {$set:
                                                                    {"auth.$.token":token,
                                                                     "auth.$.secret":secret
                                                                    }
                                                                }
                                                            ).exec(function(err, model){
                                                                console.log("Uber Record Updated " + JSON.stringify(model));
                                                            });
                                                            
                                            return done(null, service);
                                        });
                                    });
                                }));
    },
    request: function (cap, auth, res) {
        
        var config = {
                client_id:         auth.clientID
              , client_secret:      auth.clientSecret
              , server_token:       auth.server_token
              , access_token:         auth.token
              , access_token_secret:  auth.secret
              , name:                 'wsiApp'
        };
        
        if (cap.cid == "com.uber.getProfile") {
            var Uber = require('node-uber');
            var uber = new Uber(config);
            console.log("config info:");
            console.log(JSON.stringify(config));
            
            uber.user.getProfile(function (err, res) {
                 console.log("Getting profile");
                if (err) console.log(err);
				  else console.log(res);
			});
        }
        if (cap.cid == "com.uber.getPrice") {
            var Uber = require('node-uber');
            var uber = new Uber(config);
            
             var params = {
                "start_latitude": "3.1357",        // Replace with your specific latitude for start point
                 "start_longitude" : "101.6880",    // Replace with your specific latitude for start point
                 "end_latitude" : "3.0833",         // Replace with your specific latitude for end point
                 "end_longitude" : "101.6500"       // Replace with your specific latitude for end point
            };
            uber.estimates.getPriceForRoute(params.start_latitude, params.start_longitude, 
                                            params.end_latitude, params.end_longitude, function (err, res) {
                                            
                console.log("Getting estimates for specific route");
                console.log("params:" + JSON.stringify(params));
                console.log("start lat & long" + cap.params.start_latitude + "/" + cap.params.start_longitude);
                if (err) console.log(err);
				  else console.log(res);
			});
        }
    }
}
console.log("Uber initialized");
