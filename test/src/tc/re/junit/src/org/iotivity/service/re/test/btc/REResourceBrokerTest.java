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

package org.iotivity.service.re.test.btc;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.RcsException;
import org.iotivity.service.re.test.helper.REHelper;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REResourceBrokerTest extends InstrumentationTestCase {
  private static final String LOG_TAG    = "RETest";
  private REHelper            m_REHelper;
  private StringBuilder       m_ErrorMsg = new StringBuilder();

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");

    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
    }

    m_ErrorMsg.setLength(0);
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    m_REHelper.distroyResources();

    Log.d(LOG_TAG, "tearDown called for REResourceBrokerTest");
  }

  /**
   * @since 2015-09-20
   * @see none
   * @objective Test 'startMonitoring' function with positive basic way
   * @target void startMonitoring(OnStateChangedListener listener)
   * @test_data OnStateChangedListener function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform startMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testStartMonitoring_SRC_P() {
    if (!m_REHelper.startMonitoring(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-23
   * @see none
   * @objective Test 'startMonitoring' function with null listener
   * @target void startMonitoring(OnStateChangedListener listener)
   * @test_data null listener
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform startMonitoring() API with null listener
   * @post_condition None
   * @expected The API should throws exception : listener is null
   */
  public void testStartMonitoring_ETC_N() {
    try {
      m_REHelper.mResourceObj.startMonitoring(null);
    } catch (NullPointerException e) {
      if (!(e.getLocalizedMessage().contains("listener is null"))) {
        fail("Doesn't got proper exception. Expected: listener is null. Actual: "
            + e.getLocalizedMessage());
      }
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2015-09-20
   * @see None
   * @objective Test 'stopMonitoring' function with positive basic way
   * @target void stopMonitoring()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform stopMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testStopMonitoring_SRC_P() {
    if (!m_REHelper.stopMonitoring(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-09-20
   * @see None
   * @objective Test 'getState' function with positive basic way
   * @target ResourceState getState()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getState() API
   * @post_condition None
   * @expected 1. No crash occurs 2. Should return correct state
   */
  public void testGetState_SRC_P() {
    if (!m_REHelper.getState(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-23
   * @see None
   * @objective Test 'isMonitoring' function with positive basic way
   * @target boolean isMonitoring()
   * @test_data none
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform isMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testIsMonitoring_SRC_P() {
    boolean isMonitoring = true;

    try {
      isMonitoring = m_REHelper.mResourceObj.isMonitoring();
    } catch (RcsException e) {
      fail("Should not throw exception when calling isMonitoring API.");
    }

    if (isMonitoring) {
      fail("Monitoring is true without calling startMonitoring API.");
    }
  }
}
