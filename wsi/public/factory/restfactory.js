(function() {
    var restfactory = function($http) {
        return {
            auth : function(authbody) {
                console.log("restfactory - auth  " + authbody.sid );
                return $http.post('/wsi/cap/auth', authbody);
            },
            post : function(sid, cap) {
                console.log("restfactory - post " + cap.cid );
                return $http.post('/wsi/cap/' + sid, cap);
            }
        }
    }
    restfactory.$inject = ['$http'];
    angular.module('wsi').factory('restfactory',  restfactory); 
}());