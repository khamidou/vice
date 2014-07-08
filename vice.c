#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>

#include <Scintilla.h>
#include <SciLexer.h>
#define PLAT_GTK 1
#include <ScintillaWidget.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

GtkWidget *app;
GtkWidget *vbox;
GtkWidget *minibuffer;
GtkWidget *editor;
ScintillaObject *sci;

lua_State *vice_lua_state;

#define SSM(m, w, l) scintilla_send_message(sci, m, w, l)

static int exit_app(GtkWidget*w, GdkEventAny*e, gpointer p) {
   gtk_main_quit();
   return w||e||p||1;	// Avoid warnings
}

static int minibuffer_write(lua_State *L) {
    if(!lua_isstring(L, 1)) {
        lua_pushstring(L, "incorrect argument to function `display'");
        lua_error(L);
    }

    gtk_entry_set_text(GTK_ENTRY(minibuffer), lua_tostring(L, 1)); 
    return 0; 
}

static int get_cursor_position(lua_State *L) {
    int len = 0;
    len = SSM(SCI_GETCURRENTPOS, 0, 0);
    lua_pushnumber(L, len);
    return 1;
}

static gboolean handle_keypress(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    lua_pushstring(vice_lua_state, "keyboard_event_handler");
    lua_gettable(vice_lua_state, LUA_GLOBALSINDEX);               /* function to be called */
    lua_pushstring(vice_lua_state, gdk_keyval_name(event->keyval));
    lua_call(vice_lua_state, 1, 0);
    return TRUE;
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    app = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    editor = scintilla_new();
    sci = SCINTILLA(editor);
    minibuffer = gtk_entry_new();

    vbox = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(app), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), editor, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), minibuffer, TRUE, TRUE, 0);

    gtk_signal_connect(GTK_OBJECT(app), "delete_event",
            GTK_SIGNAL_FUNC(exit_app), 0);
    gtk_signal_connect(GTK_OBJECT(app), "key_press_event", GTK_SIGNAL_FUNC(handle_keypress), 0);

    scintilla_set_id(sci, 0);
    gtk_widget_set_usize(editor, 800, 600);

    SSM(SCI_STYLECLEARALL, 0, 0);
    SSM(SCI_SETLEXER, SCLEX_CPP, 0);
    SSM(SCI_SETKEYWORDS, 0, (sptr_t)"int char");
    SSM(SCI_STYLESETFORE, SCE_C_COMMENT, 0x008000);
    SSM(SCI_STYLESETFORE, SCE_C_COMMENTLINE, 0x008000);
    SSM(SCI_STYLESETFORE, SCE_C_NUMBER, 0x808000);
    SSM(SCI_STYLESETFORE, SCE_C_WORD, 0x800000);
    SSM(SCI_STYLESETFORE, SCE_C_STRING, 0x800080);
    SSM(SCI_STYLESETBOLD, SCE_C_OPERATOR, 1);

    vice_lua_state = lua_open();

    luaopen_io(vice_lua_state); // provides io.*
    luaopen_base(vice_lua_state);
    luaopen_table(vice_lua_state);
    luaopen_string(vice_lua_state);
    luaopen_math(vice_lua_state);
    luaopen_loadlib(vice_lua_state);
    
    lua_register(vice_lua_state, "display", minibuffer_write);
    lua_register(vice_lua_state, "get_cursor_position", get_cursor_position);

    int s = luaL_loadfile(vice_lua_state, "main.lua");

    if ( s==0 ) {
      // execute Lua program
      s = lua_pcall(vice_lua_state, 0, LUA_MULTRET, 0);
    }

    SSM(SCI_INSERTTEXT, 0, (sptr_t) "YOLO SWAG");

    gtk_widget_show_all(app);
    gtk_widget_grab_focus(GTK_WIDGET(editor));
    gtk_main();

    return 0;
}
