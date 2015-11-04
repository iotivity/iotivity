(function() {
    var restfactory = function($http) {
        return {
            auth : function(authbody) {
                console.log("restfactory - auth  " + authbody.sid );
                return $http.post('/wsi/cap/auth', authbody);
            },
            get : function(sid, cid, params) {
                return $http.get('/wsi/cap/get', sid, cid, params);
            },
            post : function(sid, cid, params, body) {
                return $http.post('/wsi/cap/post',sid, cid, params, body);
            }
        }
    }
    restfactory.$inject = ['$http'];
    angular.module('wsi').factory('restfactory',  restfactory); 
}());