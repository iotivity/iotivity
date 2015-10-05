var servicedb = require('./models/service');
var Mustache = require('mustache');
var request_http = require('./endpoint/request_http');
//var request_iotivity = require('./endpoint/request_iotivity');

function getServices(res){
    servicedb.find(function(err, services) {
        // if there is an error retrieving, send the error. nothing after res.send(err) will execute
        if (err)
            res.send(err);

        res.json(services); // return all services in JSON format
    });
};

function match(item, filter) {
    var keys = Object.keys(filter);
    return keys.some(function (key) {
        return item[key] == filter[key];
    });
}


function sendRequest(capability, query, res) {
    var endpoint = Mustache.render(capability.endpoint, query);
    console.log(capability.endpointtype + '://' + endpoint);

    if(capability.endpointtype == "http" || capability.endpointtype == "https") {
        console.log("Accessing http/https endpoint");
        request_http(capability, query, res, endpoint); 
    }
    
    if(capability.endpointtype == "iotivity") {
        console.log("Accessing iotivity endpoint");
        request_iotivity(capability, query, res, endpoint);
    }
}



function sendPutRequest(capability, query, res) {
    var endpoint = Mustache.render(capability.endpoint, query);
    console.log(capability.endpointtype + '://' + endpoint);

    if(capability.endpointtype == "http" || capability.endpointtype == "https") {
        console.log("Accessing http/https endpoint");
//        request_http_put(capability, query, res, endpoint); 
    }
    
//    if(capability.endpointtype == "iotivity") {
//        console.log("Accessing iotivity endpoint");
//        request_iotivity(capability, query, res, endpoint);
//    }
}



function processRequest(query, res, callback){
    servicedb.find({ 'id': query.id }, function (err, service) {
        if(err)
            res.send(err);

        for( var i = 0, len = service[0].capability.length; i < len; i++) {
            if(match(service[0].capability[i], { id: query.capabilityid })) {
                return callback(service[0].capability[i], query, res);
            }
        }
    });
};





module.exports = function(app) {

    // api ---------------------------------------------------------------------
    app.get('/api/services', function(req, res) {

        getServices(res);
    });

    app.post('/api/service', function(req, res) {

        var jsonObj = JSON.parse(req.body.text);
        console.log(jsonObj);
        
        servicedb.create({
            id:         jsonObj.id,
            name:       jsonObj.name,
            desc:       jsonObj.description,
            platforms : jsonObj.platforms,
            auth :      jsonObj.auth,
            capability: jsonObj.capability
        }, function(err, service) {
            if (err)
                res.send(err);
            getServices(res);
        });

    });

    // delete a service
    app.delete('/api/service/:id', function(req, res) {
        console.log(req.params.id);
        servicedb.find({name:req.params.id}).remove({
        }, function(err, service) {
            if (err)
                res.send(err);

            getServices(res);
        });
    });
    
    
    app.get('/api/service/:id', function(req, res) {
       console.log('GET req.query:');
       console.log(req.query);
       console.log(req.query.id);
       processRequest(req.query, res, sendRequest);
    });

    app.put('/api/service/:id', function(req, res) {
       console.log('PUT req.query:');
       console.log(req.query);
       console.log(req.query.id);
       processRequest(req.query, res, sendPutRequest);
    });

    // application -------------------------------------------------------------
    app.get('*', function(req, res) {
        res.sendfile('./public/index.html'); // load the single view file (angular will handle the page changes on the front-end)
    });
};
