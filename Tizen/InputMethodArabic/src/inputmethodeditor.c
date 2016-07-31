#include <stdlib.h>
#include <Elementary.h>
#include <dlog.h>
#include <inputmethod.h>
#include "inputmethodeditor.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "INPUTMETHODEDITOR"

static void ime_app_create_cb(void *user_data);
static void ime_app_terminate_cb(void *user_data);
static void ime_app_show_cb(int ic, ime_context_h context, void *user_data);
static void ime_app_hide_cb(int ic, void *user_data);

static void ime_app_cursor_position_updated_cb(int cursor_pos, void *user_data);
static void ime_app_focus_out_cb(int ic, void *user_data);
static void ime_app_focus_in_cb(int ic, void *user_data);
static void ime_app_return_key_type_set_cb(Ecore_IMF_Input_Panel_Return_Key_Type type, void *user_data);
static void ime_app_return_key_state_set_cb(bool disabled, void *user_data);
static void ime_app_layout_set_cb(Ecore_IMF_Input_Panel_Layout layout, void *user_data);
static bool ime_app_process_key_event_cb(ime_key_code_e keycode, ime_key_mask_e keymask, ime_device_info_h dev_info, void *user_data);
static void ime_app_display_language_changed_cb(const char *language, void *user_data);

static Evas_Object *enter_key_btn = NULL;

static void button_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_commit_string(elm_object_text_get(obj));
}

static void back_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_send_key_event(IME_KEY_BackSpace, IME_KEY_MASK_PRESSED, false);
	ime_send_key_event(IME_KEY_BackSpace, IME_KEY_MASK_RELEASED, false);
}

static void space_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_send_key_event(IME_KEY_space, IME_KEY_MASK_PRESSED, true);
	ime_send_key_event(IME_KEY_space, IME_KEY_MASK_RELEASED, true);
}

static void return_key_clicked(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	ime_send_key_event(IME_KEY_Return, IME_KEY_MASK_PRESSED, true);
	ime_send_key_event(IME_KEY_Return, IME_KEY_MASK_RELEASED, true);
}

static Evas_Object *create_key_button(Evas_Object *parent, char *str)
{
	char *markup_str = NULL;

	Evas_Object *btn = elm_button_add(parent);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);

	if (str) {
		markup_str = elm_entry_utf8_to_markup(str);

		if (markup_str) {
			elm_object_text_set(btn, markup_str);
			free(markup_str);
			markup_str = NULL;
		}
	}

	evas_object_show(btn);

	return btn;
}

static void set_return_key_type(Ecore_IMF_Input_Panel_Return_Key_Type type)
{
	switch (type) {
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_DONE:
		elm_object_text_set(enter_key_btn, "Done");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_GO:
		elm_object_text_set(enter_key_btn, "Go");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_JOIN:
		elm_object_text_set(enter_key_btn, "Join");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_LOGIN:
		elm_object_text_set(enter_key_btn, "Login");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_NEXT:
		elm_object_text_set(enter_key_btn, "Next");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH:
		elm_object_text_set(enter_key_btn, "Search");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_SEND:
		elm_object_text_set(enter_key_btn, "Send");
		break;
	case ECORE_IMF_INPUT_PANEL_RETURN_KEY_TYPE_SIGNIN:
		elm_object_text_set(enter_key_btn, "Sign in");
		break;
	default:
		elm_object_text_set(enter_key_btn, "Enter");
		break;
	}
}

static void ime_app_create_cb(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "ime_app_create_cb");
	unsigned int i = 0;
	Evas_Object *btn = NULL;
	int w, h;

	Evas_Object *ime_win = ime_get_main_window();
	if (!ime_win) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "Can't get main window: %d", get_last_result());
		return;
	}

	elm_win_screen_size_get(ime_win, NULL, NULL, &w, &h);
	ime_set_size(w, h*2/5, h, w*3/5);

	Evas_Object *bg = elm_bg_add(ime_win);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ime_win, bg);
	evas_object_show(bg);

	Evas_Object *table = elm_table_add(ime_win);
	elm_win_resize_object_add(ime_win, table);

	/* row 0 */
	char *keypad_row0[] = {"١", "٢", "٣", "٤", "٥", "٦", "٧", "٨", "٩"};

	for (i = 0; i < sizeof(keypad_row0)/sizeof(keypad_row0[0]); i++) {
		btn = create_key_button(table, keypad_row0[i]);
		/*elm_bg_color_set (btn, 255, 0, 51);*/
		evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
		elm_table_pack(table, btn, i, 0, 1, 1);
	}

	/* row 1 */
	char *keypad_row1[] = {"ض", "ص", "ق", "ف", "غ", "ع", "ه", "خ", "ح"};

	for (i = 0; i < sizeof(keypad_row1)/sizeof(keypad_row1[0]); i++) {
		btn = create_key_button(table, keypad_row1[i]);
		/*elm_bg_color_set (btn, 0, 153, 204);*/
		evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
		elm_table_pack(table, btn, i, 1, 1, 1);
	}

	/* row 2 */
	char *keypad_row2[] = {"ش", "س", "ي", "ﺏ", "ل", "ا", "ت", "ن", "م"};

	for (i = 0; i < sizeof(keypad_row2)/sizeof(keypad_row2[0]); i++) {
		btn = create_key_button(table, keypad_row2[i]);
		/*elm_bg_color_set (btn, 0, 153, 204);*/
		evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
		elm_table_pack(table, btn, i, 2, 1, 1);
	}

	/* row 3 */
	char *keypad_row3[] = {"ظ", "ط", "ذ", "د", "ز", "ر", "و", "ة", "ث"};

	for (i = 0; i < sizeof(keypad_row3)/sizeof(keypad_row3[0]); i++) {
		btn = create_key_button(table, keypad_row3[i]);
		/*elm_bg_color_set (btn, 0, 153, 204);*/
		evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
		elm_table_pack(table, btn, i, 3, 1, 1);
	}

	/* row 4 */
	char *keypad_row4[] = {",", ".", "ج", "ك", "پ", "چ", "ژ", "!", "?"};

	for (i = 0; i < sizeof(keypad_row4)/sizeof(keypad_row4[0]); i++) {
		btn = create_key_button(table, keypad_row4[i]);
		/*elm_bg_color_set (btn, 0, 153, 204);*/
		evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
		elm_table_pack(table, btn, i, 4, 1, 1);
	}

	/* row 5 */
	/* Add return key */
	enter_key_btn = create_key_button(table, "Enter");
	/*elm_bg_color_set (enter_key_btn, 204, 255, 102);*/
	evas_object_smart_callback_add(enter_key_btn, "clicked", return_key_clicked, NULL);
	elm_table_pack(table, enter_key_btn, 0, 5, 1, 1);

	btn = create_key_button(table, "ء");
	/*elm_bg_color_set (btn, 0, 153, 204);*/
	evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
	elm_table_pack(table, btn, 1, 5, 1, 1);

	btn = create_key_button(table, "گ");
	/*elm_bg_color_set (btn, 0, 153, 204);*/
	evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
	elm_table_pack(table, btn, 2, 5, 1, 1);

	/* Add space key */
	btn = create_key_button(table, "Space");
	/*elm_bg_color_set (btn, 204, 255, 102);*/
	evas_object_smart_callback_add(btn, "clicked", space_key_clicked, NULL);
	elm_table_pack(table, btn, 3, 5, 3, 1);

	/* Add backspace key */
	btn = create_key_button(table, "←");
	/*elm_bg_color_set (btn, 204, 255, 102);*/
	evas_object_smart_callback_add(btn, "clicked", back_key_clicked, NULL);
	elm_table_pack(table, btn, 6, 5, 1, 1);

	btn = create_key_button(table, "ى");
	/*elm_bg_color_set (btn, 0, 153, 204);*/
	evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
	elm_table_pack(table, btn, 7, 5, 1, 1);

	btn = create_key_button(table, "٠");
	/*elm_bg_color_set (btn, 255, 0, 51);*/
	evas_object_smart_callback_add(btn, "clicked", button_clicked, NULL);
	elm_table_pack(table, btn, 8, 5, 1, 1);

	evas_object_show(table);
}

static void ime_app_terminate_cb(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "ime_app_terminate_cb");
	/* Release the resources */
}

static void ime_app_show_cb(int ic, ime_context_h context, void *user_data)
{
	Ecore_IMF_Input_Panel_Layout layout;
	ime_layout_variation_e layout_variation;
	int cursor_pos;
	Ecore_IMF_Autocapital_Type autocapital_type;
	Ecore_IMF_Input_Panel_Return_Key_Type return_key_type;
	bool return_key_state, prediction_mode, password_mode;

	dlog_print(DLOG_DEBUG, LOG_TAG, "%s, %d", "ime_app_show_cb", ic);

	if (ime_context_get_layout(context, &layout) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "layout: %d", layout);
	if (ime_context_get_layout_variation(context, &layout_variation) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "layout variation: %d", layout_variation);
	if (ime_context_get_cursor_position(context, &cursor_pos) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "cursor position: %d", cursor_pos);
	if (ime_context_get_autocapital_type(context, &autocapital_type) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "autocapital_type: %d", autocapital_type);
	if (ime_context_get_return_key_type(context, &return_key_type) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "return_key_type: %d", return_key_type);
	if (ime_context_get_return_key_state(context, &return_key_state) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "return_key_state: %d", return_key_state);
	if (ime_context_get_prediction_mode(context, &prediction_mode) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "prediction_mode: %d", prediction_mode);
	if (ime_context_get_password_mode(context, &password_mode) == IME_ERROR_NONE)
		dlog_print(DLOG_DEBUG, LOG_TAG, "password_mode: %d", password_mode);

	set_return_key_type(return_key_type);

	Evas_Object *ime_win = ime_get_main_window();
	if (!ime_win) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%d", get_last_result());
		return;
	}

	evas_object_show(ime_win);
}

static void ime_app_hide_cb(int ic, void *user_data)
{
	Evas_Object *ime_win = ime_get_main_window();
	if (!ime_win) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%d", get_last_result());
		return;
	}

	evas_object_hide(ime_win);
}

static void ime_app_cursor_position_updated_cb(int cursor_pos, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "cursor position: %d", cursor_pos);
}

static void ime_app_focus_out_cb(int ic, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "focus out: %d", ic);
}

static void ime_app_focus_in_cb(int ic, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "focus in: %d", ic);
}

static void ime_app_return_key_type_set_cb(Ecore_IMF_Input_Panel_Return_Key_Type type, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Return key type: %d", type);

	set_return_key_type(type);
}

static void ime_app_return_key_state_set_cb(bool disabled, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Return key disabled: %d", disabled);
}

static void ime_app_layout_set_cb(Ecore_IMF_Input_Panel_Layout layout, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "layout: %d", layout);
}

static bool ime_app_process_key_event_cb(ime_key_code_e keycode, ime_key_mask_e keymask, ime_device_info_h dev_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "keycode=0x%x, keymask=0x%x", keycode, keymask);

	if ((keymask & IME_KEY_MASK_CONTROL) || (keymask & IME_KEY_MASK_ALT) || (keymask & IME_KEY_MASK_META) || (keymask & IME_KEY_MASK_WIN) || (keymask & IME_KEY_MASK_HYPER))
		return false;

	return false;
}

static void ime_app_display_language_changed_cb(const char *language, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "language: %s", language);
}

void ime_app_main(int argc, char **argv)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "Start main function !");

	ime_callback_s basic_callback = {
		ime_app_create_cb,
		ime_app_terminate_cb,
		ime_app_show_cb,
		ime_app_hide_cb,
	};

	/* Set the necessary callback functions */
	ime_event_set_focus_in_cb(ime_app_focus_in_cb, NULL);
	ime_event_set_focus_out_cb(ime_app_focus_out_cb, NULL);
	ime_event_set_cursor_position_updated_cb(ime_app_cursor_position_updated_cb, NULL);
	ime_event_set_layout_set_cb(ime_app_layout_set_cb, NULL);
	ime_event_set_return_key_type_set_cb(ime_app_return_key_type_set_cb, NULL);
	ime_event_set_return_key_state_set_cb(ime_app_return_key_state_set_cb, NULL);
	ime_event_set_process_key_event_cb(ime_app_process_key_event_cb, NULL);
	ime_event_set_display_language_changed_cb(ime_app_display_language_changed_cb, NULL);

	/* Start IME */
	ime_run(&basic_callback, NULL);
}
