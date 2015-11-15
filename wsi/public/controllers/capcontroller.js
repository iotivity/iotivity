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
                                if (obj.isauthrequired == "true")
                                {
                                    restfactory.auth(jsonObj);
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
                                restfactory.post($scope.selService, obj);
                            }
                        }
                        return;
                    });
        };
    };
    capcontroller.$inject = ['$scope', 'dbfactory', 'restfactory'];
    angular.module('wsi').controller('capcontroller', capcontroller);
}());