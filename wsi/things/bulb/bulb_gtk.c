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
#include <functional>
#include <mutex>
#include <condition_variable>
#include "OCPlatform.h"
#include "OCApi.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
//#include <gtkmm.h>


//static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window);
GtkWidget *image;
GtkWidget *window;

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

int gObservation = 0;
bool isListOfObservers = false;
void * ChangeLightRepresentation(void *param);

static void delete_event(GtkWidget* widget, GtkWidget* window) {
    gtk_main_quit();
}

//static gboolean closeWebViewCb(WebKitWebView* webView, GtkWidget* window) {
//    gtk_widget_destroy(window);
//    return TRUE;
//}
//
typedef enum {
    ON,
    ALARM,
    MORNING,
    EVENING,
    NIGHT,
    OFF,
} bulbstate;

const char *bulbfile[] = {
    "on.png",
    "bulb.png",
    "orange.png",
    "blue.png",
    "violet.png",
    "off.png",
    "blue.png",
    "violet.png",
    "off.png",

};

void change_bg_image(bulbstate state) {
    char buf[PATH_MAX];
    strcpy(buf, "images/");
    strcat(buf, bulbfile[state]);
    gtk_image_set_from_file((GtkImage *) image, buf);
}

void
launch_bulb() {
    image = gtk_image_new_from_file("images/on.png");
}

class LightResource{

    public :
    /// Access this property from a TB client
    std::string m_name;
    int m_state;
    std::string m_lightUri;
    OCResourceHandle m_resourceHandle;
    OCRepresentation m_lightRep;
    ObservationIds m_interestedObservers;

public:
    /// Constructor
    LightResource()
    : m_name("OIC Light"), m_state(0), m_lightUri("/a/light"),
    m_resourceHandle(nullptr) {
        // Initialize representation
        m_lightRep.setUri(m_lightUri);
        m_lightRep.setValue("state", m_state);
        m_lightRep.setValue("name", m_name);
    }



    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.

    void createResource() {
        std::string resourceURI = m_lightUri;
        std::string resourceTypeName = "core.light";
        std::string resourceInterface = DEFAULT_INTERFACE;
        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
        EntityHandler cb = std::bind(&LightResource::entityHandler, this, PH::_1);
        OCStackResult result = OCPlatform::registerResource(
        m_resourceHandle, resourceURI, resourceTypeName,
        resourceInterface, cb, resourceProperty);
        if (OC_STACK_OK != result) {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    OCResourceHandle getHandle() {
        return m_resourceHandle;
    }

    OCRepresentation post(OCRepresentation & rep) {
        cout << "Post incoked......................................." << endl;
        try
        {
            if (rep.getValue("state", m_state)) {
                cout << "\t\t\t\t" << "----state: " << m_state << endl;
                change_bg_image(m_state);
            } else {
                cout << "\t\t\t\t" << "state not found in the representation" << endl;
            }
        }

        catch(exception & e) {
            cout << e.what() << endl;
        }
        return rep;
    }

    OCRepresentation get() {
        m_lightRep.setValue("state", m_state);
        return m_lightRep;
    }

    void addType(const std::string & type) const {
        OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle, type);
        if (OC_STACK_OK != result) {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }

    void addInterface(const std::string & interface) const {
        OCStackResult result = OCPlatform::bindInterfaceToResource(m_resourceHandle, interface);
        if (OC_STACK_OK != result) {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }

private:
    OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request) {
        cout << "\tIn Server CPP entity handler:\n";
        OCEntityHandlerResult ehResult = OC_EH_ERROR;
        if (request) {
            std::string requestType = request->getRequestType();
            int requestFlag = request->getRequestHandlerFlag();
            if (requestFlag & RequestHandlerFlag::RequestFlag) {
                cout << "\t\trequestFlag : Request\n";
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
                if (requestType == "GET") {
                    cout << "\t\t\trequestType : GET\n";
                    pResponse->setErrorCode(200);
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(get());
                    if (OC_STACK_OK == OCPlatform::sendResponse(pResponse)) {
                        ehResult = OC_EH_OK;
                    }
                } else if (requestType == "POST") {
                    cout << "\t\t\trequestType : POST\n";

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

            if (requestFlag & RequestHandlerFlag::ObserverFlag) {
                ObservationInfo observationInfo = request->getObservationInfo();
                if (ObserveAction::ObserveRegister == observationInfo.action) {
                    m_interestedObservers.push_back(observationInfo.obsId);
                } else if (ObserveAction::ObserveUnregister == observationInfo.action) {
                    m_interestedObservers.erase(std::remove(
                            m_interestedObservers.begin(),
                            m_interestedObservers.end(),
                            observationInfo.obsId),
                            m_interestedObservers.end());
                }
                pthread_t threadId;
                cout << "\t\trequestFlag : Observer\n";
                gObservation = 1;
                static int startedThread = 0;

                // Observation happens on a different thread in ChangeLightRepresentation function.
                // If we have not created the thread already, we will create one here.
                if (!startedThread) {
                    pthread_create(&threadId, NULL, ChangeLightRepresentation, (void *) this);
                    startedThread = 1;
                }
                ehResult = OC_EH_OK;
            }
        } else {
            std::cout << "Request invalid" << std::endl;
        }

        return ehResult;
    }

};

LightResource myLight;

void * ChangeLightRepresentation(void *param) {
    LightResource* lightPtr = (LightResource*) param;

    // This function continuously monitors for the changes
    while (1) {
        sleep(3);

        if (gObservation) {
            lightPtr->m_state = lightPtr->m_state + 1;
            if (lightPtr->m_state > 6)
                lightPtr->m_state = 0;

            cout << "Notifying observers with resource handle: " << lightPtr->getHandle() << endl;

            OCStackResult result = OC_STACK_OK;

            if (isListOfObservers) {
                std::shared_ptr<OCResourceResponse> resourceResponse = {std::make_shared<OCResourceResponse>()};

                resourceResponse->setErrorCode(200);
                resourceResponse->setResourceRepresentation(lightPtr->get(), DEFAULT_INTERFACE);

                result = OCPlatform::notifyListOfObservers(lightPtr->getHandle(),
                        lightPtr->m_interestedObservers,
                        resourceResponse);
            } else {
                result = OCPlatform::notifyAllObservers(lightPtr->getHandle());
            }

            if (OC_STACK_NO_OBSERVERS == result) {
                cout << "No More observers, stopping notifications" << endl;
                gObservation = 0;
            }
        }
    }

    return NULL;
}

static FILE* client_open(const char *path, const char *mode) {
    return fopen("./oic_svr_db_server.json", mode);
}

int main(int argc, char **argv) {
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
    try{
//        myLight.createResource();
//        std::cout << "Created resource." << std::endl;
//        myLight.addType(std::string("core.brightlight"));
//        myLight.addInterface(std::string(LINK_INTERFACE));
//        std::cout << "Added Interface and Type" << std::endl;
    }

    catch(OCException & e) {
        std::cout << "OCException in main : " << e.what() << endl;
    }

    // Initialize GTK+
    GtkWidget *window;
    
    GtkWidget *images[9];

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 360, 600);
    GtkWidget *table = gtk_table_new (3, 3, TRUE);
    gtk_container_add (GTK_CONTAINER (window), table);
    
    char buffer[20];
    for(int i = 0; i<3; i++)
    {
        sprintf(buffer, "images/%s", bulbfile[i]);
        images[i] = gtk_image_new_from_file (buffer);
        printf("Image Name %s %p\n", buffer, images[i]);
        gtk_image_set_from_file((GtkImage *) images[i], buffer);
        gtk_widget_set_size_request(images[i], 100, 150);
        gtk_table_attach_defaults (GTK_TABLE (table), images[i], 0, 1, 0, 1);
    }
    
    gtk_container_set_border_width (GTK_CONTAINER (window), 20);
    gtk_widget_show(window);
    g_signal_connect (window, "delete-event",G_CALLBACK (delete_event), NULL);
    
    gtk_main();

    return 0;
}

// Create an 800x600 window that will contain the browser instance
//    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);
// Create a browser instance
//WebKitWebView *webView = WEBKIT_WEB_VIEW(webkit_web_view_new());

// Put the browser area into the main window
//gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(webView));

// Set up callbacks so that if either the main window or the browser instance is
// closed, the program will exit
//g_signal_connect(main_window, "destroy", G_CALLBACK(destroyWindowCb), NULL);
//g_signal_connect(webView, "close", G_CALLBACK(closeWebViewCb), main_window);

// Load a web page into the browser instance
//webkit_web_view_load_uri(webView, "http://www.webkitgtk.org/");

// Make sure that when the browser area becomes visible, it will get mouse
// and keyboard events
//gtk_widget_grab_focus(GTK_WIDGET(webView));
//launch_bulb();
//gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(image));
//Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("homethings.glade");


// Make sure the main window and all its contents are visible
//gtk_widget_show_all(main_window);


//    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
//
//    //Load the GtkBuilder file and instantiate its widgets:
//    Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
//    try{
//        refBuilder->add_from_file("homethings.glade");
//    }
//    catch(const Glib::FileError & ex) {
//        std::cerr << "FileError: " << ex.what() << std::endl;
//        return 1;
//    }
//    catch(const Glib::MarkupError & ex) {
//        std::cerr << "MarkupError: " << ex.what() << std::endl;
//        return 1;
//    }
//    catch(const Gtk::BuilderError & ex) {
//        std::cerr << "BuilderError: " << ex.what() << std::endl;
//        return 1;
//    }
//    window = 
//    app->run(refBuilder);

