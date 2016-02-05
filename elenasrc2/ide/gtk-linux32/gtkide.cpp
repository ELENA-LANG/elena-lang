//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ GTK IDE
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

//#include "gtk-linux32/gtkcommon.h"
//#include "gtk-linux32/gtksdi.h"
//#include "gtk-linux32/gtkmenu.h"
//#include "gtk-linux32/gtktoolbar.h"
//#include "gtk-linux32/gtkstatusbar.h"
//#include "gtkeditframe.h"

#include "gtkide.h"
//#include<pthread.h>

#define COMPILER_PATH "/usr/bin/elena-lc"
#define COMPILER_NAME "elena-lc"
#define PIPE_READ 0
#define PIPE_WRITE 1

using namespace _GUI_;

bool MainWindow::OutputProcess :: isStopped()
{
   return stopped;
}

void MainWindow::OutputProcess :: writeOut(Gtk::TextView& view)
{
   Glib::Threads::Mutex::Lock lock(_mutex);

   Gtk::TextIter iter = view.get_buffer()->end();
   view.get_buffer()->insert(iter, buffer, buffer + buf_len);

   buf_len = 0;
}

void MainWindow::OutputProcess :: compile(MainWindow* owner)
{
   stopped = false;

   int  stdinPipe[2];
   int  stdoutPipe[2];

   const char* arg = argument;

   if (pipe(stdinPipe) < 0) {
      //perror("allocating pipe for child input redirect");
      return;
   }
   if (pipe(stdoutPipe) < 0) {
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      //perror("allocating pipe for child output redirect");
      return;
   }

   int child = fork();
   if (child == 0) {
      // child continues here

      // redirect stdin
      if (dup2(stdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
         //perror("redirecting stdin");
         return;
      }

      // redirect stdout
      if (dup2(stdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
         //perror("redirecting stdout");
         return;
      }

      // redirect stderr
      if (dup2(stdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
         //perror("redirecting stderr");
         return;
      }

      // all these are for use by parent only
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);

      int retVal = execl(COMPILER_PATH, COMPILER_NAME, arg ,0);

      // if we get here at all, an error occurred, but we are in the child
      // process, so just exit
      //perror("exec of the child process");
      ::exit(retVal);
   }
   else if (child > 0) {
      // parent continues here

      // close unused file descriptors, these are for child only
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);

      // Include error check here
      //if (NULL != szMessage) {
      //   write(stdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
      //}

      while (true) {
         buf_len = read(stdoutPipe[PIPE_READ], buffer, 512);
         if (buf_len == 0)
            break;

         owner->notifyOutput();

         // wait until the buffer is read
         while (true) {
            Glib::usleep(100);
            {
               Glib::Threads::Mutex::Lock lock(_mutex);
               if (buf_len == 0)
                  break;
            }
         }
      }

      // done with these in this example program, you would normally keep these
      // open of course as long as you want to talk to the child
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);

      stopped = true;
   }
   else {
      // failed to create child
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);
   }
}

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
        "      <menuitem action='FileSave'/>"
        "      <menuitem action='FileSaveAs'/>"
        "      <menuitem action='FileProjectAs'/>"
        "      <menuitem action='FileSaveAll'/>"
        "      <separator/>"
        "      <menuitem action='FileClose'/>"
        "      <menuitem action='FileCloseAll'/>"
        "      <menuitem action='ProjectClose'/>"
        "      <menuitem action='FileCloseAllButActive'/>"
        "      <separator/>"
        "      <menuitem action='FileQuit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "      <menuitem action='EditUndo'/>"
        "      <menuitem action='EditRedo'/>"
        "      <separator/>"
        "      <menuitem action='EditCut'/>"
        "      <menuitem action='EditCopy'/>"
        "      <menuitem action='EditPaste'/>"
        "      <menuitem action='EditDelete'/>"
        "      <separator/>"
        "      <menuitem action='EditSelectAll'/>"
        "      <separator/>"
        "      <menuitem action='EditInsertTab'/>"
        "      <menuitem action='EditRemoveTab'/>"
        "      <menuitem action='EditTrim'/>"
        "      <menuitem action='EditEraseLine'/>"
        "      <separator/>"
        "      <menuitem action='EditUpper'/>"
        "      <menuitem action='EditLower'/>"
        "      <separator/>"
        "      <menuitem action='EditComment'/>"
        "      <menuitem action='EditUncomment'/>"
        "    </menu>"
        "    <menu action='ProjectMenu'>"
        "      <menuitem action='ProjectInclude'/>"
        "      <menuitem action='ProjectExclude'/>"
        "      <separator/>"
        "      <menuitem action='ProjectCompile'/>"
        "      <menuitem action='ProjectCleanup'/>"
        "      <separator/>"
        "      <menuitem action='ProjectForwards'/>"
        "      <menuitem action='ProjectOptions'/>"
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
   _refActionGroup->add( Gtk::Action::create("EditMenu", "_Edit") );
   _refActionGroup->add( Gtk::Action::create("ProjectMenu", "_Project") );

   _refActionGroup->add( Gtk::Action::create("FileNew", "New") );
   _refActionGroup->add( Gtk::Action::create("FileNewSource", "Source"), sigc::mem_fun(*this, &MainWindow::on_menu_file_new_source));
   _refActionGroup->add( Gtk::Action::create("FileNewProject", "Project"), sigc::mem_fun(*this, &MainWindow::on_menu_file_new_project));
   _refActionGroup->add( Gtk::Action::create("FileOpen", "Open") );
   _refActionGroup->add( Gtk::Action::create("FileOpenSource", "Source"), sigc::mem_fun(*this, &MainWindow::on_menu_file_open_source));
   _refActionGroup->add( Gtk::Action::create("FileOpenProject", "Project"), sigc::mem_fun(*this, &MainWindow::on_menu_file_open_project));
   _refActionGroup->add( Gtk::Action::create("FileSave", "Save"), sigc::mem_fun(*this, &MainWindow::on_menu_file_save));
   _refActionGroup->add( Gtk::Action::create("FileSaveAs", "Save As..."), sigc::mem_fun(*this, &MainWindow::on_menu_file_saveas));
   _refActionGroup->add( Gtk::Action::create("FileProjectAs", "Save Project As..."), sigc::mem_fun(*this, &MainWindow::on_menu_project_saveas));
   _refActionGroup->add( Gtk::Action::create("FileSaveAll", "Save All"), sigc::mem_fun(*this, &MainWindow::on_menu_file_saveall));
   _refActionGroup->add( Gtk::Action::create("FileClose", "Close"), sigc::mem_fun(*this, &MainWindow::on_menu_file_close));
   _refActionGroup->add( Gtk::Action::create("FileCloseAll", "Close All"), sigc::mem_fun(*this, &MainWindow::on_menu_file_closeall));
   _refActionGroup->add( Gtk::Action::create("ProjectClose", "Close Project"), sigc::mem_fun(*this, &MainWindow::on_menu_file_close));
   _refActionGroup->add( Gtk::Action::create("FileCloseAllButActive", "Close All But Active"), sigc::mem_fun(*this, &MainWindow::on_menu_file_closeproject));
   _refActionGroup->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &MainWindow::on_menu_file_quit));

   _refActionGroup->add( Gtk::Action::create("EditUndo", "Undo"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_undo));
   _refActionGroup->add( Gtk::Action::create("EditRedo", "Redo"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_redo));
   _refActionGroup->add( Gtk::Action::create("EditCut", "Cut"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_cut));
   _refActionGroup->add( Gtk::Action::create("EditCopy", "Copy"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_copy));
   _refActionGroup->add( Gtk::Action::create("EditPaste", "Paste"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_paste));
   _refActionGroup->add( Gtk::Action::create("EditDelete", "Delete"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_delete));
   _refActionGroup->add( Gtk::Action::create("EditSelectAll", "Select All"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_select_all));
   _refActionGroup->add( Gtk::Action::create("EditInsertTab", "Insert Tab (Indent)"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_indent));
   _refActionGroup->add( Gtk::Action::create("EditRemoveTab", "Remove Tab (Outdent)"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_outdent));
   _refActionGroup->add( Gtk::Action::create("EditTrim", "Trim whitespace"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_trim));
   _refActionGroup->add( Gtk::Action::create("EditEraseLine", "Erase line"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_erase_line));
   _refActionGroup->add( Gtk::Action::create("EditUpper", "To upper case"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_upper));
   _refActionGroup->add( Gtk::Action::create("EditLower", "To lower case"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_lower));
   _refActionGroup->add( Gtk::Action::create("EditComment", "Block comment"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_comment));
   _refActionGroup->add( Gtk::Action::create("EditUncomment", "Block uncomment"), sigc::mem_fun(*this, &MainWindow::on_menu_edit_uncomment));

   _refActionGroup->add( Gtk::Action::create("ProjectInclude", "Include"), sigc::mem_fun(*this, &MainWindow::on_menu_project_include));
   _refActionGroup->add( Gtk::Action::create("ProjectExclude", "Exclude"), sigc::mem_fun(*this, &MainWindow::on_menu_project_exclude));
   _refActionGroup->add( Gtk::Action::create("ProjectCompile", "Compile"), sigc::mem_fun(*this, &MainWindow::on_menu_project_compile));
   _refActionGroup->add( Gtk::Action::create("ProjectCleanup", "Clean up"), sigc::mem_fun(*this, &MainWindow::on_menu_project_cleanup));
   _refActionGroup->add( Gtk::Action::create("ProjectForwards", "Forwards..."), sigc::mem_fun(*this, &MainWindow::on_menu_project_forwards));
   _refActionGroup->add( Gtk::Action::create("ProjectOptions", "Options..."), sigc::mem_fun(*this, &MainWindow::on_menu_project_options));

   loadUI(ui_info, "/MenuBar");
}

void MainWindow :: populateToolbar()
{
   //Create the toolbar and add it to a container widget:
   Gtk::ToolButton* button = Gtk::manage(new Gtk::ToolButton());
   button->set_icon_name("document-new");

   //We can't do this until we can break the ToolButton ABI: button->set_detailed_action_name("example.new");
   gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "IDE.FileNewSource");
   _toolbar.add(*button);

   _box.pack_start(_toolbar, Gtk::PACK_SHRINK);
}

int MainWindow :: newDocument(const char* name, Document* doc)
{
   return _mainFrame.newDocument(name, doc);
}

int MainWindow :: getCurrentDocumentIndex()
{
   return _mainFrame.getCurrentIndex();
}

void MainWindow :: closeDocument(int index)
{
   _mainFrame.eraseDocumentTab(index);
}

void MainWindow :: selectDocument(int docIndex)
{
   _mainFrame.selectTab(docIndex);
}

void MainWindow :: refreshDocument()
{
   _mainFrame.refreshDocument();
}

bool MainWindow :: copyToClipboard(Document* document)
{
   int length = document->getSelectionLength();
   char* text = _ELENA_::StringHelper::allocate(length, DEFAULT_STR);

   document->copySelection(text);

   _clipboard.settext(text);

   _ELENA_::freestr(text);

   return true;
}

void MainWindow :: pasteFrameClipboard(Document* document)
{
   char* text = _clipboard.gettext();
   if  (!_ELENA_::emptystr(text)) {
      document->insertLine(text, _ELENA_::getlength(text));

      _clipboard.freetext(text);
   }
}

void MainWindow :: reloadProjectView(_ProjectManager* project)
{
    _projectTree->clear();

   _ELENA_::ConfigCategoryIterator p_it = project->SourceFiles();
   int index = 0;
   while (!p_it.Eof()) {
      _ELENA_::ident_t name = p_it.key();
      Gtk::TreeModel::Children children = _projectTree->children();

      int start = 0;
      int end = 0;
      while (end != -1) {
         end = _ELENA_::StringHelper::find(name + start, PATH_SEPARATOR);

         _ELENA_::IdentifierString nodeName(name + start, (end == -1 ? _ELENA_::getlength(name) : end) - start);
         Gtk::TreeModel::iterator it = children.begin();
         while (it != children.end()) {
            Gtk::TreeModel::Row row = *it;
            Glib::ustring current = row[_projectTreeColumns._caption];

            if (nodeName.compare(current.c_str()))
               break;

            it++;
         }
         if (it == children.end()) {
            Gtk::TreeModel::Row row = *(_projectTree->append(children));
            row[_projectTreeColumns._caption] = (const char*)nodeName;
            row[_projectTreeColumns._index] = end == -1 ? index : -1;

            children = row.children();
         }
         else children = (*it).children();

         start = end + 1;
      }

      p_it++;
      index++;
   }

//   projectView->expand(root);
   show_all_children();
}

bool MainWindow :: compileProject(_ProjectManager* manager, int postponedAction)
{
   _output.get_buffer()->set_text("");

   _outputProcess.setArgument(_model->project.path, _model->project.name, "project");

   _outputThread = Glib::Threads::Thread::create(
      sigc::bind(sigc::mem_fun(_outputProcess, &OutputProcess::compile), this));
}

void MainWindow :: on_notification_from_output()
{
   if (_outputProcess.isStopped()) {
      _outputThread->join();
      _outputThread = NULL;
   }
   else _outputProcess.writeOut(_output);
}

void MainWindow :: notifyOutput()
{
   _outputDispatcher.emit();
}

MainWindow :: MainWindow(const char* caption, _Controller* controller, Model* model)
   : SDIWindow(caption), _mainFrame(model), _outputThread(NULL)
{
   _controller = controller;
   _model = model;

   populateMenu();
   populateToolbar();

   // !! temporal
   _statusbar.push("Example");

   _projectTree = Gtk::TreeStore::create(_projectTreeColumns);
   _projectView.set_model(_projectTree);

   _projectView.append_column("module", _projectTreeColumns._caption);

   _projectView.signal_row_activated().connect(sigc::mem_fun(*this,
              &MainWindow::on_projectview_row_activated));

   _outputScroller.add(_output);
   _bottomTab.append_page(_outputScroller, "Output");

   populate(&_mainFrame, &_projectView, &_bottomTab, &_statusbar);

   //_output.set_editable(false);

   _outputDispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_notification_from_output));
}
