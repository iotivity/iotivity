(function() {
    var dbfactory = function($http) {
        return {
            create : function(serviceData) {
                return $http.post('/wsi/service', serviceData);
            },
            read : function(sid) {
                return $http.get('/wsi/service/' + sid);
            },
            update : function(sid, serviceData) {
                return $http.put('/wsi/service/' + sid, serviceData);
            },
            delete : function(sid) {
                return $http.delete('/wsi/service/' + sid);
            },
            get : function() {
                return $http.get('/wsi/services');
            },
        }
    }
    dbfactory.$inject = ['$http'];
    
    angular.module('wsi', [])
            .factory('dbfactory',  dbfactory); 

}());
