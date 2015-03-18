#include <tizen.h>
#include <wifi.h>
#include <net_connection.h>
#include <algorithm>
#include "string.h"
#include "ssmtesterapp.h"
#include "SSMInterface.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *naviframe;
	Evas_Object *layout;
	Evas_Object *scroller;
	Evas_Object *box;

	Evas_Object *query_text;
	Evas_Object *log;
	Evas_Object *unregister_query_id;

	Evas_Object *register_button;
	Evas_Object *clear_button;
	Evas_Object *unregister_button;
	Evas_Object *search_devices_button;
	Evas_Object *discomfort_index_button;
	Evas_Object *plus_button;
	Evas_Object *minus_button;
	Evas_Object *clear_log_button;

	Evas_Object *example_queries_label;
	Evas_Object *unregister_query_label;
} appdata_s;

typedef struct threadContext{
	appdata_s *ad;
	const char *log;
} threadContext_s;

#define ELM_DEMO_EDJ "opt/usr/apps/org.iotivity.service.ssm.ssmtesterapp/res/ui_controls.edj"

char log_buffer[10000];
void updateLog(appdata_s *ad, const char *newlog)
{
	const char *log_text = NULL;

	log_text = elm_entry_entry_get(ad->log);
	strcpy(log_buffer,log_text);
	strcat(log_buffer,newlog);
	elm_entry_entry_set(ad->log,log_buffer);
	elm_entry_cursor_end_set(ad->log);
}

void* updateCallbackLog(void *data)
{
	threadContext_s  *pThreadContext = (threadContext_s*)data;

	updateLog(pThreadContext->ad, pThreadContext->log);

	return NULL;
}

static int s_hc2i(char hexChar)
{
    if ((hexChar <= '9') && (hexChar >= '0')) return (int)hexChar-(int)'0';
    if ((hexChar <= 'F') && (hexChar >= 'A')) return 10+(int)hexChar-(int)'A';
    if ((hexChar <= 'f') && (hexChar >= 'a')) return 10+(int)hexChar-(int)'a';
    return -1;
}

long long s_h2uint64(const char *hex, size_t maxdigit)
{
    long long nNumber = 0;
    int nInt;
    size_t nLen=0;
    while (nLen < maxdigit && (nInt = s_hc2i(*hex)) >= 0) {
        nNumber=(nNumber*(long long)16) + (long long)nInt;
        hex++;
        nLen++;
    }
    return nNumber;
}

int s_h2i(char *strHex) {
    return (int) s_h2uint64(strHex, sizeof(int)*2);
}

void unescape(char *val)
{
    char *tmp = (char *)val;
    int i,nChar;
    int len;

    if (!val) return;
    len=(int)strlen(val);

    // process special chars
    while (*tmp) {
        if (!strncmp((char *)tmp,"&amp;",5)) {
            *tmp='&';
            memmove(&tmp[1],&tmp[5],strlen((char *)&tmp[5])+1);
        }
        else if (!strncmp((char *)tmp,"&nbsp;",6)) {
            *tmp=' ';
            memmove(&tmp[1],&tmp[6],strlen((char *)&tmp[6])+1);
        }
        else if (!strncmp((char *)tmp,"&lt;",4)) {
            *tmp='<';
            memmove(&tmp[1],&tmp[4],strlen((char *)&tmp[4])+1);
        }
        else if (!strncmp((char *)tmp,"&gt;",4)) {
            *tmp='>';
            memmove(&tmp[1],&tmp[4],strlen((char *)&tmp[4])+1);
        }
        else if (!strncmp((char *)tmp,"&apos;",6)) {
            *tmp='\'';
            memmove(&tmp[1],&tmp[6],strlen((char *)&tmp[6])+1);
        }
        else if (!strncmp((char *)tmp,"&quot;",6)) {
            *tmp='"';
            memmove(&tmp[1],&tmp[6],strlen((char *)&tmp[6])+1);
        }
        else if (!strncmp((char *)tmp,"&#",2)) { // &#nnn; || &#xnn;
            for(i=2;i<7;i++) {
                if (tmp[i] == ';') {
                    if (tmp[2]=='x') nChar=s_h2i((char *)&tmp[3]);
                    else nChar=atoi((char *)&tmp[2]);
                    *tmp=(char)nChar;
                    memmove(&tmp[1],&tmp[i+1],strlen((char *)&tmp[i+1])+1);
                    break;
                }
            }
        }
        tmp++;
    }

    if (tmp < (char *)(val+len))
        *tmp='\0';
}

class CQueryEngineEvent : public OIC::IQueryEngineEvent
{
private:
	appdata_s *m_pAppData;
	threadContext_s m_ThreadContext;

public:
	CQueryEngineEvent(appdata_s *pAppData)
	{
		m_pAppData = pAppData;
		m_ThreadContext.ad = m_pAppData;
		m_ThreadContext.log = NULL;
	}

	OIC::SSMRESULT onQueryEngineEvent(int cqid, OIC::IDataReader *pResult)
	{
		int     dataCount = 0;
		char log[2000],buf[100];
		OIC::IModelData      *pModelData = NULL;
		std::vector<std::string>        affectedModels;

		dlog_print(DLOG_ERROR,LOG_TAG,"Event received!");

		sprintf(buf,"Event received! cqid = %d<br>", cqid);
		strcpy(log,buf);

		pResult->getAffectedModels(&affectedModels);

		for (std::vector<std::string>::iterator itor = affectedModels.begin();
				itor != affectedModels.end(); ++itor)
		{
			sprintf(buf,"Printing = %s model<br>", (*itor).c_str());
			strcat(log,buf);

			pResult->getModelDataCount(*itor, &dataCount);
			for (int i = 0; i < dataCount; i++)
			{
				pResult->getModelData(*itor, i, &pModelData);
				sprintf(buf,"dataId: %d<br>", pModelData->getDataId());
				strcat(log,buf);
				for (int j = 0; j < pModelData->getPropertyCount(); j++)
				{
					sprintf(buf,"Type: %s Value: %s<br>", (pModelData->getPropertyName(j)).c_str(), (pModelData->getPropertyValue(j)).c_str());
					strcat(log,buf);
				}
				m_ThreadContext.log = log;
				ecore_main_loop_thread_safe_call_sync((void *(*)(void *))updateCallbackLog, &m_ThreadContext);
			}
		}

		return OIC::SSM_S_OK;
	}
};

CQueryEngineEvent		*g_pQueryEngineEvent = NULL;


static void
win_delete_request_cb(void *data , Evas_Object *obj , void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	//appdata_s *ad = (appdata_s *)data;
	/* Let window go to hide state. */
	//elm_win_lower(ad->win);
	ui_app_exit();
}


static void
register_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	const char *main_text = NULL;
	char *escaped_text = NULL;
	if (!ad->query_text)
		return;
	main_text = elm_entry_entry_get(ad->query_text);
	if (!main_text || (strlen(main_text) == 0))
		return;

	//invoke registerQuery
	int qid = 0;
	char log[50];

	escaped_text = strdup(main_text);
	unescape(escaped_text);

    dlog_print(DLOG_ERROR,LOG_TAG,"registering query");

	OIC::SSMRESULT res = OIC::RegisterQuery(escaped_text, g_pQueryEngineEvent, qid);
	if(res == OIC::SSM_S_OK)
	{
		updateLog(ad, "The query has been registered!<br>");
		sprintf(log, "QID : %d<br>", qid);
		updateLog(ad, log);
	}
	else
	{
		sprintf(log,"Error occured(%d)<br>", res);
		updateLog(ad, log);
	}

	free(escaped_text);

	dlog_print(DLOG_ERROR,LOG_TAG,"registering query done");
}

static void
search_devices_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	elm_entry_entry_set(ad->query_text, "subscribe Device if Device.dataId > 0");
}

static void
discomfort_index_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	elm_entry_entry_set(ad->query_text, "subscribe Device.DiscomfortIndexSensor "\
			"if Device.DiscomfortIndexSensor.discomfortIndex != 0");
}

static void
minus_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	char output[10];
	const char *query_id_str = elm_entry_entry_get(ad->unregister_query_id);
	if (!query_id_str || (strlen(query_id_str) == 0))
		return;
	try{
		int val = atoi(query_id_str);
		sprintf(output,"%d",val-1);
		elm_entry_entry_set(ad->unregister_query_id,output);
	}
	catch(...){
		dlog_print(DLOG_ERROR,LOG_TAG,"#### atoi() conversion error");
	}
}

static void
plus_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	char output[10];
	const char *query_id_str = elm_entry_entry_get(ad->unregister_query_id);
	if (!query_id_str || (strlen(query_id_str) == 0))
		return;
	try{
		int val = atoi(query_id_str);
		sprintf(output,"%d",val+1);
		elm_entry_entry_set(ad->unregister_query_id,output);
	}
	catch(...){
		dlog_print(DLOG_ERROR,LOG_TAG,"#### atoi() conversion error");
	}
}

static void
unregister_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	const char *qidstr = NULL;
	if (!ad->unregister_query_id)
		return;
	qidstr = elm_entry_entry_get(ad->unregister_query_id);
	if (!qidstr || (strlen(qidstr) == 0))
		return;
	char log[50];
	int qid;
	try{
		qid = atoi(qidstr);
		//invoke unregisterQuery
		OIC::SSMRESULT res = OIC::UnregisterQuery(qid);
		if(res == OIC::SSM_S_OK)
		{
			updateLog(ad, "The query has been unregistered!<br>");
			sprintf(log, "QID : %d<br>", qid);
			updateLog(ad, log);
		}
		else
		{
			sprintf(log,"Error occured(%d)<br>", res);
			updateLog(ad, log);
		}
	}
	catch(...){
		dlog_print(DLOG_ERROR,LOG_TAG,"#### atoi() conversion error");
	}
}

static void
clear_log_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	elm_entry_entry_set(ad->log,"");
}

static void
clear_cb(void *data , Evas_Object *obj , void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	elm_entry_entry_set(ad->query_text,"");
}

bool is_connected()
{
	bool isConnected = false;

	static connection_h connection;

	if(connection_create(&connection) == CONNECTION_ERROR_NONE)
	{
	    char *ip_addr = NULL;
	    connection_get_ip_address(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &ip_addr);
	    connection_destroy(connection);
	    if(strlen(ip_addr) > 0)
	        isConnected = true;
	    free(ip_addr);
	}

	return isConnected;
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ad->win, EINA_TRUE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* naviframe */
	ad->naviframe = elm_naviframe_add(ad->conform);
	elm_object_content_set(ad->conform,ad->naviframe);
	evas_object_show(ad->naviframe);

	/* scroller */
	ad->scroller = elm_scroller_add(ad->naviframe);

	/* Box container */
	ad->box = elm_box_add(ad->scroller);
	evas_object_size_hint_align_set(ad->box, EVAS_HINT_FILL, 0.0);
	evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND, 0.0);
	elm_object_content_set(ad->scroller, ad->box);

	ad->layout = elm_layout_add(ad->box);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, 0.0);
	//elm_layout_theme_set(ad->layout, "layout", "application", "default");
	evas_object_size_hint_align_set(ad->layout, EVAS_HINT_FILL, 0.0);
	elm_layout_file_set(ad->layout,ELM_DEMO_EDJ,"mainpage_layout");

	ad->query_text = elm_entry_add(ad->layout);
	elm_object_part_text_set(ad->query_text, "elm.guide", "Enter the query here!!!");
	evas_object_size_hint_align_set(ad->query_text, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(ad->query_text, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(ad->layout,"query_text",ad->query_text);
	elm_entry_scrollable_set(ad->query_text,EINA_TRUE);
	//evas_object_show(ad->query_text);
	elm_entry_input_panel_show_on_demand_set(ad->query_text,EINA_TRUE);

	ad->register_button = elm_button_add(ad->layout);
	elm_object_text_set(ad->register_button, "Register");
	evas_object_size_hint_weight_set(ad->register_button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->register_button, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(ad->register_button, "clicked", register_cb, ad);
	elm_object_part_content_set(ad->layout,"register",ad->register_button);
	//evas_object_show(ad->register_button);

	ad->clear_button = elm_button_add(ad->layout);
	elm_object_text_set(ad->clear_button, "Clear");
	evas_object_size_hint_weight_set(ad->search_devices_button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->search_devices_button, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(ad->clear_button, "clicked", clear_cb, ad);
	elm_object_part_content_set(ad->layout,"clear",ad->clear_button);
	//evas_object_show(ad->clear_button);

	ad->example_queries_label = elm_label_add(ad->layout);
	elm_object_text_set(ad->example_queries_label, _("<color=#00008BFF><b><align=left>Example Queries</align></b></color>"));
	elm_object_part_content_set(ad->layout,"example_queries",ad->example_queries_label);
	//evas_object_show(ad->example_queries_label);

	ad->search_devices_button = elm_button_add(ad->layout);
	//evas_object_size_hint_align_set(ad->search_devices_button, EVAS_HINT_FILL, 0.0);
	elm_object_text_set(ad->search_devices_button, "Search Devices");
	evas_object_smart_callback_add(ad->search_devices_button, "clicked", search_devices_cb, ad);
	elm_object_part_content_set(ad->layout,"search_devices",ad->search_devices_button);
	//evas_object_show(ad->search_devices_button);

	ad->discomfort_index_button = elm_button_add(ad->layout);
	//evas_object_size_hint_align_set(ad->discomfort_index_button, EVAS_HINT_FILL, 0.0);
	elm_object_part_content_set(ad->layout,"discomfort_index",ad->discomfort_index_button);
	elm_object_text_set(ad->discomfort_index_button, "Discomfort Index");
	evas_object_smart_callback_add(ad->discomfort_index_button, "clicked", discomfort_index_cb, ad);
	//evas_object_show(ad->discomfort_index_button);

	ad->unregister_query_label = elm_label_add(ad->layout);
	elm_object_text_set(ad->unregister_query_label,_("<color=#00008BFF><b><align=left>Unregister Query</align></b></color>"));
	//evas_object_size_hint_weight_set(ad->unregister_query_label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(ad->layout,"unregister_query_label",ad->unregister_query_label);
	//evas_object_show(ad->unregister_query_label);


	ad->minus_button = elm_button_add(ad->layout);
	//evas_object_size_hint_align_set(ad->discomfort_index_button, EVAS_HINT_FILL, 0.0);
	elm_object_part_content_set(ad->layout,"minus",ad->minus_button);
	elm_object_text_set(ad->minus_button, "-");
	evas_object_smart_callback_add(ad->minus_button, "clicked", minus_cb, ad);
	//evas_object_show(ad->minus_button);

	ad->unregister_query_id = elm_entry_add(ad->layout);
	elm_object_part_text_set(ad->unregister_query_id, "elm.guide", "Query id!!!");
	//evas_object_size_hint_align_set(ad->unregister_query_id, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_part_content_set(ad->layout,"unregister_query_id",ad->unregister_query_id);
	elm_entry_input_panel_show_on_demand_set(ad->unregister_query_id,EINA_TRUE);
	elm_entry_input_panel_layout_set(ad->unregister_query_id,ELM_INPUT_PANEL_LAYOUT_NUMBER);
	evas_object_show(ad->unregister_query_id);

	ad->plus_button = elm_button_add(ad->layout);
	//evas_object_size_hint_align_set(ad->discomfort_index_button, EVAS_HINT_FILL, 0.0);
	elm_object_part_content_set(ad->layout,"plus",ad->plus_button);
	elm_object_text_set(ad->plus_button, "+");
	evas_object_smart_callback_add(ad->plus_button, "clicked", plus_cb, ad);
	//evas_object_show(ad->plus_button);

	ad->unregister_button = elm_button_add(ad->layout);
	//evas_object_size_hint_align_set(ad->unregister_button, EVAS_HINT_FILL, 0.0);
	elm_object_part_content_set(ad->layout,"unregister_button",ad->unregister_button);
	elm_object_text_set(ad->unregister_button, "Unregister");
	evas_object_smart_callback_add(ad->unregister_button, "clicked", unregister_cb, ad);
	//evas_object_show(ad->unregister_button);

	ad->clear_log_button = elm_button_add(ad->layout);
	//evas_object_size_hint_align_set(ad->clear_log_button, EVAS_HINT_FILL, 0.0);
	elm_object_part_content_set(ad->layout,"log_button",ad->clear_log_button);
	elm_object_text_set(ad->clear_log_button, "Clear Log");
	evas_object_smart_callback_add(ad->clear_log_button, "clicked", clear_log_cb, ad);
	//evas_object_show(ad->clear_log_button);

	ad->log = elm_entry_add(ad->layout);
	elm_object_part_content_set(ad->layout,"log_text",ad->log);
	elm_object_part_text_set(ad->log, "elm.guide", "Log messages will be update here!!!");
	elm_entry_scrollable_set(ad->log,EINA_TRUE);
	elm_entry_editable_set(ad->log,EINA_FALSE);
	//evas_object_size_hint_align_set(ad->log, EVAS_HINT_FILL, EVAS_HINT_FILL);
	//evas_object_show(ad->log);

	elm_box_pack_end(ad->box, ad->layout);
	evas_object_show(ad->layout);
	elm_naviframe_item_push(ad->naviframe, "Soft Sensor Manager App", NULL, NULL, ad->scroller, NULL);

	//Show window after base gui is set up
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = (appdata_s *)data;

	std::string xmlDescription = "<SSMCore>"
				"<Device>"
				"<UDN>abcde123-31f8-11b4-a222-08002b34c050</UDN>"
				"<Name>MyPC</Name>"
				"<Type>PC</Type>"
				"</Device>"
				"<Config>"
				"<SoftSensorDescription>/opt/usr/apps/org.iotivity.service.ssm.ssmtesterapp/lib/SoftSensorDescription.xml</SoftSensorDescription>"
				"<SoftSensorRepository>/opt/usr/apps/org.iotivity.service.ssm.ssmtesterapp/lib/</SoftSensorRepository>"
				"</Config>"
				"</SSMCore>";

	g_pQueryEngineEvent = new CQueryEngineEvent(ad);

	create_base_gui(ad);

	if(is_connected())
	{
	    if (OIC::InitializeSSM(xmlDescription) == OIC::SSM_S_OK){
	            dlog_print(DLOG_DEBUG, LOG_TAG, "#### InitializeSSM() returned SSM_S_OK");
	        }
	        else{
	            dlog_print(DLOG_DEBUG, LOG_TAG, "#### InitializeSSM() failed");
	        }
	}
	else
	{
	    updateLog(ad, "WiFi not connected, connect first and re-run application");

	}

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
	dlog_print(DLOG_INFO,LOG_TAG,"#### in app_control");
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
	dlog_print(DLOG_INFO,LOG_TAG,"#### in app_pause");
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
	dlog_print(DLOG_INFO,LOG_TAG,"#### in app_resume");
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	dlog_print(DLOG_INFO,LOG_TAG,"#### in app_terminate");

	if (OIC::TerminateSSM() == OIC::SSM_S_OK){
		dlog_print(DLOG_DEBUG, LOG_TAG, "#### TerminateSSM() returned SSM_S_OK");
	}
	else{
		dlog_print(DLOG_DEBUG, LOG_TAG, "#### TerminateSSM() failed");
	}

	delete g_pQueryEngineEvent;
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	dlog_print(DLOG_INFO,LOG_TAG,"#### app orient changed");
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ad);

	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
