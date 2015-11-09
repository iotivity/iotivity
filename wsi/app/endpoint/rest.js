var mustache = require('mustache');
var request = require('request');

module.exports = {
    init: function () {
        //dummy
        return {};
    },
    request: function (cap, res) {
        console.log("Trying to perform rest request : "+ JSON.stringify(cap));
        if (cap.operation == "GET") {
            var ep = cap.endpoint;
            if(ep.startsWith('http')){ //local URI
                var uri = mustache.render(cap.endpoint, cap.params);
                console.log("Final URL = " + uri + " " + cap.params);
                request.get(uri, function (error, response, body) {
                    if (!error && response.statusCode == 200) {
                        console.log("Success = " + error + "Response = " + response + "Body = " + body);
                        res.send(response.statusCode);
                    }else{
                        console.log("Error = " + error + "Response = " + response + "Body = " + body);
                        res.send(response.statusCode);
                    }
                });
            }else{ //local URL
                res.redirect(302, cap.endpoint);
            }
        } else if (cap.operation == "POST") {
            console.log("Making a POST HTTP Request");
            var uri = mustache.render(cap.endpoint, cap.params);
            console.log("Final URL = " + uri + " " + cap.params);
            
            request.post({
                url: uri,
            },function (error, response, body) {
                if (!error && response.statusCode == 200) {
                    console.log("Success = " + error + "Response = " + response + "Body = " + body);
                    res.send(response.statusCode);
                }else{
                    console.log("Error = " + error + "Response = " + response + "Body = " + body);
                    res.send(response.statusCode);
                }
            });
        }
    }
};