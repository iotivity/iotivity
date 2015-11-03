// set up ======================================================================
var express  = require('express');
var session = require('express-session');
var mongoose = require('mongoose'); 					// mongoose for mongodb
var port  	 = process.env.PORT || 8080; 				// set the port
var database = require('./app/config/database'); 			// load the database config
var morgan   = require('morgan');
var bodyParser = require('body-parser');
var methodOverride = require('method-override');
var webservice = require('./app/passport/auth');

mongoose.connect(database.url); 	// connect to mongoDB database on modulus.io

var app      = express(); 								// create our app w/ express
app.use(morgan('dev')); // log every request to the console
app.use(bodyParser.urlencoded({'extended':'true'})); // parse application/x-www-form-urlencoded
app.use(bodyParser.json()); // parse application/json
app.use(bodyParser.json({ type: 'application/vnd.api+json' })); // parse application/vnd.api+json as json
app.use(methodOverride('X-HTTP-Method-Override')); // override with the X-HTTP-Method-Override header in the request
app.use(session({ secret: 'SECRET' , resave: true, saveUninitialized: true}));
app.use(express.static('public'));

require('./app/routes')(app);
webservice.init(app);

app.listen(port);
console.log("App listening on port " + port);
module.exports = app;