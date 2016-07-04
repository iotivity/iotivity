//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * These are adapter APIs will make caller modules agnostic to platform.
 */

#include "softapnative.h"
#include "easysetup.h"
#include "softap.h"
#include "networkhandler.h"

void ESCreateSoftapCallback(int result, const char *ip, const char* mac_addr,
                                                            const char* device_name );

ESEnrolleeNetworkEventCallback gCallback;

/**
 * It will return Device which is creating Soft AP.
 * i.e.Enrollee or Mediator.
 * This decesion is based on, what is the value set for ES_SOFTAP_MODE in build command as well as 
 * in iotivity.spec file.
 */
DeviceRole ESSoftapHost()
{
#ifdef ENROLLEE_SOFTAP
    //SoftAp at enrollee
    return ENROLLEE;
#else
    //SoftAp at mediator
    return MEDIATOR;
#endif
}

/**
 * It will return true if the Soft AP is created at Enrollee device
 */
bool ESSoftapAtEnrollee()
{
    return ( ESSoftapHost() == ENROLLEE) ? true : false;
}

/**
 * It will return true if the Soft AP is created at Mediator device
 */
bool ESSoftapAtMediator()
{
    return (ESSoftapHost() == MEDIATOR) ? true : false;
}

/**
 * This API will map result returned from the native to the caller.
 * Callback API's signature needs to be modified under build switch according
 * to the underline platform.
 */
void ESCreateSoftapCallback(int result, const char *ip, const char* mac_addr,
                                                                const char* device_name)
{
    if(SOFTAP_SUCCESS ==  result) //on Success
    {
        gCallback(ES_OK);
    }
    else //on Failure
    {
        gCallback(ES_ERROR);
    }
}

/**
 * This API will create the softap at enrollee
 */
void ESCreateSoftap(const char * ssid, const char* passwd, ESEnrolleeNetworkEventCallback cb)
{
    gCallback = cb;

    // TODO: To be implemented
    // Create Soft AP through native APIs -- start
    // native API call by and pass ESCreateSoftapCallback() as callback
    ESCreateSoftapNative(ssid, passwd,ESCreateSoftapCallback);
    // Create Soft AP through native APIs -- end
}

