(function () {
    var scene3controller = function ($scope, $http) {
        $scope.scene3updates = [
            {
                title : "Success",
                content : "RVI Service Initialized"
            }
        ]
        
        var findrviservices = {
                "cid": "org.genivi.rvi.findrviservices",                    
                "endpointtype": "RVI",
                "operation": "GET",
        };
                    
        var changeHVAC = {
            "cid": "org.genivi.rvi.getresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "RVI",
            "operation": "GET",
            "resourceID" : "",
            "params": 
            {
                "address": "server ip address",
                "port": "server port",
                "uri": "server's uri"
            }
        };
        
        var subHVAC = {
            "cid": "org.genivi.rvi.putresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "RVI",
            "operation": "POST",
            "resourceID" : "",
            "params": {
                  "address": "server ip address",
                  "port": "server port",
                  "uri": "server's uri"
             },
            "payload": {
                 "property" : "value"
             },
            "tags": [
              "put reosurce properties and value"
            ]
        };
        
        var unsubHVAC = {
            "cid": "org.genivi.rvi.putresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "RVI",
            "operation": "POST",
            "resourceID" : "",
            "params": {
                  "address": "server ip address",
                  "port": "server port",
                  "uri": "server's uri"
             },
            "payload": {
                 "property" : "value"
             },
            "tags": [
              "put reosurce properties and value"
            ]
        };        
        var subLocation = {
                "cid": "org.genivi.rvi.putresource",
                "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                "endpointtype": "RVI",
                "operation": "POST",
                "resourceID" : "",
                "params": {
                      "address": "server ip address",
                      "port": "server port",
                      "uri": "server's uri"
                 },
                "payload": {
                     "property" : "value"
                 },
                "tags": [
                  "put reosurce properties and value"
                ]
            };
            
        var unsubLocation = {
                "cid": "org.genivi.rvi.putresource",
                "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                "endpointtype": "RVI",
                "operation": "POST",
                "resourceID" : "",
                "params": {
                      "address": "server ip address",
                      "port": "server port",
                      "uri": "server's uri"
                 },
                "payload": {
                     "property" : "value"
                 },
                "tags": [
                  "put reosurce properties and value"
                ]
            };
	        var regSmartHome = {
                "cid": "org.genivi.rvi.putresource",
                "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
                "endpointtype": "RVI",
                "operation": "POST",
                "resourceID" : "",
                "params": {
                      "address": "server ip address",
                      "port": "server port",
                      "uri": "server's uri"
                 },
                "payload": {
                     "property" : "value"
                 },
                "tags": [
                  "put reosurce properties and value"
                ]
            };

	      document.getElementById('rvicap').value = JSON.stringify(findrviservices);
        
        

        <button ng-click="()" type="button" class="btn btn-primary">Get All Services</button>                                
        <button ng-click="()" type="button" class="btn btn-success">Change HVAC</button>
    </div>
    <div class="form-group">
        <label>RVI Capability</label>
        <textarea id="rvicap" class="form-control" rows="5"></textarea>
        <button ng-click="()" type="button" class="btn btn-primary">Submit</button>        
        
        $scope.subscribehvac = function() {
            document.getElementById('rvicap').value = JSON.stringify(subscribehvac);
        };
        
        $scope.setLightColor = function() {
            document.getElementById('rvicap').value = JSON.stringify(putresource);
        };

        $scope.changeHVAC    = function() {
        	
        }
        
        $scope.executeRVICap = function() {
        	
        }
        
        $scope.findRVIServices = function() {
            var uri = location.origin + "/wsi/cap/org.genivi.rvi";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('rvicap').value);
            
            var res = $http.post(uri, reqbody);
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
