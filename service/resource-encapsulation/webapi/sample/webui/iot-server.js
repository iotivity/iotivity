/******************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/


var iotivityre = require('./../../js/iotivity-re.js');
var server = require('http').createServer(handler);
var fs = require('fs');
var url = require('url');
var qs = require('querystring');
var path = require('path');
var io = require('socket.io').listen(server);

var PROFILING_ENABLED = 0;

/*********************************************
* variables to maintain status 
**********************************************/
var STATUS_NONE = 0;
var STATUS_ALIVE = 1;
var STATUS_REQUESTED = 2;
var STATUS_LOST_SIGNAL = 3;
var STATUS_DESTROYED = 4;


iotivityre.initPlatform();

var reqGetAttrId =0;
var  requestGetAttrMap={};

var reqSetAttrId =0;
var  requestSetAttrMap={};

var reqDiscoveryId =0;
var requestDiscoveryMap={};

/*****************************************************************
 * Function Name::SendEmitResponse 
 * Description:: function to send caching response
 * In Para:: 
 *           handle ::unique resource identifier
 *           mapAttributes::a map containing resource attributes
 *			 socket:: reference to the created socket
 *			 deviceName:: name of the device	
 * Out Para:: None
 *****************************************************************/
 
function SendEmitResponse(handle, mapAttributes, socket, deviceName)
{
	console.log("Entry Send Emit response function");
	
	var deviceAddress = iotivityre.getAddress(handle);
	var deviceUri = iotivityre. getUri(handle);
	var resourceTypes = iotivityre.getResourceTypes(handle);
	var resourceType = resourceTypes[0];
	
	socket.emit('caching-response', { "Handle":handle,"Name":deviceName ,"Address":deviceAddress,"Uri": deviceUri, "Type": resourceType,"Attributes":mapAttributes});
	console.log("Exit Send Emit response function");
}

/**************************************************
 * Function Name::SendAttributeResponse 
 * Description:: function to send response for setAttribute
 *				 request.
 * In Para:: 
 *           handle ::unique resource identifier
 *           mapAttributes::a map containing resource 
 *							attributes
 *			res:: reference to the response object
 *			deviceName:: name of the device	
 *			type :: type of the resource
 *			
 * Out Para:: None
 *********************************************************/
 
function SendAttributeResponse(handle, mapAttributes, res, deviceName, type, flag)
{   console.log("Entry Send Attributes response function");
	
	var accessResponse = {};
	var Attributes= {};	

	var devAddress = iotivityre.getAddress(handle);
	var devUri = iotivityre. getUri(handle);		
					
	accessResponse.Attributes= mapAttributes;
	accessResponse.Status=STATUS_ALIVE;	
	accessResponse.Handle=handle;
	accessResponse.Name=deviceName;
	accessResponse.Address= devAddress;
	accessResponse.Uri=devUri;
	accessResponse.Type=type;

	console.log("After :: --->"+JSON.stringify(accessResponse));
	res.writeHead(200, {'Content-Type': 'application/json', 'Access-Control-Allow-Origin': '*'});
	res.write(JSON.stringify(accessResponse));
	res.end();
	
	console.log("Exit Send Attributes response function");
}

/**************************************************
 * Function Name::handler
 * Description:: function to handle request from client
 * In Para:: 
 *           req:: request object
 *			 res:: response object 
 * 
 * Out Para:: None
 *********************************************************/

function handler (req, res) {

	var parsedUrl = url.parse(req.url,true);
    console.log("Received req = " + req.method + " " + parsedUrl.pathname) ;
    
	if (req.method === 'POST') {
		
		console.log("POST Request Received");

      if(parsedUrl.pathname === '/signup') 
	  {
			fs.createReadStream('table.html').pipe(res);
      }
	  else if(parsedUrl.pathname === '/discover')
	  {
		  console.log("discovery:: type " + discoverResourceType);	
      }  
	  else if (parsedUrl.pathname === '/getremoteattr')
	  {
		  
		    console.log("/getremoteattr, getremoteattr request:: ");
			var body = '';
			req.on('data', function(data){ body += data;});
			
			//request data is json
			req.on('end', function(){
				
				var json = JSON.parse(body);
				console.log("getremoteattr req handle received ::" +json.Handle);
				
				if(PROFILING_ENABLED == 1)
				{
					reqGetAttrId++;
					var curTime=new Date().getTime();
					
					var requestTime = {"id" : reqGetAttrId, "time" : curTime} ;
					
					requestGetAttrMap[json.Handle] = requestTime;
					
					console.log("[PROFILING][GET REMOTE ATTR] [ReqId] = "+reqSetAttrId+  " [Time] = "+curTime);
				}
		
				//iotivity callback getRemoteAttributes
				iotivityre.getRemoteAttributes(json.Handle, function callback(handle, mapAttributes) {	
					
					if(PROFILING_ENABLED == 1)
					{
						var requestTime = requestGetAttrMap[handle];
						var curTime=new Date().getTime();
						
						var totalTime = curTime - requestTime.time;
						console.log("[PROFILING][GET REMOTE ATTR] [RespId] = "+requestTime.id+  " [Time] = "+curTime + " [TotalTime] = " + totalTime + " ms");
					}
					
                    var resourceTypes = iotivityre.getResourceTypes(handle);
					if(resourceTypes[0] == "core.light")
				    {
						SendAttributeResponse(handle, mapAttributes, res, "Light", resourceTypes[0] ,0);
					}else if(resourceTypes[0] == "oic.r.refrigeration")
					{
						SendAttributeResponse(handle, mapAttributes, res, "Fridge", resourceTypes[0], 0);
					}else if(resourceTypes[0] == "oic.r.airconditioner")
					{
						SendAttributeResponse(handle, mapAttributes, res, "AC", resourceTypes[0], 0);
					}

				});
			});	
			
		}
		else if (parsedUrl.pathname === '/getcachedattr')
		{
		  
		    console.log("/getcachedattr, getcachedattr request:: ");
			var body = '';
			req.on('data', function(data){ body += data;});
			
			//request data is json
			req.on('end', function(){
				
				var json = JSON.parse(body);
				console.log("getcachedattr req handle received ::" +json.Handle);
				
				if(PROFILING_ENABLED == 1)
				{
					reqGetAttrId++;
					var curTime=new Date().getTime();
					
					var requestTime = {"id" : reqGetAttrId, "time" : curTime} ;
					
					requestGetAttrMap[json.Handle] = requestTime;
					
					console.log("[PROFILING][GET CACHED ATTR] [ReqId] = "+reqSetAttrId+  " [Time] = "+curTime);
				}
		
				//iotivity callback getRemoteAttributes
				var mapAttributes = iotivityre.getCachedAttributes(json.Handle);	
					
				if(PROFILING_ENABLED == 1)
				{
					var requestTime = requestGetAttrMap[json.Handle];
					var curTime=new Date().getTime();
					
					var totalTime = curTime - requestTime.time;
					console.log("[PROFILING][GET CACHED ATTR] [RespId] = "+requestTime.id+  " [Time] = "+curTime + " [TotalTime] = " + totalTime + " ms");
				}
				
				var resourceTypes = iotivityre.getResourceTypes(json.Handle);
				if(resourceTypes[0] == "core.light")
				{
					SendAttributeResponse(json.Handle, mapAttributes, res, "Light", resourceTypes[0] ,0);
				}else if(resourceTypes[0] == "oic.r.refrigeration")
				{
					SendAttributeResponse(json.Handle, mapAttributes, res, "Fridge", resourceTypes[0], 0);
				}else if(resourceTypes[0] == "oic.r.airconditioner")
				{
					SendAttributeResponse(json.Handle, mapAttributes, res, "AC", resourceTypes[0], 0);
				}
			});	
		}
		else if (parsedUrl.pathname === '/setattr')
		{   console.log("/setattr ,set Attributes request");
			
			var body = '';
			var attrMap={};
			
			req.on('data', function(data){ body += data;});
			
			//request data is json
			req.on('end', function(){
				
				var json = JSON.parse(body);
				var obj=json["Attributes"];
				var loopVar=0;
				
				//traverse through the arrtibute list and make a map{key,value}
				for (var key in obj) 
				{ 	
					if (obj.hasOwnProperty(key)) 
					{
						
						attrMap[key]=obj[key];
						console.log("attribute received "+key+"value ::"+obj[key] + " Type of Value : " + typeof(obj[key]));
					}
					
					loopVar++;
				}
				
				if(PROFILING_ENABLED == 1)
				{
					reqSetAttrId++;
					var curTime = new Date().getTime();
					
					var requestTime = {"id" : reqSetAttrId, "time" : curTime} ;
					
					requestSetAttrMap[json.Handle] = requestTime;
					
					console.log("[PROFILING][SET ATTR] [ReqId] = "+reqSetAttrId+  " [Time] = "+curTime);
				}
				
				//iotivity callback setRemoteAttributes
				iotivityre. setRemoteAttributes(json.Handle, attrMap ,function callback(handle, mapAttributes) {	
					console.log("before entry callback setRemoteAttributes");
					
					if(PROFILING_ENABLED == 1)
					{
						var requestTime = requestSetAttrMap[handle];
						var curTime=new Date().getTime();
						
						var totalTime = curTime - requestTime.time;

						console.log("[PROFILING][SET ATTR] [RespId] = "+requestTime.id+  " [Time] = "+curTime + " [TotalTime] = " + totalTime + " ms");
					}
					
                    var resourceTypes = iotivityre.getResourceTypes(handle);
					if(resourceTypes[0] == "core.light")
				    {
						SendAttributeResponse(handle, mapAttributes, res, "Light", resourceTypes[0], 1);
					}
					else if(resourceTypes[0] == "oic.r.refrigeration")
					{
						SendAttributeResponse(handle, mapAttributes, res, "Fridge", resourceTypes[0], 1);

					}
					else if(resourceTypes[0] == "oic.r.airconditioner")
					{
						SendAttributeResponse(handle, mapAttributes, res, "AC", resourceTypes[0], 1);
					}

				});
			});	
			
		} 
		else if (parsedUrl.pathname === '/observe') 
	    {
			console.log("/observe request received");
		}

    }
	else if (req.method === 'GET') 
	{	console.log("GET Request Received");
        
		var request = req;
		var response = res;
	
		var filePath = '.' + parsedUrl.pathname;
		if (filePath == './')
			filePath = './home.html';
		
		var extname = path.extname(filePath);
		var contentType = 'text/html';

        console.log(filePath);
		

		switch (extname) {
			case '.js':
				contentType = 'text/javascript';
				break;
			case '.css':
				contentType = 'text/css';
				break;
			case '.json':
				contentType = 'application/json';
				break;
			case '.png': {
				contentType = 'image/png';
                            //filePath = "oic.r.light.png";
				break; 
                     }     
		}

		fs.readFile(filePath, function(error, content) {
			if (error) {
				response.writeHead(500);
				response.end('Error in opening file resource '+error.code+' ..\n');
				response.end(); 
			}
			else {
				response.writeHead(200, { 'Content-Type': contentType });
				response.end(content, 'utf-8');
			}
		});
    }
}


io.sockets.on('connection', function (socket) {
	socket.emit('iotivity-push-msg', { ConnectionStatus: 'Connection established' });
	
	/*Receiving discovery request*/
	socket.on('discover-request', function (data) {

		/*Define variables for creating response JSON*/
		var discoveryResponse = {};
		var resourceList = [];
		var discoveryReq = data;
		
		//making a proper response msg discovery reply
		discoveryResponse.Status = STATUS_NONE;
		//discoveryResponse.ResourceType = discoverResourceType ;
		discoveryResponse.ResourceList = resourceList;
	  
		//This variable holds type of the resource to be discovered 
		var discoverResourceType = discoveryReq.Resource.Type;
		if(PROFILING_ENABLED == 1)
		{
			reqDiscoveryId++;
			var curTime=new Date().getTime();
			
			var requestTime = {"id" : reqDiscoveryId, "time" : curTime} ;
			
			requestDiscoveryMap[discoverResourceType] = requestTime;
			
			console.log("[PROFILING][Discovery] [ReqId] = "+reqDiscoveryId+  " [Time] = "+curTime);
		}

		//iotivity discoverResourceByType called	
		iotivityre.discoverResourceByType("/oic/res", discoverResourceType , function callback(handle) {
			var resourceTypes = iotivityre.getResourceTypes(handle);
			var resourceType = resourceTypes[0];
			
			if(PROFILING_ENABLED == 1)
			{
				var requestTime = requestDiscoveryMap[resourceType];
				var curTime=new Date().getTime();
				
				var totalTime = curTime - requestTime.time;
				
				console.log("[PROFILING][Discovery] [RespId] = "+requestTime.id+  " [Time] = "+curTime + " [TotalTime] = " + totalTime + " ms");
			}
			
			var deviceAddress = iotivityre.getAddress(handle);
			var deviceUri = iotivityre. getUri(handle);
			
			var deviceName = "";
			if(resourceType == "core.light")  
			{
				 deviceName = "Light";
			}
			else if(resourceType == "oic.r.refrigeration")
			{
				 deviceName = "Fridge";
			}
			else if(resourceType == "oic.r.airconditioner")
			{
				 deviceName = "AC";
			}

			/*Sending discovery response*/
			socket.emit('discover-response', { "Handle":handle,"Name":deviceName ,"Address":deviceAddress,"Uri": deviceUri, "Type": resourceType});
			console.log('Server pushed discovered device on client');		
		});  
	});
	//start monitoring
	
	/*Receiving start monitor request*/
	socket.on('start-monitor', function (data) {
              var json = JSON.parse(data);

		console.log("start monitoring request");
		var handle = json.Handle;
		console.log("Given Handle : " + handle);
		iotivityre.startMonitoring(handle, function callback(handle, status) {
	        console.log("inside monitor response");
			var deviceAddress = iotivityre.getAddress(handle);
			var deviceUri = iotivityre. getUri(handle);
			var resourceTypes = iotivityre.getResourceTypes(handle);
			var resourceType = resourceTypes[0];
			var deviceName = "";
			if(resourceType == "core.light")  
			{
				 deviceName = "Light";
			}
			else if(resourceType == "oic.r.refrigeration")
			{
				 deviceName = "Fridge";
			}
			else if(resourceType == "oic.r.airconditioner")
			{
				 deviceName = "AC";
			}

			//emit monitor response
			socket.emit('monitor-response', { "Handle":handle,"Name":deviceName ,"Address":deviceAddress,"Uri": deviceUri, "Type": resourceType,"Status":status});
			console.log('[Monitor]::Server pushed device status on client');		
		});  
	});
		//Receiving stop monitor request
		socket.on('stop-monitor', function (data) {
        
			var json = JSON.parse(data);
			console.log("[Monitoring]::stop monitoring request");
			var handle = json.Handle;
			console.log("Given Handle : " + handle);
			iotivityre.stopMonitoring(handle);
		
		});
	
		//Receiving stop caching request
		socket.on('stop-caching', function (data) {
        
			var json = JSON.parse(data);
			console.log("[Caching]::stop caching request");
			var handle = json.Handle;
			console.log("Given Handle : " + handle);
			iotivityre.stopCaching(handle);
		
		});
	
	//Receiving start caching request
	socket.on('active-start-caching', function (data) {
              var json = JSON.parse(data);

		console.log("start active caching request");
		var handle = json.Handle;
		console.log("Given Handle : " + handle);
		iotivityre.startCachingWithCb(handle, function callback(handle, attributeMap) {
	        console.log("inside caching response");
			var deviceAddress = iotivityre.getAddress(handle);
			var deviceUri = iotivityre. getUri(handle);
			var resourceTypes = iotivityre.getResourceTypes(handle);
			var resourceType = resourceTypes[0];
			var deviceName = "";
			if(resourceType == "core.light")  
			{
				 deviceName = "Light";
			}
			else if(resourceType == "oic.r.refrigeration")
			{
				 deviceName = "Fridge";
			}
			else if(resourceType == "oic.r.airconditioner")
			{
				 deviceName = "AC";	  
			}
			 SendEmitResponse(handle, attributeMap, socket, deviceName);

		
			console.log('[Caching]::Server pushed cached attributes to client');		
		});  
	});
	
	socket.on('passive-start-caching', function (data) {
        var json = JSON.parse(data);

		console.log("start passive caching request");
		var handle = json.Handle;
		console.log("Given Handle : " + handle);
		iotivityre.startCachingWithoutCb(handle);	
	}); 
	
});

if(process.argv[3] == undefined)
	PROFILING_ENABLED = 0;
else
	PROFILING_ENABLED = process.argv[3];

console.log("PROFILING STATUS " + PROFILING_ENABLED);

//start server to listen for incoming request 
console.log("Started iotivity-server, listening on port " + process.argv[2]);
server.listen(parseInt(process.argv[2]));



