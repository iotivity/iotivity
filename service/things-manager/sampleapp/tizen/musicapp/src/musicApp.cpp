//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <appcore-efl.h>
#include <Ecore_X.h>
#include <efl_assist.h>

#include <OCPlatform.h>
#include <OCApi.h>

#include <pthread.h>
#include <player.h>

#include "musicApp.h"

using namespace OC;
using namespace std;

player_h g_player;
player_state_e g_state;

int g_playingCount;
int g_stopCount;

int g_flag = 0;

static Evas_Object *g_imagePlaying;
static Evas_Object *g_imageStop;

// Forward declaring the entityHandler
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);

/// This class represents a single resource named 'MusicplayerResource'. This resource has
/// two simple properties named 'state' and 'power'

class MusicplayerResource
{
public:
    /// Access this property from a TB client
    std::string m_powerstate;
    std::string m_musicplayerUri;
    OCResourceHandle m_resourceHandle;

public:
    /// Constructor
    MusicplayerResource(): m_powerstate("off"), m_musicplayerUri("/core/music_player") {}

    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform)
    {
        printf("LOG : create resource\n");
        std::string resourceURI = m_musicplayerUri; // URI of the resource
        std::string resourceTypeName = "core.music_player"; // resource type name. In this case, it is Musicplayer
        std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

        // OCResourceProperty is defined ocstack.h
        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_resourceHandle, resourceURI, resourceTypeName,
                                    resourceInterface, &entityHandler, resourceProperty);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    void setRepresentation(OCRepresentation& musicplayer)
    {
        printf("LOG : setRepresenatation\n");

        AttributeMap attributeMap = musicplayer.getAttributeMap();

        if(attributeMap.find("powerstate") != attributeMap.end())
        {

            cout << "\t\t\t" << "Received representation: " << endl;
            cout << "\t\t\t\t" << "powerstate: " << attributeMap["powerstate"][0] << endl;

            std::string TEMP_powerstate;
            int ret;
            
            TEMP_powerstate = attributeMap["powerstate"][0];

            if( TEMP_powerstate == "on" )
            {   // TEMP_powerstate is "on". then turn on the XXXX

                printf("\nLOG : MUSIC PLAY ON\n");

                
                if(g_flag == 0){

                    char buf[PATH_MAX];

                    getcwd(buf, PATH_MAX);
                    printf("LOG : getcwd - %s\n", buf);

                    ret= player_create(&g_player);
                    if(ret!=0){
                        printf("LOG : player create error --- %d\n", ret);
                        elm_exit();
                        return;
                    }
                    printf("LOG : player_create success --- %d\n", ret);

                    g_flag = 1;
                }   //g_flag check

                // music play
                // step 1. set uri
                if((ret=player_set_uri(g_player, "file:///usr/apps/com.samsung.oicapp-test/res/musics/musicTest.mp3"))!=0){
                    printf("LOG : player set uri error --- %d\n", ret);
                    elm_exit();
                    return;
                }

                printf("LOG : player_set_uri success --- %d\n", ret);


                // step 2. player prepare
                if((ret = player_prepare(g_player))!=0){
                    printf("LOG : player prepare error --- %d\n", ret);
                    elm_exit();
                    return;
                }
                printf("LOG : player_prepare success --- %d\n", ret);


                // player status check
                int playerStatus;
                player_get_state(g_player, &g_state);
                playerStatus = g_state;

                cout << "LOG : playerStatus(start) --- " << playerStatus <<std::endl;

                if(playerStatus == PLAYER_STATE_READY){

                    printf("\nLOG : PLAYER_STATE_READY\n");

                    evas_object_hide(g_imageStop);
                    evas_object_show(g_imagePlaying);

                    ret = player_start(g_player);
         
                    if(ret != 0){
                        cout << "player start error - ret" << ret << std::endl;
                        elm_exit();
                    }

                    g_playingCount ++;  
                    printf("LOG : player_start success --- %d\n", ret);
                    printf("LOG : playing count : %d\n\n", g_playingCount);   

                }
                else{
                    cout << "LOG : playerStatus(in player_start Command) ---" << playerStatus << std::endl;
                }

            }

            // music play stop
            else 
            {   // TEMP_powerstate is "off" or else. then turn off the XXXX
                int playerStatus;
                player_get_state(g_player, &g_state);
                playerStatus = g_state;

                cout << "LOG : playerStatus(stop) --- " << playerStatus <<std::endl;

               if(playerStatus == PLAYER_STATE_PLAYING){

                    printf("\nLOG : PLAYER_STATE_PLAYING\n");

                    evas_object_show(g_imageStop);
                    evas_object_hide(g_imagePlaying);

                    ret = player_stop(g_player);

                    if(ret != 0){
                        cout << "player stop error - ret" << ret << std::endl;
                        elm_exit();
                    }

                    g_stopCount ++;

                    player_unprepare(g_player);
                    
                    cout << "LOG : PLAYER_STATE_STOP" << std::endl;           
                    printf("LOG : player_stop success --- %d\n", ret);   
                    printf("LOG : puase count : %d\n\n", g_stopCount);  
                    
                }

                else{
                    cout << "LOG : playerStatus(in player_stop Command) ---" << playerStatus << std::endl;
                }
                
            }

            m_powerstate = TEMP_powerstate;
        }
    }

    OCRepresentation getRepresentation()
    {
        OCRepresentation musicplayer;

        musicplayer.setUri(m_musicplayerUri);

        AttributeValues interfaces;
        interfaces.push_back("oc.if.def");       // DEFAULT INTERFACE.
        musicplayer.setResourceInterfaces(interfaces);

        AttributeValues types;
        types.push_back("core.music_player");
        musicplayer.setResourceTypes(types);

        AttributeMap attributeMap;
        AttributeValues powerstateVal;
        powerstateVal.push_back(m_powerstate);  
        attributeMap["powerstate"] = powerstateVal;
  
        musicplayer.setAttributeMap(attributeMap);

        return musicplayer;
    }

    void addType(const OC::OCPlatform& platform, const std::string& type) const
    {
        printf("LOG : addType\n");

        OCStackResult result = platform.bindTypeToResource(m_resourceHandle, type);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }

    void addInterface(const OC::OCPlatform& platform, const std::string& interface) const
    {
        printf("LOG : addInterface\n");

        OCStackResult result = platform.bindInterfaceToResource(m_resourceHandle, interface);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }
};

// Create the instance of the resource class (in this case instance of class 'MusicplayerResource').
MusicplayerResource g_myMusicplayerResource;

// This is just a sample implementation of entity handler.
// Entity handler can be implemented in several ways by the manufacturer
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if(requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for(QueryParamsMap::iterator it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                // AttributeMap attributeMap;
                OCRepresentation rep;
                rep = g_myMusicplayerResource.getRepresentation();

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }
            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Get the representation from the request
                OCRepresentation rep = request->getResourceRepresentation();

                g_myMusicplayerResource.setRepresentation(rep);

                // Do related operations related to PUT request
                rep = g_myMusicplayerResource.getRepresentation();

                if(response)
                {
                    // TODO Error Code
                    // response->setRepresentation(rep);
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }

            }
            else if(requestType == "POST")
            {
                // POST request operations
            }
            else if(requestType == "DELETE")
            {
                // DELETE request operations
            }
        }
        else if(requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}

void InitAndRun()
{
    printf("\nLOG : InitAndRun \n");
 
    PlatformConfig cfg(
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "192.168.20.30",
        5683,
        OC::QualityOfService::NonConfirmable );
 
    // // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform platform(cfg);

        // Invoke createResource function of class Musicplayer.
        g_myMusicplayerResource.createResource(platform);
        g_myMusicplayerResource.addType(platform, std::string("core.music_player"));
        g_myMusicplayerResource.addInterface(platform, std::string("oc.mi.ll"));

        // Perform app tasks
        while(true)
        {
        
        }

    }
    catch(OCException e)
    {
        //log(e.what());
    }

    return;
}


static Eina_Bool back_cb(void *data, Elm_Object_Item *item)
{
    if(player_stop(g_player)){
        cout << "player player_stop error" << std::endl ;
    }
    if(player_unprepare(g_player)!=0){
        cout << "player player_unprepare error" << std::endl ;
    }
    if(player_destroy(g_player) !=0){
        cout << "player player_destroy error" << std::endl ;
    }

    elm_exit();

    return EINA_FALSE;
}


static void win_del(void *data, Evas_Object *obj, void *event)
{
    DBG("test win_del()");

    elm_exit();
}

static Evas_Object* create_win(const char *name)
{
    Evas_Object *eo;
    int w, h;

    eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
    if (eo) {
        elm_win_title_set(eo, name);
        elm_win_borderless_set(eo, EINA_TRUE);
        evas_object_smart_callback_add(eo, "delete,request", win_del, NULL);
        ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
        evas_object_resize(eo, w, h);
        elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
        elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_OPAQUE);
    }

    evas_object_show(eo);

    return eo;
}

static Evas_Object* create_bg(Evas_Object *parent)
{
    Evas_Object *bg;

    bg = elm_bg_add(parent);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(parent, bg);

    evas_object_show(bg);

    return bg;
}

static Evas_Object* create_conformant(Evas_Object* parent)
{

    Evas_Object *conformant;
    conformant = elm_conformant_add(parent);

    if(NULL == conformant)
        DBG("elm_conformant_add() Failed");

    evas_object_size_hint_weight_set(conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(parent, conformant);
    evas_object_show(conformant);

    return conformant;
}

static void on_clickExit(void *data, Evas_Object *obj, void *event_info)
{
    elm_exit();
}

static void create_view_layout(void *data)
{
    MusicAppData *md = (MusicAppData *)data;

    Elm_Object_Item *it;

    if (md == NULL) return;
    if (md->nf == NULL) return;

    ea_object_event_callback_add(md->nf, EA_CALLBACK_BACK, ea_naviframe_back_cb, NULL);
    ea_object_event_callback_add(md->nf, EA_CALLBACK_MORE, ea_naviframe_more_cb, NULL);

    it = elm_naviframe_item_push(md->nf, "Music Player", NULL, NULL, NULL, NULL);
    elm_naviframe_item_pop_cb_set(it, back_cb, md);

    return;
}

static Evas_Object* create_layout_main(Evas_Object* parent)
{
    Evas_Object *layout;

    if (parent == NULL)
        return NULL;

    layout = elm_layout_add(parent);

    elm_layout_theme_set(layout, "layout", "application", "default");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(layout);

    return layout;
}

static Evas_Object* create_naviframe_layout(Evas_Object* parent)
{
    Evas_Object *nf;

    if (parent == NULL) return NULL;

    nf = elm_naviframe_add(parent);

    elm_object_part_content_set(parent, "elm.swallow.content", nf);
    evas_object_show(nf);

    return nf;
}

static void init_button(void *data)
{
    MusicAppData *md = (MusicAppData *)data;
    static Evas_Object *btnExit;

    md->btnExit = btnExit; 
    btnExit = elm_button_add(md->win);

    elm_object_text_set(btnExit, "EXIT");

    evas_object_resize(btnExit, 340, 50);
    evas_object_move(btnExit, 195, 1170);

    evas_object_smart_callback_add(btnExit, "clicked", on_clickExit, NULL);

    // show stop image
    char buf[PATH_MAX];
    char buf2[PATH_MAX];

    g_imagePlaying = elm_image_add(md->win);
    g_imageStop = elm_image_add(md->win);

    snprintf(buf, sizeof(buf), "%s/usr/apps/com.samsung.oicapp-test/res/images/playing.png", elm_app_data_dir_get());
    elm_image_file_set(g_imagePlaying, buf, NULL);

    snprintf(buf2, sizeof(buf2), "%s/usr/apps/com.samsung.oicapp-test/res/images/stop.png", elm_app_data_dir_get());
    elm_image_file_set(g_imageStop, buf2, NULL);

    evas_object_resize(g_imagePlaying, 400, 400);
    evas_object_move(g_imagePlaying, 170, 400);

    evas_object_resize(g_imageStop, 400, 400);
    evas_object_move(g_imageStop, 170, 400);

    evas_object_show(g_imageStop);
    evas_object_hide(g_imagePlaying);
    evas_object_show(btnExit);

    printf("\nLOG : init image ok \n");

    return;

}

static int musicapp_create(void *data)
{
    MusicAppData *md = (MusicAppData *)data;

    /* create window */
    md->win = create_win(PACKAGE);
    if (md->win == NULL)
        return -1;

    md->bg = create_bg(md->win);
    if (NULL == md->bg)
        return -1;

    md->conformant = create_conformant(md->win);
    if(NULL == md->conformant)
        return -1;

    /* create layout */
    md->layout_main = create_layout_main(md->conformant);
    if (md->layout_main == NULL)
        return -1;

    elm_object_content_set(md->conformant, md->layout_main);
    elm_win_conformant_set(md->win, EINA_TRUE);

    // Indicator
    elm_win_indicator_mode_set(md->win, ELM_WIN_INDICATOR_SHOW);

    // Naviframe
    md->nf = create_naviframe_layout(md->layout_main);
    // Naviframe Content
    create_view_layout(md);

    init_button(md);

    return 0;
}

static int musicapp_terminate(void *data)
{
    MusicAppData *md = (MusicAppData *)data;

    if (md->win)
        evas_object_del(md->win);

    return 0;
}

static int musicapp_pause(void *data)
{
    return 0;
}


static int musicapp_resume(void *data)
{
    return 0;
}

static int musicapp_reset(bundle *b, void *data)
{
    MusicAppData *md = (MusicAppData *)data;

    if (md->win)
        elm_win_activate(md->win);

    return 0;
}

void* callInitAndRun(void* charStr)
{
    InitAndRun();
}

int main(int argc, char *argv[])
{
    MusicAppData md;

    appcore_ops ops;
    ops.create = musicapp_create;
    ops.terminate = musicapp_terminate;
    ops.pause = musicapp_pause;
    ops.resume = musicapp_resume;
    ops.reset = musicapp_reset;

    memset(&md, 0x0, sizeof(MusicAppData));
    ops.data = &md;
    

    // create thread for InitAndRun()
    pthread_t threadId;
    pthread_create(&threadId, NULL, callInitAndRun, NULL);

    return appcore_efl_main(PACKAGE, &argc, &argv, &ops); 

}
