var request = require('request');
module.exports = {
    init: function(app) {
        var template = {
                "sid":"org.genivi.rvi",
                "handler" : "rvicallback",
                "description":"The app to handle responses from WSI.",
                "platforms":[
		                {
		                    "name":"Linux",
		                }
                ],
                "capability":[
                    {
                        "cid":"org.genivi.",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Callback invocations from OCF-RVI.",
                        "endpoint":"http://localhost:8081/rvicallback",
                        "endpointtype":"REST",
                        "operation":"RVICALLBACK",
                        "params":{},
                    }
                ]
        };
        return template;
    },
    request: function (req, res) {
        console.log("Received RVI Callback : "+ JSON.stringify(req.body));
        res.sendStatus(200);
    }
}
console.log("App initialized");


