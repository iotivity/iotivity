module.exports = function(app) {
    app.post('/observenotify', function(req, res) {
        app.settings.strategy['wsidemo'].request(req, res);
    });
    app.post('/callback', function(req, res) {
        app.settings.strategy['callback'].request(req, res);
    });
};
