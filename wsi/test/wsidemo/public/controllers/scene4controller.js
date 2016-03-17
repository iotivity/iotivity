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

        $scope.observeIoTivityCap = function() {
            var uri = "http://localhost:8080/wsi/cap/org.openinterconnect";
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
