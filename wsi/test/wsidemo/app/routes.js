module.exports = function(app) {
    app.post('/observenotify', function(req, res) {
        app.settings.strategy['wsidemo'].request(req, res);
    });
    app.post('/rvicallback', function(req, res) {
        app.settings.strategy['rvicallback'].request(req, res);
    });
};
