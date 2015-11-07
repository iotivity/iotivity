var FacebookStrategy = require('passport-facebook').Strategy;
var servicedb = require('../models/service');

var fb_service_description = {
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
                        "cid": "com.facebook.auth",
                        "isauthrequired": "true",
                        "description": "Authorize this app to use facebook.",
                        "endpoint": "/wsi/facebook/auth",
                        "endpointtype": "REST",
                        "operation": "GET"
                    },                    
                    {
                        "cid": "com.facebook.post",
                        "isauthrequired": "true",
                        "description": "Post Message to Facebook.",
                        "endpoint": "http://graph.facebook.com/me/feed?message={{message}}&access_token={{access_token}}",
                        "endpointtype": "REST",
                        "operation": "POST",
                        "params": {
                            "msg": "Hello World From WSI",
                            "access_token" : ""
                        },
                        "tags": [
                            "share",
                            "post"
                        ]
                    }
                ]
            };

module.exports = {

    init: function(app, passport) {
        
        console.log("Facebook description found " + fb_service_description.auth[0].endpoint+ " " + fb_service_description.auth[0].callbackURL);
        app.get(fb_service_description.auth[0].endpoint,
            passport.authenticate('facebook', {scope : fb_service_description.auth[0].scope}));
        //called by facebook
        app.get(fb_service_description.auth[0].callbackURL, 
            passport.authenticate('facebook', { successRedirect: '/', failureRedirect: '/' }),function(req, res) {});
            
            
    },
/*
        app.post('/facebook/post', function(req, res) {
            var jsonObj = req.body;
            console.log("FB POST : " + jsonObj.userid + ' ' + jsonObj.message + ' ' + jsonObj.access_token );
            httphandler.post({url:''+jsonObj.userid+''+jsonObj.message+''+jsonObj.access_token},
                function (error, response, body) {
                    if (!error && response.statusCode == 200) {
                        console.log(body);
                    }
                }
            )
        });
*/
    auth :  function(app, authcred, passport, serviceid) {
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
                                        var options = { overwrite: true };
                                        servicedb.findOne({ sid : serviceid}, function(err, service) {
                                            if (err)
                                                return done(err);

                                            console.log(JSON.stringify(profile));
                                            console.log("Access Token  : " + access_token);
                                            console.log("Refresh Token : " + refresh_token);
                                            service.auth[0].userid    = profile.id;
                                            service.auth[0].access_token = access_token;
                                            //service.auth[0].refresh_token = refresh_token;
                                            service.markModified('');
                                            
                                            servicedb.update({sid:service.sid, 'auth.type':'oauth2.0'},
                                                                {$set:
                                                                    {"auth.$.access_token":access_token}
                                                                }
                                                            ).exec(function(err, model){
                                                                console.log("Facebook Record Updated " + JSON.stringify(model));
                                                            });
                                            return done(null, service);
                                            
//                                            servicedb.update({ sid: service.serviceid }, service, options, function(err, model) {
//                                                if (err) {
//                                                    console.log(err);
//                                                    model.save();
//                                                    return done(err);
//                                                }
//                                                console.log("Facebook Record Updated " + JSON.stringify(service));
//                                            });
//                                            return done(null, service);
                                        });
                                    });
                                }));
        //return passport.authenticate('facebook', {scope : authcred.scope});
    }
}
console.log("Facebook initialized");
