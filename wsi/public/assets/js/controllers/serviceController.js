angular.module('wsiController', [])
    .controller('serviceController', ['$scope','$http','Services', function($scope, $http, Services) {
        $scope.webgetweather = function() {
            $scope.formData = {
                text: '{ "name":"OpenWeatherMap", "id":"org.openweathermap", "capability":[ { "id":"org.openweathermap.getweather.cityname", "isauthrequired":false, "endpoint":"api.openweathermap.org/data/2.5/weather?q={{p1}}", "endpointtype":"http", "operation":"GET" } ] } '
            };
            $scope.requestData = {
                text: '{"id":"org.openweathermap", "capabilityid":"org.openweathermap.getweather.cityname", "p1":"seattle", "filter": "weather" }' 
            };
        }

        $scope.webputtemp = function() {
            $scope.formData = {
                text: '{ "name":"PutTempExample", "id":"io.mocky", "capability":[ { "id":"io.mocky.puttemp.c", "isauthrequired":false, "endpoint":"www.mocky.io/v2/55ffa3483a1553780b41a602", "endpointtype":"http", "operation":"PUT" } ] } '
            };
            $scope.requestData = {
                text: '{"id":"io.mocky", "capabilityid":"io.mocky.puttemp.c", "p1":"24.5" }' 
            };
        }

        $scope.iotivitygettemp = function() {
            $scope.formData = {
                text: '{ "name":"IoTivityTempGET", "id":"oic.temperature.get", "capability":[ { "id":"oic.temperature.gettemp", "isauthrequired":false, "endpoint":"oic.r.temperature", "endpointtype":"iotivity", "operation":"GET" } ] }'
            };
            $scope.requestData = {
                text: '{"id":"oic.temperature.get", "capabilityid":"oic.temperature.gettemp"}'
            };
        }

        $scope.iotivityposttemp = function() {
            $scope.formData = {
                text: '{ "name":"IoTivityTempPOST", "id":"oic.temperature.post", "capability":[ { "id":"oic.temperature.posttemp", "isauthrequired":false, "endpoint":"oic.r.temperature", "endpointtype":"iotivity", "operation":"POST" } ] }' 
            };
            $scope.requestData = {
                text: '{"id":"oic.temperature.post", "capabilityid":"oic.temperature.posttemp", "resource_id":"return id", "property":"temperature", "value":"20" }'
            };
        }

        $scope.webgetweather();
        $scope.loading = true;
        Services.get()
            .success(function(data) {
                $scope.services = data;
                $scope.loading = false;
            });

        $scope.createService = function() {

            // validate the formData to make sure that something is there
            // if form is empty, nothing will happen
            if ($scope.formData.text != undefined) {
                $scope.loading = true;

                // call the create function from our service (returns a promise object)
                Services.create($scope.formData)
                    // if successful creation, call our get function to get all the new todos
                    .success(function(data) {
                        $scope.loading = false;
//                        $scope.formData = {}; // clear the form so our user is ready to enter another
                        $scope.services = data; // assign our new list of todos
                    });
            }
        };

        $scope.sendRequest = function() {
            if ($scope.requestData.text != undefined) {
                var param=$scope.$eval($scope.requestData.text);
                // call the create function from our service (returns a promise object)
                Services.sendRequest(param)
                    // if successful creation, call our get function to get all the new todos
                    .success(function(data) {
                        $scope.response = data;
                    });
            }
        };

        $scope.deleteService = function(id, name) {
            $scope.loading = true;

            Services.delete(id, name)
                // if successful creation, call our get function to get all the new todos
                .success(function(data) {
                    $scope.loading = false;
                    $scope.services = data; // assign our new list of services
                });
        };
    }]);
