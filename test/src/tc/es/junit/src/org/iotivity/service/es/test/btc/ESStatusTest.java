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

package org.iotivity.service.es.test.btc;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;
import org.iotivity.service.easysetup.mediator.CloudPropProvisioningStatus;
import org.iotivity.service.easysetup.mediator.DevicePropProvisioningStatus;
import org.iotivity.service.easysetup.mediator.ESConstants;
import org.iotivity.service.easysetup.mediator.EnrolleeConf;
import org.iotivity.service.easysetup.mediator.EnrolleeStatus;
import org.iotivity.service.easysetup.mediator.GetConfigurationStatus;
import org.iotivity.service.easysetup.mediator.enums.ESCloudProvState;
import org.iotivity.service.easysetup.mediator.enums.ESErrorCode;
import org.iotivity.service.easysetup.mediator.enums.ESResult;
import org.iotivity.service.easysetup.mediator.enums.ProvStatus;
import org.iotivity.service.es.test.helper.ESEnrolleeConfHelper;

import android.test.AndroidTestCase;

public class ESStatusTest extends AndroidTestCase {

    protected void setUp() throws Exception {
        super.setUp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();

    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getESResult API positively
     * @target public int getESResult()
     * @test_data none
     * @pre_condition none
     * @procedure 1.call getESResult API 2.check result
     * @post_condition none
     * @expected return result value as set
     */
    public void testESGetESProvisioningResult_SRC_P() {
        assertEquals("Fail to get ES result", ESResult.ES_OK.getValue(),
                new DevicePropProvisioningStatus(ESResult.ES_OK.getValue())
                        .getESResult());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test getESResult API positively
     * @target public int getESResult()
     * @test_data none
     * @pre_condition none
     * @procedure 1.call getESResult API 2.check result
     * @post_condition none
     * @expected return result value as set
     */
    public void testESGetESConfigurationResult_SRC_P() throws OcException {
        EnrolleeConf enrolleeConf = new EnrolleeConf(
                ESEnrolleeConfHelper.cretaeOcRepresentation());
        assertEquals("Fail to get ES result", ESResult.ES_OK.getValue(),
                new GetConfigurationStatus(ESResult.ES_OK.getValue(),
                        enrolleeConf).getESResult());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test getESResult API positively
     * @target public int getESResult()
     * @test_data none
     * @pre_condition none
     * @procedure 1.call getESResult API 2.check result
     * @post_condition none
     * @expected return result value as set
     */
    public void testESGetEnrolleeConf_SRC_P() throws OcException {
        EnrolleeConf enrolleeConf = new EnrolleeConf(
                ESEnrolleeConfHelper.cretaeOcRepresentation());
        EnrolleeConf enrolleeConfig = new GetConfigurationStatus(
                ESResult.ES_OK.getValue(), enrolleeConf).getEnrolleeConf();
        assertEquals("Enrollee configuration should same", enrolleeConf,
                enrolleeConfig);
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getESCloudState API positively
     * @target public int getESCloudState()
     * @test_data none
     * @pre_condition none
     * @procedure 1.call getESCloudState API 2.check result
     * @post_condition none
     * @expected return result value as set
     */
    public void testESGetESCloudState_SRC_P() {
        CloudPropProvisioningStatus cloudPropProvisioningStatus = new CloudPropProvisioningStatus(
                ESResult.ES_OK.getValue(),
                ESCloudProvState.ES_CLOUD_ENROLLEE_FOUND.getValue());
        assertEquals("Enrollee configuration should same",
                ESCloudProvState.ES_CLOUD_ENROLLEE_FOUND.getValue(),
                cloudPropProvisioningStatus.getESCloudState());
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getESResult API positively
     * @target public int getESResult()
     * @test_data none
     * @pre_condition none
     * @procedure 1.call getESResult API 2.check result
     * @post_condition none
     * @expected return result value as set
     */
    public void testESGetCloudResult_SRC_P() {
        CloudPropProvisioningStatus cloudPropProvisioningStatus = new CloudPropProvisioningStatus(
                ESResult.ES_OK.getValue(),
                ESCloudProvState.ES_CLOUD_ENROLLEE_FOUND.getValue());
        assertEquals("Enrollee configuration should same",
                ESResult.ES_OK.getValue(),
                cloudPropProvisioningStatus.getESResult());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test getProvStatus API positively
     * @target public int getProvStatus()
     * @test_data none
     * @pre_condition create oc presentation
     * @procedure 1.call getProvStatus API 2.check prov state
     * @post_condition none
     * @expected return prov state as set
     */
    public void testESGetEnrolleeProvStatus_SRC_P() throws OcException {
        OcRepresentation ocRepresentation = new OcRepresentation();
        ocRepresentation.setValue(ESConstants.OC_RSRVD_ES_PROVSTATUS,
                ProvStatus.ES_STATE_CONNECTED_TO_ENROLLER.getValue());
        EnrolleeStatus enrolleeStatus = new EnrolleeStatus(ocRepresentation);
        assertEquals("Enrollee configuration should same",
                ProvStatus.ES_STATE_CONNECTED_TO_ENROLLER,
                enrolleeStatus.getProvStatus());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test getLastErrCode API positively
     * @target public ESErrorCode getLastErrCode()
     * @test_data none
     * @pre_condition create oc presentation
     * @procedure 1.call getLastErrCode API 2.check last error code
     * @post_condition none
     * @expected return error code as set
     */
    public void testESGetLastErrCode_SRCC_P() throws OcException {
        OcRepresentation ocRepresentation = new OcRepresentation();
        ocRepresentation.setValue(ESConstants.OC_RSRVD_ES_LAST_ERRORCODE,
                ESErrorCode.ES_ERRCODE_SSID_NOT_FOUND.getValue());
        EnrolleeStatus enrolleeStatus = new EnrolleeStatus(ocRepresentation);
        assertEquals("Enrollee configuration should same",
                ESErrorCode.ES_ERRCODE_SSID_NOT_FOUND,
                enrolleeStatus.getLastErrCode());
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getProvStatus API negatively
     * @target public ProvStatus getProvStatus()
     * @test_data none
     * @pre_condition create empty oc representation
     * @procedure 1.call getProvStatus API 2.check prov status
     * @post_condition none
     * @expected return prov init state
     */
    public void testESGetEnrolleeProvStatus_USV_SRC_N() {
        OcRepresentation ocRepresentation = new OcRepresentation();
        EnrolleeStatus enrolleeStatus = new EnrolleeStatus(ocRepresentation);
        assertEquals("Should return init prov state", ProvStatus.ES_STATE_INIT,
                enrolleeStatus.getProvStatus());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test getLastErrCode API negatively
     * @target public ESErrorCode getLastErrCode()
     * @test_data none
     * @pre_condition create empty oc representation
     * @procedure 1.call getLastErrCode API 2.check error code
     * @post_condition none
     * @expected return errror code
     */
    public void testESGetProvStatus_SRCC_N() {
        OcRepresentation ocRepresentation = new OcRepresentation();
        EnrolleeStatus enrolleeStatus = new EnrolleeStatus(ocRepresentation);
        assertEquals("ES_ERRCODE_SSID_NOT_FOUND error code should return",
                ESErrorCode.ES_ERRCODE_SSID_NOT_FOUND,
                enrolleeStatus.getLastErrCode());
    }
}