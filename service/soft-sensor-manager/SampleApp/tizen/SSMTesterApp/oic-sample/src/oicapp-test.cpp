/*
 * Copyright (c) 2014 Samsung Electronics, Inc.
 * All rights reserved.
 *
 * This software is a confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

#include <appcore-efl.h>
#include <Ecore_X.h>
#include <efl_assist.h>

#include <string>
#include <sstream>

#include "oicapp-test.h"
#include "oicapp-utils.h"

#include "SSMInterface.h"

using namespace OIC;

static oicapp_data *g_ad = NULL;

int g_CQID = 9999;

class CQueryEngineEvent : public IQueryEngineEvent
{
    public:

        SSMRESULT onQueryEngineEvent(int cqid, IDataReader *pResult)
        {
            std::stringstream sstream;

            int     dataCount = 0;
            IModelData      *pModelData = NULL;
            std::vector<std::string>        affectedModels;

            pResult->getAffectedModels(&affectedModels);

            for (std::vector<std::string>::iterator itor = affectedModels.begin();
                 itor != affectedModels.end(); ++itor)
            {
                printf("Printing %s\n", itor->c_str());
                pResult->getModelDataCount(*itor, &dataCount);
                for (int i = 0; i < dataCount; i++)
                {
                    pResult->getModelData(*itor, i, &pModelData);
                    printf("dataId: %d\n", pModelData->getDataId());
                    for (int j = 0; j < pModelData->getPropertyCount(); j++)
                    {
                        sstream << "Type: " << pModelData->getPropertyName(j).c_str() <<
                                " Value: " << pModelData->getPropertyValue(j).c_str() << "<br>";
                    }
                }
            }

            sstream << std::ends;

            oicapp_util_put_msg(g_ad, sstream.str().c_str());

            return SSM_S_OK;
        }
};

CQueryEngineEvent   *g_SSMClientListener = new CQueryEngineEvent();

static Elm_Object_Item *oicapp_append_separator(Evas_Object *genlist,
        oicapp_data *ad)
{
    Elm_Object_Item *item = NULL;

    item = elm_genlist_item_append(genlist, &ad->itc_seperator, NULL, NULL,
                                   ELM_GENLIST_ITEM_NONE, NULL, NULL);
    //elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

    return item;
}

static void _win_del(void *data, Evas_Object *obj, void *event)
{
    DBG("test _win_del()");

    elm_exit();
}

static Evas_Object *oicapp_create_win(const char *name)
{
    Evas_Object *eo;
    int w, h;

    eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
    if (eo)
    {
        elm_win_title_set(eo, name);
        elm_win_borderless_set(eo, EINA_TRUE);
        evas_object_smart_callback_add(eo, "delete,request", _win_del, NULL);
        ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
        evas_object_resize(eo, w, h);
        elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
        elm_win_indicator_opacity_set(eo, ELM_WIN_INDICATOR_OPAQUE);
    }

    evas_object_show(eo);

    return eo;
}

static Evas_Object *oicapp_create_bg(Evas_Object *parent)
{
    Evas_Object *bg;

    bg = elm_bg_add(parent);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(parent, bg);
    //elm_object_part_content_set(parent, "elm.swallow.bg", bg);
    evas_object_show(bg);

    return bg;
}

static Evas_Object *oicapp_create_base_layout(Evas_Object *parent)
{
    Evas_Object *base;

    base = elm_layout_add(parent);
    elm_layout_theme_set(base, "layout", "application", "default");
    evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(base);

    return base;
}

void __gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Object_Item *item = (Elm_Object_Item *)event_info;
    /*
        list_item = elm_object_item_data_get(item);
        if (NULL == list_item)
        {
            elm_object_item_signal_emit(item, "elm,state,center", "");
            return;
        }
        if (list_item->group_style == OICAPP_GENLIST_GRP_TOP)
            elm_object_item_signal_emit(item, "elm,state,top", "");
        else if (list_item->group_style == OICAPP_GENLIST_GRP_CENTER)
            elm_object_item_signal_emit(item, "elm,state,center", "");
        else if (list_item->group_style == OICAPP_GENLIST_GRP_BOTTOM)
            elm_object_item_signal_emit(item, "elm,state,bottom", "");
        else
    */
    elm_object_item_signal_emit(item, "elm,state,normal", "");
}


static Evas_Object *oicapp_create_genlist(Evas_Object *parent)
{
    Evas_Object *genlist;

    genlist = elm_genlist_add(parent);
    if (NULL == genlist)
    {
        ERR("elm_genlist_add() Fail");
        return NULL;
    }

    elm_object_style_set(genlist, "dialogue");
    elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
    evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show(genlist);

    evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb, NULL);

    return genlist;
}

static Elm_Object_Item *_gl_append_item(oicapp_data *ad, Elm_Genlist_Item_Class *itc)
{
    Elm_Object_Item *item;

    item = elm_genlist_item_append(ad->genlist, itc, ad, NULL,
                                   ELM_GENLIST_ITEM_NONE, NULL, NULL);

    elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

    return item;
}

static void oicapp_append_contents(oicapp_data *ad)
{
    Elm_Object_Item *item;
    /*
    oicapp_append_separator(ad->genlist, ad);

    _gl_append_item(ad, &ad->itc_edit);

    oicapp_append_separator(ad->genlist, ad);

    _gl_append_item(ad, &ad->itc_btn);

    oicapp_append_separator(ad->genlist, ad);

    item = _gl_append_item(ad, &ad->itc_multiline);
    ad->item_multiline = item;

    oicapp_append_separator(ad->genlist, ad);
    */
    oicapp_append_separator(ad->genlist, ad);

    elm_genlist_item_append(ad->genlist, &ad->itc_edit, ad, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

    oicapp_append_separator(ad->genlist, ad);

    elm_genlist_item_append(ad->genlist, &ad->itc_btn, ad, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

    oicapp_append_separator(ad->genlist, ad);

    oicapp_append_separator(ad->genlist, ad);

    oicapp_append_separator(ad->genlist, ad);

    item = _gl_append_item(ad, &ad->itc_multiline);

    ad->item_multiline = item;

    oicapp_append_separator(ad->genlist, ad);
}

static Evas_Object *oicapp_create_conform(Evas_Object *win)
{
    Evas_Object *conform = NULL;
    conform = elm_conformant_add(win);

    evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(win, conform);
    evas_object_show(conform);

    Evas_Object *bg_indicator = elm_bg_add(conform);
    elm_object_style_set(bg_indicator, "indicator/headerbg");
    elm_object_part_content_set(conform, "elm.swallow.indicator_bg", bg_indicator);
    evas_object_show(bg_indicator);

    return conform;
}

static Eina_Bool _back_cb(void *data, Elm_Object_Item *item)
{
    elm_exit();

    return EINA_FALSE;
}

static char *_gl_multiline_text_get(void *data, Evas_Object *obj, const char *part)
{
    oicapp_data *ad = (oicapp_data *)data;

    if (ad->output_msg)
        return strdup(ad->output_msg);
    else
        return strdup("");
}

static void _btn_clicked(void *data, Evas_Object *obj, void *event_info)
{
    oicapp_data *ad = (oicapp_data *)data;

    std::string str = std::string(ad->input_msg);

    std::string strGT = "&gt;";

    std::string strLT = "&lt;";

    std::size_t foundGT = str.find(strGT);

    std::size_t foundLT = str.find(strLT);

    std::stringstream sstream;

    if (foundGT != std::string::npos)
    {
        str.replace(foundGT, strGT.length(), ">");
    }

    if (foundLT != std::string::npos)
    {
        str.replace(foundLT, strLT.length(), "<");
    }

    RegisterQuery(str, g_SSMClientListener, g_CQID);

    sstream << "Query executed! cqid = " << g_CQID << std::ends;

    oicapp_util_put_msg(ad, sstream.str().c_str());

    ERR("button clicked(%s)", ad->input_msg);
}

static Evas_Object *_gl_btn_content_get(void *data, Evas_Object *obj,
                                        const char *part)
{
    Evas_Object *button;
    oicapp_data *ad = (oicapp_data *)data;

    button = elm_button_add(obj);
    //elm_object_style_set(button, "");
    elm_object_part_text_set(button, NULL, "Send Query");
    evas_object_propagate_events_set(button, EINA_FALSE);
    evas_object_smart_callback_add(button, "clicked", _btn_clicked, ad);

    return button;
}

static void _edit_unfocused(void *data, Evas_Object *obj, void *event_info)
{
    oicapp_data *ad = (oicapp_data *)data;

    ad->input_msg = strdup(elm_entry_entry_get(obj));

    DBG("Clicked : %s", ad->input_msg);

}
static Evas_Object *_gl_edit_content_get(void *data, Evas_Object *obj,
        const char *part)
{
    oicapp_data *ad = (oicapp_data *)data;

    Evas_Object *entry = ea_editfield_add(obj, EA_EDITFIELD_MULTILINE);
    ea_editfield_clear_button_disabled_set(entry, EINA_TRUE);
    elm_entry_entry_set(entry, ad->input_msg);
    evas_object_smart_callback_add(entry, "unfocused", _edit_unfocused, ad);

    return entry;
}

static char *_item_Text_get(void *data, Evas_Object *obj, const char *part)
{
    char *buf =
        "subscribe Device.DiscomfortIndexSensor if Device.DiscomfortIndexSensor.discomfortIndex > 0";

    return strdup(buf);
}

static inline void oicapp_init_itcs(oicapp_data *ad)
{
    ad->itc_seperator.item_style = "dialogue/separator";
    ad->itc_seperator.func.text_get = NULL;
    ad->itc_seperator.func.content_get = NULL;
    ad->itc_seperator.func.state_get = NULL;
    ad->itc_seperator.func.del = NULL;

    ad->itc_edit.item_style = "dialogue/editfield";
    ad->itc_edit.func.text_get = _item_Text_get;
    ad->itc_edit.func.content_get = _gl_edit_content_get;
    ad->itc_edit.func.state_get = NULL;
    ad->itc_edit.func.del = NULL;

    ad->itc_btn.item_style = "dialogue/1icon";
    ad->itc_btn.func.text_get = NULL;
    ad->itc_btn.func.content_get = _gl_btn_content_get;
    ad->itc_btn.func.state_get = NULL;
    ad->itc_btn.func.del = NULL;

    ad->itc_multiline.item_style = "multiline/1text";
    ad->itc_multiline.func.text_get = _gl_multiline_text_get;
    ad->itc_multiline.func.content_get = NULL;
    ad->itc_multiline.func.state_get = NULL;
    ad->itc_multiline.func.del = NULL;
}

static int oicapp_create(void *data)
{
    oicapp_data *ad = (oicapp_data *)data;
    Elm_Object_Item *it;

    g_ad = (oicapp_data *)data;

    oicapp_init_itcs(ad);

    /* create window */
    ad->win = oicapp_create_win(PACKAGE);
    if (NULL == ad->win)
        return -1;

    ad->bg = oicapp_create_bg(ad->win);
    if (NULL == ad->bg)
        return -1;

    ad->conform = oicapp_create_conform(ad->win);
    if (NULL == ad->conform)
        return -1;

    /* create layout */
    ad->base = oicapp_create_base_layout(ad->conform);
    if (NULL == ad->base)
        return -1;
    elm_object_content_set(ad->conform, ad->base);

    ad->navi = elm_naviframe_add(ad->base);
    elm_object_part_content_set(ad->base, "elm.swallow.content", ad->navi);
    ea_object_event_callback_add(ad->navi, EA_CALLBACK_BACK, ea_naviframe_back_cb, NULL);

    ad->genlist = oicapp_create_genlist(ad->navi);

    it = elm_naviframe_item_push(ad->navi, "SSM Tester", NULL, NULL, ad->genlist, NULL);
    elm_naviframe_item_pop_cb_set(it, _back_cb, ad);

    ad->ip_addr = oicapp_util_wifi();
    if (NULL == ad->ip_addr)
    {
        ERR("wifi is not connected");
        oicapp_fail_popup(ad, (char *)"Error", (char *)"No WIFI connection", 3);
    }
    else
    {
        INFO("IP Address = %s", ad->ip_addr);
    }

    oicapp_append_contents(ad);

    return 0;
}

static int oicapp_terminate(void *data)
{
    oicapp_data *ad = (oicapp_data *)data;

    if (g_CQID != 9999)
        UnregisterQuery(g_CQID);

    if (ad->win)
        evas_object_del(ad->win);

    free(ad->ip_addr);
    free(ad->output_msg);
    return 0;
}

static int oicapp_pause(void *data)
{
    return 0;
}

static int oicapp_resume(void *data)
{
    /*
    static int tmpCnt = 0;
    oicapp_data *ad = (oicapp_data *)data;
    char buf[1024];

    snprintf(buf, sizeof(buf), "Test %d", tmpCnt++);

    oicapp_util_put_msg(ad, buf);
    */
    return 0;
}

static int oicapp_reset(bundle *b, void *data)
{
    oicapp_data *ad = (oicapp_data *)data;

    if (ad->win)
        elm_win_activate(ad->win);

    return 0;
}

int main(int argc, char *argv[])
{
    oicapp_data ad;

    /*
    struct appcore_ops ops = {
        .create = oicapp_create,
        .terminate = oicapp_terminate,
        .pause = oicapp_pause,
        .resume = oicapp_resume,
        .reset = oicapp_reset,
    };*/

    appcore_ops ops;

    ops.create = oicapp_create;
    ops.terminate = oicapp_terminate;
    ops.pause = oicapp_pause;
    ops.resume = oicapp_resume;
    ops.reset = oicapp_reset;

    memset(&ad, 0x0, sizeof(oicapp_data));
    ops.data = &ad;

    return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}


