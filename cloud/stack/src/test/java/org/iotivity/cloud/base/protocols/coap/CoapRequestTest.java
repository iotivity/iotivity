package org.iotivity.cloud.base.protocols.coap;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;

import java.util.List;

import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.junit.Test;

public class CoapRequestTest {

    @Test
    public void testCoapRequest() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertNotNull(request);
    }

    @Test
    public void testGetRequestMethod() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertEquals(request.getRequestMethod(), CoapMethod.GET);
    }

    @Test
    public void testSetUriPath() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath("sample");
    }

    @Test
    public void testGetUriPath() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertNull(request.getUriPath());
        request.setUriPath("sample");
        assertEquals(request.getUriPath(), "sample");
    }

    @Test
    public void testGetUriPathSegments() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertNull(request.getUriPathSegments());
        request.setUriPath("parent/child");
        List<String> list = request.getUriPathSegments();
        if (list != null) {
            assertEquals(list.size(), 2);
        }
    }

    @Test
    public void testSetUriQuery() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriQuery("sample=sample");
    }

    @Test
    public void testGetUriQuery() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertNull(request.getUriQuery());
        request.setUriQuery("sample=sample");
        assertEquals(request.getUriQuery(), "sample=sample");
    }

    @Test
    public void testGetUriQuerySegments() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertNull(request.getUriQuerySegments());
        request.setUriQuery("sample=samplle&sample2=sample2");
        List<String> list = request.getUriQuerySegments();
        if (list != null) {
            assertEquals(list.size(), 2);
        }
    }

    @Test
    public void testClearUriPath() {
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        assertNull(request.getUriPathSegments());
        request.setUriPath("sample");
        List<String> list = request.getUriPathSegments();
        if (list != null) {
            assertEquals(list.size(), 1);
        }
        request.clearUriPath();
        list = request.getUriPathSegments();
        if (list != null) {
            assertEquals(list.size(), 0);
        }
    }
}
