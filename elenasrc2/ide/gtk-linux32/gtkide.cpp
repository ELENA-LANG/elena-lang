//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ GTK IDE
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtk-linux32/gtkcommon.h"
//#include "gtk-linux32/gtksdi.h"
//#include "gtk-linux32/gtkmenu.h"
//#include "gtk-linux32/gtktoolbar.h"
//#include "gtk-linux32/gtkstatusbar.h"
#include "gtkeditframe.h"

#include "gtkide.h"

using namespace _GUI_;

//int AppToolBarButtonNumber = 12;
//ToolBarButton AppToolBarButtons[] =
//{
//   {IDM_FILE_NEW, GTK_STOCK_NEW},
//   {IDM_FILE_OPEN, GTK_STOCK_OPEN},
//   {IDM_FILE_SAVE, GTK_STOCK_SAVE},
//   {IDM_FILE_SAVEALL, GTK_STOCK_SELECT_ALL},
//   {IDM_FILE_CLOSE, GTK_STOCK_CLOSE},
//   {0, NULL},
//   {IDM_EDIT_CUT, GTK_STOCK_CUT},
//   {IDM_EDIT_COPY, GTK_STOCK_COPY},
//   {IDM_EDIT_PASTE, GTK_STOCK_PASTE},
//   {0, NULL},
//   {IDM_EDIT_UNDO, GTK_STOCK_UNDO},
//   {IDM_EDIT_REDO, GTK_STOCK_REDO},
/////*   {0, IDR_SEPARATOR},
////   {IDM_DEBUG_RUN, IDR_RUN},
////   {IDM_DEBUG_STEPINTO, IDR_STEPINTO},
////   {IDM_DEBUG_STEPOVER, IDR_STEPOVER},
////   {IDM_DEBUG_STOP, IDR_STOP},
////   {IDM_DEBUG_GOTOSOURCE, IDR_GOTO},*/
//
//};
//
//int StatusBars[5] = {0, 100, 100, 100, 100};
//
//static void menu_command(gpointer window, guint option, GtkWidget* menu_item)
//{
//   ((SDIWindow*)window)->_onMenu(option);
//}
//
//static void tabpage_changed(GtkNotebook* book, GtkNotebookPage *page, int index, void* window)
//{
//   ((SDIWindow*)window)->_onClientChanged(book, index, TABCHANGED_NOTIFY);
//}
//
//#define GtkItemFactoryEntryNumber 99

static Glib::ustring ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menu action='FileNew'>"
        "           <menuitem action='FileNewSource'/>"
        "      </menu>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "  </menubar>"
//        "  <toolbar  name='ToolBar'>"
//        "    <toolitem action='FileNewStandard'/>"
//        "    <toolitem action='FileQuit'/>"
//        "  </toolbar>"
        "</ui>";

//static GtkItemFactoryEntry menu_items[] = {
//  { "/_File",                    NULL, NULL,                                 0, "<Branch>" },
//  { "/_File/_New",               NULL, NULL,                                 0, "<Branch>" },

//  { "/_File/New/_Source File",   "<ctrl>N", (GtkItemFactoryCallback)menu_command, IDM_FILE_NEW, "<Item>"},

//  { "/_File/New/_Project",       NULL, (GtkItemFactoryCallback)menu_command, IDM_PROJECT_NEW, "<Item>"},
//  { "/_File/_Open",              NULL, NULL,                                 0, "<Branch>" },
//  { "/_File/Open/_File",         "<ctrl>O", (GtkItemFactoryCallback)menu_command, IDM_FILE_OPEN, "<Item>"},
//  { "/_File/Open/_Project",      "<ctrl><shift>O", (GtkItemFactoryCallback)menu_command, IDM_PROJECT_OPEN, "<Item>"},
//  { "/File/sep1",                NULL, NULL, NULL, "<Separator>"},
//  { "/File/_Save",               "<ctrl>S", (GtkItemFactoryCallback)menu_command, IDM_FILE_SAVE, "<Item>"},
//  { "/File/Save _As...",         NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_SAVEAS, "<Item>"},
//  { "/File/Save _Project As...", NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_SAVEPROJECT, "<Item>"},
//  { "/File/Save A_ll",           "<ctrl><shift>S", (GtkItemFactoryCallback)menu_command, IDM_FILE_SAVEALL, "<Item>"},
//  { "/File/sep2",                NULL, NULL, NULL, "<Separator>"},
//  { "/File/_Close",              "<ctrl>W", (GtkItemFactoryCallback)menu_command, IDM_FILE_CLOSE, "<Item>"},
//  { "/File/Cl_ose All",          "<ctrl><shift>W", (GtkItemFactoryCallback)menu_command, IDM_FILE_CLOSEALL, "<Item>"},
//  { "/File/Clo_se Project",      NULL, (GtkItemFactoryCallback)menu_command, IDM_PROJECT_CLOSE, "<Item>"},
//  { "/File/Close All But Acti_ve",NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_CLOSEALLBUT, "<Item>"},
//  { "/File/sep3",                NULL, NULL, NULL, "<Separator>"},
//  { "/File/Recent Files",        NULL, NULL, NULL, "<Branch>"},
//  { "/File/Recent Files/1",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_1, "<Item>"},
//  { "/File/Recent Files/2",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_2, "<Item>"},
//  { "/File/Recent Files/3",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_3, "<Item>"},
//  { "/File/Recent Files/4",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_4, "<Item>"},
//  { "/File/Recent Files/5",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_5, "<Item>"},
//  { "/File/Recent Files/6",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_6, "<Item>"},
//  { "/File/Recent Files/7",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_7, "<Item>"},
//  { "/File/Recent Files/8",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_8, "<Item>"},
//  { "/File/Recent Files/9",      NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_9, "<Item>"},
//  { "/File/Recent Files/10",     NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_10, "<Item>"},
//  { "/File/Recent Files/sep31",  NULL, NULL, IDM_FILE_FILES, "<Separator>"},
//  { "/File/Recent Files/Clear History", NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_FILES_CLEAR, "<Item>"},
//  { "/File/Recent Projects",     NULL, NULL, NULL, "<Branch>"},
//  { "/File/Recent Projects/1",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_1, "<Item>"},
//  { "/File/Recent Projects/2",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_2, "<Item>"},
//  { "/File/Recent Projects/3",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_3, "<Item>"},
//  { "/File/Recent Projects/4",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_4, "<Item>"},
//  { "/File/Recent Projects/5",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_5, "<Item>"},
//  { "/File/Recent Projects/6",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_6, "<Item>"},
//  { "/File/Recent Projects/7",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_7, "<Item>"},
//  { "/File/Recent Projects/8",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_8, "<Item>"},
//  { "/File/Recent Projects/9",   NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_9, "<Item>"},
//  { "/File/Recent Projects/10",  NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_10, "<Item>"},
//  { "/File/Recent Projects/sep32",  NULL, NULL, IDM_FILE_PROJECTS, "<Separator>"},
//  { "/File/Recent Projects/Clear History", NULL, (GtkItemFactoryCallback)menu_command, IDM_FILE_PROJECTS_CLEAR, "<Item>"},
//  { "/File/sep4",                NULL, NULL, NULL, "<Separator>"},

//  { "/File/_Exit",               "<ctrl>Q", (GtkItemFactoryCallback)menu_command, IDM_FILE_EXIT, "<Item>"},

//  { "/_Edit",                    NULL, NULL,                                 0, "<Branch>" },
//  { "/Edit/_Undo",                "<ctrl>Z", (GtkItemFactoryCallback)menu_command, IDM_EDIT_UNDO, "<Item>"},
//  { "/Edit/_Redo",               "<ctrl>Y", (GtkItemFactoryCallback)menu_command, IDM_EDIT_REDO, "<Item>"},
//  { "/Edit/sep1",                NULL, NULL, NULL, "<Separator>"},
//  { "/Edit/Cu_t",                "<ctrl>X", (GtkItemFactoryCallback)menu_command, IDM_EDIT_CUT, "<Item>"},
//  { "/Edit/_Copy",               "<ctrl>C", (GtkItemFactoryCallback)menu_command, IDM_EDIT_COPY, "<Item>"},
//  { "/Edit/_Paste",              "<ctrl>V", (GtkItemFactoryCallback)menu_command, IDM_EDIT_PASTE, "<Item>"},
//  { "/Edit/_Delete",             NULL, (GtkItemFactoryCallback)menu_command, IDM_EDIT_DELETE, "<Item>"},
//  { "/Edit/sep2",                NULL, NULL, NULL, "<Separator>"},
//  { "/Edit/_Select All",         "<ctrl>A", (GtkItemFactoryCallback)menu_command, IDM_EDIT_SELECTALL, "<Item>"},
//  { "/Edit/sep3",                NULL, NULL, NULL, "<Separator>"},
//  { "/Edit/Insert Tab (Indent)", "<ctrl>I", (GtkItemFactoryCallback)menu_command, IDM_EDIT_INDENT, "<Item>"},
//  { "/Edit/Remove Tab (Outdent)", "<ctrl><shift>O", (GtkItemFactoryCallback)menu_command, IDM_EDIT_OUTDENT, "<Item>"},
//  { "/Edit/_Trim whitespace",    "<ctrl>T", (GtkItemFactoryCallback)menu_command, IDM_EDIT_TRIM, "<Item>"},
//  { "/Edit/Erase _line",         "<ctrl>L", (GtkItemFactoryCallback)menu_command, IDM_EDIT_ERASELINE, "<Item>"},
//  { "/Edit/sep4",                NULL, NULL, NULL, "<Separator>"},
//  { "/Edit/To upper case",       "<ctrl>U", (GtkItemFactoryCallback)menu_command, IDM_EDIT_UPPERCASE, "<Item>"},
//  { "/Edit/To lower case",       "<ctrl><shift>U", (GtkItemFactoryCallback)menu_command, IDM_EDIT_LOWERCASE, "<Item>"},
//  { "/Edit/sep5",                NULL, NULL, NULL, "<Separator>"},
//  { "/Edit/Bloc_k comment",      "<ctrl>K", (GtkItemFactoryCallback)menu_command, IDM_EDIT_COMMENT, "<Item>"},
//  { "/Edit/Bl_ock uncomment",    "<ctrl><shift>K", (GtkItemFactoryCallback)menu_command, IDM_EDIT_UNCOMMENT, "<Item>"},
//  { "/Edit/sep7",                NULL, NULL, NULL, "<Separator>"},
//  { "/Edit/Commands",            NULL, NULL,                                 0, "<Branch>" },
//  { "/Edit/Commands/Duplicate",  "<ctrl>D", (GtkItemFactoryCallback)menu_command, IDM_EDIT_DUPLICATE, "<Item>"},
//  { "/Edit/Commands/Swap",       "<ctrl>E", (GtkItemFactoryCallback)menu_command, IDM_EDIT_SWAP, "<Item>"},
//  { "/_Search",                  NULL, NULL,                                 0, "<Branch>" },
//  { "/Search/_Find...",          "<ctrl>F", (GtkItemFactoryCallback)menu_command, IDM_SEARCH_FIND, "<Item>"},
//  { "/Search/Find _next",        "F3", (GtkItemFactoryCallback)menu_command, IDM_SEARCH_FINDNEXT, "<Item>"},
//  { "/Search/sep8",              NULL, NULL, NULL, "<Separator>"},
//  { "/Search/_Replace...",       "<ctrl>R", (GtkItemFactoryCallback)menu_command, IDM_SEARCH_REPLACE, "<Item>"},
//  { "/Search/sep9",              NULL, NULL, NULL, "<Separator>"},
//  { "/Search/_Go to line...",    "<ctrl>G", (GtkItemFactoryCallback)menu_command, IDM_SEARCH_GOTOLINE, "<Item>"},
//  { "/_Tools",                   NULL, NULL,                                 0, "<Branch>" },
//  { "/Tools/_Editor Options...", NULL, (GtkItemFactoryCallback)menu_command, IDM_EDITOR_OPTIONS, "<Item>"},
//  { "/_Window",                  NULL, NULL,                                 0, "<Branch>" },
//  { "/Window/_Next",             "<ctrl>Tab", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_NEXT, "<Item>"},
//  { "/Window/_Previous",         "<ctrl><shift>Tab", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_PREVIOUS, "<Item>"},
//  { "/Window/sep6",              NULL, NULL, NULL, "<Separator>"},
//  { "/Window/1",                 "<alt>1", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_FIRST, "<Item>"},
//  { "/Window/2",                 "<alt>2", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_SECOND, "<Item>"},
//  { "/Window/3",                 "<alt>3", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_THIRD, "<Item>"},
//  { "/Window/4",                 "<alt>4", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_FOURTH, "<Item>"},
//  { "/Window/5",                 "<alt>5", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_FIFTH, "<Item>"},
//  { "/Window/6",                 "<alt>6", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_SIXTH, "<Item>"},
//  { "/Window/7",                 "<alt>7", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_SEVENTH, "<Item>"},
//  { "/Window/8",                 "<alt>8", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_EIGHTH, "<Item>"},
//  { "/Window/9",                 "<alt>9", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_NINTH, "<Item>"},
//  { "/Window/10",                NULL, (GtkItemFactoryCallback)menu_command, IDM_WINDOW_TENTH, "<Item>"},
//  { "/Window/Windows...",        "<alt>0", (GtkItemFactoryCallback)menu_command, IDM_WINDOW_WINDOWS, "<Item>"},
//  { "/_Help",                    NULL, NULL,                                 0, "<Branch>" },
//  { "/Help/_ELENA API",          NULL, (GtkItemFactoryCallback)menu_command, IDM_HELP_API, "<Item>"},
//  { "/Help/sep10",               NULL, NULL, NULL, "<Separator>"},
//  { "/Help/_About",              NULL, (GtkItemFactoryCallback)menu_command, IDM_HELP_ABOUT, "<Item>"},
//};

// --- GTKEIDE ---

GTKIDE :: GTKIDE()
   : IDE(NULL), controls(NULL, _ELENA_::freeobj)
{
   // create main window & menu
   this->_appWindow = new MainWindow("IDE", this);

   controls.add(_appWindow);

//   // initialize recent files / projects / windows manager
//   _recentFiles.assign(_appMenu);
//   _recentProjects.assign(_appMenu);
//   _windowList.assign(_appMenu);
}

void GTKIDE :: start(bool maximized)
{
   onIDEInit();

   if (maximized)
      _appWindow->maximize();

   IDE::start();

   Gtk::Main::run(*_appWindow);
}

//Menu* GTKIDE :: createMainMenu()
//{
//   Menu* menu = new Menu(menu_items, GtkItemFactoryEntryNumber, _appWindow);
//
//   // hide unused menu items
//   menu->eraseItemById(IDM_FILE_FILES);
//   menu->eraseItemById(IDM_FILE_FILES_1);
//   menu->eraseItemById(IDM_FILE_FILES_2);
//   menu->eraseItemById(IDM_FILE_FILES_3);
//   menu->eraseItemById(IDM_FILE_FILES_4);
//   menu->eraseItemById(IDM_FILE_FILES_5);
//   menu->eraseItemById(IDM_FILE_FILES_6);
//   menu->eraseItemById(IDM_FILE_FILES_7);
//   menu->eraseItemById(IDM_FILE_FILES_8);
//   menu->eraseItemById(IDM_FILE_FILES_9);
//   menu->eraseItemById(IDM_FILE_FILES_10);
//   menu->eraseItemById(IDM_FILE_PROJECTS);
//   menu->eraseItemById(IDM_FILE_PROJECTS_1);
//   menu->eraseItemById(IDM_FILE_PROJECTS_2);
//   menu->eraseItemById(IDM_FILE_PROJECTS_3);
//   menu->eraseItemById(IDM_FILE_PROJECTS_4);
//   menu->eraseItemById(IDM_FILE_PROJECTS_5);
//   menu->eraseItemById(IDM_FILE_PROJECTS_6);
//   menu->eraseItemById(IDM_FILE_PROJECTS_7);
//   menu->eraseItemById(IDM_FILE_PROJECTS_8);
//   menu->eraseItemById(IDM_FILE_PROJECTS_9);
//   menu->eraseItemById(IDM_FILE_PROJECTS_10);
//
//   menu->eraseItemById(IDM_WINDOW_FIRST);
//   menu->eraseItemById(IDM_WINDOW_SECOND);
//   menu->eraseItemById(IDM_WINDOW_THIRD);
//   menu->eraseItemById(IDM_WINDOW_FOURTH);
//   menu->eraseItemById(IDM_WINDOW_FIFTH);
//   menu->eraseItemById(IDM_WINDOW_SIXTH);
//   menu->eraseItemById(IDM_WINDOW_SEVENTH);
//   menu->eraseItemById(IDM_WINDOW_EIGHTH);
//   menu->eraseItemById(IDM_WINDOW_NINTH);
//   menu->eraseItemById(IDM_WINDOW_TENTH);
//
//   menu->enableItemById(IDM_FILE_FILES_CLEAR, false);
//   menu->enableItemById(IDM_FILE_PROJECTS_CLEAR, false);
//
//   return menu;
//}
//
//
void GTKIDE :: onIDEInit()
{
////   contextMenu.create(8, contextMenuInfo);
////
//   _appToolBar = new ToolBar(_appWindow, 16, AppToolBarButtonNumber, AppToolBarButtons);
//   _statusBar = new StatusBar(_appWindow, 5, StatusBars);
////   _outputBar = createOutputBar();
//   _contextBrowser = /*createContextBrowser()*/NULL;

   _mainFrame = new EditFrame(_appWindow);
////   TextView* textView = new TextView(_mainFrame, 5, 28, 400, 400);
////   _mainFrame->populate(textView);
////   textView->setReceptor(_appWindow);

   _appWindow->populate(_mainFrame/*, _statusBar*/);

//   g_signal_connect(G_OBJECT(_mainFrame->getHandle()), "switch-page",
//      G_CALLBACK(tabpage_changed), _appWindow);
//
//   _mainFrame->show();
//   _appMenu->_show();
//
//   _appToolBar->show();
//   _statusBar->show();
//
//   _appWindow ->show(); // !! temporal
////   _outputBar->show();  // !! temporal

   controls.add(_mainFrame);
////   controls.add(textView);
//   controls.add(_statusBar);
//   controls.add(_appToolBar);

   IDE::onIDEInit();
}

// --- MainWindow ---

MainWindow :: MainWindow(const _text_t* caption, GTKIDE* env)
   : SDIWindow(caption)
{
   _ide = env;

   //File menu:
   _refActionGroup->add(Gtk::Action::create("FileMenu", "File"));
   _refActionGroup->add(Gtk::Action::create("FileNew", "New"));
   _refActionGroup->add(Gtk::Action::create("FileNewSource", Gtk::Stock::NEW),
          sigc::mem_fun(*this, &MainWindow::on_menu_file_new_source));
   _refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT),
          sigc::mem_fun(*this, &MainWindow::on_menu_file_quit));

   loadUI(ui_info);

   show_all_children();
}
