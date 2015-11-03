var NativeExtension = require('./build/Release/NativeExtension');

var i = 0;


setInterval(function() {
  console.log(i++);
}, 500);

NativeExtension.findResource('', '/oic/res?rt=core.light', 'CT_ADAPTER_IP', function(resourceid) {
  	console.log(resourceid);
	NativeExtension.get(resourceid, function(resource) {
    	console.log(resource);
    	NativeExtension.post(resourceid, "power", 20, function(resource__) {
    		console.log(resource__);
    		NativeExtension.put(resourceid, "power", 30, function(resource2) {
    			console.log(resource2);
    			NativeExtension.observe(resourceid, 0,function(resource3) {
	   				console.log(resource3);
	   				// NativeExtension.delete(resourceid, function(resource4) {
	    			// 	console.log(resource4);
	    				NativeExtension.cancelObserve(resourceid,function(resource5) {
	    					console.log(resource5);	
	    				});
					//});
   				});
    		});
    	});
  	});
});

// NativeExtension.getValue("coap://[fe80::92e6:baff:fec8:eee0]:41565/oic/res", function(resource) {
//   console.log(resource);
// });

// NativeExtension.postValue("coap://[fe80::92e6:baff:fec8:eee0]:41565/oic/res","power",20, function(resource) {
//   console.log(resource);
// });
