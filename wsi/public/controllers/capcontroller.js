(function () {
    var capcontroller = function ($scope, dbfactory, restfactory) {
        //Run a Capability
        $scope.runCapability = function () {
            var capid = document.getElementById('capidtext').value;
            alert(JSON.stringify(capid));
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
                                        alert(JSON.stringify(auth));
                                        $scope.strategy[obj.handler].init(passport, app, auth);
                                    }
                                    alert(JSON.stringify(obj.cid));
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