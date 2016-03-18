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

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


#include <Elementary.h>


#include "common.h"


using namespace OC;
using namespace std;
namespace PH = std::placeholders;
Evas_Object *images[1][1];

int uisock;
struct sockaddr_un uiname;
pthread_t thing_thread;

int notisock;
struct sockaddr_un notiname;
pthread_t noti_thread;
void * noti_handler(void *param);

void change_bg_image(int state) {
    char buf[PATH_MAX];
    things[chosenThing].state = state;
    sprintf(buf, "images/%s/%d.png", things[chosenThing].name.c_str(), state);
    printf("Changing state to %d\n", state);
    elm_photo_file_set(images[0][0], buf);
}

void
my_win_del(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED) {
    pthread_kill(thing_thread, SIGKILL);
    pthread_kill(noti_thread, SIGKILL);
    pthread_join(thing_thread, NULL);
    pthread_join(noti_thread, NULL);
    char socket_path[100];
    sprintf(socket_path, "/tmp/.hidden%s", things[chosenThing].name.c_str());
    unlink(socket_path);
    sprintf(socket_path, "/tmp/.hiddennoti%s", things[chosenThing].name.c_str());
    unlink(socket_path);
    elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

void sender(int sock, struct sockaddr_un *sname, int data){
    if (sendto(sock, &data, sizeof(int), 0, (struct sockaddr *)sname, sizeof(struct sockaddr_un)) < 0) {
        perror("sending datagram message");
    }
}


class Server {
    public:
        /// Access this property from a TB client
        std::string m_name;
        std::string m_thingURI;
        std::string m_id;
        OCResourceHandle m_resourceHandle;
        OCRepresentation m_thingRep;
        ObservationIds m_interestedObservers;

    public:
        /// Constructor

        Server(PlatformConfig& cfg)
            : m_name(things[chosenThing].name), m_thingURI(things[chosenThing].uri), m_id("RangeRover") {
                m_thingRep.setUri(m_thingURI);
                m_thingRep.setValue("name", m_name);
                m_thingRep.setValue("vehicle_id", m_id);
                map<string, double>::iterator it;

                for(it = things[chosenThing].props.begin(); it != things[chosenThing].props.end(); it++) {
                	m_thingRep.setValue(it->first, it->second);
                	cout<<"Property Name = "<<it->first<<" Value = "<<it->second<<endl;
                }
            }

        void createResource() {
            std::string resourceURI = m_thingURI; // URI of the resource
            std::string resourceTypeName = things[chosenThing].res;
            std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.
            uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
            EntityHandler cb = std::bind(&Server::entityHandler, this, PH::_1);
            OCStackResult result = OCPlatform::registerResource(
                    m_resourceHandle, resourceURI, resourceTypeName,
                    resourceInterface, cb, resourceProperty);

            if (OC_STACK_OK != result) {
                std::cout << "\nResource creation was unsuccessful\n";
            }else{
            	std::cout << "\nResource created successfully\n";
            }
            pthread_create(&noti_thread, NULL, noti_handler, (void *)this);
        }

        OCResourceHandle getHandle() {
            return m_resourceHandle;
        }

        OCRepresentation post(OCRepresentation& rep) {
            std::cout << "Post invoked......................................." << std::endl;
            try {
                map<string, double>::iterator it;

                for(it = things[chosenThing].props.begin(); it != things[chosenThing].props.end(); it++) {
					if (rep.getValue(it->first, it->second)) {
						m_thingRep.setValue(it->first, it->second);
						std::cout << "\t\t\t\t" << "Param = "<<it->first << " Value = " <<it->second << std::endl;
						sender(notisock, &notiname, 1);
            			int number = 100 * rand() % 6;
            			if(number<0) number*=-1;
						sender(uisock, &uiname, number);
					} else {
						std::cout << "\t\t\t\t" << it->first << "not found in the representation" << std::endl;
					}
                }
            } catch (std::exception & e) {
                std::cout << e.what() << std::endl;
            }
            return get();
        }

        OCRepresentation get() {
        	std::cout << "Get invoked......................................." << std::endl;
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
void * noti_handler(void *param)
{
	Server* ptr = (Server*) param;
	int trigger = 0;

    int sock;
    struct sockaddr_un name;
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    name.sun_family = AF_UNIX;
    char socket_path[100];
    sprintf(socket_path, "/tmp/.hiddennoti%s", things[chosenThing].name.c_str());
    strcpy(name.sun_path, socket_path);
    if (bind(sock, (struct sockaddr *) &name, sizeof(struct sockaddr_un))) {
        printf("binding %s to datagram socket\n", socket_path);
    }else{
        printf("client socket %d-->%s\n", sock, socket_path);
    }

	cout << "Noti Thread Created and Waiting to notify observers : " << endl;

    while (read(sock, &trigger, sizeof(int))>0)
    {
		cout << "Notifying observers : " << endl;
		OCStackResult result = OC_STACK_OK;
		std::shared_ptr<OCResourceResponse> resourceResponse =
			std::make_shared<OCResourceResponse>();
		resourceResponse->setErrorCode(200);
		resourceResponse->setResourceRepresentation(ptr->get(), DEFAULT_INTERFACE);
		result = OCPlatform::notifyListOfObservers(
				ptr->getHandle(),
				ptr->m_interestedObservers,
				resourceResponse,
				OC::QualityOfService::HighQos);
		cout << "Notifying observers result  : " << result <<endl;

		if(OC_STACK_NO_OBSERVERS == result)
		{
			cout << "No More observers" << endl;
		}
    }
    return NULL;
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
        printf("binding %s to datagram socket\n", socket_path);
    }else{
        printf("client socket %d-->%s\n", sock, socket_path);
        ecore_main_fd_handler_add(sock,ECORE_FD_READ | ECORE_FD_ERROR,_fd_handler_cb,NULL, NULL, NULL);
    }
    cout<<things[chosenThing].name<< " launched"<<endl;
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
        Server myThing(cfg);
        myThing.createResource();
        std::cout << "Created resource." << std::endl;
        myThing.addType(std::string(things[chosenThing].intf));
        myThing.addInterface(std::string(LINK_INTERFACE));
        std::cout << "Added Interface and Type" << std::endl;

        uisock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (uisock < 0) {
            printf("error opening sending datagram socket\n");
        }else{
            printf("UI Socket Ready - %d\n", uisock);
        }
        uiname.sun_family = AF_UNIX;
        char socket_path[100];
        sprintf(socket_path, "/tmp/.hidden%s", things[chosenThing].name.c_str());
        strcpy(uiname.sun_path, socket_path);
        printf("SNAME initialized to %s\n", uiname.sun_path);


        notisock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (notisock  < 0) {
            printf("error opening sending datagram socket\n");
        }else{
            printf("Noti Socket Ready - %d\n", notisock);
        }
        notiname.sun_family = AF_UNIX;
        sprintf(socket_path, "/tmp/.hiddennoti%s", things[chosenThing].name.c_str());
        strcpy(notiname.sun_path, socket_path);
        printf("SNAME initialized to %s\n", notiname.sun_path);


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
    if(argc<2)
    {
	printf("\nUsage : Provide thing to create\n");
	printf("0 : Bulb\n");
	printf("1 : Thermostat\n");
	printf("2 : HVAC\n");
	printf("3 : Vehicle Location\n");
	return -1;
    }


    //create the UI
    if(pthread_create(&thing_thread, NULL, thing_handler, NULL)) {
        fprintf(stderr, "Error creating thread\n");
    }else{
        printf("Thing Handler Created\n");
    }

    chosenThing = atoi(argv[1]);
    thing_ui();
    printf("Thing UI Created\n");


    elm_run();

    elm_shutdown();
    return 0;
}
ELM_MAIN();
