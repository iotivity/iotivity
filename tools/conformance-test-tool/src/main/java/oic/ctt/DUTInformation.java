//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt;

import java.util.ArrayList;
import java.util.HashMap;
import oic.ctt.formatter.IotivityKey;
import oic.ctt.DUTResource.CRUDNType;

/**
 *
 * This Class provides Resource Information according to various queries. Also
 * this Class Provides options to query using key
 *
 */
public class DUTInformation {

    private String                       mDi;
    private HashMap<String, DUTResource> mDUTResourcesMap;

    public DUTInformation(IDutLoader dutLoader) {
        setDi(dutLoader.loadDUTId());
        setDutResourcesMap(dutLoader.loadDutResources());
    }

    private boolean isExistResourceWithQuery(String queryBody,
            DUTResource dutResource) {
        String[] queries = queryBody.split("&");

        for (String query : queries) {
            if (query.length() <= 0 || (query.contains("=") == false)) {
                return false;
            }

            String[] queryPart = query.split("=");

            if (queryPart.length > 1) {
                String queryKey = queryPart[0];
                String queryValue = queryPart[1];
                String dutResourceValue = dutResource.getDUTResourceValue(
                        queryKey).get(0);

                if (dutResourceValue.equals(queryValue) == false) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * This method returns corresponding Resource list if it matches query
     *
     * @param query
     *            : Query to get the corresponding Resources
     * @return resourceList: Resource List that matches query
     */
    public ArrayList<DUTResource> getResourceWithQuery(String query) {

        ArrayList<DUTResource> resourceList = new ArrayList<DUTResource>();

        if (query.length() > 0)
            for (DUTResource tempResource : mDUTResourcesMap.values()) {

                if (isExistResourceWithQuery(query, tempResource)) {
                    resourceList.add(tempResource);
                }
            }
        return resourceList;
    }

    /**
     * This method returns corresponding Resource List if it matches the query
     * and key
     *
     * @param Key
     *            : key to get the corresponding Resources
     * @param query
     *            : Query to get the corresponding Resources
     * @return resourceList: Resource List that matches query and Key
     */
    public ArrayList<String> getResourceValue(String Key, String query) {

        ArrayList<String> resourceList = new ArrayList<String>();

        if (query.length() > 0)
            for (DUTResource tempResource : mDUTResourcesMap.values()) {

                if (isExistResourceWithQuery(query, tempResource)) {
                    for (String value : tempResource.getDUTResourceValue(Key)) {
                        resourceList.add(value);
                    }
                }
            }
        return resourceList;
    }

    /**
     * This Method returns all the Key set from the Resource map
     *
     * @return ArrayList<String> : An ArrayList of String that contains all the
     *         KeySet
     */
    public ArrayList<String> getResourceKeys() {
        return new ArrayList<String>(mDUTResourcesMap.keySet());
    }

    /**
     * This Method returns List of all the DUT Resources
     *
     * @return ArrayList<DUTResource> : An ArrayList of DUTResource that
     *         contains all Resources
     */
    public ArrayList<DUTResource> getResources() {
        return new ArrayList<DUTResource>(mDUTResourcesMap.values());
    }

    /**
     * This Method returns the DUT Information
     *
     * @return String: DUT Information
     */
    public String getDi() {
        return mDi;
    }

    /**
     * This Method sets the DUT Information
     *
     * @param di
     *            : DUT Information
     */
    public void setDi(String di) {
        this.mDi = di;
    }

    /**
     * This Method sets the entire Map of DUT Resources
     *
     * @param dutResourcesMap
     *            : Map of DUT Resources
     */
    public void setDutResourcesMap(HashMap<String, DUTResource> dutResourcesMap) {
        this.mDUTResourcesMap = dutResourcesMap;
    }

    /**
     * Gets the unique resource
     *
     * @param href : href of the resource
     * @return DUTResource : the resource identified with the href
     */
    public DUTResource getResource(String href){
        return mDUTResourcesMap.get(href);
    }

    /**
     * This method generate the CRUDN states supported by the resource
     *
     * @param : href of the resource
     */
    public void generateCRUDNState(String href) {
        DUTResource dutRes = mDUTResourcesMap.get(href);
        dutRes.generateCRUDNState();
    }

    /**
     * This method checks if a CRUDN operation is supported by the resource
     *
     * @param href : href of the resource
     * @param key : the CRUDN operation type to check
     * @return true : if the key is supported false otherwise
     */
    public boolean getCRUDNState(String href, CRUDNType key) {
        DUTResource dutRes = getResource(href);
        return dutRes.getCRUDNState(key);
    }

    /**
     * This method checks if a CRUDN operation is supported by the resource
     *
     * @param href : href of the resource
     * @return true : if the key is supported false otherwise
     */
    public ArrayList<String> getInterfaceList(String href) {
        DUTResource dutRes = getResource(href);
        return dutRes.getDUTResourceValue(IotivityKey.IF.toString());
    }
}
