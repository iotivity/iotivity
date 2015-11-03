var mongoose = require('mongoose');

var wsSchema = new mongoose.Schema({
    sid:   {type: String, default: ''},
    handler: {type: String, default: ''},
    desc: {type: String, default: ''},
    platforms :     [],
    auth : [],
    capability :    []
});
module.exports = mongoose.model('wsSchema', wsSchema);