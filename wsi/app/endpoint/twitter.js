var TwitterStrategy = require('passport-twitter').Strategy;
var servicedb = require('../models/service');
var Twit = require('twit');
var mustache = require('mustache');

module.exports = {

    init: function(app, passport) {
        var template = {
            "handler": "twitter",
            "sid": "com.twitter",
            "description": "Twitter RESTful API.",
            "platforms": [
                {
                    "name": "linux",
                    "appURL": "https://twitter.com"
                }
            ],
            "auth": [
                {
                "type": "oauth2.0",
                "consumerKey": "U5ofpxyQWPvcWUn8nMLr55vuJ",
                "consumerSecret": "0bkHEoXMP2e51JSCrOl6Za3S4ZoWGHaIT4YdgIIJceWHnmLM2I",
                "endpoint": "/wsi/twitter/auth",
                "callbackURL": "/wsi/twitter/auth/callback",
                "userid": "",
                "token": "",
                "secret": "",
                "firstName": "",
                "lastName": "",
                "email": "",
                "scope" : ['email', 'publish_actions']
                },
            ],
            "capability": [
                {
                    "cid": "com.twitter.post",
                    "isauthrequired": "true",
                    "description": "Post Message to Twiiter.",
                    "endpoint": "https://api.twitter.com/1.1/direct_messages/new.json?text={{text}}&screen_name={{screen_name}}",
                    "endpointtype": "twitter",
                    "operation": "POST",
                    "params": {
                        "text": "Message to post",
                        "screen_name" : "oicdemo",
                        "consumerKey": "U5ofpxyQWPvcWUn8nMLr55vuJ",
                        "consumerSecret": "0bkHEoXMP2e51JSCrOl6Za3S4ZoWGHaIT4YdgIIJceWHnmLM2I",
                        "token": "",
                        "secret": ""
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
                }
            ]
        };
        console.log("Twitter description found " + template.auth[0].endpoint+ " " + template.auth[0].callbackURL);
        app.get(template.auth[0].endpoint, passport.authenticate(template.handler));
        app.get(template.auth[0].callbackURL, 
            passport.authenticate(template.handler, { successRedirect: '/', failureRedirect: '/' }),function(req, res) {});
            
        return template;

    },
    auth :  function(authcred, passport, serviceid) {
        console.log("Client ID = " + authcred.clientID);
        console.log("Before Use = " + authcred.callbackURL);
        console.log("Twitter auth initialized");

        return passport.use(new TwitterStrategy({
                                        consumerKey    : authcred.consumerKey,
                                        consumerSecret : authcred.consumerSecret,
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
                                                                     "auth.$.secret":secret,
                                                                     "capability.$.params.token": token,
                                                                     "capability.$.params.secret": secret
                                                                    }
                                                                }
                                                            ).exec(function(err, model){
                                                                console.log("Twitter Record Updated " + JSON.stringify(model));
                                                            });
                                                            
                                            return done(null, service);
                                        });
                                    });
                                }));
    },
    request: function (cap, res) {
        console.log("Trying to perform rest request : "+ JSON.stringify(cap));
        if (cap.operation == "POST") {
            var ep = cap.endpoint;
            var uri = mustache.render(cap.endpoint, cap.params);
            console.log("Final URL = " + uri + " " + JSON.stringify(cap.params));
            
            var config = {
                    consumer_key:         cap.params.consumerKey
                  , consumer_secret:      cap.params.consumerSecret
                  , access_token:         cap.params.token
                  , access_token_secret:  cap.params.secret
            };
            
            console.log("Config : " + JSON.stringify(config));
            var client = new Twit(config);
            client.post('statuses/update', { status: 'hello world!' }, function(err, data, response) {
              if(err) {
                console.log(JSON.stringify(err));
                res.sendStatus(500);
                return;
              }
              console.log(JSON.stringify(data));  // Tweet body. 
              console.log(response);  // Raw response object. 
              res.sendStatus(200);
            });
        }
    }
}
console.log("Twitter initialized");
