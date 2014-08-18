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

#define PCF(str) ((const prog_char*)(F(str)))

const prog_char TAG[] PROGMEM = "ArduinoServer";

int gQuitFlag = 0;
int gLEDUnderObservation = 0;
void createLEDResource();
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;

// TODO: hard coded for now, change after Sprint4
static unsigned char responsePayloadGet[] = "{\"oc\": {\"payload\": {\"state\" : \"on\",\"power\" : \"10\"}}}";
static unsigned char responsePayloadPut[] = "{\"oc\": {\"payload\": {\"state\" : \"off\",\"power\" : \"0\"}}}";
static uint16_t OC_WELL_KNOWN_PORT = 5683;

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest )
{
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
    { //[CL]
        if(OC_REST_GET == entityHandlerRequest->method)
        {
            //entityHandlerRequest->resJSONPayload = reinterpret_cast<unsigned char*>(const_cast<unsigned char*> (responsePayloadGet.c_str()));
            entityHandlerRequest->resJSONPayload = responsePayloadGet;
        }
        if(OC_REST_PUT == entityHandlerRequest->method)
        {
            //std::cout << std::string(reinterpret_cast<const char*>(entityHandlerRequest->reqJSONPayload)) << std::endl;
            OC_LOG_V(INFO, TAG, "PUT JSON payload from client: %s", entityHandlerRequest->reqJSONPayload);
            //entityHandlerRequest->resJSONPayload = reinterpret_cast<unsigned char*>(const_cast<char*> (responsePayloadPut.c_str()));
            entityHandlerRequest->resJSONPayload = responsePayloadPut;
            //responsePayloadGet = responsePayloadPut; // just a bad hack!
         }
    }
    else if (entityHandlerRequest && flag == OC_OBSERVE_FLAG)
    {
        gLEDUnderObservation = 1;
    }

    //OC_LOG_V(INFO, TAG, "/nReceiving message type:/n/t %s. /n/nWith request:/n/t %s", typeOfMessage, request);

    return OC_EH_OK;
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
const char *getResult(OCStackResult result)
{
    char resString[40] = {0};
    strcpy(resString, "Result: ");
    switch (result) {
    case OC_STACK_OK:
        strcat(resString, "OC_STACK_OK");
        break;
    case OC_STACK_INVALID_URI:
        strcat(resString, "OC_STACK_INVALID_URI");
        break;
    case OC_STACK_INVALID_QUERY:
        strcat(resString, "OC_STACK_INVALID_QUERY");
        break;
    case OC_STACK_INVALID_IP:
        strcat(resString, "OC_STACK_INVALID_IP");
        break;
    case OC_STACK_INVALID_PORT:
        strcat(resString, "OC_STACK_INVALID_PORT");
        break;
    case OC_STACK_INVALID_CALLBACK:
        strcat(resString, "OC_STACK_INVALID_CALLBACK");
        break;
    case OC_STACK_INVALID_METHOD:
        strcat(resString, "OC_STACK_INVALID_METHOD");
        break;
    case OC_STACK_NO_MEMORY:
        strcat(resString, "OC_STACK_NO_MEMORY");
        break;
    case OC_STACK_COMM_ERROR:
        strcat(resString, "OC_STACK_COMM_ERROR");
        break;
    case OC_STACK_INVALID_PARAM:
        strcat(resString, "OC_STACK_INVALID_PARAM");
        break;
    case OC_STACK_NOTIMPL:
        strcat(resString, "OC_STACK_NOTIMPL");
        break;
    case OC_STACK_NO_RESOURCE:
        strcat(resString, "OC_STACK_NO_RESOURCE");
        break;
    case OC_STACK_RESOURCE_ERROR:
        strcat(resString, "OC_STACK_RESOURCE_ERROR");
        break;
    case OC_STACK_SLOW_RESOURCE:
        strcat(resString, "OC_STACK_SLOW_RESOURCE");
        break;
    case OC_STACK_NO_OBSERVERS:
        strcat(resString, "OC_STACK_NO_OBSERVERS");
        break;
    case OC_STACK_ERROR:
        strcat(resString, "OC_STACK_ERROR");
        break;
    default:
        strcat(resString, "UNKNOWN");
    }
    char* retString = resString;
    return retString;
}
#ifdef __cplusplus
} // extern "C"
#endif
