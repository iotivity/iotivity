(function () {
    var wsicontroller = function ($scope, dbfactory) {
        var sampleServices = [
            {
                "handler": "facebook",
                "sid": "com.facebook",
                "description": "Facebook RESTful API.",
                "platforms": [
                    {
                        "name": "linux",
                        "appURL": "https://facebook.com"
                    }
                ],
                "auth": [
                    {
                    "type": "oauth2.0",
                    "clientID": "949606648410465",
                    "clientSecret": "2a35037f1919e7cbefcc1d2f95ef24fb",
                    "endpoint": "/wsi/facebook/auth",
                    "callbackURL": "/wsi/facebook/auth/callback",
                    "userid": "",
                    "access_token": "",
                    "refresh_token": "",
                    "firstName": "",
                    "lastName": "",
                    "email": "",
                    "scope" : ['email', 'publish_actions']
                    },
                ],
                "capability": [
                    {
                        "cid": "com.facebook.auth",
                        "isauthrequired": "true",
                        "description": "Authorize this app to use facebook.",
                        "endpoint": "/wsi/facebook/auth",
                        "endpointtype": "REST",
                        "operation": "GET"
                    },                    
                    {
                        "cid": "com.facebook.post",
                        "isauthrequired": "true",
                        "description": "Post Message to Facebook.",
                        "endpoint": "https://graph.facebook.com/me/feed?message={{message}}&access_token={{access_token}}",
                        "endpointtype": "REST",
                        "operation": "POST",
                        "params": {
                            "userid" : "userid",
                            "message": "Message to post",
                            "access_token" : "access token"
                        },
                        "tags": [
                            "share",
                            "post"
                        ]
                    }
                ]
            },
            {
                "handler": "iotivity",
                "sid": "org.iotivity",
                "capability": [
                    {
                        "cid": "org.iotivity.getresource",
                        "isauthrequired": false,
                        "endpoint": "/wsi/iotivity/getresource",
                        "endpointtype": "IOTIVITY",
                        "operation": "GET",
                        "params": {
                            "sampleUri": "/a/light"
                        },
                        "tags": [
                            "get reosurce properties"
                        ]
                    }
                ]
            },            
            {
                "handler": "google",
                "sid": "com.google",
                "capability": [
                    {
                        "cid": "com.google.search",
                        "isauthrequired": "false",
                        "endpoint": "https://www.googleapis.com/customsearch/v1",
                        "endpointtype": "REST",
                        "operation": "GET",
                        "params": {
                            "key": "AIzaSyBGXrtIyGsrOOT9soXavjGwj9lxmJYOli4",
                            "cx" : "017576662512468239146:omuauf_lfve",
                            "q": "Topic"
                        },
                        "tags": [
                            "search",
                            "keyword"
                        ]
                    }
                ]
            },
            {
                "handler": "openweathermap",
                "id": "org.openweathermap",
                "capability": [
                    {
                        "id": "org.openweathermap.getbycity",
                        "isauthrequired": false,
                        "endpoint": "http://api.openweathermap.org/data/2.5/weather",
                        "endpointtype": "REST",
                        "operation": "GET",
                        "params": {
                            "q": "cityname",
                            "appid": "appid"                            
                        },
                        "tags": [
                            "weather by name"
                        ]
                    },
                    {
                        "id": "org.openweathermap.getbylocation",
                        "isauthrequired": false,
                        "endpoint": "http://api.openweathermap.org/data/2.5/weather",
                        "endpointtype": "REST",
                        "operation": "GET",
                        "params": {
                            "lat": "135",
                            "long": "135",
                            "appid": "appid"
                        },
                        "tags": [
                            "weather by name"
                        ]
                    },
                    {
                        "id": "org.openweathermap.getbycode",
                        "isauthrequired": false,
                        "endpoint": "http://api.openweathermap.org/data/2.5/weather",
                        "endpointtype": "REST",
                        "operation": "GET",
                        "params": {
                            "zip": "zip",
                            "cc": "country code",
                            "appid": "appid"
                        },
                        "tags": [
                            "weather by name"
                        ]
                    }
                ]
            }

        ];
        
        $scope.getSampleServices = function(){
            return sampleServices;
        }

        $scope.selectService = function (service) {
            //alert("select Service = " + service);
            //editor.set(sampleServices[index]);
            editor.set(service);
            document.getElementById("create").disabled = false;
            document.getElementById("save").disabled = true;
            preview.set({});
        };
        // create the editor
        var c = document.getElementById('jsoneditor');
        var p = document.getElementById('jsoneditorpreview');
        var options = {
            mode: "text"
        };
        var optionspreview = {
            mode: "tree"
        };

        var editor  = new JSONEditor(c, options);
        var preview = new JSONEditor(p, optionspreview);
        preview.expandAll();
        $scope.selService = null;
        $scope.strategy = [];

        dbfactory.get()
                .success(function (data) {
                    $scope.services = data;
                });

        //CRUDN for Services.
        $scope.createService = function () {
            if (editor.get() != undefined) {
                dbfactory.create(JSON.stringify(editor.get()))
                        .success(function (data) {
                            $scope.services = data;
                            preview.set(data);
                            preview.expandAll();
                        });
            }
        };
        $scope.readService = function (_id) {
            console.log("Read Service : " + _id);
            document.getElementById("create").disabled = true;
            document.getElementById("save").disabled = false;
            dbfactory.read(_id)
                    .success(function (data) {
                        editor.set(data);
                        preview.set(data);
                        preview.expandAll();
                        $scope.selService = _id;
                    });
        };

        $scope.updateService = function () {
            if (preview.get() != null) {
                dbfactory.update($scope.selService, preview.get())
                        .success(function (data) {
                            $scope.services = data;

                            //FIXME : View Not Updating
                            editor.set(data);
                            preview.set(data);
                            preview.expandAll();
                        });
            }
        };
        $scope.deleteService = function (id) {
            dbfactory.delete(id)
                    .success(function (data) {
                        $scope.services = data; // assign our new list of services
                        editor.set({});
                        preview.set({});
                    });
        };
    };

    wsicontroller.$inject = ['$scope', 'dbfactory'];

    angular.module('wsi').controller('wsicontroller', wsicontroller);
}());