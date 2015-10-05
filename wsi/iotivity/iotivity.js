
var NativeExtension = require('./')

exports.getInfo = function(endpoint){
    var returnValue;
    NativeExtension.findResource(endpoint,function(result){
        console.log(result);
        returnValue = result;
    });
    return returnValue;
};

exports.postInfo = function(resourceId, property, value){
    var returnValue;
    NativeExtension.postResource(resourceId,property,value,function(result){
        console.log(result);
        returnValue = result;
    });
    return returnValue;
};
