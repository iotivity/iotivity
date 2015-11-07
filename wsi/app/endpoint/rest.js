var mustache = require('mustache');
var request = require('request');

module.exports = {
    init: function () {
        //dummy
    },
    request: function (cap) {
        console.log(JSON.stringify(cap));
        var uri = mustache.render(cap.endpoint, cap.params);
        console.log("Final URL = " + uri + " " + cap.param);
        if (cap.operation == "GET") {
            //request(uri, handle_response);
        } else if (cap.operation == "POST") {
            console.log("Making a POST HTTP Request");
            request.post({
                url: uri,
            },function (error, response, body) {
                if (!error && response.statusCode == 200) {
                    console.log("Success = " + error + "Response = " + response + "Body = " + body);
                }else{
                    console.log("Error = " + error + "Response = " + response + "Body = " + body);
                }
            });
        }
    }
};