(function () {
    var scene1controller = function ($scope, $http) {
        
        $scope.scene1updates = [
            {
                title : "Success",
                content : "Twitter Service Initialized"
            }
        ];
        
        var findresource = {
            "cid": "org.openinterconnect.findresource",                    
            "endpointtype": "OCF",
            "operation": "GET",
            "resourceType" : "all"
        };
                
        var getresource = {
            "cid": "org.openinterconnect.getresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "OCF",
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
            "cid": "org.openinterconnect.putresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "OCF",
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
            var uri = "http://localhost:8080/wsi/cap/com.twitter";
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
                            getresource.params.address = obj.address;
                            getresource.params.port = obj.port;
                            getresource.params.uri = obj.uri;

                            putresource.params.address = obj.address;
                            putresource.params.port = obj.port;
                            putresource.params.uri = obj.uri;

                        }
                    }
                }
                $scope.scene1updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene1updates.push({title: 'Failure', content: data});
            });
        };
        
    };

    scene1controller.$inject = ['$scope', '$http'];

    angular.module('wsidemo').controller('scene1controller', scene1controller);
}());
