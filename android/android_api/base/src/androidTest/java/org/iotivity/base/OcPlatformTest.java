/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import android.test.InstrumentationTestCase;
import android.util.Log;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

public class OcPlatformTest extends InstrumentationTestCase {

    private boolean success = false;
    private OcPlatformTestHelper helper;
    private static final String TAG = StringConstants.PLATFORM_TEST_TAG;

    @Override
    protected void setUp() {
        try {
            super.setUp();
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
        /**
         * OcPlatformTestHelper - all the helper functions (e.g. different versions of
         * registerResource() etc are defined here).
         * These functions are used at multiple places in OcPlatformTest
         */
        helper = new OcPlatformTestHelper();
        PlatformConfig cfg = new PlatformConfig(
                getInstrumentation().getContext(),
                ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER,
                "0.0.0.0",
                0,
                QualityOfService.LOW);

        OcPlatform.Configure(cfg);
    }

    /**
     * test register resource.
     *
     * @return ocResourceHandle
     */
    public void testRegisterResource_WithUriAndResourceType() {
        try {
            OcResourceHandle resourceHandle = helper.registerResource1(); // uri, resourceType both set
            OcPlatform.unregisterResource(resourceHandle);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public void testRegisterResource_WithUri_WithoutResourceType() {
        try {
            OcResourceHandle resourceHandle = helper.registerResource2(); // has uri, no resourceType
            OcPlatform.unregisterResource(resourceHandle);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public void testRegisterResource_WithoutUri_WithResourceType() {
        try {
            OcResourceHandle resourceHandle = helper.registerResource3(); // no uri, yes resourceType
            OcPlatform.unregisterResource(resourceHandle);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    /**
     * helper function to test getDeviceInfo()
     *
     * @param host hostName
     * @param uri  uri of the device
     * @param qos  Quality of Service
     */
    private void doGetDeviceInfo(String host, String uri, QualityOfService qos) throws InterruptedException {
        final CountDownLatch signal = new CountDownLatch(1);
        try {
            OcPlatform.getDeviceInfo(host, uri,
                    OcConnectivityType.WIFI,
                    new OcPlatform.OnDeviceFoundListener() {
                        @Override
                        public void onDeviceFound(OcRepresentation ocRepresentation) {
                            signal.countDown();
                        }
                    }, qos);

            assertTrue(signal.await(60, TimeUnit.SECONDS));
        } catch (OcException e) {
            if (!uri.equals(OcPlatform.WELL_KNOWN_QUERY + "/x/y/a/b")) {
                Log.e(TAG, e.getMessage());
                assertTrue(false);
            }
        }
    }

    /**
     * testDeviceInfo with different arguments
     */
    public void testDeviceInfoWithoutQos() throws InterruptedException {
        OcDeviceInfo deviceInfo = helper.createDeviceInfo();
        try {
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
        doGetDeviceInfo("", OcPlatform.WELL_KNOWN_QUERY + "/d", QualityOfService.NA); // no QoS
    }

    public void testDeviceInfoWithQos() throws InterruptedException {
        OcDeviceInfo deviceInfo = helper.createDeviceInfo();
        try {
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
        doGetDeviceInfo("", OcPlatform.WELL_KNOWN_QUERY + "/d", QualityOfService.HIGH); //high QoS
    }

    public void testDeviceInfoWithHost() throws InterruptedException {
        OcDeviceInfo deviceInfo = helper.createDeviceInfo();
        try {
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
        doGetDeviceInfo(StringConstants.PLATFORM_TEST_RES_URI1,
                OcPlatform.WELL_KNOWN_QUERY + "/d", QualityOfService.HIGH); // has host
    }

    public void testDeviceInfoWithWrongUri() throws InterruptedException {
        OcDeviceInfo deviceInfo = helper.createDeviceInfo();
        try {
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
        doGetDeviceInfo("", OcPlatform.WELL_KNOWN_QUERY + "/x/y/a/b", QualityOfService.HIGH); // random uri
    }

    /**
     * actual function to test findResource()
     *
     * @param host hostName
     * @param uri  uri of the resource
     */
    private void doFindResource(String host, String uri) throws InterruptedException {
        final CountDownLatch signal = new CountDownLatch(1);
        try {
            OcPlatform.findResource(host,
                    uri,
                    OcConnectivityType.WIFI,
                    new OcPlatform.OnResourceFoundListener() {
                        @Override
                        public void onResourceFound(OcResource resource) {
                            signal.countDown();
                        }
                    });
            assertTrue(signal.await(60, TimeUnit.SECONDS));
        } catch (OcException e) {
            if (("".equals(uri))) {
                return;
            }
            if (!uri.contains(StringConstants.PLATFORM_TEST_FIND_RES1)) {
                assertTrue(false);
            }
        }
    }

    /**
     * testFindResource() with different arguments
     */
    public void testFindResource() throws InterruptedException {
        try {
            OcResourceHandle resourceHandle1 = helper.registerResource1();
            doFindResource("", OcPlatform.WELL_KNOWN_QUERY + "?rt=" + StringConstants.PLATFORM_TEST_RES1);

            OcPlatform.unregisterResource(resourceHandle1);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
    }

    public void testFindResourceWrongUri() throws InterruptedException {

        try {
            OcResourceHandle resourceHandle1 = helper.registerResource1();

            final CountDownLatch signal = new CountDownLatch(1);
            try {
                OcPlatform.findResource("",
                        OcPlatform.WELL_KNOWN_QUERY + "?rt=" + StringConstants.PLATFORM_TEST_FIND_RES1,
                        OcConnectivityType.WIFI,
                        new OcPlatform.OnResourceFoundListener() {
                            @Override
                            public void onResourceFound(OcResource resource) {
                                signal.countDown();
                            }
                        });
                assertFalse(signal.await(5, TimeUnit.SECONDS));
            } catch (OcException e) {
                assertTrue(false);
            }
            OcPlatform.unregisterResource(resourceHandle1);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }

    }

    public void testFindResource_NoHost_NoUri() throws InterruptedException {
        OcResourceHandle resourceHandle1 = null;
        try {
            resourceHandle1 = helper.registerResource1();
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
//        boolean thrown = false;
//        try {
        doFindResource("", ""); // no host, no uri
//        } catch (OcException e) {
//
//            thrown = true;
//        }
//        assertTrue(thrown);
        try {
            OcPlatform.unregisterResource(resourceHandle1);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
    }

    /**
     * testPresence() - tests the presence/ availability of a resource
     */
    public void testPresence() throws InterruptedException {
        final CountDownLatch signal = new CountDownLatch(1);
        try {
            OcResourceHandle resourceHandle = helper.registerResource1();

            OcPresenceHandle presenceHandle = OcPlatform.subscribePresence(
                    OcPlatform.MULTICAST_IP,
                    OcConnectivityType.WIFI,
                    new OcPlatform.OnPresenceListener() {
                        @Override
                        public void onPresence(OcPresenceStatus ocPresenceStatus, int nonce,
                                               String hostAddress) {
                            Log.i(TAG, "onPresence status " + ocPresenceStatus.toString() +
                                    " nonce " + nonce);
                            signal.countDown();
                        }
                    }
            );
            OcPlatform.startPresence(OcPlatform.DEFAULT_PRESENCE_TTL);
            assertTrue(signal.await(60, TimeUnit.SECONDS));
            OcPlatform.unsubscribePresence(presenceHandle);
            OcPlatform.stopPresence();
            OcPlatform.unregisterResource(resourceHandle);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
    }

    /**
     * test bind and unbind of three resources
     */
    public void testBindAndUnbindThreeResources() {
        try {
            OcResourceHandle rh1 = helper.registerBindResource1();
            OcResourceHandle rh2 = helper.registerBindResource2();
            OcResourceHandle rh3 = helper.registerBindResource3();
            // bind resources
            OcPlatform.bindResource(rh1, rh2);
            OcPlatform.bindResource(rh1, rh3);
            OcPlatform.bindResource(rh2, rh3);
            // unbind resources
            OcPlatform.unbindResource(rh1, rh3);
            OcPlatform.unbindResource(rh1, rh2);
            OcPlatform.unbindResource(rh2, rh3);

            OcPlatform.unregisterResource(rh2);
            OcPlatform.unregisterResource(rh3);
            OcPlatform.unregisterResource(rh1);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
    }

    /**
     * bind same resource
     */
    public void testBindSameResource() {
        boolean thrown = false;
        OcResourceHandle rh1 = null;
        try {
            rh1 = helper.registerBindResource1();
            OcPlatform.bindResource(rh1, rh1);
        } catch (OcException e) {
            thrown = true;
        }
        assertTrue(thrown);
        try {
            OcPlatform.unregisterResource(rh1);
        } catch (OcException e) {
            assertTrue(false);
        }
    }

    /**
     * testResourceProxy() - client constructs the resourceProxy with the same arguments
     * with which the server will create the actual resource later on.
     */
    public void testResourceProxy() throws InterruptedException {
        final CountDownLatch signal = new CountDownLatch(1);
        try {
            String query = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + StringConstants.PLATFORM_TEST_RES1;
            OcResourceHandle serverRH = helper.registerResource1();

            OcPlatform.OnResourceFoundListener resourceFoundListener =
                    new OcPlatform.OnResourceFoundListener() {
                        @Override
                        public void onResourceFound(OcResource resource) {

                            try {
                                //client: construct resource proxy
                                OcResource resourceProxy = OcPlatform.constructResourceObject(
                                        resource.getHost(),
                                        resource.getUri(),
                                        OcConnectivityType.WIFI,
                                        resource.isObservable(),
                                        resource.getResourceTypes(),
                                        resource.getResourceInterfaces());

                                //client: register resource proxy
                                OcResourceHandle resourceProxyHandle =
                                        OcPlatform.registerResource(resourceProxy);

                                OcPlatform.unregisterResource(resourceProxyHandle);
                            } catch (OcException e) {
                                assertTrue(false);
                            }
                            signal.countDown();
                        }
                    };

            //client
            OcPlatform.findResource("", query, OcConnectivityType.WIFI, resourceFoundListener);
            //wait for onResourceFound event
            assertTrue(signal.await(60, TimeUnit.SECONDS));

            //server
            OcPlatform.unregisterResource(serverRH);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            assertTrue(false);
        }
    }

    @Override
    protected void tearDown() {
        try {
            super.tearDown();
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }
}

