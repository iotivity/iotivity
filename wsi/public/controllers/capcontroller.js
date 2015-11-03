(function () {
    var capcontroller = function ($scope, dbfactory) {
        //Run a Capability
        $scope.runCapability = function () {
            var capid = document.getElementById('capidtext').value;
            dbfactory.read($scope.selService)
                    .success(function (data) {
                        var jsonObj = data[0];
                        for (var i = 0; i < jsonObj.capability.length; i++) {
                            var obj = jsonObj.capability[i];
                            if (obj.cid == capid) {
                                if (obj.endpointtype == "REST") {
                                    if (obj.isauthrequired == "true")
                                    {
                                        //alert(JSON.stringify(obj.cid));
                                    }
                                }
                                if (obj.endpointtype == "IOTIVITY") {

                                }
                            }
                        }
                    });
        };
    };
    capcontroller.$inject = ['$scope', 'dbfactory'];
    angular.module('wsi').controller('capcontroller', capcontroller);
}());