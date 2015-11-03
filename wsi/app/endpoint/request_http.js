

module.exports = function (capability, res) {

    function handle_response(error, response, body) {
        if (!error && response.statusCode == 200) {
            if(query.hasOwnProperty('filter')) {
                 jsonbody = JSON.parse(body);
                 body = eval("jsonbody." + query.filter);
            }
            console.log(body);
            res.json(body);
        }
    }

    if(capability.operation == "GET") {
        request(capability.endpointtype + '://' + endpoint, handle_response);
    } else if(capability.operation == "PUT") {
        request({
            method: 'PUT',
            uri: capability.endpointtype + '://' + endpoint,
            multipart: [{
                'content-type':'application/json',
                body: JSON.stringify(query) 
                }]
            }, handle_response);
    }
};
