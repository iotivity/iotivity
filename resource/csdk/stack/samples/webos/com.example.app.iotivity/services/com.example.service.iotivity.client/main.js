/* ******************************************************************
 *
 * Copyright (c) 2018 LG Electronics, Inc.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
var Service = require('webos-service');
var service = new Service('com.example.service.iotivity.client');

var subscriptions = {};

var client = require('./client-lowlevel');

function isEmpty(obj) {
    for ( var key in obj) {
        if (obj.hasOwnProperty(key))
            return false;
    }
    return true;
};

function subscriptionCallback(response) {
    for (var i in subscriptions){
        if (subscriptions.hasOwnProperty(i)) {
            var s = subscriptions[i];
            s.respond({
                response : response,
                returnValue : true,
                subscribed : true
            });
        }
    }
};

function validateUri(_uri){
    if(_uri.charAt(0)!="/")
        return "/"+_uri;
    else return _uri;
}

/*
    Discovering available resources from all devices.

    @b Syntax \n
    luna://com.example.service.iotivity.client/discoverResources

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type   | Description
        ----------------------------------------------------------
        none
    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
        "discoveryResponse" : object,
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful.
    @param              discoveryResponse
    List of discoverd resources.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
        discoveryResponse |Required       |object | list of all resources from all devices

    @b Examples:
    @code
    luna-send -n 5 -f luna://com.example.service.iotivity.client/discoverResources '{}'
    @endcode

    Example response for a succesful call:
    @code
    {
        "discoveryResponse": {
            ...
        },
        "returnValue": true
    }
    @endcode
*/
service.register("discoverResources", function(message) {
    client.startDiscovery(function(response) {
        message.respond({
            discoveryResponse : response,
            returnValue : true
        });
    });
});

/*
    Fetch the value of resource from specified URI

    @b Syntax \n
    luna://com.example.service.iotivity.client/getResource

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type   | Description
        ----------------------------------------------------------
        uri         |Required       |string |
        destination |Required       |object | target device address include adapter, flags, ifindex, port and addr
        question    |Required       |string | payload with specifiied question
    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
        "response" : object,
        "errorText" : string
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful or not.
    @param              response
    Detailed information.
    @param              errorText
    Describes the error if call was not succesful.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
        response          |Optional       |object | value of requested resource
        errorText         |Optional       |object | value of error text

    @b Examples:
    @code
    luna-send -n 1 -f luna://com.example.service.iotivity.client/getResource '{"uri":"/a/fan", "question":"abc","destination": {"adapter": 1,"flags": 32,"ifindex": 2,"port": 54406,"addr": "fe80::ba27:ebff:fe04:f661%eth0"}}'
    @endcode

    Example response for a succesful call:
    @code
    {
        "returnValue": true,
        "response": {
            "devAddr": {
                "adapter": 1,
                "flags": 82,
                "ifindex": 0,
                "port": 34711,
                "addr": "10.177.242.177"
            },
            "connType": 65618,
            "addr": {
                "adapter": 1,
                "flags": 82,
                "ifindex": 0,
                "port": 34711,
                "addr": "10.177.242.177"
            },
            "sequenceNumber": 16777216,
            "result": 0,
            "identity": [
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49,
                49
            ],
            "payload": {
                "type": 4,
                "values": {
                    "answer": "123"
                }
            },
            "resourceUri": "/a/fan"
        }
    }
    @endcode
*/
service.register("getResource", function(message) {
    var uri = message.payload.uri;
    var question = message.payload.question;
    var destination = message.payload.destination;

    if(!uri){
        message.respond({
            errorText : "invalid uri",
            returnValue : false
        });
        return;
    }
    else if(!destination
        || !destination.adapter
        || !destination.flags
        || !destination.port
        || !destination.addr ){
        message.respond({
            errorText : "invalid destination formats",
            returnValue : false
        });
        return;
    }
    else if(!question){
        message.respond({
            errorText : "invalid question",
            returnValue : false
        });
        return;
    }

    client.getResource(validateUri(uri), destination, question, function(response) {
        message.respond({
            response : response,
            returnValue : true
        });
    });
});

/*
    Delete resource from specified URI

    @b Syntax \n
    luna://com.example.service.iotivity.client/deleteResource

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type   | Description
        ----------------------------------------------------------
        uri         |Required       |string | uri which is removed
        destination |Required       |object | target device address include adapter, flags, ifindex, port and addr
    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
        "response" : object,
        "errorText" : string
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful.
    @param              response
    Detailed information for removed resources.
    @param              errorText
    Describes the error if call was not succesful.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
        response          |Optional       |object | value of requested resource
        errorText         |Optional       |string | value of error text
    @remarks
    The \e returnValue will be true as long as the syntax is correct, even if the process ID is not a valid one.

    @b Examples:
    @code
    luna-send -n 1 -f luna://com.example.service.iotivity.client/deleteResource '{"uri":"/a/fan","destination":{"adapter":1,"flags":32,"ifindex":0,"port":35128,"addr":"fe80::ba27:ebff:fe04:f661%eth0"}}'
    @endcode

    Example response for a succesful call:
    @code
    {
        "returnValue": true,
        "response": {
            "sequenceNumber": 16777216,
            "identity": [
            ],
            "devAddr": {
                "adapter": 1,
                "flags": 32,
                "ifindex": 2,
                "port": 35128,
                "addr": "fe80::ba27:ebff:fe04:f661%eth0"
            },
            "connType": 65568,
            "result": 2,
            "addr": {
                "adapter": 1,
                "flags": 32,
                "ifindex": 2,
                "port": 35128,
                "addr": "fe80::ba27:ebff:fe04:f661%eth0"
            },
            "resourceUri": "/a/fan"
        }
    }
    @endcode
*/
service.register("deleteResource", function(message) {
    var uri = message.payload.uri;
    var destination = message.payload.destination;
    if(!uri){
        message.respond({
            errorText : "invalid uri",
            returnValue : false
        });
        return;
    }
    else if(!destination
        || !destination.adapter
        || !destination.flags
        || !destination.port
        || !destination.addr ){
        message.respond({
            errorText : "invalid destination formats",
            returnValue : false
        });
        return;
    }
    client.deleteResource(validateUri(uri), destination, function(response) {
        message.respond({
            response : response,
            returnValue : true
        });
    });
});

/*
    Observe the value of resource from specified URI

    @b Syntax \n
    luna://com.example.service.iotivity.client/observeResource

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type   | Description
        ----------------------------------------------------------
        subscribe   |Required       |boolean |
        uri         |Required       |string |
        destination |Required       |object | target device address include adapter, flags, ifindex, port and addr
    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
        "response" : object,
        "errorText" : string
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful.
    @param              response
    Detailed information for removed resources.
    @param              errorText
    Describes the error if call was not succesful.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
        response          |Required       |object | value of requested resource
        errorText         |Optional       |string | value of error text
    @remarks
    The \e returnValue will be true as long as the syntax is correct, even if the process ID is not a valid one.

    @b Examples:
    @code
    luna-send -n 1 -f luna://com.example.service.iotivity.client/observeResource '{"uri":"/a/fan","destination":{"adapter":1,"flags":32,"ifindex":0,"port":58941,"addr":"fe80::ba27:ebff:fe04:f661%eth0"},"subscribe":true}'
    @endcode

    Example response for a succesful call:
    @code
    {
        "subscribed": true,
        "returnValue": true,
        "response": {
            "devAddr": {
                "adapter": 1,
                "flags": 32,
                "ifindex": 2,
                "port": 58941,
                "addr": "fe80::ba27:ebff:fe04:f661%eth0"
            },
            "connType": 65568,
            "addr": {
                "adapter": 1,
                "flags": 32,
                "ifindex": 2,
                "port": 58941,
                "addr": "fe80::ba27:ebff:fe04:f661%eth0"
            },
            "sequenceNumber": 23,
            "result": 0,
            "identity": [
            ],
            "payload": {
                "type": 4,
                "values": {
                    "someOtherValue": "Espoo",
                    "someValue": 18
                }
            },
            "resourceUri": "/a/fan"
        }
    }
    @endcode
*/
var clientObserve = service.register("observeResource");
clientObserve.on("request", function(message) {
    var uri = message.payload.uri;
    var destination = message.payload.destination;
    if(!uri){
        message.respond({
            errorText : "invalid uri",
            returnValue : false,
            subscribed : false
        });
        return;
    }
    else if(!destination
        || !destination.adapter
        || !destination.flags
        || !destination.port
        || !destination.addr ){
        message.respond({
            errorText : "invalid destination formats",
            returnValue : false,
            subscribed : false
        });
        return;
    }
    if (message.isSubscription) {
        subscriptions[message.uniqueToken] = message;
        client.observeResource(validateUri(uri), destination, subscriptionCallback);
    } else {
        message.respond({
            returnValue : false,
            subscribed : false,
        });
    }
});
clientObserve.on("cancel", function(message) {
    delete subscriptions[message.uniqueToken];
    var uri = message.payload.uri;
    client.cancelObservation(uri);
    message.respond({
        returnValue : true,
        subscribed : false
    });
});