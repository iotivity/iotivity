var request = require('request');
module.exports = {
    init: function(app) {
        var template = {
                "sid":"org.wsidemo",
                "handler" : "wsidemoapp",
                "description":"The app to handle responses from WSI.",
                "platforms":[
                    {
                        "name":"Linux",
                        "minPlatformVersion":"3.0",
                        "minApiVersion":"0.1",
                        "appURL":"http://openweathermap.org",
                        "minAppVersion":"1.0"
                    }
                ],
                "capability":[
                    {
                        "cid":"org.wsidemo.observeresult",
                        "platform" : "linux",
                        "isauthrequired":"false",
                        "description":"Gets the current status of a resource and its properties.",
                        "endpoint":"http://localhost:8081/observenotify",
                        "endpointtype":"REST",
                        "operation":"OBSERVERESULT",
                        "params":{},
                        "tags":[
                            "iotivity",
                            "observe"
                        ]
                    }
                ]
        };
        return template;
    },
    request: function (req, res) {
        console.log("Received Observe Result : "+ JSON.stringify(req.body));
        res.sendStatus(200);
        
        //post a Tweet
         var text = new Date().toUTCString() + "\n";
         text += JSON.stringify(req.body);
         
        
        var resbody = {
            "cid": "com.twitter.post",
            "isauthrequired": "true",
            "description": "Post Message to Twiiter.",
            "endpoint": "statuses/update",
            "endpointtype": "twitter",
            "operation": "POSTTWEET",
            "params": {
                "text": text,
                "screen_name" : "oicdemo"
            },
            "tags": [
                "share",
                "post"
            ]
        };
        
        var options = {
            url: "http://localhost:8080/wsi/cap/post/com.twitter",
            json : true,
            method: 'POST',
            body: resbody
        };
        console.log("JSON Body Sent = " + JSON.stringify(resbody));

        request(options, function (error, response, body) {
            console.log(error + "  - " + body);
            if(error)
                console.log("Success");
            else
                console.log("Failure");
                //$scope.scene4updates.push({title: 'Success', content: body});
                //$scope.scene4updates.push({title: 'Failure', content: body});

        });
        
        
    }
}
console.log("Twitter initialized");


