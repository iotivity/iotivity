/******************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/

var addon = require('./../build/Release/JSIoTivityRE.node');
var http = require('http');

var webServerDiscoveryCallback;
var webServerCachingCallback; 
var WebServerStateChangedCallback; 
var webServerSetRemoteAttrCallback; 
var webServerGetRemoteAttrCallback; 

var discoveryCallback = function(handle) {
    
    console.log("JS:: discoveryCallback called");
    console.log("JS:: Device Handle:: " + handle);
    
    webServerdiscoveryCallback(handle);
};

// Note: Upon receiving this callback, one needs to call getCachedAttribute to get updated values.
var cachingCallback = function(handle, attributeObjectMap) {

    console.log("JS:: cachingCallback called");
    console.log("JS:: handle:: " + handle);
    webServerCachingCallback(handle, attributeObjectMap);
};

// Monitoring Callback.
var stateChangedCallback = function(handle, resState) {

    console.log("JS:: stateChangedCallback called");
    console.log("JS:: handle:: " + handle);
    console.log("JS:: resState:: " + resState);
    WebServerStateChangedCallback(handle,resState);
};

var setRemoteAttrCallback = function(handle, attributeObjectMap) {

    console.log("JS:: setRemoteAttrCallback called");
    console.log("JS:: handle:: " + handle);
    webServerSetRemoteAttrCallback(handle, attributeObjectMap);
};

var getRemoteAttrCallback = function(handle, attributeObjectMap) {

    console.log("JS:: getRemoteAttributes called");
    console.log("JS:: handle:: " + handle);
    webServerGetRemoteAttrCallback(handle, attributeObjectMap);
};

exports.initPlatform = function initPlatform()
{
   console.log("JS:: Init Platform Entry");

   addon.InitPlatform();

   console.log("JS:: Init Platform exit");
}


exports.discoverResource = function discoverResource(uri, cb) {

    console.log("JS:: In discoverResource, uri =" + uri);
    webServerdiscoveryCallback = cb;
    console.log(addon.DiscoverResource(uri, discoveryCallback));
}

exports.discoverResourceByType = function discoverResourceType(uri, resourceType, cb) {

    console.log("JS:: In discoverResource, resourceType =" + resourceType);
    webServerdiscoveryCallback = cb;
    console.log(addon.DiscoverResourceByType(uri, resourceType, discoveryCallback));
}

exports.startCachingWithCb = function startCachingWithCb(handle, cb) {

    webServerCachingCallback = cb;
    console.log("JS:: In active startCaching, handle =" + handle);
    console.log(addon.StartCachingWithCb(handle, cachingCallback));
}

exports.startCachingWithoutCb = function startCachingWithoutCb(handle) {

    console.log("JS:: In passive startCaching, handle =" + handle);
    console.log(addon.StartCachingWithoutCb(handle));
}

exports.stopCaching = function stopCaching(handle) {

    console.log("JS:: In stopCaching, handel =" + handle);
    console.log(addon.StopCaching(handle));
}

exports.startMonitoring = function startMonitoring(handle, cb) {

    WebServerStateChangedCallback = cb;
    console.log("JS:: In startMonitoring, handle =" + handle);
    console.log(addon.StartMonitoring(handle, stateChangedCallback));
}

exports.stopMonitoring = function stopMonitoring(handle) {

    console.log("JS:: In stopMonitoring, handle =" + handle);
    console.log(addon.StopMonitoring(handle));
}

// Note: Returns Key Value Pairs of Cached Attributes. 
exports.getCachedAttributes = function getCachedAttribute(handle) {

    console.log("JS:: In getCachedAttribute, handle =" + handle);
    return addon.GetCachedAttribute(handle);
}

// Note: Returns Key Value Pairs of Remote Resource's Attributes.
exports.getRemoteAttributes = function getRemoteAttributes(handle, cb) {

    webServerGetRemoteAttrCallback= cb;
    console.log("JS:: In getRemoteAttributes, handle =" + handle);
    console.log(addon.GetRemoteAttributes(handle,getRemoteAttrCallback));    
}

// Note: setAttrCallback will be called, once attribute values are set at server side.
exports.setRemoteAttributes = function setRemoteAttributes(handle, attrMap, cb) {

    webServerSetRemoteAttrCallback= cb;
    console.log("JS:: In setRemoteAttributes, handle =" + handle);
    console.log(addon.SetRemoteAttributes(handle, attrMap, setRemoteAttrCallback));    
}

exports.getAddress = function getAddress(handle) {

    console.log("JS:: In getAddress, handle =" + handle);
    return addon.GetAddress(handle);
}

exports.getUri = function getUri(handle) {

    console.log("JS:: In getAddress, handle =" + handle);
    return addon.GetUri(handle);
}

exports.getResourceTypes = function getResourceTypes(handle) {

    console.log("JS:: In getResourceTypes, handle =" + handle);
    return addon.GetResourceTypes(handle);
}

exports.getInterfaces = function getInterfaces(handle) {
    console.log("JS:: In getInterfaces, handle =" + handle);
    return addon.GetInterfaces(handle);    
}
