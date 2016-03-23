(function () {
    var scene4controller = function ($scope, $http) {
        
        $scope.scene4updates = [
            {
                title : "Success",
                content : "None Initialized"
            }
        ];
        
        var findresource = {
            "cid": "org.openinterconnect.findresource",                    
            "endpointtype": "OCFCLIENT",
            "operation": "GET",
            "resourceType" : "all"
        };
                
        var observeresource = {
            "cid": "org.openinterconnect.observeresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "OCFCLIENT",
            "operation": "GET",
            "chain" : "http://localhost:8081/callback",
            "params": 
            {
                "address": "server ip address",
                "port": "server port",
                "uri": "server's uri"
            }            
        };
        var createresource = {
            "cid": "org.openinterconnect.createresource",
            "endpointtype": "OCFSERVER",
            "operation": "CREATE",
            "chain" : "http://localhost:8081/callback",
            "params":
            {
                "uri": "/a/rvi",
                "type" : "core.rvi",
                "payload": {
                }
            },
            "tags": [
                "create an OCF server with resource"
            ]
        };
        var putresource = {
                "cid": "org.openinterconnect.putresource",
                "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                "endpointtype": "OCFCLIENT",
                "operation": "POST",
                "resourceID" : "",
                "params": {
                      "address": "server ip address",
                      "port": "server port",
                      "uri": "server's uri"
                 },
                "payload": {
					"leftTemperature": 0,
					"rightTemperature" : 0,
					"leftSeatHeat" : 0,
					"rightSeatHeat" : 0,
					"fanSpeed" : 0,
					"fanDown" : 0,
					"fanRight" : 0,
					"fanUp" : 0,
					"fanAC" : 0,
					"fanAuto" : 0,
					"fanRecirc" : 0,
					"defrostMax" : 0,
					"defrostFront" : 0,
					"defrostRear" : 0,
					"lat" : 0,
					"lon" : 0,
					"bearing" : 0,
					"vehicle_id" : "JLR"
				},
                "tags": [
                  "put reosurce properties and value"
                ]
            };        
        
        
        var putresource = {
                    "cid": "org.openinterconnect.putresource",
                    "endpoint": "oic://{{address}}:{{port}}{{uri}}",
                    "endpointtype": "OCFSERVER",
                    "operation": "PUT",
                    "params":
                    {

                    },
                    "tags": [
                        "put reosurce properties and value"
                    ]
        }
        
        
                
        document.getElementById('iotivitycap').value = JSON.stringify(findresource);

        $scope.findIoTivityDevices = function() {
            document.getElementById('iotivitycap').value = JSON.stringify(findresource);
        };
        
        $scope.startObserve = function() {
            document.getElementById('iotivitycap').value = JSON.stringify(observeresource);
        };
        
        $scope.stopObserve = function() {
            document.getElementById('iotivitycap').value = JSON.stringify(observeresource);
        };
        
        $scope.createResource = function() {
        	document.getElementById('iotivitycap').value = JSON.stringify(createresource);
        }
        $scope.putResource = function() {
        	document.getElementById('iotivitycap').value = JSON.stringify(putresource);
        }        
        
        

        $scope.observeIoTivityCap = function() {
            var uri = "http://localhost:8080/wsi/cap/org.openinterconnect.ocfserver";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('iotivitycap').value);
            
            var res = $http.post(uri, reqbody);
            res.success(function(data, status, headers, config) {
                console.log("Success Response = " + JSON.stringify(data));

                var addresses = JSON.parse(JSON.stringify(data));
                
                if(addresses instanceof Array){
                    for(var i = 0; i<addresses.length; i++)
                    {
                        var obj = addresses[i];
                        console.log("Checking : " + obj.uri);
                        if(obj.uri == "/a/wsilight")
                        {
                            observeresource.params.address = obj.address;
                            observeresource.params.port = obj.port;
                            observeresource.params.uri = obj.uri;
                        }
                    }
                }
                $scope.scene4updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene4updates.push({title: 'Failure', content: data});
            });
        };

    };

    scene4controller.$inject = ['$scope', '$http'];

    angular.module('wsidemo').controller('scene4controller', scene4controller);
}());
