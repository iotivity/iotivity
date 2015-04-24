package org.iotivity.base;

import android.util.Log;

import java.util.EnumSet;

/**
 * OcPlatformTestHelper - all the helper functions (e.g. different versions of
 * registerResource() etc are defined here).
 * These functions are used at multiple places in OcPlatformTest
 */
public class OcPlatformTestHelper {
    private static final String TAG = StringConstants.PLATFORM_TEST_HELPER_TAG;

    protected static OcResourceHandle registerResource3() throws OcException {
        return OcPlatform.registerResource(
                "",
                StringConstants.PLATFORM_TEST_RES3,
                StringConstants.PLATFORM_TEST_INTERFACE,
                new OcPlatform.EntityHandler() {
                    @Override
                    public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                        return EntityHandlerResult.OK;
                    }
                },
                EnumSet.of(ResourceProperty.DISCOVERABLE)
        );
    }

    protected OcResourceHandle registerResource2() throws OcException {
        return OcPlatform.registerResource(
                StringConstants.PLATFORM_TEST_RES_URI2,
                "",
                StringConstants.PLATFORM_TEST_INTERFACE,
                new OcPlatform.EntityHandler() {
                    @Override
                    public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                        return EntityHandlerResult.OK;
                    }
                },
                EnumSet.of(ResourceProperty.DISCOVERABLE)
        );

    }

    protected OcResourceHandle registerResource1() throws OcException {
        return OcPlatform.registerResource(
                StringConstants.PLATFORM_TEST_RES_URI1,
                StringConstants.PLATFORM_TEST_RES1,
                StringConstants.PLATFORM_TEST_INTERFACE,
                new OcPlatform.EntityHandler() {
                    @Override
                    public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                        return EntityHandlerResult.OK;
                    }
                },
                EnumSet.of(ResourceProperty.DISCOVERABLE)
        );

    }

    protected OcResourceHandle registerBindResource1() throws OcException {
        return OcPlatform.registerResource(
                StringConstants.PLATFORM_TEST_BIND_URI1,
                StringConstants.PLATFORM_TEST_BIND1,
                StringConstants.PLATFORM_TEST_INTERFACE,
                new OcPlatform.EntityHandler() {
                    @Override
                    public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                        return EntityHandlerResult.OK;
                    }
                },
                EnumSet.of(ResourceProperty.DISCOVERABLE)
        );

    }

    protected OcResourceHandle registerBindResource2() throws OcException {
        return OcPlatform.registerResource(
                StringConstants.PLATFORM_TEST_BIND_URI2,
                StringConstants.PLATFORM_TEST_BIND2,
                StringConstants.PLATFORM_TEST_INTERFACE,
                new OcPlatform.EntityHandler() {
                    @Override
                    public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                        return EntityHandlerResult.OK;
                    }
                },
                EnumSet.of(ResourceProperty.DISCOVERABLE)
        );

    }

    protected OcResourceHandle registerBindResource3() throws OcException {
        return OcPlatform.registerResource(
                StringConstants.PLATFORM_TEST_BIND_URI3,
                StringConstants.PLATFORM_TEST_BIND3,
                StringConstants.PLATFORM_TEST_INTERFACE,
                new OcPlatform.EntityHandler() {
                    @Override
                    public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest) {
                        return EntityHandlerResult.OK;
                    }
                },
                EnumSet.of(ResourceProperty.DISCOVERABLE)
        );

    }

    protected OcDeviceInfo createDeviceInfo() {
        OcDeviceInfo devInfo = new OcDeviceInfo();
        devInfo.setContentType("myContentType");
        devInfo.setDateOfManufacture("myDateOfManufacture");
        devInfo.setDeviceName("myDeviceName");
        devInfo.setDeviceUuid("myDeviceUUID");
        devInfo.setFirmwareVersion("myFirmwareVersion");
        devInfo.setHostName("myHostName");
        devInfo.setManufacturerName("myManufacturerNa");
        devInfo.setManufacturerUrl("myManufacturerUrl");
        devInfo.setModelNumber("myModelNumber");
        devInfo.setPlatformVersion("myPlatformVersion");
        devInfo.setSupportUrl("mySupportUrl");
        devInfo.setVersion("myVersion");
        return devInfo;
    }
}
