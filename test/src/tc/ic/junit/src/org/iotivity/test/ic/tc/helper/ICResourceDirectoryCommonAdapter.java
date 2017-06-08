/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.test.ic.tc.helper;

import java.util.List;

import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRDClient;
import org.iotivity.base.OcException;

public class ICResourceDirectoryCommonAdapter
        implements OcRDClient.OnPublishResourceListener,
        OcRDClient.OnDeleteResourceListener, OcResource.OnObserveListener {

    private static final String HREF = "href";
    public static boolean sIsOnPublishResourceCompleted = false;
    public static boolean sOnDeleteResourceCompleted    = false;
    public static boolean sIsPlatformObserveCompleted = false;

    public ICResourceDirectoryCommonAdapter() {

    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> arg0,
            OcRepresentation arg1, int arg2) {
        System.out.println("observeGroup was successful");
        sIsPlatformObserveCompleted = true;
        try {
            if (REGISTER == arg2) {
                System.out.println("REGISTER was successful");
            } else if (DEREGISTER ==arg2) {
                System.out.println("DEREGISTER was successful");
            } else {
                String gid = arg1.getValue("gid");
                System.out.println("\tGroupID: " + gid);

                String gmid = arg1.getValue("gmid");
                System.out.println("\tGroupMasterID: " + gmid);

                String[] midlist = arg1.getValue("midlist");
                if (midlist == null || midlist.length == 0) {
                    System.out.println("\tMember List is empty");
                } else {
                    System.out.println("\tMember List(" + midlist.length + ")");
                    for (String mid : midlist) {
                        System.out.println("\t : " + mid);
                    }
                }
                String[] dilist = arg1.getValue("dilist");
                if (dilist == null || dilist.length == 0) {
                    System.out.println("\tDevice List is empty");
                } else {
                    System.out.println("\tDevice List(" + dilist.length + ")");
                    for (String di : dilist) {
                        System.out.println("\t : " + di);
                    }
                }
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onObserveFailed(Throwable arg0) {
        System.out.println("observeGroup was failed");
    }

    @Override
    public void onDeleteResourceCompleted(int resultCode) {
        System.out
                .println("onDeleteResourceCompleted, result is " + resultCode);
        sOnDeleteResourceCompleted = true;
    }

    public void onDeleteResourceFailed(Throwable arg0) {
        System.out.println("onDeleteResource failed");
        sOnDeleteResourceCompleted = false;
    }

    @Override
    public void onPublishResourceCompleted(OcRepresentation ocRepresentation) {
        System.out.println("onPublishResourceCompleted");
        sIsOnPublishResourceCompleted = true;
        for (OcRepresentation child : ocRepresentation.getChildren()) {
            try {
                System.out.println(
                        "\tPublished Resource URI : " + child.getValue(HREF));
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onPublishResourceFailed(Throwable throwable) {
        System.out.println("onPublishResourceFailed has failed");
        sIsOnPublishResourceCompleted = false;
    }

}
