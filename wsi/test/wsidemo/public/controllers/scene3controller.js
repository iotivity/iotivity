var rvimsg = [
          {
              "cid": "org.genivi.rvi.connect",
              "isauthrequired": "false",
              "description": "Connect to RVI.",
              "endpoint": "ws://localhost:9008/",
              "endpointtype": "rvi",
              "chain" : "http://localhost:8081/rvicallback",
              "operation": "RVICONNECT",
              "params" : {}
          },
          {
              "cid": "org.genivi.rvi.send",
              "isauthrequired": "false",
              "description": "Connect to RVI.",
              "endpoint": "ws://localhost:9008/",
              "endpointtype": "rvi",
              "operation": "RVISEND",
              "params" : {}
          }
];

var rvibody = [
 	{
		"jsonrpc":"2.0",
		"id":"1",
		"method": "get_available_services",
		"params" : {}
    },
	{
		"jsonrpc":"2.0",
		"id":"1",
		"method": "register_service",
		"params": {
			"timeout":1459388884,
			"service_name": "servicename"
		}
	},
	{
		"jsonrpc":"2.0",
		"id":"1",
		"method": "unregister_service",
		"params": {
			"timeout":1459388884,
			"service_name": "servicename"
		}
	},
	{
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
	        "parameters": {
                "target" : "GETHVAC"
	        }
		}
	},
    {
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
			"parameters":{
				"target" : "SETHVAC",
				"function": "function",
				"newValue": "value"
			}
		}
	},
	{
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/node/vehicle_id/rvi/vehiclecontrol",
			"parameters":{
				"target" : "GETLOCATION"
			}
		}
	}
]; 

(function () {
    var scene3controller = function ($scope, $http) {
        
        $scope.scene3updates = [
            {
                title : "Success",
                content : "App Initialized"
            }
        ];
        
        $scope.connectToRVI = function(){
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[0]);	
        }
        
        $scope.getAllServices = function(){
        	rvimsg[1].params = rvibody[0];
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[1]);	
        }

        $scope.registerService = function(){
        	rvimsg[1].params = rvibody[1];
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[1]);	
        }

        $scope.unregisterService = function(){
        	rvimsg[1].params = rvibody[2];
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[1]);	
        }

        $scope.getHVAC = function(){
        	rvimsg[1].params = rvibody[3];
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[1]);	
        }

        $scope.setHVAC = function(){
        	rvimsg[1].params = rvibody[4];
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[1]);	
        }

        $scope.getVehicleLocation = function(){
        	rvimsg[1].params = rvibody[5];
        	document.getElementById('rvicap').value = JSON.stringify(rvimsg[1]);	
        }
        

        $scope.executeRVICap = function(){
            var uri = "http://localhost:8080/wsi/cap/org.genivi.rvi";
            console.log("Making a POST HTTP Request " + uri);
            var res = $http.post(uri, document.getElementById('rvicap').value);
            res.success(function(data, status, headers, config) {
                console.log("Success Response = " + data );
                $scope.scene3updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene3updates.push({title: 'Failure', content: data});
            });
        };
    };

    scene3controller.$inject = ['$scope', '$http'];

    angular.module('wsidemo').controller('scene3controller', scene3controller);
}());
