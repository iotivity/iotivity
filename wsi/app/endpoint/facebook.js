var FacebookStrategy = require('passport-facebook').Strategy;
var servicedb = require('../models/service');

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
                    "endpointtype": "REST",
                    "operation": "POST",
                    "params": {
                        "message": "Message to post",
                        "access_token" : "access token"
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
            passport.authenticate('facebook', {scope : template.auth[0].scope}));
        app.get(template.auth[0].callbackURL, 
            passport.authenticate('facebook', { successRedirect: '/', failureRedirect: '/' }),function(req, res) {});
            
        return template;

    },
    auth :  function(authcred, passport, serviceid) {
        console.log("Client ID = " + authcred.clientID);
        console.log("Before Use = " + authcred.callbackURL);
        console.log("facebook auth initialized");

        passport.use(new FacebookStrategy({
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
                                                                    {"auth.$.access_token":access_token}
                                                                }
                                                            ).exec(function(err, model){
                                                                console.log("Facebook Record Updated " + JSON.stringify(model));
                                                            });
                                            return done(null, service);
                                        });
                                    });
                                }));
    }
}
console.log("Facebook initialized");
