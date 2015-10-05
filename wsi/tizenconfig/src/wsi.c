#include <tizen.h>
#include "wsi.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *ewk_view;
	Evas_Object *back_button;
	Evas_Object *forward_button;
} appdata_s;

char *url_from_user_input(const char *arg)
{
   // If it is already a URL, return the argument as is
   if (has_scheme(arg) || !strcasecmp(arg, "about:blank"))
      return strdup(arg);

   Eina_Strbuf *buf = eina_strbuf_manage_new(eina_file_path_sanitize(arg));

   // Check whether the path exists
   if (ecore_file_exists(eina_strbuf_string_get(buf)))
   {
      // File exists, convert local path to a URL
      eina_strbuf_prepend(buf, "file://");
   }
   else
   {
      eina_strbuf_string_free(buf);
      eina_strbuf_append_printf(buf, "http://%s", arg);
   }
   char *url = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);

   return url;
}


Eina_Bool has_scheme(const char *url)
{
   return !!strstr(url, "://");
}

static appdata_s *window_find_with_elm_window(Evas_Object *elm_window)
{
   Eina_List *l;
   void *data;

   if (!elm_window)
      return NULL;

   EINA_LIST_FOREACH(windows, l, data)
   {
      appdata_s *a = (appdata_s *)data;
      if (a->win == elm_window)
         return a;
   }

   return NULL;
}

static appdata_s *window_find_with_ewk_view(Evas_Object *ewk_view)
{
   Eina_List *l;
   void *data;

   if (!ewk_view)
      return NULL;

   EINA_LIST_FOREACH(windows, l, data)
   {
      appdata_s *a = (appdata_s *)data;
      if (a->ewk_view == ewk_view)
         return a;
   }

   return NULL;
}

static void view_focus_set(appdata_s *ad, Eina_Bool focus)
{
   // We steal focus away from elm focus model and start to do things
   // manually here, so elm now has no clue what is up. Tell elm that its
   // top level UI component is to be unfocused so elm gives up the focus
   elm_object_focus_set(elm_object_top_widget_get(ad->ein), EINA_FALSE);
   evas_object_focus_set(ad->ewk_view, focus);
};

static void on_key_down(void *user_data, Evas *e, Evas_Object *ewk_view, void *event_info)
{
	appdata_s *ad = (appdata_s *)user_data;
	Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*) event_info;

	const Evas_Modifier *mod = evas_key_modifier_get(e);
	Eina_Bool ctrlPressed = evas_key_modifier_is_set(mod, "Control");
	Eina_Bool altPressed = evas_key_modifier_is_set(mod, "Alt");
}

static void on_mouse_down(void *user_data, Evas *e, Evas_Object *ewk_view, void *event_info)
{
	appdata_s *ad = (appdata_s *)user_data;
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;

   if (ev->button == 1)
      view_focus_set(ad, EINA_TRUE);
}


static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ewk_shutdown();
	ui_app_exit();
}


static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	Evas_Object *win, *bx, *bx2, *bt, *web, *url;
	elm_need_web();


	ad->win = elm_win_add(NULL, "minibrowser-window", ELM_WIN_BASIC);
	win = ad->win;
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}


	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	// Create vertical layout
	Evas_Object *vertical_layout = elm_box_add(ad->win);
	elm_box_padding_set(vertical_layout, 0, 2);
	evas_object_size_hint_weight_set(vertical_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, vertical_layout);
	evas_object_show(vertical_layout);

	// Create horizontal layout for top bar
	Evas_Object *horizontal_layout = elm_box_add(ad->win);
	elm_box_horizontal_set(horizontal_layout, EINA_TRUE);
	evas_object_size_hint_weight_set(horizontal_layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(horizontal_layout, EVAS_HINT_FILL, 0.0);
	elm_box_pack_end(vertical_layout, horizontal_layout);
	evas_object_show(horizontal_layout);

	Evas *evas = evas_object_evas_get(ad->win);
	window->ewk_view = ewk_view_add(evas);
	ewk_view_user_agent_set(ad->ewk_view, user_agent_string);
	evas_object_event_callback_add(window->ewk_view, EVAS_CALLBACK_KEY_DOWN, on_key_down, ad);
	evas_object_event_callback_add(window->ewk_view, EVAS_CALLBACK_MOUSE_DOWN, on_mouse_down, ad);



	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
}

static void
app_pause(void *data)
{
}

static void
app_resume(void *data)
{
}

static void
app_terminate(void *data)
{
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
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
