var FacebookStrategy = require('passport-facebook').Strategy;
var servicedb = require('../models/service');
var mustache = require('mustache');
var request = require('request');

module.exports = {

    init: function(app, passport) {
        var template = {
            "handler": "facebook",
            "sid": "com.facebook",
            "description": "Facebook RESTful API.",
            "platforms": [
                {
                    "name": "linux",
                    "appURL": "https://facebook.com"
                }
            ],
            "auth": [
                {
                "type": "oauth2.0",
                "clientID": "949606648410465",
                "clientSecret": "2a35037f1919e7cbefcc1d2f95ef24fb",
                "endpoint": "/wsi/facebook/auth",
                "callbackURL": "/wsi/facebook/auth/callback",
                "userid": "",
                "access_token": "",
                "refresh_token": "",
                "firstName": "",
                "lastName": "",
                "email": "",
                "scope" : ['email', 'publish_actions']
                },
            ],
            "capability": [
                {
                    "cid": "com.facebook.post",
                    "isauthrequired": "true",
                    "description": "Post Message to Facebook.",
                    "endpoint": "https://graph.facebook.com/me/feed?message={{message}}&access_token={{access_token}}",
                    "endpointtype": "facebook",
                    "operation": "POSTTOFB",
                    "params": {
                        "message": "Message to post"
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
                }
            ]
        };
        console.log("Facebook description found " + template.auth[0].endpoint+ " " + template.auth[0].callbackURL);
        app.get(template.auth[0].endpoint,
            passport.authenticate(template.handler, {scope : template.auth[0].scope}));
        app.get(template.auth[0].callbackURL, 
            passport.authenticate(template.handler, { successRedirect: '/', failureRedirect: '/' }),function(req, res) {});
            
        return template;

    },
    auth :  function(authcred, passport, serviceid) {
        console.log("Client ID = " + authcred.clientID);
        console.log("Before Use = " + authcred.callbackURL);
        console.log("facebook auth initialized");

        return passport.use(new FacebookStrategy({
                                        clientID        : authcred.clientID,
                                        clientSecret    : authcred.clientSecret,
                                        callbackURL     : authcred.callbackURL
                                },
                                function(access_token, refresh_token, profile, done) {
                                    process.nextTick(function() {
                                        console.log('Next Tick : ' +  profile.id);
                                        servicedb.findOne({ sid : serviceid}, function(err, service) {
                                            if (err)
                                                return done(err);

                                            console.log(JSON.stringify(profile));
                                            console.log("Access Token  : " + access_token);
                                            console.log("Refresh Token : " + refresh_token);
                                            service.auth[0].userid    = profile.id;
                                            service.auth[0].access_token = access_token;
                                            service.markModified('');
                                            
                                            servicedb.update({sid:service.sid, 'auth.type':'oauth2.0'},
                                                                {$set:
                                                                    {"auth.$.access_token":access_token,
                                                                     "capability.$.params.access_token":access_token
                                                                    }
                                                                }
                                                            ).exec(function(err, model){
                                                                console.log("Facebook Record Updated " + JSON.stringify(model));
                                                            });
                                                            
                                            return done(null, service);
                                        });
                                    });
                                }));
    },
    request: function (cap, auth, res) {
        console.log("Facebook Module : "+ JSON.stringify(cap));
        if (cap.operation == "POSTTOFB") {
            console.log("Making a POST HTTP Request");
            var params = {
                "message": cap.params.message,
                "access_token" : auth.access_token
            };
            var uri = mustache.render(cap.endpoint, params);
            console.log("Final URL = " + uri + " " + cap.params);
            
            request.post({
                url: uri,
            },function (error, response, body) {
                if (!error && response.statusCode == 200) {
                    console.log("Success = " + error + "Response = " + response + "Body = " + body);
                    (res.sendStatus || res.send).bind(res)(response.statusCode);
                }else{
                    console.log("Error = " + error + "Response = " + response + "Body = " + body);
                    (res.sendStatus || res.send).bind(res)(response.statusCode);                    
                }
            });
        }
    }    
}
console.log("Facebook initialized");
