/*
 * oicapp-test.cpp
 *
 *  Created on: Aug 29, 2014
 *      Author: parhi
 */

#include <appcore-efl.h>
#include <Ecore_X.h>
#include <efl_assist.h>

#include "oicapp-sampleNM.h"
#include "oicapp-utils.h"

#include "NotificationManager.h"

const char* OICAPP_STR_NM = "Notification Manager";
const char* OICAPP_STR_URI = "uri : ";
const char* OICAPP_STR_HOST = "host : ";
const char* OICAPP_STR_TEMP = "temperature : ";
const char* OICAPP_STR_HUMID = "humidity : ";

static oicappData *g_current_ad = nullptr;
std::shared_ptr< OCResource > g_curResource;

static void _update_resource(oicappData * ad , Elm_Object_Item * elm_item , const char* type ,
        const char* value);

class NMHandler
{
public:

    static std::function< void(std::shared_ptr< OCResource > resource) > s_startHosting;
    static std::function< void() > s_findHostingCandidate;
    static std::function< void(std::string) > s_addExtraStr;

    void Observe(AttributeMap &inputAttMap)
    {
        DBG("Onobserve");

        std::string tmpStr[2];
        int index = 0;
        for(auto it = inputAttMap.begin() ; it != inputAttMap.end() ; ++it)
        {
            tmpStr[index] = it->first;
            tmpStr[index].append(" : ");
            for(auto value = it->second.begin() ; value != it->second.end() ; ++value)
            {
                tmpStr[index].append(*value);
            }
            index++;
        }

        _update_resource(g_current_ad , g_current_ad->itemTemp , "" , tmpStr[0].c_str());
        _update_resource(g_current_ad , g_current_ad->itemHumid , "" , tmpStr[1].c_str());
    }

    void Found(std::shared_ptr< OCResource > resource)
    {
        g_curResource = resource;

        std::string uri = resource->uri();
        std::string host = resource->host();
        std::string type = *(resource->getResourceTypes().data());

        _update_resource(g_current_ad , g_current_ad->itemUri , OICAPP_STR_URI , uri.c_str());
        _update_resource(g_current_ad , g_current_ad->itemHost , OICAPP_STR_HOST , host.c_str());
    }
};

std::function< void(std::shared_ptr< OCResource > resource) > NMHandler::s_startHosting;
std::function< void() > NMHandler::s_findHostingCandidate;
std::function< void(std::string) > NMHandler::s_addExtraStr;

NMHandler handle;

static Elm_Object_Item* oicapp_append_separator(Evas_Object *genlist , oicappData *ad)
{
    DBG("Enter");
    Elm_Object_Item *item = NULL;

    item = elm_genlist_item_append(genlist , &ad->itcSeperator , NULL , NULL ,
            ELM_GENLIST_ITEM_NONE , NULL , NULL);
    elm_genlist_item_select_mode_set(item , ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
    DBG("LEAVE");
    return item;
}

static void _win_del(void *data , Evas_Object *obj , void *event)
{
    DBG("ENTER - test _win_del()");
    elm_exit();
    DBG("LEAVE - test _win_del()");
}

static Evas_Object* oicapp_create_win(const char *name)
{
    DBG("Enter");
    Evas_Object *eo;
    int w , h;

    eo = elm_win_add(NULL , name , ELM_WIN_BASIC);
    if(eo)
    {
        elm_win_title_set(eo , name);
        elm_win_borderless_set(eo , EINA_TRUE);
        evas_object_smart_callback_add(eo , "delete,request" , _win_del , NULL);
        ecore_x_window_size_get(ecore_x_window_root_first_get() , &w , &h);
        evas_object_resize(eo , w , h);
        elm_win_indicator_mode_set(eo , ELM_WIN_INDICATOR_SHOW);
        elm_win_indicator_opacity_set(eo , ELM_WIN_INDICATOR_OPAQUE);
    }

    evas_object_show(eo);
    DBG("LEAVE");
    return eo;
}

static Evas_Object* oicapp_create_bg(Evas_Object *parent)
{
    DBG("Enter");
    Evas_Object *bg;

    bg = elm_bg_add(parent);
    evas_object_size_hint_weight_set(bg , EVAS_HINT_EXPAND , EVAS_HINT_EXPAND);
    elm_win_resize_object_add(parent , bg);
    evas_object_show(bg);
    DBG("LEAVE");

    return bg;
}

static Evas_Object* oicapp_create_base_layout(Evas_Object *parent)
{
    DBG("Enter");
    Evas_Object *base;

    base = elm_layout_add(parent);
    elm_layout_theme_set(base , "layout" , "application" , "default");
    evas_object_size_hint_weight_set(base , EVAS_HINT_EXPAND , EVAS_HINT_EXPAND);
    evas_object_show(base);

    DBG("LEAVE");
    return base;
}

void __gl_realized_cb(void *data , Evas_Object *obj , void *event_info)
{
    DBG("Enter");
    Elm_Object_Item *item = (Elm_Object_Item *) event_info;

    elm_object_item_signal_emit(item , "elm,state,normal" , "");
    DBG("LEAVE");
}

static Evas_Object* oicapp_create_genlist(Evas_Object *parent)
{
    DBG("Enter");
    Evas_Object *genlist;

    genlist = elm_genlist_add(parent);
    if(NULL == genlist)
    {
        ERR("elm_genlist_add() Fail");
        return NULL;
    }

    elm_object_style_set(genlist , "dialogue");
    evas_object_size_hint_weight_set(genlist , EVAS_HINT_EXPAND , EVAS_HINT_EXPAND);
    evas_object_show(genlist);

    evas_object_smart_callback_add(genlist , "realized" , __gl_realized_cb , NULL);

    DBG("LEAVE");
    return genlist;
}

static Elm_Object_Item* _gl_append_item(oicappData *ad , Elm_Genlist_Item_Class *itc ,
        const char * title , Evas_Smart_Cb sel_func)
{
    DBG("Enter");
    Elm_Object_Item *item;
    oicappItemData *it_data;

    it_data = (oicappItemData *) calloc(1 , sizeof(oicappItemData));
    it_data->title = title;
    it_data->ad = ad;

    item = elm_genlist_item_append(ad->genlist , itc , it_data , NULL , ELM_GENLIST_ITEM_NONE ,
            sel_func , ad);

    DBG("LEAVE");
    return item;
}

static Elm_Object_Item* _gl_append_btn(oicappData *ad , Elm_Genlist_Item_Class *itc)
{
    DBG("Enter");
    Elm_Object_Item *item;

    item = elm_genlist_item_append(ad->genlist , itc , ad , NULL , ELM_GENLIST_ITEM_NONE , NULL ,
            NULL);

    elm_genlist_item_select_mode_set(item , ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

    DBG("LEAVE");
    return item;
}

static char* _gl_text_get(void *data , Evas_Object *obj , const char *part)
{
    DBG("Enter");
    oicappItemData *it_data = (oicappItemData *) data;

    DBG("title : %s" , it_data->title);

    DBG("Leave");
    return strdup(it_data->title);
}

static void oicapp_append_contents(oicappData *ad)
{
    DBG("Enter");
    Elm_Object_Item *item;

    _gl_append_item(ad , &ad->itcTitle , OICAPP_STR_NM , NULL);

    item = _gl_append_item(ad , &ad->itcText , OICAPP_STR_URI , NULL);
    ad->itemUri = item;

    item = _gl_append_item(ad , &ad->itcText , OICAPP_STR_HOST , NULL);
    ad->itemHost = item;

    _gl_append_btn(ad , &ad->itcBtnFindResource);

    item = _gl_append_item(ad , &ad->itcText , OICAPP_STR_TEMP , NULL);
    ad->itemTemp = item;

    item = _gl_append_item(ad , &ad->itcText , OICAPP_STR_HUMID , NULL);
    ad->itemHumid = item;

    item = _gl_append_item(ad , &ad->itcEdit , NULL , NULL);

    _gl_append_btn(ad , &ad->itcBtnApplyExtraStr);
    _gl_append_btn(ad , &ad->itcBtnStartHost);

    oicapp_append_separator(ad->genlist , ad);
    DBG("Leave");
}

static Evas_Object* oicapp_create_conform(Evas_Object *win)
{
    DBG("Enter");
    Evas_Object *conform = NULL;
    conform = elm_conformant_add(win);

    evas_object_size_hint_weight_set(conform , EVAS_HINT_EXPAND , EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win , conform);
    evas_object_show(conform);

    Evas_Object *bg_indicator = elm_bg_add(conform);
    elm_object_style_set(bg_indicator , "indicator/headerbg");
    elm_object_part_content_set(conform , "elm.swallow.indicator_bg" , bg_indicator);
    evas_object_show(bg_indicator);

    DBG("Leave");
    return conform;
}

static Eina_Bool _back_cb(void *data , Elm_Object_Item *item)
{
    DBG("Enter");
    elm_exit();
    DBG("Leave");
    return EINA_FALSE;
}

static void _update_resource(oicappData * ad , Elm_Object_Item * elm_item , const char* type ,
        const char* value)
{
    DBG("Enter");
    oicappItemData* it_data;
    char* temp;
    temp = (char*) calloc(100 , sizeof(char));
    strcpy(temp , type);
    strcat(temp , value);

    it_data = (oicappItemData*) calloc(1 , sizeof(oicappItemData));
    it_data->ad = ad;
    it_data->title = temp;

    DBG("it_data->title : %s" , it_data->title);

    elm_object_item_data_set(elm_item , it_data);
    elm_genlist_item_update(elm_item);
    DBG("LEAVE");
}
static void _btn_findresource_clicked(void *data , Evas_Object *obj , void *event_info)
{
    DBG("Enter");
    g_current_ad = (oicappData *) data;

    handle.s_findHostingCandidate();

    DBG("Leave");
}

static void _btn_starthost_clicked(void *data , Evas_Object *obj , void *event_info)
{
    DBG("Enter");
    g_current_ad = (oicappData *) data;

    handle.s_startHosting(g_curResource);

    DBG("Leave");
}

static void _btn_apply_clicked(void *data , Evas_Object *obj , void *event_info)
{
    DBG("Enter");
    g_current_ad = (oicappData *) data;

    if( g_current_ad->inputMsg != nullptr )
    {
        DBG("current_ad->input_msg : %s" , g_current_ad->inputMsg);
        char *tmp_s = strdup(g_current_ad->inputMsg);

        DBG("Onobserve_addstrTmp");
        handle.s_addExtraStr(tmp_s);
        DBG("Onobserve_addstr done");
    }

    DBG("Leave");
}

static Evas_Object* _gl_btn_findresource_content_get(void *data , Evas_Object *obj ,
        const char *part)
{
    DBG("Enter");
    Evas_Object *button;
    oicappData *ad = (oicappData *) data;

    button = elm_button_add(obj);
    elm_object_part_text_set(button , NULL , "Find Resource");
    evas_object_propagate_events_set(button , EINA_FALSE);
    evas_object_smart_callback_add(button , "clicked" , _btn_findresource_clicked , ad);

    DBG("Leave");
    return button;
}

static Evas_Object* _gl_btn_starthost_content_get(void *data , Evas_Object *obj , const char *part)
{
    DBG("Enter");
    Evas_Object *button;
    oicappData *ad = (oicappData *) data;

    button = elm_button_add(obj);
    elm_object_part_text_set(button , NULL , "Start Host");
    evas_object_propagate_events_set(button , EINA_FALSE);
    evas_object_smart_callback_add(button , "clicked" , _btn_starthost_clicked , ad);

    DBG("Leave");
    return button;
}

static Evas_Object* _gl_btn_applyEXstr_content_get(void *data , Evas_Object *obj , const char *part)
{
    DBG("Enter");
    Evas_Object *button;
    oicappData *ad = (oicappData *) data;

    button = elm_button_add(obj);
    elm_object_part_text_set(button , NULL , "APPLY");
    evas_object_propagate_events_set(button , EINA_FALSE);
    evas_object_smart_callback_add(button , "clicked" , _btn_apply_clicked , ad);

    DBG("Leave");
    return button;
}

static void _edit_unfocused(void *data , Evas_Object *obj , void *event_info)
{
    DBG("Enter");

    oicappData *ad = (oicappData *) data;
    DBG("ad : %p" , ad);
    ad->inputMsg = strdup(elm_entry_entry_get(obj));
    g_current_ad->inputMsg = strdup(elm_entry_entry_get(obj));
    DBG("Clicked : %s" , ad->inputMsg);
    DBG("LEAVE");
}
static Evas_Object* _gl_edit_content_get(void *data , Evas_Object *obj , const char *part)
{
    DBG("Enter");
    oicappData *ad = (oicappData *) data;
    DBG("ad : %p" , ad);
    Evas_Object *entry = ea_editfield_add(obj , EA_EDITFIELD_SINGLELINE);
    ea_editfield_clear_button_disabled_set(entry , EINA_TRUE);
    ad->inputMsg = nullptr;
    elm_entry_entry_set(entry , ad->inputMsg);
    evas_object_smart_callback_add(entry , "unfocused" , _edit_unfocused , ad);

    DBG("LEAVE");
    return entry;
}

static void _gl_item_del(void *data , Evas_Object *obj)
{
    DBG("Enter");
    if(data != NULL)
        free(data);
    DBG("LEAVE");
}

static inline void oicapp_init_itcs(oicappData *ad)
{
    DBG("Enter");
    ad->itcSeperator.item_style = "dialogue/separator";
    ad->itcSeperator.func.text_get = NULL;
    ad->itcSeperator.func.content_get = NULL;
    ad->itcSeperator.func.state_get = NULL;
    ad->itcSeperator.func.del = NULL;

    ad->itcTitle.item_style = "dialogue/title";
    ad->itcTitle.func.text_get = _gl_text_get;
    ad->itcTitle.func.content_get = NULL;
    ad->itcTitle.func.state_get = NULL;
    ad->itcTitle.func.del = _gl_item_del;

    ad->itcText.item_style = "dialogue/1text";
    ad->itcText.func.text_get = _gl_text_get;
    ad->itcText.func.content_get = NULL;
    ad->itcText.func.state_get = NULL;
    ad->itcText.func.del = _gl_item_del;

    ad->itcBtnFindResource.item_style = "dialogue/1icon";
    ad->itcBtnFindResource.func.text_get = NULL;
    ad->itcBtnFindResource.func.content_get = _gl_btn_findresource_content_get;
    ad->itcBtnFindResource.func.state_get = NULL;
    ad->itcBtnFindResource.func.del = NULL;

    DBG("edit before ad : %p" , ad);

    ad->itcEdit.item_style = "dialogue/editfield";
    ad->itcEdit.func.text_get = NULL;
    ad->itcEdit.func.content_get = _gl_edit_content_get;
    ad->itcEdit.func.state_get = NULL;
    ad->itcEdit.func.del = NULL;

    DBG("edit after ad : %p" , ad);

    ad->itcBtnApplyExtraStr.item_style = "dialogue/1icon";
    ad->itcBtnApplyExtraStr.func.text_get = NULL;
    ad->itcBtnApplyExtraStr.func.content_get = _gl_btn_applyEXstr_content_get;
    ad->itcBtnApplyExtraStr.func.state_get = NULL;
    ad->itcBtnApplyExtraStr.func.del = NULL;

    ad->itcBtnStartHost.item_style = "dialogue/1icon";
    ad->itcBtnStartHost.func.text_get = NULL;
    ad->itcBtnStartHost.func.content_get = _gl_btn_starthost_content_get;
    ad->itcBtnStartHost.func.state_get = NULL;
    ad->itcBtnStartHost.func.del = NULL;
    DBG("Leave");
}

static int oicapp_create(void *data)
{
    DBG("Enter");
    oicappData *ad = (oicappData *) data;
    Elm_Object_Item *it;

    oicapp_init_itcs(ad);

    // create window
    ad->win = oicapp_create_win(PACKAGE);
    if(NULL == ad->win)
        return -1;

    ad->bg = oicapp_create_bg(ad->win);
    if(NULL == ad->bg)
        return -1;

    ad->conform = oicapp_create_conform(ad->win);
    if(NULL == ad->conform)
        return -1;

    // create layout
    ad->base = oicapp_create_base_layout(ad->conform);
    if(NULL == ad->base)
        return -1;
    elm_object_content_set(ad->conform , ad->base);

    ad->navi = elm_naviframe_add(ad->base);
    elm_object_part_content_set(ad->base , "elm.swallow.content" , ad->navi);

    ea_object_event_callback_add(ad->navi , EA_CALLBACK_BACK , ea_naviframe_back_cb , NULL);

    ad->genlist = oicapp_create_genlist(ad->navi);

    it = elm_naviframe_item_push(ad->navi , "NM - OIC Tester" , NULL , NULL , ad->genlist , NULL);
    elm_naviframe_item_pop_cb_set(it , _back_cb , ad);

    ad->ipAddr = oicapp_util_wifi();
    if(NULL == ad->ipAddr)
    {
        ERR("wifi is not connected");
    }
    else
    {
        INFO("IP Address = %s" , ad->ipAddr);
    }
    oicapp_append_contents(ad);

    // initialize NotificationManager

    NotificationManager *Ptr = NotificationManager::getInstance();
    Ptr->initialize();
    Ptr->setOnFoundHostingCandidate(
            std::function< void(std::shared_ptr< OCResource > resource) >(
                    std::bind(&NMHandler::Found , handle , std::placeholders::_1)));
    Ptr->setOnObserve(
            std::function< void(AttributeMap &inputAttMap) >(
                    std::bind(&NMHandler::Observe , handle , std::placeholders::_1)));

    Ptr->setFindHosting(handle.s_findHostingCandidate);
    Ptr->setStartHosting(handle.s_startHosting);
    Ptr->setAddExtraStr(handle.s_addExtraStr);
    DBG("Leave");
    return 0;
}

static int oicapp_terminate(void *data)
{
    DBG("Enter");
    oicappData *ad = (oicappData *) data;

    DBG("Enter evas_object_del(ad->win)");
    if(ad->win)
        evas_object_del(ad->win);
    DBG("Leave evas_object_del(ad->win)");

    DBG("Enter ap->ip_addr");
    free(ad->ipAddr);
    DBG("Leave free(ad->ip_addr)");
    DBG("Enter free(ad)");
    DBG("Leave free(ad)");
    DBG("Leave");
    return 0;
}

static int oicapp_pause(void *data)
{
    DBG("Enter");
    DBG("Leave");
    return 0;
}

static int oicapp_resume(void *data)
{
    DBG("Enter");
    static int tmpCnt = 0;
    char buf[1024];

    snprintf(buf , sizeof(buf) , "Test %d" , tmpCnt++);

    DBG("Leave");
    return 0;
}

static int oicapp_reset(bundle *b , void *data)
{
    DBG("Enter");
    oicappData *ad = (oicappData *) data;

    if(ad->win)
        elm_win_activate(ad->win);

    DBG("Leave");
    return 0;
}

int main(int argc , char *argv[])
{
    oicappData ad;
    g_current_ad = &ad;

    struct appcore_ops ops;

    ops.create = oicapp_create;
    ops.terminate = oicapp_terminate;
    ops.pause = oicapp_pause;
    ops.resume = oicapp_resume;
    ops.reset = oicapp_reset;

    memset(&ad , 0x00 , sizeof(oicappData));
    ops.data = &ad;
    DBG("&ad : %p" , &ad);

    return appcore_efl_main(PACKAGE , &argc , &argv , &ops);
}
