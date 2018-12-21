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
var service = new Service('com.example.service.iotivity.server');

var subscriptions = {};

var server = require('./server-lowlevel');

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
    To Start iotivity server with iotivity-node

    @b Syntax \n
    luna://com.example.service.iotivity.server/startServer

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type    | Description
        ----------------------------------------------------------
        subscribe   |Required       |boolean |
    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
        "subscribed" : boolean,
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful or not.
    @param              subscribed
    Detailed information for removed resources.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
        subscribed        |Required       |boolean| value of requested resource
    @remarks
    The \e returnValue will be true as long as the syntax is correct, even if the process ID is not a valid one.

    @b Examples:
    @code
    luna-send -i -f luna://com.example.service.iotivity.server/startServer '{"subscribe":true}'&
    @endcode

    Example response for a succesful call:
    @code
    {
        "resources": [
        ],
        "subscribed": true,
        "returnValue": true
    }
    {
        "resources": [
            {
                "interfaces": [
                    "oic.if.baseline"
                ],
                "types": [
                    "core.fan"
                ],
                "uri": "a/fan"
            }
        ],
        "subscribed": true,
        "returnValue": true
    }
    @endcode
*/
var serverDiscoverable = service.register("startServer");
serverDiscoverable.on("request", function(message) {
    if (message.isSubscription) {
        if (isEmpty(subscriptions)){
            subscriptions[message.uniqueToken] = message;
            server.startServer(subscriptionCallback);
        } else {
            subscriptions[message.uniqueToken] = message;
        }
    } else {
        message.respond({
            returnValue : false,
            subscribed : false
        });
    }
});
serverDiscoverable.on("cancel", function(message) {
    delete subscriptions[message.uniqueToken];
    if (isEmpty(subscriptions))
        server.stopServer();
    message.respond({returnValue : true});
});

/*
    To create resource

    @b Syntax \n
    luna://com.example.service.iotivity.server/createResource

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type   | Description
        ----------------------------------------------------------
        uri         |Required       |string |
        types       |Required       |string |
        question    |Required       |string |
        answer      |Required       |string |
        observable  |Required       |boolean|
    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful or not.
    @param              subscribed
    Detailed information for removed resources.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
    @remarks
    The \e returnValue will be true as long as the syntax is correct, even if the process ID is not a valid one.

    @b Examples:
    @code
    luna-send -1 -f luna://com.example.service.iotivity.server/createResource '{"uri":"a/fan","question":"abc","answer":"123","observable":true,"types":"core.fan"}'
    @endcode

    Example response for a succesful call:
    @code
    {
        "returnValue": true
    }
    @endcode
*/
service.register("createResource", function(message){
    var uri = message.payload.uri;
    var types = message.payload.types;
    var interfaces = message.payload.interfaces;
    var observable = message.payload.observable;
    var q = message.payload.question;
    var a = message.payload.answer;

    if( !uri || !types || !q || !a ){
        message.respond({
            errorText : "invalid destination formats",
            returnValue : false,
            subscribed : false
        });
    }
    else{
        server.createResource(validateUri(uri), types, q, a, observable, subscriptionCallback);
        message.respond({ returnValue : true });
    }
});

/*
    To delete resource with specific URI

    @b Syntax \n
    luna://com.example.service.iotivity.server/deleteResource

    @code
    {
    }
    @endcode
    @param
        Parameter   |Requirement    |Type   | Description
        ----------------------------------------------------------
        uri         |Required       |string |

    @return
    <b> For a call: </b>
    @code
    {
        "returnValue"   : boolean,
    }

    @endcode
    @param              returnValue
    Indicates if the call was succesful or not.
    @param              subscribed
    Detailed information for removed resources.

        Parameter         |Requirement    |Type   | Description
        ----------------------------------------------------------
        returnValue       |Required       |boolean| true means success, false means faild
    @remarks
    The \e returnValue will be true as long as the syntax is correct, even if the process ID is not a valid one.

    @b Examples:
    @code
    luna-send -1 -f luna://com.example.service.iotivity.server/deleteResource '{"uri":"/a/fan"}'
    @endcode

    Example response for a succesful call:
    @code
    {
        "returnValue": true
    }
    @endcode
*/
service.register("deleteResource", function(message){
    var uri = message.payload.uri;
    if(!uri){
        message.respond({
            errorText : "invalid uri",
            returnValue : false,
            subscribed : false
        });
        return;
    }
    else{
        server.deleteResource(validateUri(uri), subscriptionCallback);
        message.respond({ returnValue : true });
    }
});