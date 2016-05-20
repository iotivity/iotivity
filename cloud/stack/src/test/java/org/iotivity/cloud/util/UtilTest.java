package org.iotivity.cloud.util;

import java.net.InetSocketAddress;
import java.util.HashMap;

import org.iotivity.cloud.base.CoapClient;
import org.iotivity.cloud.base.CoapServer;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.junit.Test;

public class UtilTest {

    @Test
    public void testCbor() {
        HashMap<String, String> setpayloadData = new HashMap<String, String>();
        setpayloadData.put("test", "test");
    }

    @Test
    public void testCoapLogHandler() throws Exception {
        CoapServer server = new CoapServer();
        server.startServer(new InetSocketAddress(5683));
        server.addHandler(new CoapLogHandler());

        CoapClient client = new CoapClient();
        client.addHandler(new CoapLogHandler());
        client.startClient(new InetSocketAddress("127.0.0.1", 5683));

        CoapRequest request = new CoapRequest(CoapMethod.PUT);
        client.sendRequest(request);

        client.stopClient();
        server.stopServer();
    }

    @Test
    public void testJSONUtil() throws Exception {
        HashMap<Object, Object> setpayloadData = new HashMap<Object, Object>();
        setpayloadData.put("test", "test");

        String msg = JSONUtil.writeJSON(setpayloadData);
        JSONUtil.parseJSON(msg, "test");
        JSONUtil.parseJSON(msg);
    }

    @Test
    public void testLogger() throws Exception {
        Logger.v("VERBOSE test");
        Logger.d("DEBUG test");
        Logger.i("INFO test");
        Logger.e("ERROR test");
        Logger.w("WARNING test");
    }
}
