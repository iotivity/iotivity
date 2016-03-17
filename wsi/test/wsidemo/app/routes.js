module.exports = function(app) {
    app.post('/callback', function(req, res) {
        app.settings.strategy['callback'].request(req, res);
    });
};
