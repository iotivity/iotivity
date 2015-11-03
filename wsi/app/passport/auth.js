var passport = require('passport');
var facebook = require('./facebook');
var servicedb = require('../models/service');

module.exports = {
    init: function (app) {
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
    },

    //TODO Dynamically load when needed
    loadstrategy: function(app) {
        facebook.init(passport, app);
    }
}
