var request = require("request");
var os = require("os");
var wsiuri = "http://localhost:8080/wsi/cap/post/org.iotivity";
var localaddress;
var interfaces = os.networkInterfaces();
for (var k in interfaces) {
    for (var k2 in interfaces[k]) {
        var address = interfaces[k][k2];
        if (address.family === 'IPv6' && !address.internal) {
            localaddress = address.address;
        }
    }
}
console.log("Try to foundresource resource type");
request({
  uri: wsiuri,
  method: "POST",
  timeout: 10000,
  headers: {
    'Content-type': 'application/json'
  },
  body: '{"endpointtype":"iotivity","cid":"org.iotivity.findresource","resourceType":"core.light"}'
}, function(error, response, body) {
    responsebody = JSON.parse(body);
    console.log("Resource response : ");
    var address="";
    for(var index=0; index < responsebody.length; index++)
    {
       if(localaddress ===responsebody[index].address)
       {
            address = JSON.stringify(responsebody[index]);
       }
    }
    if(address!=="")
    {
        console.log(" Successed found test resources in local network : " + address);
        console.log("Try to get request");
        var getrequest = '{"endpointtype":"iotivity","cid":"org.iotivity.getresource","params":'
                + address + '}'
        request({
          uri: wsiuri,
          method: "POST",
          timeout: 10000,
          headers: {
            'Content-type': 'application/json'
          },
          body: getrequest
        }, function(error, response, body) {
            console.log("Get response : ");
            responsebody = JSON.parse(body);
            var keys = Object.keys(responsebody.values);
            var putrequest ="";
            var changecheck;
            for(var index=0; index < keys.length; index++)
            {
               if(keys[index] === "state" ){
                    if(responsebody.values.state === false)
                    {
                        putrequest = '{"endpointtype":"iotivity","cid":"org.iotivity.putresource","payload":{"'
                                   +keys[index]+'":true},"params":'+ address + '}'
                        changecheck = true;
                        break;
                    }
                    else
                    {
                        putrequest = '{"endpointtype":"iotivity","cid":"org.iotivity.putresource","payload":{"'
                                   +keys[index]+'":false},"params":'+ address + '}'
                        changecheck = false;
                        break;
                    }
                }
            }
            if(putrequest ===""){
                console.log("Failed PUT TEST\n");
            }
            else{
                console.log(" Successed get resource\n");
                console.log("Try to put request");
                request({
                    uri: wsiuri,
                    method: "POST",
                    timeout: 10000,
                    headers: {
                        'Content-type': 'application/json'
                    },
                    body: putrequest
                }, function(error, response, body) {
                    console.log("Put response : ");
                    responsebody = JSON.parse(body);
                    if(responsebody.values.state === changecheck)
                    {
                      console.log(" PUT TEST is successful\n");
                    }
                    else
                    {
                      console.log("Failed PUT TEST\n");
                    }
                });
            }
        });
    }
    else
    {
        console.log("Failed found test resources : " + address);
    }
});