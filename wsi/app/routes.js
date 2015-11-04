var servicedb = require('./models/service');


/*
var Mustache = require('mustache');

function getCapabilities(sid){
    var capArray = [];
    
    var service = servicedb.find(function(err, services) {
        if (err)
            res.send(err);

        res.json(services);
    });
    
    var jsonData = JSON.parse(service);
    for (var i = 0; i < jsonData.capability.length; i++) {
        capArray[i] = jsonData.capability[i].id;
    }
    return capArray;
}

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

*/

function getServices(res){
    servicedb.find(function(err, services) {
        if (err) return;
        console.log("getServices = " + services.length);
        res.json(services);
    });
};


module.exports = function(app) {

    app.get('/wsi/services', function(req, res) {
	getServices(res);
    });

    app.post('/wsi/service', function(req, res) {
        var jsonObj = req.body;
        console.log("Create Req Body = " + JSON.stringify(req.body));

        servicedb.create({
            sid:        jsonObj.sid,
            handler:    jsonObj.handler,
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

    app.delete('/wsi/service/:id', function(req, res) {
        console.log("Delete this ID : " + req.params.id);
        servicedb.find({_id:req.params.id}).remove({}, function(err, service) {
            if (err)
                res.send(err);
            getServices(res);
        });
    });

    app.get('/wsi/service/:id', function(req, res) {
        console.log('GET req......query:' + req.params.id);
        servicedb.find({_id:req.params.id}, function(err, service) {
            if (err){
                console.log('Response Service Error: ' + err);
                res.send(err);
            }
            console.log('Response Service: ' + service);
            //res.send(JSON.stringify(JSON.parse(service), undefined, 2));
            res.send(service);
            
        });
    });

    app.put('/wsi/service/:id', function(req, res) {
        var options = { overwrite: true };
        console.log('PUT Param (' + req.params.id + '):');
        var jsonObj = req.body[0];
        console.log('PUT BODY (' + req.body[0] + '):');
        delete jsonObj._id;
        servicedb.findByIdAndUpdate(req.params.id, jsonObj, options, function(err, model) {
            if (err) {
                console.log(err);
                return res.status(500).send(err);
            }
            var response = "Success = " + JSON.stringify(model);
            console.log(response);
        });
    });
    
    app.post('/wsi/cap/auth', function(req, res) {
        var auth = req.body.auth;
        console.log("WSI Cap Auth " + req.body.sid);
        app.settings.strategy[req.body.handler].auth(app, req.body.auth[0]);
    });
    
    app.get('/wsi/cap/get', function(sid, cid, params) {
        
    });
    
    app.post('/wsi/cap/post', function(sid, cid, params, body) {
        
    });

    app.get('*', function(req, res) {
        console.log("Home Page Redirect");
        res.sendFile('index.html', {root : 'public/'});
    });
};
