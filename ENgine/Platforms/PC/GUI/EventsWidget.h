
#pragma once

class BaseWidget;


#define MAIN_WINDOW_ID                           98
#define VIEW_PORT_ID                             99

#define VIEW_PORT_ENTITY_NAME_ID                 116
#define VIEW_PORT_ENTITY_ACTIVE_BUTTON_ID        117
#define VIEW_PORT_ENTITY_VISIBLE_BUTTON_ID       118
#define VIEW_PORT_SCENE_LIST_ID                  130
#define VIEW_PORT_GUI_LIST_ID                    140
#define VIEW_PORT_SYMBOLS_LIST_ID                145
#define VIEW_PORT_ENTITY_CLASS_NAME_ID           320


#define DIALOG_BUTTON_OK_ID                      3001
#define DIALOG_CLOSE_BUTTON_ID                   3002

#define SCRIPT_EDITOR_COLOR_SCHEME_ID_OFFSET     3003
#define RECENT_OPEN_PROJECTS_ID_OFFSET           3053
#define RECENT_OPEN_FILES_ID_OFFSET              3063

#define SCRIPT_DOCUMENT_UNDO_BUTTON_ID           3101
#define SCRIPT_DOCUMENT_REDO_BUTTON_ID           3102
#define SCRIPT_DOCUMENT_CLOSE_BUTTON_ID          3103
#define SCRIPT_DOCUMENT_GOTO_WINDOW_ID           3104
#define SCRIPT_DOCUMENT_SEARCH_WINDOW_ID         3105
#define SCRIPT_DOCUMENT_SEARCH_NEXT_ID           3106
#define SCRIPT_DOCUMENT_SEARCH_PREV_ID           3107
#define SCRIPT_DOCUMENT_SEARCH_MATCH_CASE_ID     3108
#define SCRIPT_DOCUMENT_SEARCH_MATCH_WORD_ID     3109
#define SCRIPT_DOCUMENT_SEARCH_REPLACE_ID        3110
#define SCRIPT_DOCUMENT_SEARCH_REPLACEAll_ID     3111
#define SCRIPT_DOCUMENT_SEARCH_TARGET_ID         3112
#define SCRIPT_DOCUMENT_SEARCH_TEXT_ID           3113
#define SCRIPT_DOCUMENT_OPEN_ID                  3114
#define SCRIPT_DOCUMENT_SAVE_ID                  3115
#define SCRIPT_DOCUMENT_CONFIRM_CLOSE_WINDOW_ID  3116
#define SCRIPT_DOCUMENT_CONFIRM_REOPEN_WINDOW_ID 3117
#define SCRIPT_DOCUMENT_CLASSES_LIST_ID          3118
#define SCRIPT_DOCUMENT_MEMBERS_LIST_ID          3119
#define NETWORK_MENU_DEVICES_LIST_ID             2120
#define NETWORK_MENU_INTERNET_FLAG_ID            2121
#define NETWORK_DEVICES_WINDOW_ID                2122
#define NETWORK_DEVICES_WINDOW_CONNECT_ID        2123
#define NETWORK_DEVICES_WINDOW_LIST_ID           2124
#define NETWORK_DEVICES_WINDOW_SEND_ID           2125
#define NETWORK_DEVICES_WINDOW_MESSAGE_ID        2126
#define LOGGER_LIST_ID                           2127
#define EDITOR_DOCK_PANEL_ID                     2128
#define EDITOR_DOCK_WINDOW_ID                    2129
#define SIMULATION_OPTIONS_WINDOW_ID             2130
#define SIMULATION_OPTIONS_ID                    2131
#define SIMULATION_OPTIONS_YEAR_ID               2132
#define SIMULATION_OPTIONS_MONTH_ID              2133
#define SIMULATION_OPTIONS_DAY_ID                2134
#define SIMULATION_OPTIONS_HOUR_ID               2135
#define SIMULATION_OPTIONS_MINUTE_ID             2136
#define SIMULATION_OPTIONS_TIME_ID               2137
#define SIMULATION_OPTIONS_DPI_ID                2138
#define SIMULATION_OPTIONS_DEVICE_LIST_ID        2139
#define SIMULATION_OPTIONS_DEVICE_ADD_ID         2140
#define SIMULATION_OPTIONS_DEVICE_DELETE_ID      2141
#define SIMULATION_OPTIONS_DEVICE_REAL_ADD_ID    2142
#define SIMULATION_OPTIONS_CANCEL_ADD_ID         2143   
#define SIMULATION_OPTIONS_WINDOW2_ID            2144
#define SIMULATION_OPTIONS_OK_ID                 2145   
#define SIMULATION_OPTIONS_EDIT_ID               2146   


#define MENU_SHOW_LOGGER_ID                      2130
#define MENU_SHOW_PROJECTS_ID                    2131
#define MENU_SHOW_SCENE_ID                       2132
#define MENU_SHOW_OBJECT_ID                      2133

#define LOGS_TABS_ID                             2138
#define WORK_SPACE_TABS_ID                       2139
#define PROJECTS_TABS_ID                         2140
#define PROJECT_LIST_ID                          2141
#define SCRIPT_FILES_TABS_ID                     2142
#define FIND_RESULTS_TABS_ID                     2143
#define FIND_RESULTS_LIST_ID                     2144

#define PROJECT_GOTO_FOLDER_ID                   285

#define SCRIPT_DOCUMENTS_ID_OFFSET 3500


class DEF_EXPORT EventsWidget
{
	static int id;
	static int evnt;
	static BaseWidget* modal_dialog;
	static BaseWidget* main_window;
    static BaseWidget* g_pEventSender;
    static void*       g_pEventData;

public:

	enum
	{
		empty = 0,
		label_click,
		button_click,
		textbox_change,
		combobox_change,
        listbox_sel_change,
        listbox_double_click,
		treelist_change,
		treelist_click,
		treelist_double_click,
		treelist_labeledited,
		treelist_drag_ended,
		checkbox_click,
		scrollbar_change,
		menuitem_click,
		window_close,
        window_show,
		modal_ok,
		modal_cancel,
		popupmenu_copy,
		popupmenu_delete,
		popupmenu_paste,
        popupmenu_create_folder,
        popupmenu_create_widget,
        popupmenu_create_instance,
        popupmenu_convert_to_symbol,
        popupmenu_save_scene,
        popupmenu_save_scene_as,
        popupmenu_save_scene_as_duplicate,
        popupmenu_add_new_scene,
        popupmenu_add_existing_scene,
        popupmenu_add_new_script,
        popupmenu_add_existing_script,
        popupmenu_set_as_startup_scene,
        popupmenu_include_scene,
        popupmenu_get_full_whidget_name,
        popupmenu_rollback_changes,
        textbox_enter_press,

	};
	
	static void SetEvent(int id, int evnt,BaseWidget* pSender = NULL,void* pData = NULL);
    static BaseWidget* Sender() { return g_pEventSender;}
    static void* EventData() { return g_pEventData;}
    static int  Id(){return id;}
    static void Clear(){id = 0;evnt = 0;}
	static bool IsAnyEvent();
	static bool CheckEvent(int id, int evnt);
	static bool AddEventHandler(int id, int evnt);
	static void SetMainWindow(BaseWidget* main_wnd);
	static void OpenResourceBrowser(const char* res_type);
	static bool IsResourceBrowserOpened();
	static bool IsResourceBrowserFinished(bool& succeced);
	static void CloseModalDialog();
	static void Update();
};
