(function () {
    var scene5controller = function ($scope, $http) {
        
        $scope.scene1updates = [
            {
                title : "Success",
                content : "Uber Service Initialized"
            }
        ];
        
        var getProfile = {
        "cid": "com.uber.getProfile",
                    "isauthrequired": "true",
                    "description": "Get user profile",
                    "endpoint": "https://api.uber.com/v1/me?access_token={{access_token}}",
                    "endpointtype": "uber",
                    "operation": "GETPROFILE",
                    "params": {
                        
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
        };
        
                
        var getPrice = {
            "cid": "com.uber.getPrice",
                    "isauthrequired": "true",
                    "description": "Get estimate price",
                    "endpoint": "https://api.uber.com//v1/estimates/price?server_token={{server_token}}&start_latitude={{start_latitude}}&start_longitude={{start_longitude}&end_latitude={{end_latitude}}&end_longitude={{end_longitude}}",
                    "endpointtype": "uber",
                    "operation": "GETPRICE",
                    "params": {
                        "start_latitude": "",
                        "start_longitude" : "",
                        "end_latitude" : "",
                        "end_longitude" : ""
                    },
                    "tags": [
                        "share",
                        "post"
                    ]
        };
                
        document.getElementById('ubercap').value = JSON.stringify(getProfile);

        $scope.getProfile = function(){
            document.getElementById('ubercap').value = JSON.stringify(getProfile);
        };
        $scope.getPrice = function(){
            document.getElementById('ubercap').value = JSON.stringify(getPrice);
        };
                
        $scope.executeUberCap = function(){
            var uri = "http://localhost:8080/wsi/cap/com.uber";
            console.log("Making a POST HTTP Request " + uri);
            reqbody = JSON.parse(document.getElementById('ubercap').value);
            
            console.log("reqbody:");
            console.log(JSON.stringify(reqbody));
            
            //TODO: Refect response on the demo screen. The results are shown on the console ATM
        };
    };

    scene5controller.$inject = ['$scope', '$http'];

    angular.module('wsidemo').controller('scene5controller', scene5controller);
}());
