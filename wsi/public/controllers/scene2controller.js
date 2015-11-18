(function () {
    var scene2controller = function ($scope, $http) {
        
        $scope.scene2updates = [
            {
                title : "Success",
                content : "Facebook Service Initialized"
            }
        ]
        
        var findresource = {
            "cid": "org.iotivity.findresource",                    
            "endpointtype": "IOTIVITY",
            "operation": "GET",
            "resourceType" : "all"
        };
                
        var getresource = {
            "cid": "org.iotivity.getresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "IOTIVITY",
            "operation": "GET",
            "resourceID" : "",
            "params": 
            {
                "address": "server ip address",
                "port": "server port",
                "uri": "server's uri"
            }
        };
        
        var putresource = {
            "cid": "org.iotivity.putresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "IOTIVITY",
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
        
        var postfb = {
                    "cid": "com.facebook.post",
                    "isauthrequired": "true",
                    "description": "Post Message to Facebook.",
                    "endpoint": "https://graph.facebook.com/me/feed?message={{message}}&access_token={{access_token}}",
                    "endpointtype": "facebook",
                    "operation": "POSTTOFB",
                    "params": {
                        "message": "Message to post"
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
                };
                
        document.getElementById('fbcap').value = JSON.stringify(postfb);
        document.getElementById('iotivitycap').value = JSON.stringify(findresource);


        $scope.postFB = function(){
            document.getElementById('fbcap').value = JSON.stringify(postfb);
        };
        $scope.executeFbCap = function(){
            var uri = location.origin + "/wsi/cap/post/com.facebook";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('fbcap').value);
            
            var res = $http.post(uri, reqbody);
            res.success(function(data, status, headers, config) {
                console.log("Success Response = " + data );
                $scope.scene2updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene2updates.push({title: 'Failure', content: data});
            });
        };
        
        $scope.getLightColor = function() {
            document.getElementById('iotivitycap').value = JSON.stringify(getresource);
        };
        
        $scope.setLightColor = function() {
            document.getElementById('iotivitycap').value = JSON.stringify(putresource);
        };

        $scope.executeIoTivityCap = function() {
            var uri = location.origin + "/wsi/cap/post/org.iotivity";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('iotivitycap').value);
            
            var res = $http.post(uri, reqbody);
            res.success(function(data, status, headers, config) {
                console.log("Success Response = " + data );
                $scope.scene2updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene2updates.push({title: 'Failure', content: data});
            });
        };        
        
        
    };

    scene2controller.$inject = ['$scope', '$http'];

    angular.module('wsi').controller('scene2controller', scene2controller);
}());