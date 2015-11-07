// set up ======================================================================
var express  = require('express');
var session = require('express-session');
var mongoose = require('mongoose'); 					// mongoose for mongodb
var port  	 = process.env.PORT || 8080; 				// set the port
var database = require('./app/config/database'); 			// load the database config
var morgan   = require('morgan');
var passport = require('passport');
var servicedb = require('./app/models/service');
var bodyParser = require('body-parser');
var methodOverride = require('method-override');
var cors = require('cors');
mongoose.connect(database.url); 	// connect to mongoDB database on modulus.io

var app      = express(); 								// create our app w/ express
app.use(cors());

app.use(morgan('dev')); // log every request to the console

app.use(bodyParser.urlencoded({'extended':'true'})); // parse application/x-www-form-urlencoded
app.use(bodyParser.json()); // parse application/json
app.use(bodyParser.json({ type: 'application/vnd.api+json' })); // parse application/vnd.api+json as json

app.use(methodOverride('X-HTTP-Method-Override')); // override with the X-HTTP-Method-Override header in the request

app.use(express.static('public'));

app.use(session({ secret: 'SECRET' , resave: true, saveUninitialized: true}));
app.use(passport.initialize());
app.use(passport.session());

passport.serializeUser(function(user, done) {
	  done(null, user);
});
passport.deserializeUser(function(user, done) {
	  done(null, user);
});
/*
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
*/
console.log("Serializer added");

//var allowCrossDomain = function(req, res, next) {
//        res.header('Access-Control-Allow-Credentials', true);
//        res.header('Access-Control-Allow-Origin', req.headers.origin);
//        res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
//        res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization');
//
//        // intercept OPTIONS method
//        if ('OPTIONS' == req.method) {
//            res.send(200);
//        }
//        else {
//            next();
//        }
//};
//app.use(allowCrossDomain);


//app.use(function(req, res, next) {
//    console.log("CORS Enabled");
//    res.header("Access-Control-Allow-Origin", "*");
//    res.setHeader('Access-Control-Allow-Credentials', true);    
//    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
//    next();
//});


var strategy = [];
var normalizedPath = require("path").join(__dirname, "./app/endpoint");
console.log(normalizedPath);
require('fs').readdirSync(normalizedPath).forEach(function(file) {
    var svc = file.substring(0, file.indexOf(".js"));
    strategy[svc] = require(normalizedPath + "/" + svc);
    console.log("Initializing Web Service : " + svc);
    strategy[svc].init(app, passport);
    console.log(svc + " Initialized");
});

require('./app/routes')(app, passport);
app.set("strategy", strategy);
app.listen(port);

console.log("App listening on port " + port);
module.exports = app;
