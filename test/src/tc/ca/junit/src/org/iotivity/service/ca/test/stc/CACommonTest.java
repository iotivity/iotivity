/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.service.ca.test.stc;

import android.content.Context;
import android.test.AndroidTestCase;
import org.iotivity.service.ca.test.helper.*;

import static org.iotivity.service.ca.test.helper.TestInfo.*;

public class CACommonTest extends AndroidTestCase {

	private static final int TRY_COUNT = 3;

	public CACommonTest() {
		MainActivity.RM.setContext(getContext(), MainActivity.getActivity());
	}

	protected void setUp() throws Exception {
		super.setUp();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-09-17
	 * @see none
	 * @objective Test 'CAInitialize' and 'CATerminate' in loop
	 * @target CAResult_t CAInitialize()
	 * @target void CATerminate()
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Call CAInitialize() API 2. Check it's return value 3. Call
	 *            sleep to halt the program 4. Call CATerminate() API 5. Repeat
	 *            steps 1-4 for multiple times
	 * @post_condition none
	 * @expected each time CAInitialize() will return CA_STATUS_OK &
	 *           CATerminate() will terminate successfully
	 */

	public void testEdrCAInitializeAndCATerminateLoop_P() {
		for (int count = 0; count < TRY_COUNT; count++) {
			if (!MainActivity.RM.initialize()) {
				fail();
				return;
			}

			MainActivity.RM.terminate();
		}
	}

	/**
	 * @since 2015-09-17
	 * @see none
	 * @objective Test multiple times 'CAInitialize' and multiple times
	 *            'CATerminate' calling effect
	 * @target CAResult_t CAInitialize()
	 * @target void CATerminate()
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Call CAInitialize() API 2. Check it's return value 3. Call
	 *            sleep to halt the program 4. Repeat steps 1-3 for multiple
	 *            times 5. Call CATerminate() API 6. Call sleep to halt the
	 *            program 7. Repeat steps 5-6 for multiple times
	 * @post_condition none
	 * @expected 1. Each time CAInitialize() will return CA_STATUS_OK 2. Each
	 *           time CATerminate() will terminate successfully
	 */
	public void testEdrCAInitializeAndCATerminateLoop_N() {
		for (int count = 0; count < TRY_COUNT; count++) {
			if (!MainActivity.RM.initialize()) {
				fail();
				return;
			}
		}

		for (int count = 0; count < TRY_COUNT; count++) {
			MainActivity.RM.terminate();
		}
	}

	/**
	 * @since 2016-03-03
	 * @see CAResult_t CAInitialize()
	 * @see void CATerminate()
	 * @objective Test CAGenerateToken() and CADestroyToken() with checking
	 *            token length [GSRV]
	 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @target void CADestroyToken(CAToken_t token)
	 * @test_data none
	 * @pre_condition Initialize CA
	 * @procedure 1. Call CAGenerateToken() API 2. Check if token is generated
	 *            and API returns CA_STATUS_OK 3. Call CADestroyToken() API
	 * @post_condition Terminate CA
	 * @expected 1. CAGenerateToken() will generate the token & will return
	 *           CA_STATUS_OK 2. CADestroyToken() will destroy the generated
	 *           token successfully
	 */
	public void testEdrCAGenerateTokenAndCADestroyToken_GSRV_P() {
		if (!MainActivity.RM.initialize()) {
			fail();
			return;
		}

		if (!MainActivity.RM.generateTokenAndDestroyToken()) {
			fail();
		}

		MainActivity.RM.terminate();
	}

	/**
	 * @since 2016-03-03
	 * @see CAResult_t CAInitialize()
	 * @see void CATerminate()
	 * @objective Test CAGenerateToken() and CADestroyToken() with checking
	 *            token length in a loop [VLCC]
	 * @target CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
	 * @target void CADestroyToken(CAToken_t token)
	 * @test_data none
	 * @pre_condition Initialize CA
	 * @procedure 1. Call CAGenerateToken() API 2. Check if token is generated
	 *            and API returns CA_STATUS_OK 3. Call CADestroyToken() API 4.
	 *            Repeat steps 1-3 for multiple times
	 * @post_condition Terminate CA
	 * @expected 1. Each time CAGenerateToken() will generate the token & will
	 *           return CA_STATUS_OK 2. Each time CADestroyToken() will destroy
	 *           the generated token successfully
	 */
	public void testEdrCAGenerateTokenAndCADestroyTokenLoop_VLCC_P() {
		if (!MainActivity.RM.initialize()) {
			fail();
			return;
		}

		for (int count = 0; count < TRY_COUNT; count++) {
			if (!MainActivity.RM.generateTokenAndDestroyToken()) {
				fail();
				break;
			}
		}

		MainActivity.RM.terminate();
	}
}
