(function () {
    var scene1controller = function ($scope, $http) {
        
        $scope.scene1updates = [
            {
                title : "Success",
                content : "Twitter Service Initialized"
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

        var posttweet = {
            "cid": "com.twitter.post",
            "isauthrequired": "true",
            "description": "Post Message to Twiiter.",
            "endpoint": "statuses/update",
            "endpointtype": "twitter",
            "operation": "POSTTWEET",
            "params": {
                "text": "Message to post",
                "screen_name" : "oicdemo"
            },
            "tags": [
                "share",
                "post"
            ]
        };
                
        var readtweet = {
            "cid": "com.twitter.read",
            "isauthrequired": "true",
            "description": "Get Latest Message from Twiiter.",
            "endpoint": "statuses/user_timeline",
            "endpointtype": "twitter",
            "operation": "READTWEET",
            "params": {
                "text": "Get message from twitter",
                "screen_name" : "oicdemo"
            },
            "tags": [
                "share",
                "post"
            ]
        };
                
        document.getElementById('tweetcap').value = JSON.stringify(readtweet);
        document.getElementById('iotivitycap').value = JSON.stringify(findresource);

        $scope.readTweet = function(){
            document.getElementById('tweetcap').value = JSON.stringify(readtweet);
        };
        $scope.postTweet = function(){
            document.getElementById('tweetcap').value = JSON.stringify(posttweet);
        };
        $scope.executeTweetCap = function(){
            var uri = location.origin + "/wsi/cap/post/com.twitter";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('tweetcap').value);
            
            var res = $http.post(uri, reqbody);
            res.success(function(data, status, headers, config) {
                console.log("Success Response = " + data );
                $scope.scene1updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene1updates.push({title: 'Failure', content: data});
            });
        };
        
        $scope.findIoTivityDevices = function() {
            document.getElementById('iotivitycap').value = JSON.stringify(findresource);
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
                $scope.scene1updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene1updates.push({title: 'Failure', content: data});
            });
        };
        
    };

    scene1controller.$inject = ['$scope', '$http'];

    angular.module('wsi').controller('scene1controller', scene1controller);
}());