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
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.client.RcsRemoteResourceObject.ResourceState;
import org.iotivity.service.re.test.helper.REAPIHelper;
import static org.iotivity.service.re.test.helper.ResourceUtil.*;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REResourceBrokerTest extends InstrumentationTestCase {
  private static final String            LOG_TAG    = "RETest";
  private REAPIHelper                    m_REHelper;
  private static RcsRemoteResourceObject m_Resource;
  private StringBuilder                  m_ErrorMsg = new StringBuilder();
  int m_count = 10;

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REAPIHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");

    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      assertTrue(
          "Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString(),
          false);
    } else {
      m_Resource = m_REHelper.getFoundResourceList().get(0);
      Log.i(LOG_TAG, m_ErrorMsg.toString());
    }

    m_ErrorMsg.setLength(0);
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    m_REHelper.distroyResources();

    if (m_Resource != null) {
      m_Resource.destroy();
      m_Resource = null;
    }

    Log.i(LOG_TAG, "tearDown called for REResourceBrokerTest");
  }

  /**
   * @since 2015-11-03
   * @see none
   * @objective Test 'startMonitoring' function with positive basic way
   * @target void startMonitoring(OnStateChangedListener listener)
   * @test_data Callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startMonitoring() API 2. Check callback
   * @post_condition None
   * @expected No crash occurs
   */
  public void testStartMonitoring_CV_P() {
    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      if (REAPIHelper.g_IsStateChanged == false) {
        fail("Callback is not received.");
      }
    } catch (RcsException e) {
      fail("Unable to start monitoring. " + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-03-01
   * @see void stopMonitoring()
   * @see boolean isMonitoring()
   * @objective Test 'startMonitoring' function with Validation Loop Condition
   *            Check
   * @target void startMonitoring(OnStateChangedListener listener)
   * @test_data Callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startMonitoring() API 2. Wait for 5 seconds 3. Again
   *            perform startMonitoring() API
   * @post_condition None
   * @expected Should throw exception with message "Monitoring already started."
   */
  public void testStartMonitoring_VLCC_N() {
    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
    } catch (RcsException e) {
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      fail("Successfully called startMonitoring API twice.");
    } catch (RcsException e) {
      if (!e.getLocalizedMessage().contains("Monitoring already started")) {
        fail("Can't get proper exception. Expected: Monitoring already started, Actual: "
            + e.getLocalizedMessage());
      }
    }
  }

  /**
   * @since 2015-09-20
   * @see void startMonitoring()
   * @objective Test 'stopMonitoring' function with Sequential Validation
   * @target void stopMonitoring()
   * @test_data Callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startMonitoring() API 2. Perform stopMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   */

  public void testStopMonitoring_SQV_P() {
    if (!m_REHelper.startMonitoring(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }

    if (!m_REHelper.stopMonitoring(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-11-03
   * @see None
   * @objective Test 'startMonitoring' function with Sequential Validation Check
   * @target void startMonitoring(OnStateChangedListener listener)
   * @target void stopMonitoring()
   * @test_data callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startMonitoring() API with valid callback 2. Perform
   *            stopMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testStartMonitoring_SCV_P() {
    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);
      m_Resource.stopMonitoring();
    } catch (RcsException e) {
      fail("Exception occurred inside testStartMonitoring_SCV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-03-01
   * @see None
   * @objective Test 'startMonitoring' function with Sequential Validation Check
   * @target void startMonitoring(OnStateChangedListener listener)
   * @target boolean isMonitoring()
   * @target void stopMonitoring()
   * @test_data callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startMonitoring() API with valid callback 2. Check
   *            Monitoring status with isMonitoring() API 3. Perform
   *            stopMonitoring() API 4. Check Monitoring status with
   *            isMonitoring() API again.
   * @post_condition None
   * @expected 1. Should return valid status of monitoring 2. No crash occurs
   **/
  public void testStartMonitoring_SQV_P() {
    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);
      boolean isMonitoring = m_Resource.isMonitoring();
      assertEquals("isMonitoring should true after startMonitoring.", true,
          isMonitoring);
      m_Resource.stopMonitoring();
      isMonitoring = m_Resource.isMonitoring();
      assertEquals("isMonitoring should false after stopMonitoring.", false,
          isMonitoring);
    } catch (RcsException e) {
      fail("Exception occurred inside testStartMonitoring_SQV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-03
   * @see void startMonitoring(ResourceStateChangedCallback callback)
   * @objective Test 'stopMonitoring' function with terminate loop check
   * @target void stopMonitoring()
   * @test_data callback function for receiving changed state
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startMonitoring() API 3. Perform stopMonitoring()
   *                API
   * @procedure Perform stopMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testStopMonitoringTwice_DSCC_ITLC_P() {
    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);
      m_Resource.stopMonitoring();
      m_REHelper.waitInSecond(CALLBACK_WAIT_ONE);
      m_Resource.stopMonitoring();
    } catch (RcsException e) {
      fail("Exception occurred inside StopMonitoringTwice_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-03
   * @see void startMonitoring(ResourceStateChangedCallback callback)
   * @objective Test 'stopMonitoring' function with Initial/Terminate Loop Check
   * @target void stopMonitoring()
   * @test_data callback function for receiving changed state
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startMonitoring() API 3. Perform stopMonitoring()
   *                API
   * @procedure Perform stopMonitoring() API 10 times
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testStopMonitoring_ITLC_P() {
    try {
      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      m_Resource.stopMonitoring();
    } catch (RcsException e) {
      assertTrue(
          "Throws exception when calling startMonitoring() and stopMonitoring() API",
          false);
    }

    for (int i = 0; i < m_count; i++) {
      try {
        m_Resource.stopMonitoring();
      } catch (RcsException e) {
        assertTrue(
            "Fail to stop monitoring when calling stopMonitoring() in a loop.",
            false);
      }
    }
  }

  /**
   * @since 2015-12-31
   * @see None
   * @objective Test 'stopMonitoring' function without performing
   *            startMonitoring()
   * @target void stopMonitoring()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform stopMonitoring() API
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testStopMonitoring_DSCC_N() {
    try {
      m_Resource.stopMonitoring();
    } catch (RcsException e) {
      fail("Exception occurred inside testStopMonitoring_DSCC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-12-31
   * @see void startMonitoring(ResourceStateChangedCallback callback)
   * @see void stopMonitoring()
   * @objective Test 'getState' function with Sequential validation
   * @target ResourceState getState()
   * @test_data callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform getState() API. 2. Check the ResourceState. 3.
   *            Perform startMonitoring() API. 4. Perform getState() API. 5.
   *            Check the ResourceState.
   * @post_condition None
   * @expected 1. No exception occurs 2. getState should return NONE before
   *           startMonitoring 3. getState should return REQUESTED after
   *           startMonitoring
   **/
  public void testGetState_SQV_P() {
    try {
      ResourceState state = m_Resource.getState();
      if (state != ResourceState.NONE) {
        assertTrue("Resource should return NONE, but actual is " + state, false);
      }

      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      state = m_Resource.getState();

      if (state != ResourceState.REQUESTED) {
        assertTrue("Resource should return REQUESTED, but actual is " + state,
            false);
      }
    } catch (RcsException e) {
      fail("Exception occurs at testGetState_STCC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-12-31
   * @see None
   * @objective Test 'getState' function with State Check Validation
   * @target ResourceState getState()
   * @test_data callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform getState() API. 2. Check the ResourceState. 3.
   *            Perform startMonitoring() API. 4. Check the ResourceState. 5.
   *            Wait for 5 seconds. 6. Perform getState() API. 7. Check the
   *            ResourceState.
   * @post_condition None
   * @expected 1. No exception occurs 2. getState should return ALIVE after
   *           startMonitoring
   **/
  public void testGetState_SCV_P() {
    try {
      ResourceState state = m_Resource.getState();
      if (state != ResourceState.NONE) {
        assertTrue("Resource should return NONE, but actual is " + state, false);
      }

      m_Resource.startMonitoring(m_REHelper.mOnStateChangedListener);
      state = m_Resource.getState();

      if (state != ResourceState.REQUESTED) {
        assertTrue("Resource should return REQUESTED, but actual is " + state,
            false);
      }

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      state = m_Resource.getState();

      if (state != ResourceState.ALIVE) {
        assertTrue("Resource should return ALIVE, but actual is " + state,
            false);
      }
    } catch (RcsException e) {
      fail("Exception occurs at testGetState_SCV_P: " + e.getLocalizedMessage());
    }
  }
}
