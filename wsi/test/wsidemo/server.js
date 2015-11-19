var express  = require('express');
var session = require('express-session');
var port  	 = process.env.PORT || 8081;
var morgan   = require('morgan');
var bodyParser = require('body-parser');
var methodOverride = require('method-override');
var cors = require('cors');
var app      = express();
app.use(cors());
app.use(morgan('dev'));
app.use(bodyParser.urlencoded({'extended':'true'}));
app.use(bodyParser.json());
app.use(bodyParser.json({ type: 'application/vnd.api+json' }));
app.use(methodOverride('X-HTTP-Method-Override'));
app.use(session({ secret: 'SECRET' , resave: true, saveUninitialized: true}));

var strategy = [];
var normalizedPath = require("path").join(__dirname, "./app/endpoint");
console.log(normalizedPath);
require('fs').readdirSync(normalizedPath).forEach(function(file) {
    var svc = file.substring(0, file.indexOf(".js"));
    strategy[svc] = require(normalizedPath + "/" + svc);
    console.log("Initializing Web Service : " + svc);
    var template = strategy[svc].init(app);
    console.log(svc + " Initialized");
});

require('./app/routes')(app);
app.set("strategy", strategy);

var options = {
  index: "scene1.html"
};

app.use('/', express.static('public', options));

app.listen(port);
console.log("App listening on port " + port);
module.exports = app;
