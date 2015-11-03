var passport = require('passport');
var fs = require('fs');
var normalizedPath = require("path").join(__dirname, ".");
var servicedb = require('../models/service');

module.exports = {
    init: function (app) {
        var strategy = [];
        console.log("Initializing services.");
        app.use(passport.initialize());
        app.use(passport.session());

        // Passport needs to be able to serialize and deserialize users to support persistent login sessions
        passport.serializeUser(function (user, done) {
            console.log('serializing user: ');
            console.log(user);
            done(null, user._id);
        });

        passport.deserializeUser(function (id, done) {
            servicedb.findById(id, function (err, user) {
                console.log('deserializing user:', user);
                done(err, user);
            });
        });
        
        fs.readdirSync(normalizedPath).forEach(function(file) {
            if(file!='auth.js'){
                var svc = file.substring(0, file.indexOf(".js"));
                strategy[svc] = require("./" + svc);
                console.log("Initializing Web Service : " + svc);
            }
        });        
    }
}
