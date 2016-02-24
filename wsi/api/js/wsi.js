/*
 *  WSI SDK for Javascript Apps.
 *
 *  @author Samsung OSG (as2902.b@samsung.com)
 */


/**
 * expose our sdk
 */
(function(root) {
  root.WSISDK = root.WSISDK || {};
  root.WSISDK.VERSION = "1.0.0";
}(this));

/**
 * main sdk
 */
(function(root) {

  root.WSISDK = root.WSISDK || {};

  /**
   * Contains all WSISDK API classes and functions.
   * @name WSISDK
   * @namespace
   *
   * Contains all WSISDK API classes and functions.
   */
  var WSISDK = root.WSISDK;

  // If jQuery has been included, grab a reference to it.
  if (typeof(root.$) !== "undefined") {
    WSISDK.$ = root.$;
  }

  /**
   * Call this method first to set your authentication key.
   * @param {String} API Token
   */
  WSISDK.initialize = function(config) {
    console.log("WSI SDK Initialized.");
    WSISDK._initialize(config);
  };
  /**
   * This method is for WSISDK's own private use.
   * @param {String} API Token
   */
  WSISDK._initialize = function(config) {
    WSISDK.config = config;
  };

  /**
   * Get All Services
   * @param  {function} successCallback
   */
  WSISDK.getServices = function(successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/services";
    var xhr = new XMLHttpRequest();;
    xhr.open('GET', url, true);
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send();
  };

  /**
   * Get a service
   * @param  {function} serviceId
   * @param  {function} successCallback
   */
  WSISDK.getService = function(serviceId, successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/service/" + serviceId;
    var xhr = new XMLHttpRequest();;
    xhr.open('GET', url, true);
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send();
  };

  /**
   * Add a new service
   * @param  {function} body
   * @param  {function} successCallback
   */
  WSISDK.addService = function(body, successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/service";
    var xhr = new XMLHttpRequest();;
    xhr.open('POST', url, true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send(JSON.stringify(body));
  };

  /**
   * Delete a service
   * @param  {function} serviceId
   * @param  {function} successCallback
   */
  WSISDK.deleteService = function(serviceId, successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/service/" + serviceId;
    var xhr = new XMLHttpRequest();;
    xhr.open('DELETE', url, true);
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send();
  };

  /**
   * Update a service
   * @param  {function} body
   * @param  {function} successCallback
   */
  WSISDK.updateService = function(body, successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/service/" + body[0].sid;
    var xhr = new XMLHttpRequest();;
    xhr.open('PUT', url, true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send(JSON.stringify(body));
  };

  /**
   * Provide authentication and autorization information  to access a capability
   * @param  {function} body
   * @param  {function} successCallback
   */
  WSISDK.authCapability = function(body, successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/cap/auth";
    var xhr = new XMLHttpRequest();;
    xhr.open('POST', url, true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send(JSON.stringify(body));
  };

  /**
   * Access a service capability
   * @param  {function} serviceId
   * @param  {function} body
   * @param  {function} successCallback
   */
  WSISDK.execCapability = function(serviceId, body, successCallback) {
    var url = WSISDK.config.serverURL + "/wsi/cap/" + serviceId;
    var xhr = new XMLHttpRequest();;
    xhr.open('POST', url, true);
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    xhr.onload = function() {
      if (xhr.status >= 200 && xhr.status < 300) {
        successCallback(xhr.response);
      } else {
        console.log('Error !');
      }
    };
    xhr.send(JSON.stringify(body));
  };
}(this));
