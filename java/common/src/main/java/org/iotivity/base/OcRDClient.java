/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.base;

import java.util.EnumSet;
import java.util.Iterator;
import java.util.List;

public final class OcRDClient {
    private static QualityOfService sPlatformQualityOfService = QualityOfService.NA;

    private OcRDClient() {
    }
    /**
     * API to publish resource to remote resource-directory.
     *
     * @param host                        Host Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param onPublishResourceListener   Handles events, success states and failure states.
     * @throws OcException if failure
     */
    public static void publishResourceToRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            OnPublishResourceListener onPublishResourceListener) throws OcException {

        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.publishResourceToRD0(
                host,
                connTypeInt,
                onPublishResourceListener,
                sPlatformQualityOfService.getValue()
        );
    }

    /**
     * API to publish resource to remote resource-directory.
     *
     * @param host                        Host Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param onPublishResourceListener   Handles events, success states and failure states.
     * @param qualityOfService            the quality of communication.
     * @throws OcException if failure
     */
    public static void publishResourceToRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            OnPublishResourceListener onPublishResourceListener,
            QualityOfService qualityOfService) throws OcException {

        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.publishResourceToRD0(
                host,
                connTypeInt,
                onPublishResourceListener,
                qualityOfService.getValue()
        );
    }

    private static native void publishResourceToRD0(
            String host,
            int connectivityType,
            OnPublishResourceListener onPublishResourceListener,
            int qualityOfService) throws OcException;

    /**
     * API to publish resource to remote resource-directory.
     *
     * @param host                        Host Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param ocResourceHandleList        reference to list of resource handles to be published.
     * @param onPublishResourceListener   Handles events, success states and failure states.
     * @throws OcException if failure
     */
    public static void publishResourceToRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            List<OcResourceHandle> ocResourceHandleList,
            OnPublishResourceListener onPublishResourceListener) throws OcException {

        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.publishResourceToRD1(
                host,
                connTypeInt,
                ocResourceHandleList.toArray(
                        new OcResourceHandle[ocResourceHandleList.size()]),
                onPublishResourceListener,
                sPlatformQualityOfService.getValue()
        );
    }

    /**
     * API to publish resource to remote resource-directory.
     *
     * @param host                        Host IP Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param ocResourceHandleList        reference to list of resource handles to be published.
     * @param onPublishResourceListener   Handles events, success states and failure states.
     * @param qualityOfService            the quality of communication
     * @throws OcException if failure
     */
    public static void publishResourceToRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            List<OcResourceHandle> ocResourceHandleList,
            OnPublishResourceListener onPublishResourceListener,
            QualityOfService qualityOfService) throws OcException {

        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.publishResourceToRD1(
            host,
            connTypeInt,
            ocResourceHandleList.toArray(
                    new OcResourceHandle[ocResourceHandleList.size()]),
            onPublishResourceListener,
            qualityOfService.getValue()
        );
    }

    private static native void publishResourceToRD1(
            String host,
            int connectivityType,
            OcResourceHandle[] ocResourceHandleArray,
            OnPublishResourceListener onPublishResourceListener,
            int qualityOfService) throws OcException;

    /**
     * API to delete resource from remote resource-directory.
     *
     * @param host                        Host Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param onDeleteResourceListener    Handles events, success states and failure states.
     * @throws OcException if failure
     */
    public static void deleteResourceFromRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            OnDeleteResourceListener onDeleteResourceListener) throws OcException {
        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.deleteResourceFromRD0(
                host,
                connTypeInt,
                onDeleteResourceListener,
                sPlatformQualityOfService.getValue()
        );
    }

    /**
     * API to delete resource from remote resource-directory.
     *
     * @param host                        Host Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param onDeleteResourceListener    Handles events, success states and failure states.
     * @param qualityOfService            the quality of communication.
     * @throws OcException if failure
     */
    public static void deleteResourceFromRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            OnDeleteResourceListener onDeleteResourceListener,
            QualityOfService qualityOfService) throws OcException {
        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.deleteResourceFromRD0(
                host,
                connTypeInt,
                onDeleteResourceListener,
                qualityOfService.getValue()
        );
    }

    private static native void deleteResourceFromRD0(
            String host,
            int connectivityType,
            OnDeleteResourceListener onDeleteResourceListener,
            int qualityOfService) throws OcException;

    /**
     * API to delete resource from remote resource-directory.
     *
     * @param host                        Host Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param ocResourceHandleList        reference to list of resource handles to be published.
     * @param onDeleteResourceListener    Handles events, success states and failure states.
     * @throws OcException if failure
     */
    public static void deleteResourceFromRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            List<OcResourceHandle> ocResourceHandleList,
            OnDeleteResourceListener onDeleteResourceListener) throws OcException {
        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.deleteResourceFromRD1(
                host,
                connTypeInt,
                ocResourceHandleList.toArray(
                        new OcResourceHandle[ocResourceHandleList.size()]),
                onDeleteResourceListener,
                sPlatformQualityOfService.getValue()
        );
    }

    /**
     * API to delete resource from remote resource-directory.
     *
     * @param host                        Host IP Address of a service to publish resource.
     * @param connectivityTypeSet         Set of types of connectivity. Example: IP
     * @param ocResourceHandleList        reference to list of resource handles to be published.
     * @param onDeleteResourceListener    Handles events, success states and failure states.
     * @param qualityOfService            the quality of communication
     * @throws OcException if failure
     */
    public static void deleteResourceFromRD(
            String host,
            EnumSet<OcConnectivityType> connectivityTypeSet,
            List<OcResourceHandle> ocResourceHandleList,
            OnDeleteResourceListener onDeleteResourceListener,
            QualityOfService qualityOfService) throws OcException {
        int connTypeInt = 0;

        for (OcConnectivityType connType : OcConnectivityType.values()) {
            if (connectivityTypeSet.contains(connType)) {
                connTypeInt |= connType.getValue();
            }
        }

        OcRDClient.deleteResourceFromRD1(
            host,
            connTypeInt,
            ocResourceHandleList.toArray(
                    new OcResourceHandle[ocResourceHandleList.size()]),
            onDeleteResourceListener,
            qualityOfService.getValue()
        );
    }

    private static native void deleteResourceFromRD1(
            String host,
            int connectivityType,
            OcResourceHandle[] ocResourceHandleArray,
            OnDeleteResourceListener onDeleteResourceListener,
            int qualityOfService) throws OcException;

    /**
     * An OnPublishResourceListener can be registered via the OcRDClient.publishResourceToRD call.
     * Event listeners are notified asynchronously
     */
    public interface OnPublishResourceListener {
        public void onPublishResourceCompleted(OcRepresentation ocRepresentation);
        public void onPublishResourceFailed(Throwable ex);
    }

    /**
     * An OnDeleteResourceListener can be registered via the OcRDClient.deleteResourceFromRD call.
     * Event listeners are notified asynchronously
     */
    public interface OnDeleteResourceListener {
        public void onDeleteResourceCompleted(int result);
    }
}
