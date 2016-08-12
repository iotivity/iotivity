//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.service.re.test.stc;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.RcsException;
import org.iotivity.service.client.RcsAddress;
import org.iotivity.service.client.RcsDiscoveryManager;
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.re.test.helper.REAPIHelper;
import static org.iotivity.service.re.test.helper.ResourceUtil.*;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REResourceWrapperTest extends InstrumentationTestCase {
  private static final String     LOG_TAG    = "RETest";
  private StringBuilder           m_ErrorMsg = new StringBuilder();
  private REAPIHelper             m_REHelper;
  private RcsRemoteResourceObject m_Resource;

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REAPIHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");
  }

  protected void tearDown() throws Exception {
    super.tearDown();
  }

  /**
   * @since 2015-11-05
   * @see None
   * @objective Test 'getUri' function with positive basic way
   * @target string getUri()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getUri() API with TEMP_SENSOR_URI and LIGHT_URI
   * @post_condition None
   * @expected Returned uri should not be empty
   **/
  public void testGetUri_SQV_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());

      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    String uri = "";
    if (m_Resource != null) {
      try {
        uri = m_Resource.getUri();
      } catch (RcsException e) {
        fail("Throws exception when getUri() API called: "
            + e.getLocalizedMessage());
        return;
      }

      if (uri.compareTo(TEMP_SENSOR_URI) != 0 && uri.compareTo(LIGHT_URI) != 0
          && uri.compareTo(FAN_URI) != 0) {
        fail("Expected uri: " + TEMP_SENSOR_URI + " Actual: " + uri);
      }
    }
  }

  /**
   * @since 2015-11-05
   * @see None
   * @objective Test 'getInterfaces' function with BASELINE_INTERFACE
   * @target String[] getInterfaces()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getInterfaces() API
   * @post_condition None
   * @expected 1. Returned interface list should not be empty 2. At least one
   *           interface should not be an empty string
   **/
  public void testGetInterfaces_SQV_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());

      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    try {
      String[] interfaces = m_Resource.getInterfaces();

      if (interfaces.length > 0) {
        boolean foundInterface = false;
        int i = 0;

        for (i = 0; i < interfaces.length; i++) {
          if (interfaces[i].compareTo(INTERFACE) == 0
              || interfaces[i].compareTo(BASELINE_INTERFACE) == 0) {
            foundInterface = true;
            break;
          }
        }

        if (foundInterface == false) {
          fail("Could not found expected interface: " + INTERFACE + " or "
              + BASELINE_INTERFACE);
        }
      } else {
        fail("Unable to get interfaces");
      }
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'getTypes' function with LIGHT_TYPE, FAN_TYPE,
   *            TEMP_SENSOR_TYPE
   * @target std::vector< std::string > getTypes()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getTypes() API
   * @post_condition None
   * @expected 1. Returned type list should not be empty 2. At least one type
   *           should not be an empty string
   **/
  public void testGetTypes_SQV_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    try {
      String[] types = m_Resource.getTypes();

      if (types.length > 0) {
        boolean foundType = false;
        int i = 0;

        for (i = 0; i < types.length; i++) {
          if (types[i].compareTo(LIGHT_TYPE) == 0
              || types[i].compareTo(FAN_TYPE) == 0
              || types[i].compareTo(TEMP_SENSOR_TYPE) == 0) {
            foundType = true;
            break;
          }
        }

        if (foundType == false) {
          fail("Could not found expected type: " + LIGHT_TYPE + " or "
              + FAN_TYPE + " or " + TEMP_SENSOR_TYPE);
        }
      } else {
        fail("Unable to get types");
      }
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    }
  }
}
