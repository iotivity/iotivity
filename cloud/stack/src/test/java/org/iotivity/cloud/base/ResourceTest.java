package org.iotivity.cloud.base;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.junit.Test;

import io.netty.channel.ChannelHandlerContext;

public class ResourceTest {

    private static class SampleResource extends Resource {

        public SampleResource() {
            setUri("sampleUri");
        }

        @Override
        public void onRequestReceived(ChannelHandlerContext ctx,
                CoapRequest request) {
            // TODO Auto-generated method stub
        }
    }

    @Test
    public void testGetSetUri() {
        SampleResource sampleResource = new SampleResource();
        assertEquals(sampleResource.getUri(), "sampleUri");
        sampleResource.setUri("sampleUri2");
        assertEquals(sampleResource.getUri(), "sampleUri2");
    }

    @Test
    public void testGetSetType() {
        SampleResource sampleResource = new SampleResource();
        assertNull(sampleResource.getType());
        sampleResource.setType("sampleType");
        assertEquals(sampleResource.getType(), "sampleType");
    }

    @Test
    public void testGetSetRif() {
        SampleResource sampleResource = new SampleResource();
        assertNull(sampleResource.getRif());
        sampleResource.setRif("sampleRif");
        assertEquals(sampleResource.getRif(), "sampleRif");
    }

    @Test
    public void testOnRequestReceived() {

    }
}
