var FacebookStrategy = require('passport-facebook').Strategy;
var passport = require('passport');
var servicedb = require('../models/service');
console.log("Facebook initialized");

module.exports = {
    init :  function(app, auth) {
            passport.use('facebook', new FacebookStrategy({
                clientID        : auth.clientid,
                clientSecret    : auth.secret,
                callbackURL     : auth.callbackurl

            },
            function(access_token, refresh_token, profile, done) {
                console.log('profile', profile);
                process.nextTick(function() {
                    servicedb.findOne({ 'id' : profile.id }, function(err, user) {
                        if (err)
                            return done(err);

                        if (user) {
                            return done(null, user);
                        } else {
                            newUser.fb.id    = profile.id;
                            newUser.fb.access_token = access_token;
                            newUser.fb.refresh_token = refresh_token;
                            newUser.fb.firstName  = profile.name.givenName;
                            newUser.fb.lastName = profile.name.familyName;
                            //newUser.fb.email = profile.emails[0].value;

                            newUser.save(function(err) {
                                if (err)
                                    throw err;
                                console.log("New User Credentials Saved.");
                                return done(null, newUser);
                            });
                        }
                    });
                });
            }));
            // route for facebook authentication and login
            // different scopes while logging in
            app.get('/auth/facebook', function(req, res) {
                    console.log("Auth via facebook " + req.query.userid);
                    passport.authenticate('facebook', 
                    { 
                        //TODO : remove hardcoding
                        scope : ['email', 'publish_actions']
                    });
                }
            );
            app.get('/auth/facebook/callback', 
              passport.authenticate('facebook', { successRedirect: '/fbSuccess', failureRedirect: '/fbFail' }),
              function(req, res) {
                //res.redirect('/app/public/200.html');
              });
            app.get('/fbSuccess', function(req, res) {
                console.log("Facebook Auth Successful");
                console.log('Request Params' + req.params.id);
                console.log('Request' + req.body);
                console.log('Response' + res.body);
                //res.sendFile(__dirname + '/public/index.html');
            });

            app.get('/fbFail', function(req, res) {
                console.log("Facebook Auth Failed... Try Again");
                //res.sendFile(__dirname + '/public/index.html');
            });
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
    }
}
