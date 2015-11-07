(function () {
    var capcontroller = function ($scope, dbfactory, restfactory) {
        
        $scope.initCapability = function (capid) {
            console.log("InitCapability invoked" + capid);
            dbfactory.read($scope.selService)
                    .success(function (data) {
                        var jsonObj = data[0];
                        for (var i = 0; i < jsonObj.capability.length; i++) {
                            var obj = jsonObj.capability[i];
                            var auth = jsonObj.auth;
                            if (obj.cid == capid) {
                                if (obj.endpointtype.toUpperCase() == "REST") {
                                    //TODO request chaining
                                    if (obj.isauthrequired == "true")
                                    {
                                        restfactory.auth(jsonObj);
                                    }
                                }
                                if (obj.endpointtype.toUpperCase() == "IOTIVITY") {
                                }
                            }
                        }
                        return $scope;
                    });
        };
        
        //Run a Capability
        $scope.runCapability = function (capid) {
            console.log("RunCapability invoked" + capid);
            dbfactory.read($scope.selService)
                    .success(function (data) {
                        var jsonObj = data[0];
                        for (var i = 0; i < jsonObj.capability.length; i++) {
                            var obj = jsonObj.capability[i];
                            if (obj.cid == capid) {
                                if (obj.endpointtype.toUpperCase() == "REST") {
                                    if (obj.operation.toUpperCase() == "GET")
                                    {
                                        restfactory.get(obj);
                                    }
                                    if (obj.operation.toUpperCase() == "POST")
                                    {
                                        restfactory.post(obj);
                                    }
                                }
                                if (obj.endpointtype.toUpperCase() == "IOTIVITY") {
                                    if (obj.operation.toUpperCase() == "GET")
                                    {
                                        restfactory.get(obj);
                                    }
                                }
                            }
                        }
                        return;
                    });
        };
    };
    capcontroller.$inject = ['$scope', 'dbfactory', 'restfactory'];
    angular.module('wsi').controller('capcontroller', capcontroller);
}());