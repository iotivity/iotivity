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
                    "endpoint": "/wsi/auth/facebook",
                    "callbackURL": "/wsi/auth/facebook/callback",
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
                        "cid": "com.facebook.post",
                        "isauthrequired": "true",
                        "description": "Post Message to Facebook.",
                        "endpoint": "http://graph.facebook.com/{userid}/feed/message={msg}&access_token={access_token}",
                        "endpointtype": "REST",
                        "operation": "POST",
                        "params": {
                            "msg": "Message to post"
                        },
                        "tags": [
                            "share",
                            "post"
                        ]
                    }
                ]
            },
            {
                "handler": "naver",
                "id": "com.naver",
                "capability": [
                    {
                        "id": "com.naver.search",
                        "isauthrequired": false,
                        "endpoint": "",
                        "endpointtype": "REST",
                        "operation": "GET",
                        "params": {
                            "msg": "Text to search"
                        },
                        "tags": [
                            "search",
                            "keyword"
                        ]
                    }
                ]
            },
            {
                "handler": "twitter",
                "id": "com.twitter",
                "auth": [
                    {"type": "oauth2.0"},
                    {"appId": "949606648410465"},
                    {"appSecret": "2a35037f1919e7cbefcc1d2f95ef24fb"},
                    {"endpoint": "/auth/twitter"},
                    {"callbackUrl": "/auth/twitter/callback"},
                    {"access_token": ""},
                    {"userid": ""},
                    {"access_token": ""},
                    {"refresh_token": ""},
                    {"firstName": ""},
                    {"lastName": ""},
                    {"email": ""}
                ],
                "capability": [
                    {
                        "id": "com.twitter.timeline",
                        "isauthrequired": false,
                        "endpoint": "",
                        "endpointtype": "REST",
                        "operation": "GET",
                        "params": {
                            "twitterhandle": "twitter handle"
                        },
                        "tags": [
                            "tweets",
                            "handle"
                        ]
                    }
                ]
            },
            {
                "handler": "iotivity",
                "id": "org.iotivity",
                "capability": [
                    {
                        "id": "org.iotivity.findresource",
                        "isauthrequired": false,
                        "endpoint": "",
                        "endpointtype": "IOTIVITY",
                        "operation": "GET",
                        "params": {
                            "rt": "core.light"
                        },
                        "tags": [
                            "find reosurces",
                            "discover devices"
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