//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ GTK IDE
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

//#include "gtk-linux32/gtkcommon.h"
//#include "gtk-linux32/gtksdi.h"
//#include "gtk-linux32/gtkmenu.h"
//#include "gtk-linux32/gtktoolbar.h"
//#include "gtk-linux32/gtkstatusbar.h"
//#include "gtkeditframe.h"

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
        "           <menuitem action='FileNewProject'/>"
        "      </menu>"
        "      <menu action='FileOpen'>"
        "           <menuitem action='FileOpenSource'/>"
        "           <menuitem action='FileOpenProject'/>"
        "      </menu>"
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "  </menubar>"
//        "  <toolbar  name='ToolBar'>"
//        "    <toolitem action='FileNewStandard'/>"
//        "    <toolitem action='FileQuit'/>"
//        "  </toolbar>"
        "</ui>";

// --- MainWindow ---

void MainWindow :: populateMenu()
{
   //File menu:
   _refActionGroup->add( Gtk::Action::create("FileMenu", "_File") );
   _refActionGroup->add( Gtk::Action::create("FileNew", "New") );
   _refActionGroup->add( Gtk::Action::create("FileNewSource", "Source"), sigc::mem_fun(*this, &MainWindow::on_menu_file_new_source));
   _refActionGroup->add( Gtk::Action::create("FileNewProject", "Project"), sigc::mem_fun(*this, &MainWindow::on_menu_file_new_project));
   _refActionGroup->add( Gtk::Action::create("FileOpen", "Open") );
   _refActionGroup->add( Gtk::Action::create("FileOpenSource", "Source"), sigc::mem_fun(*this, &MainWindow::on_menu_file_open_source));
   _refActionGroup->add( Gtk::Action::create("FileOpenProject", "Project"), sigc::mem_fun(*this, &MainWindow::on_menu_file_open_project));
   _refActionGroup->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &MainWindow::on_menu_file_quit));

   loadUI(ui_info, "/MenuBar");
}

void MainWindow :: populateToolbar()
{
   //Create the toolbar and add it to a container widget:
   _toolbar = Gtk::manage(new Gtk::Toolbar());
   Gtk::ToolButton* button = Gtk::manage(new Gtk::ToolButton());
   button->set_icon_name("document-new");

   //We can't do this until we can break the ToolButton ABI: button->set_detailed_action_name("example.new");
   gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "IDE.FileNewSource");
   _toolbar->add(*button);

   _box.pack_start(*_toolbar, Gtk::PACK_SHRINK);
}

int MainWindow :: newDocument(const char* name, Document* doc)
{
   return _mainFrame->newDocument(name, doc);
}

int MainWindow :: getCurrentDocumentIndex()
{
   return _mainFrame->getCurrentIndex();
}

void MainWindow :: closeDocument(int index)
{
   _mainFrame->eraseDocumentTab(index);
}

MainWindow :: MainWindow(const char* caption, _Controller* controller, Model* model)
   : SDIWindow(caption)
{
   _controller = controller;
   _model = model;

   populateMenu();
   populateToolbar();

   show_all_children();

   _mainFrame = new EditFrame(model/*this*/);
//////   TextView* textView = new TextView(_mainFrame, 5, 28, 400, 400);
//////   _mainFrame->populate(textView);
//////   textView->setReceptor(_appWindow);

   populate(_mainFrame/*, _statusBar*/);

////   g_signal_connect(G_OBJECT(_mainFrame->getHandle()), "switch-page",
////      G_CALLBACK(tabpage_changed), _appWindow);
////
////   _mainFrame->show();
////   _appMenu->_show();
////
////   _appToolBar->show();
////   _statusBar->show();
////
////   _appWindow ->show(); // !! temporal
//////   _outputBar->show();  // !! temporal
//
//   controls.add(_mainFrame);
//////   controls.add(textView);
////   controls.add(_statusBar);
////   controls.add(_appToolBar);
}
