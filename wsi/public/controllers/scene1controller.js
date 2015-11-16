(function () {
    var scene1controller = function ($scope, $http) {
        
        $scope.scene1updates = [
            {
                title : "Success",
                content : "Twitter Service Initialized"
            }
        ]
        
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
                
        document.getElementById('tweetcap').value = JSON.stringify(posttweet);

        $scope.readTweet = function(){
            document.getElementById('tweetcap').value = JSON.stringify(readtweet);
        };
        $scope.postTweet = function(){
            document.getElementById('tweetcap').value = JSON.stringify(posttweet);
        };
        $scope.executeTweetCap = function(){
            var uri = "http://localhost:8080/wsi/cap/post/com.twitter";
            console.log("Making a POST HTTP Request");
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
    };

    scene1controller.$inject = ['$scope', '$http'];

    angular.module('wsi').controller('scene1controller', scene1controller);
}());