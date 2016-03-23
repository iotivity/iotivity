var servicedb = require('./models/service');

function getServices(res){
    servicedb.find(function(err, services) {
        if (err) return;
        console.log("getServices = " + services.length);
        res.json(services);
    });
};

module.exports = function(app, passport) {
    
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
        servicedb.find({'sid':req.params.id}).remove({}, function(err, service) {
            if (err)
                res.send(err);
            getServices(res);
        });
    });

    app.get('/wsi/service/:id', function(req, res) {
        console.log('GET req......query:' + req.params.id);
        servicedb.find({'sid':req.params.id}, function(err, service) {
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
        servicedb.update({sid : req.params.id}, jsonObj, options, function(err, model) {
            if (err) {
                console.log(err);
                return res.status(500).send(err);
            }
            var response = "Success = " + JSON.stringify(model);
            console.log(response);
        });
    });

    app.post('/wsi/cap/auth', function(req, res) {
        var authcred = req.body.auth[0];
        console.log("WSI Cap Auth " + req.body.sid);
        app.settings.strategy[req.body.handler].auth(authcred, passport, req.body.sid);
        (res.sendStatus || res.send).bind(res)(200);
    });
    
    app.post('/wsi/cap/:id', function(req, res) {
        var cap = req.body;
        console.log("WSI POST " + JSON.stringify(cap));
        servicedb.find({'sid':req.params.id}, function(err, service) {
            if (err){
                console.log('Response Service Error: ' + err);
                res.send(err);
            }
            console.log('Response Service Auth : ' + service);
            auth = service[0].auth[0];
	    var sgy = cap.endpointtype.toLowerCase();
	    if(sgy != null)
            app.settings.strategy[cap.endpointtype.toLowerCase()].request(cap, auth, res);
	    else
	        res.status(400);	
        });
    });

    app.get('*', function(req, res) {
        console.log("Home Page Redirect");
        res.sendFile('index.html', {root : 'public/'});
    });
};
