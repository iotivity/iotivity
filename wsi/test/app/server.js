// set up ======================================================================
var express  = require('express');
var session = require('express-session');
var port  	 = process.env.PORT || 8081; 				// set the port
var morgan   = require('morgan');
var bodyParser = require('body-parser');
var methodOverride = require('method-override');
var cors = require('cors');
var app      = express(); 								// create our app w/ express
app.use(cors());
app.use(morgan('dev')); // log every request to the console
app.use(bodyParser.urlencoded({'extended':'true'})); // parse application/x-www-form-urlencoded
app.use(bodyParser.json()); // parse application/json
app.use(bodyParser.json({ type: 'application/vnd.api+json' })); // parse application/vnd.api+json as json
app.use(methodOverride('X-HTTP-Method-Override')); // override with the X-HTTP-Method-Override header in the request
//app.use(express.static('public'));
var options = {
  index: "scene1.html"
};

app.use('/', express.static('public', options));

app.use(session({ secret: 'SECRET' , resave: true, saveUninitialized: true}));
app.listen(port);
console.log("App listening on port " + port);
module.exports = app;
