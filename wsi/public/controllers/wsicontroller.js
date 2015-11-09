(function () {
    var wsicontroller = function ($scope, dbfactory) {

        $scope.selectService = function (service) {
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