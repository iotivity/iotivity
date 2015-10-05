var mongoose = require('mongoose');

var wsSchema = new mongoose.Schema({
    id:             String,
    name:           String,
    desc:           String,
    platforms :     [],
    auth :          [],
    capability :    []
});

module.exports = mongoose.model('wsSchema', wsSchema);
