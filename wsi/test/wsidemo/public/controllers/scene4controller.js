(function () {
    var scene4controller = function ($scope, $http) {
        
        $scope.scene4updates = [
            {
                title : "Success",
                content : "None Initialized"
            }
        ];
        
        var findresource = {
            "cid": "org.iotivity.findresource",                    
            "endpointtype": "IOTIVITY",
            "operation": "GET",
            "resourceType" : "all"
        };
                
        var observeresource = {
            "cid": "org.iotivity.observeresource",
            "endpoint": "oic://{{address}}:{{port}}/{{uri}}",
            "endpointtype": "IOTIVITY",
            "operation": "GET",
            "resourceID" : "",
            "chain" : "http://localhost:8081/observenotify"
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
                
        document.getElementById('iotivitycap').value = JSON.stringify(findresource);

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
                $scope.scene4updates.push({title: 'Success', content: data});
            });
            res.error(function(data, status, headers, config) {
                console.log("Failed Response = " + data );
                $scope.scene4updates.push({title: 'Failure', content: data});
            });
        };
        
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
            var uri = "http://localhost:8080/wsi/cap/post/org.iotivity";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('iotivitycap').value);
            
            var res = $http.post(uri, reqbody);
            res.success(function(data, status, headers, config) {
                console.log("Success Response = " + data );
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
