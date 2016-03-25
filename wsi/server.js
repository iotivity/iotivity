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
console.log("Serializer added");

var strategy = [];
var normalizedPath = require("path").join(__dirname, "./app/endpoint");
console.log(normalizedPath);
require('fs').readdirSync(normalizedPath).forEach(function(file) {
    var svc = file.substring(0, file.indexOf(".js"));
    strategy[svc] = require(normalizedPath + "/" + svc);
    console.log("Initializing Web Service : " + svc);
    var template = strategy[svc].init(app, passport);
    if(Object.keys(template).length > 0){
        servicedb.update({sid: template.sid}, {$setOnInsert : template}, {upsert : true}, function(err, num){console.log("Added " + num + " record");});
    }
    console.log(svc + " Initialized");
});

require('./app/routes')(app, passport);
app.set("strategy", strategy);
app.listen(port);

console.log("App listening on port " + port);
module.exports = app;


//Exit gracefully when interrupted
process.on( "SIGINT", function() {
	console.log( "SIGINT: Quitting..." );
	process.exit( 0 );
} );
