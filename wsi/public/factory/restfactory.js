(function() {
    var restfactory = function($http) {
        return {
            auth : function(authbody) {
                console.log("restfactory - auth  " + authbody.sid );
                return $http.post('/wsi/cap/auth', authbody);
            },
            get : function(cap) {
                console.log("restfactory - GET " + cap.cid);
                return $http.post('/wsi/cap/get', cap);
            },
            post : function(cap) {
                console.log("restfactory - post " + cap.cid );
                return $http.post('/wsi/cap/post',cap);
            }
        }
    }
    restfactory.$inject = ['$http'];
    angular.module('wsi').factory('restfactory',  restfactory); 
}());