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

import java.security.InvalidParameterException;
import java.util.LinkedList;
import java.util.List;

public class OcRepresentationTest extends InstrumentationTestCase {

    private static final String TAG = "OcRepresentationTest";

    @Override
    protected void setUp() throws Exception {
        super.setUp();
    }

    public void testChildrenManagement(){
        OcRepresentation representation = new OcRepresentation();

        List<OcRepresentation> emptyList = representation.getChildren();
        assertTrue(emptyList.isEmpty());

        OcRepresentation child1 = new OcRepresentation();
        OcRepresentation child2 = new OcRepresentation();
        String key = "key";
        int value = 75;

        child1.setValueInt(key, value);
        child2.setValueInt(key, value);
        representation.addChild(child1);
        representation.addChild(child2);
        List<OcRepresentation> twoChildren = representation.getChildren();
        assertEquals(2, twoChildren.size());
        for(OcRepresentation rep : twoChildren){
            assertEquals(value, rep.getValueInt(key));
        }

        representation.clearChildren();
        emptyList = representation.getChildren();
        assertTrue(emptyList.isEmpty());
    }

    public void testUriGetSet(){
        OcRepresentation representation = new OcRepresentation();

        String emptyUri = representation.getUri();
        assertTrue(emptyUri.isEmpty());

        String expected = "a/resource/uri";
        representation.setUri(expected);
        String actual = representation.getUri();
        assertEquals(expected, actual);
    }

    public void testJSONRepresentation() {
        OcRepresentation representation = new OcRepresentation();
        String key = "key";
        int value = 75;

        String emptyJson1 = representation.getJSONRepresentation();
        representation.setValueInt(key, value);
        String intValue1 = representation.getJSONRepresentation();
        representation.remove(key);
        String emptyJson2 = representation.getJSONRepresentation();
        assertEquals(emptyJson1, emptyJson2);
        representation.setValueInt(key, value);
        String intValue2 = representation.getJSONRepresentation();
        assertEquals(intValue1, intValue2);
    }

    public void testResourceTypesGetSet() {
        OcRepresentation representation = new OcRepresentation();

        List<String> emptyResourceTypeList = representation.getResourceTypes();
        assertTrue(emptyResourceTypeList.isEmpty());

        representation.setResourceTypes(emptyResourceTypeList);
        emptyResourceTypeList = representation.getResourceTypes();
        assertTrue(emptyResourceTypeList.isEmpty());

        List<String> resourceTypeListExpected = new LinkedList<String>();
        resourceTypeListExpected.add("type1");
        resourceTypeListExpected.add("type2");
        resourceTypeListExpected.add("type3");
        
        representation.setResourceTypes(resourceTypeListExpected);
        List<String> resourceTypeListActual = representation.getResourceTypes();
        assertEquals(resourceTypeListExpected.size(), resourceTypeListActual.size());
        for(int i = 0; i < resourceTypeListExpected.size() ; i++){
            assertEquals(resourceTypeListExpected.get(i), resourceTypeListActual.get(i));
        }

        boolean thrown = false;
        try {
            representation.setResourceTypes(null);
        }catch(InvalidParameterException e){
            thrown = true;
        }
        assertTrue(thrown);
    }

    public void testResourceInterfacesGetSet() {
        OcRepresentation representation = new OcRepresentation();

        List<String> emptyResourceInterfaceList = representation.getResourceInterfaces();
        assertTrue(emptyResourceInterfaceList.isEmpty());

        representation.setResourceInterfaces(emptyResourceInterfaceList);
        emptyResourceInterfaceList = representation.getResourceInterfaces();
        assertTrue(emptyResourceInterfaceList.isEmpty());

        List<String> resourceInterfaceListExpected = new LinkedList<String>();
        resourceInterfaceListExpected.add("Interface1");
        resourceInterfaceListExpected.add("Interface2");
        resourceInterfaceListExpected.add("Interface3");

        representation.setResourceInterfaces(resourceInterfaceListExpected);
        List<String> resourceInterfaceListActual = representation.getResourceInterfaces();
        assertEquals(resourceInterfaceListExpected.size(), resourceInterfaceListActual.size());
        for(int i = 0; i < resourceInterfaceListExpected.size() ; i++){
            assertEquals(resourceInterfaceListExpected.get(i), resourceInterfaceListActual.get(i));
        }

        boolean thrown = false;
        try {
            representation.setResourceInterfaces(null);
        }catch(InvalidParameterException e){
            thrown = true;
        }
        assertTrue(thrown);
    }

    public void testAttributeManagement(){
        OcRepresentation representation = new OcRepresentation();

        assertTrue(representation.isEmpty());
        assertEquals(0, representation.size());

        String integerKey = "integerKey";
        int integerValue = 75;
        representation.setValueInt(integerKey, integerValue);
        assertFalse(representation.isEmpty());
        assertEquals(1, representation.size());

        int actualIntValue = representation.getValueInt(integerKey);
        assertEquals(integerValue, actualIntValue);

        String stringKey = "stringKey";
        String stringValue = "stringValue";
        representation.setValueString(stringKey, stringValue);
        assertEquals(2, representation.size());

        assertTrue(representation.hasAttribute(integerKey));
        representation.remove(integerKey);
        assertFalse(representation.hasAttribute(integerKey));
        assertEquals(1, representation.size());

        representation.setValueInt(integerKey, integerValue);
        assertFalse(representation.isNull(integerKey));
        representation.setNull(integerKey);
        assertTrue(representation.isNull(integerKey));

        String nonexistentKey = "nonexistentKey";
        assertFalse(representation.hasAttribute(nonexistentKey));
        representation.setNull(nonexistentKey);
        assertTrue(representation.isNull(nonexistentKey));

        //TODO test by type
    }
}