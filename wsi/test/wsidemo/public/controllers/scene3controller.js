(function () {
    var scene3controller = function ($scope, $http) {
        $scope.scene3updates = [
            {
                title : "Success",
                content : "App Initialized"
            }
        ];
    };
    scene3controller.$inject = ['$scope', '$http'];

    angular.module('wsidemo').controller('scene3controller', scene3controller);
}());
