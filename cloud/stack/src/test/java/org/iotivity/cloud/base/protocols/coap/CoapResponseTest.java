package org.iotivity.cloud.base.protocols.coap;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;
import org.junit.Test;

public class CoapResponseTest {
    @Test
    public void testCoapResponse() throws Exception {
        CoapResponse response = new CoapResponse(CoapStatus.VALID);
        assertNotNull(response);
    }

    @Test
    public void testGetResponseCode() throws Exception {
        CoapResponse response = new CoapResponse(CoapStatus.VALID);
        assertEquals(response.getResponseCode(), CoapStatus.VALID);
    }
}
