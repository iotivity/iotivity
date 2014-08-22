// Do not remove the include below
#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include <string.h>

#include <EthernetServer.h>
#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *getResult(OCStackResult result);

#define PCF(str) ((PROGMEM const char *)(F(str)))

PROGMEM const char TAG[] = "ArduinoServer";

int gQuitFlag = 0;
int gLEDUnderObservation = 0;
void createLEDResource();
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;

static char responsePayloadGet[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"on\",\"power\":10}}";
static char responsePayloadPut[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"off\",\"power\":0}}";
static uint16_t OC_WELL_KNOWN_PORT = 5683;

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest )
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    const char* typeOfMessage;

    switch (flag)
    {
        case OC_INIT_FLAG:
            typeOfMessage = "OC_INIT_FLAG";
            break;
        case OC_REQUEST_FLAG:
            typeOfMessage = "OC_REQUEST_FLAG";
            break;
        case OC_OBSERVE_FLAG:
            typeOfMessage = "OC_OBSERVE_FLAG";
            break;
        default:
            typeOfMessage = "UNKNOWN";
    }
    OC_LOG_V(INFO, TAG, "Receiving message type: %s", typeOfMessage);

    if(entityHandlerRequest && flag == OC_REQUEST_FLAG)
    {
        if(OC_REST_GET == entityHandlerRequest->method)
        {
            if (strlen(responsePayloadGet) < entityHandlerRequest->resJSONPayloadLen)
            {
                strncpy((char *)entityHandlerRequest->resJSONPayload, responsePayloadGet, entityHandlerRequest->resJSONPayloadLen);
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
        }
        if(OC_REST_PUT == entityHandlerRequest->method)
        {
            //Do something with the 'put' payload
            if (strlen(responsePayloadPut) < entityHandlerRequest->resJSONPayloadLen)
            {
                strncpy((char *)entityHandlerRequest->resJSONPayload, responsePayloadPut, entityHandlerRequest->resJSONPayloadLen);
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
         }
    }
    else if (entityHandlerRequest && flag == OC_OBSERVE_FLAG)
    {
        gLEDUnderObservation = 1;
    }

    return ehRet;
}

static uint8_t modCounter = 0;
void *ChangeLEDRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;
    modCounter += 1;
    if(modCounter % 10 == 0)  // Matching the timing that the Linux Sample Server App uses for the same functionality.
    {
        LED.power += 5;
        if (gLEDUnderObservation)
        {
            OC_LOG_V(INFO, TAG, " =====> Notifying stack of new power level %d\n", LED.power);
            result = OCNotifyObservers (LED.handle);
            if (OC_STACK_NO_OBSERVERS == result)
            {
                gLEDUnderObservation = 0;
            }
        }
    }
    return NULL;
}

//The setup function is called once at startup of the sketch
void setup()
{
    // Add your initialization code here
    OC_LOG_INIT();

    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
    uint16_t port = OC_WELL_KNOWN_PORT;

    //Mac address of my ethernet shield
    uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xC4, 0x05};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    Serial.print(Ethernet.localIP());
    if (error  == 0)
    {
        OC_LOG_V(ERROR, TAG, "error is: %d", error);
        return;
    }

    if (OCInit(NULL, port, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack init error"));
        return;
    }

    /*
     * Declare and create the example resource: LED
     */
    createLEDResource();

}

// The loop function is called in an endless loop
void loop()
{
    delay(2000);
    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack process error"));
        return;
    }
    ChangeLEDRepresentation(NULL);
}

void createLEDResource()
{
    LED.state = false;
    OCStackResult res = OCCreateResource(&LED.handle,
            "core.led",
            "core.rw",
            "/a/led",
            OCEntityHandlerCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created LED resource with result: %s", getResult(res));
}

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
    case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
    case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
    case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
    case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
    case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
    case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
    case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
    case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
    case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
    case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
    case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
    case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
    case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}
#ifdef __cplusplus
} // extern "C"
#endif
