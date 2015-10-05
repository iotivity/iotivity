
var iotivity = require('../../iotivity/iotivity')

module.exports = function (capability, query, res, endpoint) {    
    var body;
    if(capability.operation == "GET") {    	
        body = iotivity.getInfo(endpoint);
    } else if(capability.operation == "POST") {
        body = iotivity.postInfo(query.resource_id, query.property, query.value);
    }

    if(query.hasOwnProperty('filter')) {
        jsonbody = JSON.parse(body);
        body = eval("jsonbody." + query.filter);
    }

    console.log(body);
    res.json(body);
};
