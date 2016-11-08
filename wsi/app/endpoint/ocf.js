var async = require("async"),
	chain = "http://localhost:8080/wsi/iot/noti",
	request = require('request'),
	isObserving = false;
	ocfapp = null,
	ocfmodel = null,
	thingmap = new Object(),
	propmap = new Object();
var device = require("iotivity-node").client;
var endpointList = [];

var ocfupdate = {
	"cid": "org.openinterconnect.ocfupdate",
	"endpointtype": "OCF",
	"operation": "UPDATE",
	"params":{
		"target" : "OCF_UPDATE",
		"status" : {}
	},
	"tags": [
		"Update to a created OCF server resource"
	]
};

var findocfresponse = {
	"params":{
		"target" : "FINDOCFDEVICES",
		"status" : {}
	}
};

var ocfupdateresponse = {
	"params":{
		"target" : "UPDATEDEVICESTATUS",
		"status" : {}
	}
};


getproperties = function(callback, res) {
	var len = Object.keys(thingmap).length;
	if(len==0){
		console.log("No devices to search for.");
		return {};
	}
	len = Object.keys(propmap).length;
	if(len!=0){
		console.log("Completed Finding Properties.");
		callback(res);
		return propmap;
	}

    //Discover
	async.forEach(Object.keys(thingmap), function(key, callback){
		var thing = thingmap[key];
		device.retrieve(thing.id).then(
			function( prop ) {
				console.log("Client: Retrieving " + JSON.stringify(thing.id) + " succeeded: " + JSON.stringify(thing.observable));
				propmap[thing.id.path] = prop.properties;
			},
			function( error ) {
				console.log("Client: Retrieving " + JSON.stringify(thing.id) + "Failed " + error);
			}
		);
	}, function(){
		console.log("Error while finding properties");
	});
	setTimeout(function(){
		console.log("Completed Finding Properties.");
		callback(res);
		return propmap;
	},15000);	
}

resourceUpdate = function(res){
	console.log("Resource To Be  Updated" + res.resourcePath);
	ocfmodel[res.resourcePath] = res.properties;
}


rescallback = function(res){
	if(res) {
		findocfresponse.params.status = propmap;
		res.status(200).send(findocfresponse);
	}

	if(!isObserving)
	{
		for (var key in thingmap) {
			if (!thingmap.hasOwnProperty(key)) continue;
			if(thingmap[key].observable) {
				var obj = thingmap[key];
				console.log("Started Observing " + JSON.stringify(obj.id.path));
				obj.addEventListener( "change", resourceUpdate );
			}
		}
		isObserving = true;
	}else{
		console.log("Observing resource already ");
	}

}

device.on("resourcefound",  function( response ) {
	console.log(response);
	if(response.resourcePath.indexOf("/oic") !== 0)
	{
		endpoint = response.resourcePath;
		console.log( "Discovered resource:" + JSON.stringify(response.resourcePath));
		thingmap[endpoint] = response;
		endpointList.push(endpoint);
	}else{
		console.log( "Ignoring resource:" + JSON.stringify(response.resourcePath));
	}
});

findresource = function(cap, res){
	console.log("Finding resources...");

	var len = Object.keys(thingmap).length;
	if(len!=0){
		console.log("Completed Finding Devices.");
		getproperties(rescallback, res);
	}else{
		//device.addEventListener( "resourcefound", findEventListener);
		device.findResources().catch( function( error ) {
			console.error( error.stack ? error.stack : ( error.message ? error.message : error ) );
		});
		setTimeout(function(){
			console.log("Completed Finding Devices.");
			//already discovered
			var len = Object.keys(propmap).length;
			if(len!=0){
				console.log("Completed Finding Properties.");
				callback(res);
			}else{
				getproperties(rescallback, res);
			}
			//device.removeEventListener("resourcefound", findEventListener);
		},15000);
	}
}

module.exports = {
	init: function (app, passport) {
		var template = {
			"handler": "ocf",
			"sid": "org.openinterconnect",
			"capability": [
			{
				"cid": "org.openinterconnect.findresource",
				"endpointtype": "OCF",
				"operation": "GET",
				"resourceType" : "resourceType or all"
			},
			{
				"cid": "org.openinterconnect.getresource",
				"endpoint": "oic://{{address}}:{{port}}{{uri}}",
				"endpointtype": "OCF",
				"operation": "GET",
				"params":
				{
					"address": "server ip address",
					"port": "server port",
					"uri": "server's uri"
				}
			},
			{
				"cid": "org.openinterconnect.putresource",
				"endpoint": "oic://{{address}}:{{port}}{{uri}}",
				"endpointtype": "OCF",
				"operation": "PUT",
				"params":
				{
					"address": "server ip address",
					"port": "server port",
					"uri": "server's uri"
				},
				"payload":
				{
					"property" : "value"
				},
				"tags": [
					"put reosurce properties and value"
				]
			}
			]
		};
		findresource(null, null);
		return template;
	},
	request: function (cap, auth, res) {
		if(cap.cid == "org.openinterconnect.findresource"){
			findresource(cap, res);
		}
		else if(cap.cid == "org.openinterconnect.getresource"){
			getresource(cap,res);
		}
		else if(cap.cid == "org.openinterconnect.putresource"){
			putresource(cap,res);
		}
		else if(cap.cid == "org.openinterconnect.observeresource"){
		}
		else if(cap.cid == "org.openinterconnect.createresource"){
		}
	}
}
