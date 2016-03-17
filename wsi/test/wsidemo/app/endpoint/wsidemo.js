var request = require('request');
module.exports = {
    init: function(app) {
        var template = {
                "sid":"org.wsidemo.callback",
                "handler" : "callback",
                "description":"The app to handle responses from WSI.",
                "platforms":[
                    {
                        "name":"Linux",
                        "minPlatformVersion":"3.0",
                        "minApiVersion":"0.1",
                        "appURL":"http://wsidemoapp.com",
                        "minAppVersion":"1.0"
                    }
                ],
                "capability":[
                    {
                        "cid":"org.wsidemo.observeresult",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Gets the current status of a resource and its properties.",
                        "endpoint":"http://localhost:8081/callback",
                        "endpointtype":"REST",
                        "operation":"OBSERVERESULT",
                        "params":{},
                        "tags":[
                            "iotivity",
                            "observe"
                        ]
                    },
                    {
                        "cid":"org.genivi.rvi",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Callback invocations from OCF-RVI.",
                        "endpoint":"http://localhost:8081/callback",
                        "endpointtype":"REST",
                        "operation":"RVICALLBACK",
                        "params":{},
                    }
                ]
        };
        return template;
    },
    request: function (req, res) {
        console.log("Received Observe Result : "+ JSON.stringify(req.body));
        res.sendStatus(200);
        
        // Send notify to Controller.
        
        
    }
}
console.log("App initialized");


