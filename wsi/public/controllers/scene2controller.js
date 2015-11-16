(function () {
    var scene2controller = function ($scope, $http) {
        
        $scope.scene2updates = [
            {
                title : "Success",
                content : "Facebook Service Initialized"
            }
        ]
        
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
    };

    scene2controller.$inject = ['$scope', '$http'];

    angular.module('wsi').controller('scene2controller', scene2controller);
}());