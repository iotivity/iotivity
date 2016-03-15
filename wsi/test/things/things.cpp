//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
///

#include <functional>

#include <pthread.h>
#include <mutex>
#include <string>
#include <condition_variable>

#include "OCPlatform.h"
#include "OCApi.h"
#include <Elementary.h>


#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

int sendsock;
struct sockaddr_un sname;
pthread_t thread_id;
int gObservation = 0;
bool isListOfObservers = false;
Evas_Object *images[1][1];

typedef enum {
    BULB,
    THERMOSTAT,
    HVAC,
    CARLOCATION
} things_e;

int chosenThing = BULB;

typedef struct thingbox
{
	things_e t;
	int state;
	string name;
	string desc;
	string res;
	string intf;
	string uri;
	map<string, string> props;
};

thingbox things[] = {
		{BULB, 			0, "bulb", "OCF Light", "core.light", "core.brightlight", "/a/light",
				{
					{"power", ""},
					{"state", ""}
				}
		},
		{THERMOSTAT,	0, "thermostat", "OCF Thermostat", "core.thermostat", "core.thermostat", "/a/thermostat",
				{
					{"temp", ""},
					{"state", ""}
				}
		},
		{HVAC, 			0, "hvac", "Vehicle HVAC", "core.rvihvac", "core.rvihvac", "/rvi/hvac",
				{
					{"leftTemperature", ""},
					{"rightTemperature", ""},
					{"leftSeatHeat", ""},
					{"rightSeatHeat", ""},
					{"fanSpeed", ""},
					{"fanDown", ""},
					{"fanRight", ""},
					{"fanUp", ""},
					{"fanAC", ""},
					{"fanAuto", ""},
					{"fanRecirc", ""},
					{"defrostMax", ""},
					{"defrostFront", ""},
					{"defrostRear", ""}
				}
		},
		{CARLOCATION,	0, "carloc", "Vehicle Location", "core.rvilocation", "core.rvilocation", "/rvi/location",
				{
					{"vehicle_id", "RangeRover"},
					{"lat", ""},
					{"lon", ""},
					{"bearing", ""}
				}
		}
};

void change_bg_image(int state) {
    char buf[PATH_MAX];
    things[chosenThing].state = state % 6;
    sprintf(buf, "images/%s/%d.png", things[chosenThing].name.c_str(), things[chosenThing].state);
    printf("Changing state to %d\n", state);
    elm_photo_file_set(images[0][0], buf);
}

void
my_win_del(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED) {
    pthread_kill(thread_id, SIGKILL);
    elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

void sender(int power) {
    if (sendto(sendsock, &power, sizeof(int), 0, (struct sockaddr *)&sname, sizeof(struct sockaddr_un)) < 0) {
        perror("sending datagram message");
    }
}

class TestResource {
    public:
        /// Access this property from a TB client
        std::string m_name;
        std::string m_thingURI;
        OCResourceHandle m_resourceHandle;
        OCRepresentation m_thingRep;
        ObservationIds m_interestedObservers;

    public:
        /// Constructor

        TestResource(PlatformConfig& /*cfg*/)
            : m_name(things[chosenThing].name), m_thingURI(things[chosenThing].uri) {
                m_thingRep.setUri(m_thingURI);
                m_thingRep.setValue("name", m_name);
            }

        void createResource() {
            std::string resourceURI = m_thingURI; // URI of the resource
            std::string resourceTypeName = things[chosenThing].res;
            std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.
            uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
            EntityHandler cb = std::bind(&TestResource::entityHandler, this, PH::_1);
            OCStackResult result = OCPlatform::registerResource(
                    m_resourceHandle, resourceURI, resourceTypeName,
                    resourceInterface, cb, resourceProperty);

            if (OC_STACK_OK != result) {
                std::cout << "Resource creation was unsuccessful\n";
            }
        }

        OCResourceHandle getHandle() {
            return m_resourceHandle;
        }

        OCRepresentation post(OCRepresentation& rep) {
            std::cout << "Post invoked......................................." << std::endl;
            try {
//                if (rep.getValue("state", m_state)) {
//                    std::cout << "\t\t\t\t" << "----state: " << m_state << std::endl;
//                } else {
//                    std::cout << "\t\t\t\t" << "state not found in the representation" << std::endl;
//                }
//                if (rep.getValue("power", m_power)) {
//                    std::cout << "\t\t\t\t" << "----state: " << m_power << std::endl;
//                    sender(m_power);
//                } else {
//                    std::cout << "\t\t\t\t" << "state not found in the representation" << std::endl;
//                }
                if (gObservation)
                {
                    //cout << "\nPower updated to : " << m_power << endl;
                    cout << "Notifying observers : " << endl;
                    OCStackResult result = OC_STACK_OK;
                    if(isListOfObservers)
                    {
                        std::shared_ptr<OCResourceResponse> resourceResponse =
                            std::make_shared<OCResourceResponse>();
                        resourceResponse->setErrorCode(200);
                        resourceResponse->setResourceRepresentation(get(), DEFAULT_INTERFACE);
                        result = OCPlatform::notifyListOfObservers(
                                getHandle(),
                                m_interestedObservers,
                                resourceResponse,
                                OC::QualityOfService::HighQos);
                    }
                    else
                    {
                        result = OCPlatform::notifyAllObservers(getHandle(),
                                OC::QualityOfService::HighQos);
                    }

                    if(OC_STACK_NO_OBSERVERS == result)
                    {
                        cout << "No More observers, stopping notifications" << endl;
                        gObservation = 0;
                    }
                }

            } catch (std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return get();
        }

        OCRepresentation get() {
            //std::cout << "OCRepresentation get." << m_power << " and " <<m_state <<std::endl;
//            m_thingRep.setValue("state", m_state);
//            m_thingRep.setValue("power", m_power);
            //change_bg_image(m_power);
            return m_thingRep;
        }

        void addType(const std::string& type) const {
            OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle, type);
            if (OC_STACK_OK != result) {
                std::cout << "Binding TypeName to Resource was unsuccessful\n";
            }
        }

        void addInterface(const std::string& interface) const {
            OCStackResult result = OCPlatform::bindInterfaceToResource(m_resourceHandle, interface);
            if (OC_STACK_OK != result) {
                std::cout << "Binding TypeName to Resource was unsuccessful\n";
            }
        }

    private:

        OCStackResult sendResponse(std::shared_ptr<OCResourceRequest> pRequest) {
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(pRequest->getRequestHandle());
            pResponse->setResourceHandle(pRequest->getResourceHandle());
            pResponse->setResourceRepresentation(get());
            pResponse->setErrorCode(200);
            pResponse->setResponseResult(OC_EH_OK);

            return OCPlatform::sendResponse(pResponse);
        }

        OCStackResult sendPostResponse(std::shared_ptr<OCResourceRequest> pRequest) {
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(pRequest->getRequestHandle());
            pResponse->setResourceHandle(pRequest->getResourceHandle());

            OCRepresentation rep = pRequest->getResourceRepresentation();
            OCRepresentation rep_post = post(rep);

            pResponse->setResourceRepresentation(rep_post);
            pResponse->setErrorCode(200);
            pResponse->setResponseResult(OC_EH_OK);

            return OCPlatform::sendResponse(pResponse);
        }

        OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request) {
            std::cout << "\tIn Server CPP entity handler:\n";
            OCEntityHandlerResult ehResult = OC_EH_ERROR;
            if (request) {
                std::string requestType = request->getRequestType();
                int requestFlag = request->getRequestHandlerFlag();
                if (requestFlag & RequestHandlerFlag::RequestFlag) {
                    std::cout << "\t\trequestFlag : Request\n";
                    auto pResponse = std::make_shared<OC::OCResourceResponse>();
                    pResponse->setRequestHandle(request->getRequestHandle());
                    pResponse->setResourceHandle(request->getResourceHandle());
                    QueryParamsMap queries = request->getQueryParameters();
                    if (!queries.empty()) {
                        std::cout << "\nQuery processing upto entityHandler" << std::endl;
                    }
                    for (auto it : queries) {
                        std::cout << "Query key: " << it.first << " value : " << it.second
                            << std::endl;
                    }
                    std::cout << "\t\t\trequestType :"<<requestType<<" \n";

                    if (!requestType.compare("GET")) {
                        pResponse->setErrorCode(200);
                        pResponse->setResponseResult(OC_EH_OK);
                        pResponse->setResourceRepresentation(get());
                        if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
                            ehResult = OC_EH_OK;
                        }
                    } else if (!requestType.compare("POST") || !requestType.compare("PUT")){

                        OCRepresentation rep = request->getResourceRepresentation();
                        OCRepresentation rep_post = post(rep);
                        pResponse->setResourceRepresentation(rep_post);
                        pResponse->setErrorCode(200);
                        if (rep_post.hasAttribute("createduri")) {
                            pResponse->setResponseResult(OC_EH_RESOURCE_CREATED);
                            pResponse->setNewResourceUri(rep_post.getValue<std::string>("createduri"));
                        } else {
                            pResponse->setResponseResult(OC_EH_OK);
                        }

                        if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
                            ehResult = OC_EH_OK;
                        }
                    }   
                }

                if(requestFlag & RequestHandlerFlag::ObserverFlag)
                {
                    ObservationInfo observationInfo = request->getObservationInfo();
                    if(ObserveAction::ObserveRegister == observationInfo.action)
                    {
                        m_interestedObservers.push_back(observationInfo.obsId);
                    }
                    else if(ObserveAction::ObserveUnregister == observationInfo.action)
                    {
                        m_interestedObservers.erase(std::remove(
                                    m_interestedObservers.begin(),
                                    m_interestedObservers.end(),
                                    observationInfo.obsId),
                                m_interestedObservers.end());
                    }

                    cout << "\t\trequestFlag : Observer\n";
                    gObservation = 1;
                    ehResult = OC_EH_OK;
                }
            } else {
                std::cout << "Request invalid" << std::endl;
            }

            return ehResult;
        }

};

static FILE* client_open(const char *path, const char *mode) {
    return fopen("./oic_svr_db_server.json", mode);
}

static Eina_Bool _fd_handler_cb(void *data, Ecore_Fd_Handler *handler)
{
    int power = 0;
    int sock = ecore_main_fd_handler_fd_get(handler);
    if (read(sock, &power, sizeof(int)) < 0){
        printf("receiving datagram packet\n");
    }
    else{
        printf("-->%d\n", power);
        change_bg_image(power);
    }
    return ECORE_CALLBACK_RENEW;
}

void thing_ui() {
    Evas_Object *win, *bg;
    Evas_Object *box;
    char buf[PATH_MAX];

    win = elm_win_add(NULL, "bg-image", ELM_WIN_BASIC);
    elm_win_title_set(win, "WSI Demo Control");
    elm_win_autodel_set(win, EINA_TRUE);
    evas_object_smart_callback_add(win, "delete,request", my_win_del, NULL);

    bg = elm_bg_add(win);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, bg);
    evas_object_show(bg);

    box = elm_box_add(win);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, box);
    evas_object_show(box);

    Evas_Object *tb = elm_table_add(win);
    evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);


    int n = 0;
    Evas_Object *ph;
    for (int j = 0; j < 1; j++) {
        for (int i = 0; i < 1; i++) {
            ph = elm_photo_add(win);
            sprintf(buf, "images/%s/%d.png", things[chosenThing].name.c_str(), things[chosenThing].state);
            n++;
            if (n >= 5) n = 0;
            images[j][i] = ph;
            elm_photo_aspect_fixed_set(ph, EINA_FALSE);
            elm_photo_size_set(ph, 280);
            elm_photo_file_set(ph, buf);
            elm_photo_editable_set(ph, EINA_TRUE);
            evas_object_size_hint_weight_set(ph, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(ph, EVAS_HINT_FILL, EVAS_HINT_FILL);
            elm_table_pack(tb, ph, i, j, 1, 1);
            evas_object_show(ph);
        }
    }
    elm_box_pack_end(box, tb);
    evas_object_show(tb);
    evas_object_size_hint_min_set(bg, 160, 160);
    evas_object_size_hint_max_set(bg, 640, 640);
    evas_object_resize(win, 320, 320);
    evas_object_show(win);

    int sock;
    struct sockaddr_un name;
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    name.sun_family = AF_UNIX;

    char socket_path[100];
    sprintf(socket_path, "/tmp/.hidden%s", things[chosenThing].name.c_str());

    strcpy(name.sun_path, socket_path);
    if (bind(sock, (struct sockaddr *) &name, sizeof(struct sockaddr_un))) {
        printf("binding name to datagram socket\n");
    }else{
        printf("client socket %d-->%s\n", sock, socket_path);
        ecore_main_fd_handler_add(sock,ECORE_FD_READ | ECORE_FD_ERROR,_fd_handler_cb,NULL, NULL, NULL);
    }
    printf("Bulb launched");
    elm_run();
    return NULL;
}

void *thing_handler(void *ptr)
{
    OCPersistentStorage ps{client_open, fread, fwrite, fclose, unlink};
    PlatformConfig cfg{
        OC::ServiceType::InProc,
            OC::ModeType::Server,
            "0.0.0.0",
            0,
            OC::QualityOfService::LowQos,
            &ps
    };
    OCPlatform::Configure(cfg);
    try
    {
        TestResource myThing(cfg);
        myThing.createResource();
        std::cout << "Created resource." << std::endl;
        myThing.addType(std::string(things[chosenThing].intf));
        myThing.addInterface(std::string(LINK_INTERFACE));
        std::cout << "Added Interface and Type" << std::endl;

        sendsock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sendsock < 0) {
            printf("opening sending datagram socket\n");
        }else{
            printf("Sender Socket Ready - %d\n", sendsock);
        }
        sname.sun_family = AF_UNIX;
        char socket_path[100];
        sprintf(socket_path, "/tmp/.hidden%s", things[chosenThing].name.c_str());
        strcpy(sname.sun_path, socket_path);
        printf("SNAME initialized to %s\n", sname.sun_path);
        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        std::cout <<"Waiting" << std::endl;
        cv.wait(lock, []{return false;});
    } catch (OCException & e) {
        std::cout << "OCException in main : " << e.what() << std::endl;
    }
    return NULL;
}


    EAPI_MAIN int
elm_main(int argc, char **argv)
{
    //create the UI
    if(pthread_create(&thread_id, NULL, thing_handler, NULL)) {
        fprintf(stderr, "Error creating thread\n");
    }else{
        printf("Thing Handler Created");
    }

    chosenThing = atoi(argv[1]);
    thing_ui();
    printf("Thing UI Created");


    elm_run();

    pthread_join(thread_id, NULL);

    char socket_path[100];
    sprintf(socket_path, "/tmp/.hidden%s", things[chosenThing].name.c_str());
    unlink(socket_path);
    elm_shutdown();
    return 0;
}
ELM_MAIN();
