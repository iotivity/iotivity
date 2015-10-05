angular.module('wsiService', [])

    // super simple service
    // each function returns a promise object
    .factory('Services', ['$http',function($http) {
        return {
            get : function() {
                return $http.get('/api/services');
            },
            create : function(serviceData) {
                return $http.post('/api/service', serviceData);
            },
            delete : function(id, name) {
                return $http.delete('/api/service/' + name);
            },
            sendRequest : function(reqData) { 
                return $http.get('/api/service/' + reqData.id, {params: reqData});
            }
        }
    }]);
