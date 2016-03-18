package src.test.java.oic.ctt.network.control;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;
import javafx.geometry.Side;
import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import oic.ctt.logger.CTLogger;
import org.slf4j.Logger;

import oic.ctt.network.OICHelper;
import oic.ctt.network.control.ControlClient;
import oic.ctt.network.control.ControlMessage;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.provision.ProvisioningClient;
import oic.ctt.testhelper.ConformanceTestHelper;
import oic.ctt.testhelper.ReportGenerator;

//import build.classes.test.oic.ctt.testhelper.ConformanceTestHelper;



public class ControlClientTest extends ConformanceTestHelper{

	@Rule
    public ReportGenerator reportGenerator = new ReportGenerator();

    private ControlClient controlClient;
    int relayPort = 12345;
    int messageID =12345;
    String clientDeviceId = "adminDeviceUUID0";
    String serverDeviceId = "justworksDevUUID";
    ControlMessage controlMsg;
    private Logger                    mlogger        = CTLogger.getInstance();

    
    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
    	controlClient = new ControlClient();
    }

    @After
    public void tearDown() throws Exception {
    }
    
    /**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-01
	 * @reviewer
	 * @since 2015-01-01
	 * @see none
	 * @objective Test 'startDtlsWithPvData' positively
	 * @target public ControlMessage initDtlsWithRndPin
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call startDtlsWithPvData(String relayIp, int relayPort,
            String dutIp, int dutPortSecured, int cipherSuite, String clientId,
            String serverId, String password)
	 * @post_condition none
	 * @expected startDtlsWithPvData() will be called without exception
	 */
    
    @Test
	public void startDtlsWithPvData_P() {
    	ProvisioningClient pclient = new ProvisioningClient();
		String relayIp = pclient.getCurrentSystemIpAddress(false);
		String dutIp = relayIp;
		ControlMessage response = null;
    	try {
    		response = controlClient.startDtlsWithPvData(relayIp, relayPort,
    				dutIp, pclient.getSecuredPort(), CipherSuite.TLS_PSK_WITH_AES_128_CCM_8.getValue(), clientDeviceId,
    				serverDeviceId, OICHelper.hexStringToByteArray("6fa5e39969bc8eb4d76a412f7872267c"));
    		
    		//mlogger.info("response =" + response);
    		//assertNotNull(response);
        } catch (Exception ex) {
        	ex.printStackTrace();
            fail("Fail"+ ex.getLocalizedMessage());
        }  	
	}
    
    /**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-01
	 * @reviewer
	 * @since 2015-01-01
	 * @see none
	 * @objective Test 'initDtlsWithRndPin' positively
	 * @target public ControlMessage initDtlsWithRndPin
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call initDtlsWithRndPin(String relayIp, int relayPort,
            String dutIp, int dutPortSecured, int cipherSuite, String clientId,
            String serverId, String password)
	 * @post_condition none
	 * @expected initDtlsWithRndPin() will be called without exception
	 */
    
    
    @Test
	public void initDtlsWithRndPin_P() {
    	ProvisioningClient pclient = new ProvisioningClient();
		String relayIp = pclient.getCurrentSystemIpAddress(false);
		String dutIp = relayIp;
		ControlMessage response = null;
    	try {    		
    		response = controlClient.initDtlsWithRndPin(relayIp, relayPort,
    				dutIp, pclient.getSecuredPort(), CipherSuite.TLS_PSK_WITH_AES_128_CCM_8.getValue(), clientDeviceId,
    				serverDeviceId, "123");
		} catch (Exception ex) {
			fail("Fail"+ ex.getLocalizedMessage());
		}    	
    }
    
    /**
   	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-01
   	 * @reviewer
   	 * @since 2015-01-01
   	 * @see none
   	 * @objective Test 'initDtlsWithJustWorks' positively
   	 * @target public ControlMessage initDtlsWithJustWorks
   	 * @test_data none
   	 * @pre_condition none
   	 * @procedure Call initDtlsWithJustWorks(String relayIp, int relayPort,
               String dutIp, int dutPortSecured, int cipherSuite, String clientId,
               String serverId)
   	 * @post_condition none
   	 * @expected initDtlsWithJustWorks() will be called without exception
   	 */
    
    @Test
   	public void initDtlsWithJustWorks_P() {
       	ProvisioningClient pclient = new ProvisioningClient();
   		String relayIp = pclient.getCurrentSystemIpAddress(false);
   		String dutIp = relayIp;
   		ControlMessage response = null;
       	try {
       		
       		response = controlClient.initDtlsWithJustWorks(relayIp, relayPort,
       				dutIp, pclient.getSecuredPort(), CipherSuite.TLS_PSK_WITH_AES_128_CCM_8.getValue(), clientDeviceId,
       				serverDeviceId);
       		
   		} catch (Exception ex) {
   			fail("Fail"+ ex.getLocalizedMessage());
   		}    	
       }
    
    /**
   	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-01
   	 * @reviewer
   	 * @since 2015-01-01
   	 * @see none
   	 * @objective Test 'terminateDtls' positively
   	 * @target public ControlMessage terminateDtls
   	 * @test_data none
   	 * @pre_condition none
   	 * @procedure Call terminateDtls()
   	 * @post_condition none
   	 * @expected terminateDtls() will be called without exception
   	 */
    
    @Test
   	public void terminateDtls_P() {
   		ControlMessage response = null;
       	try {
       		response = controlClient.terminateDtls();
   		} catch (Exception ex) {
   			fail("Fail"+ ex.getLocalizedMessage());
   		}    	
       }
    
    /**
   	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-01
   	 * @reviewer
   	 * @since 2015-01-01
   	 * @see none
   	 * @objective Test 'handleMessage' positively
   	 * @target public void handleMessage
   	 * @test_data none
   	 * @pre_condition none
   	 * @procedure Call handleMessage()
   	 * @post_condition none
   	 * @expected handleMessage() will be called without exception
   	 */
    
    @Test
   	public void handleMessage_P() {
   		ControlMessage response = null;
       	try {
       		controlClient.handleMessage(controlMsg);
   		} catch (Exception ex) {
   			fail("Fail"+ ex.getLocalizedMessage());
   		}    	
       }
    
    
}
