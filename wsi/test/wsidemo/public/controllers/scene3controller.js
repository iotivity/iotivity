var rvimsg = [
          {
              "cid": "org.genivi.rvi.connect",
              "isauthrequired": "false",
              "description": "Connect to RVI.",
              "endpoint": "ws://localhost:9008/",
              "endpointtype": "rvi",
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
 	{//0
		"jsonrpc":"2.0",
		"id":"1",
		"method": "get_available_services",
		"params" : {}
    },
    {//1 - Set
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac",
			"parameters":{
				"right_temp":"value",
				"left_temp":"value",
				"fan_speed":"value",
				"defroster":"value"
			}
		}
	},
    {//2 - Get
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/carlocation",
			"parameters":{
				"lat":"value",
				"long":"value",
				"vehicle_name":"value"
			}
		}
	},
	{//3
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/subscribe_hvac",
			'callback_service':'genivi.org/oic_gw/callback/report_hvac'
		}
	},
	{//4
		"jsonrpc":"2.0",
		"id":"1",
		"method": "message",
		"params": {
			"timeout":1459388884,
			"service_name": "genivi.org/oic_gw/hvac/subscribe_carlocation",
			'callback_service':'genivi.org/oic_gw/callback/report_carlocation'
		}
	},
	{//5
		"jsonrpc":"2.0",
		"id":"1",
		"method": "register_service",
		"params": {
			"timeout":1459388884,
			"service_name": "smarthome/control"
		}
	},
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
