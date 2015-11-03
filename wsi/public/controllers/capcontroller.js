(function () {
    var capcontroller = function ($scope, dbfactory, restfactory) {
        //Run a Capability
        $scope.runCapability = function () {
            var capid = document.getElementById('capidtext').value;
            dbfactory.read($scope.selService)
                    .success(function (data) {
                        var jsonObj = data[0];
                        for (var i = 0; i < jsonObj.capability.length; i++) {
                            var obj = jsonObj.capability[i];
                            var auth = jsonObj.auth;
                            if (obj.cid == capid) {
                                if (obj.endpointtype.toUpperCase() == "REST") {
                                    if (obj.isauthrequired == "true")
                                    {
                                        alert(JSON.stringify(capid) + " " + JSON.stringify(auth));
                                        restfactory.auth(jsonObj.sid, capid, auth);
                                    }
                                    if (obj.endpointtype.toUpperCase() == "GET")
                                    {
                                        alert("GET   " + obj.endpoint);
                                        restfactory.get(jsonObj.sid, capid, auth);
                                    }
                                    else if (obj.endpointtype.toUpperCase() == "POST")
                                    {
                                        alert("POST " + obj.endpoint);
                                        restfactory.post(jsonObj.sid, capid, auth);
                                    }
                                }
                                if (obj.endpointtype.toUpperCase() == "IOTIVITY") {
                                }
                            }
                        }
                    });
        };
    };
    capcontroller.$inject = ['$scope', 'dbfactory', 'restfactory'];
    angular.module('wsi').controller('capcontroller', capcontroller);
}());