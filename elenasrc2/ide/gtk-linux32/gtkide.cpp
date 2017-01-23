//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ GTK IDE
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

//#include "gtk-linux32/gtkcommon.h"
//#include "gtk-linux32/gtksdi.h"
//#include "gtk-linux32/gtkmenu.h"
//#include "gtk-linux32/gtktoolbar.h"
//#include "gtk-linux32/gtkstatusbar.h"
//#include "gtkeditframe.h"

#include "gtkide.h"
#include <sys/wait.h>
//#include<pthread.h>

#define COMPILER_PATH "/usr/bin/elena-lc"
#define COMPILER_NAME "elena-lc"
#define PIPE_READ 0
#define PIPE_WRITE 1

using namespace _GUI_;

bool GTKIDEWindow::OutputProcess :: isStopped()
{
   return stopped;
}

void GTKIDEWindow::OutputProcess :: writeOut(Gtk::TextView& view)
{
   Glib::Threads::Mutex::Lock lock(_mutex);

   Gtk::TextIter iter = view.get_buffer()->end();
   view.get_buffer()->insert(iter, buffer, buffer + buf_len);

   buf_len = 0;
}

void GTKIDEWindow::OutputProcess :: compile(GTKIDEWindow* owner)
{
   stopped = false;
   exitCode = 0;

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

      int status;
      waitpid(child, &status, 0);
      if (WIFEXITED(status)) {
         exitCode = WEXITSTATUS(status);
      }

      stopped = true;
      owner->notifyCompletion(exitCode);
   }
   else {
      // failed to create child
      ::close(stdinPipe[PIPE_READ]);
      ::close(stdinPipe[PIPE_WRITE]);
      ::close(stdoutPipe[PIPE_READ]);
      ::close(stdoutPipe[PIPE_WRITE]);
   }
}

// --- StatusLine ---

void GTKIDEWindow::StatusLine :: set(int index, text_t message)
{
   switch(index) {
      case 0:
         line1.copy(message);
         break;
      case 1:
         line2.copy(message);
         break;
      case 2:
         line3.copy(message);
         break;
      case 3:
         line4.copy(message);
         break;
   }
}

void GTKIDEWindow::StatusLine :: show(Gtk::Statusbar& statusbar)
{
   _ELENA_::String<char, 512> info;
   if (line1.Length() > 100) {
      info.copy(line1, 100);
   }
   else info.copy(line1);

   while (info.Length() < 101)
      info.append(' ');

   info.append(line2);

   while (info.Length() < 122)
      info.append(' ');

   info.append(line3);

   while (info.Length() < 143)
      info.append(' ');

   info.append(line4);

   while (info.Length() < 164)
      info.append(' ');

   statusbar.pop();
   statusbar.push(info.str());
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
        "      <menu action='FileRecentFiles'>"
        "           <menuitem action='FileRecentFilesClear'/>"
        "      </menu>"
        "      <menu action='FileRecentProjects'>"
        "           <menuitem action='FileRecentProjectsClear'/>"
        "      </menu>"
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
        "    <menu action='ViewMenu'>"
        "      <menuitem action='ProjectView'/>"
        "      <separator/>"
        "      <menuitem action='ProjectOutput'/>"
        "      <menuitem action='ProjectMessages'/>"
        "      <menuitem action='ProjectWatch'/>"
        "      <menuitem action='ProjectCallstack'/>"
        "      <menuitem action='ProjectConsole'/>"
        "    </menu>"
        "    <menu action='SearchMenu'>"
        "      <menuitem action='Search'/>"
        "      <menuitem action='SearchNext'/>"
        "      <separator/>"
        "      <menuitem action='Replace'/>"
        "      <menuitem action='ReplaceNext'/>"
        "      <separator/>"
        "      <menuitem action='GoToLine'/>"
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
        "    <menu action='DebugMenu'>"
        "      <menuitem action='DebugRun'/>"
        "      <menuitem action='DebugStepover'/>"
        "      <menuitem action='DebugStepin'/>"
        "      <menuitem action='DebugGoto'/>"
        "      <separator/>"
        "      <menuitem action='DebugToggle'/>"
        "      <menuitem action='DebugClearBreakpoints'/>"
        "      <separator/>"
        "      <menuitem action='DebugGotoSource'/>"
        "      <separator/>"
        "      <menuitem action='DebugStop'/>"
        "    </menu>"
        "    <menu action='ToolsMenu'>"
        "      <menuitem action='ToolsEditor'/>"
        "      <menuitem action='ToolsDebugger'/>"
        "    </menu>"
        "    <menu action='WindowMenu'>"
        "      <menuitem action='WindowNext'/>"
        "      <menuitem action='WindowPrevious'/>"
        "      <separator/>"
        "      <menuitem action='WindowWindows'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='HelpAPI'/>"
        "      <separator/>"
        "      <menuitem action='HelpAbout'/>"
        "    </menu>"
        "  </menubar>"
//        "  <toolbar  name='ToolBar'>"
//        "    <toolitem action='FileNewStandard'/>"
//        "    <toolitem action='FileQuit'/>"
//        "  </toolbar>"
        "</ui>";

// --- GTKIDEWindow ---

void GTKIDEWindow :: populateMenu()
{
   //File menu:
   _refActionGroup->add( Gtk::Action::create("FileMenu", "_File") );
   _refActionGroup->add( Gtk::Action::create("EditMenu", "_Edit") );
   _refActionGroup->add( Gtk::Action::create("ProjectMenu", "_Project") );
   _refActionGroup->add( Gtk::Action::create("ViewMenu", "_View") );
   _refActionGroup->add( Gtk::Action::create("DebugMenu", "_Debug") );
   _refActionGroup->add( Gtk::Action::create("ToolsMenu", "_Tools") );
   _refActionGroup->add( Gtk::Action::create("WindowMenu", "_Window") );
   _refActionGroup->add( Gtk::Action::create("HelpMenu", "_Help") );
   _refActionGroup->add( Gtk::Action::create("SearchMenu", "_Search") );

   _refActionGroup->add( Gtk::Action::create("FileNew", "New") );
   _refActionGroup->add( Gtk::Action::create("FileNewSource", "Source"), Gtk::AccelKey("<control>N"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_new_source));
   _refActionGroup->add( Gtk::Action::create("FileNewProject", "Project"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_new_project));
   _refActionGroup->add( Gtk::Action::create("FileOpen", "Open") );
   _refActionGroup->add( Gtk::Action::create("FileOpenSource", "Source"), Gtk::AccelKey("<control>O"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_open_source));
   _refActionGroup->add( Gtk::Action::create("FileOpenProject", "Project"), Gtk::AccelKey("<control><shift>O"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_open_project));
   _refActionGroup->add( Gtk::Action::create("FileSave", "Save"), Gtk::AccelKey("<control>S"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_save));
   _refActionGroup->add( Gtk::Action::create("FileSaveAs", "Save As..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_saveas));
   _refActionGroup->add( Gtk::Action::create("FileProjectAs", "Save Project As..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_saveas));
   _refActionGroup->add( Gtk::Action::create("FileSaveAll", "Save All"), Gtk::AccelKey("<control><shift>S"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_saveall));
   _refActionGroup->add( Gtk::Action::create("FileClose", "Close"), Gtk::AccelKey("<control>W"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_close));
   _refActionGroup->add( Gtk::Action::create("FileCloseAll", "Close All"), Gtk::AccelKey("<control><shift>W"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_closeall));
   _refActionGroup->add( Gtk::Action::create("ProjectClose", "Close Project"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_close));
   _refActionGroup->add( Gtk::Action::create("FileCloseAllButActive", "Close All But Active"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_closeproject));
   _refActionGroup->add( Gtk::Action::create("FileQuit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_quit));

   _refActionGroup->add( Gtk::Action::create("FileRecentFiles", "Recent files") );
   _refActionGroup->add( Gtk::Action::create("FileRecentProjects", "Recent projects") );
   _refActionGroup->add( Gtk::Action::create("FileRecentFilesClear", "Clear history"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_clearfilehistory));
   _refActionGroup->add( Gtk::Action::create("FileRecentProjectsClear", "Clear history"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_clearprojecthistory));

   _refActionGroup->add( Gtk::Action::create("EditUndo", "Undo"), Gtk::AccelKey("<control>Z"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_undo));
   _refActionGroup->add( Gtk::Action::create("EditRedo", "Redo"), Gtk::AccelKey("<control>Y"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_redo));
   _refActionGroup->add( Gtk::Action::create("EditCut", "Cut"), Gtk::AccelKey("<control>X"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_cut));
   _refActionGroup->add( Gtk::Action::create("EditCopy", "Copy"), Gtk::AccelKey("<control>C"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_copy));
   _refActionGroup->add( Gtk::Action::create("EditPaste", "Paste"), Gtk::AccelKey("<control>V"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_paste));
   _refActionGroup->add( Gtk::Action::create("EditDelete", "Delete"), Gtk::AccelKey("Delete"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_delete));
   _refActionGroup->add( Gtk::Action::create("EditSelectAll", "Select All"), Gtk::AccelKey("<control>A"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_select_all));
   _refActionGroup->add( Gtk::Action::create("EditInsertTab", "Insert Tab (Indent)"), Gtk::AccelKey("TAB"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_indent));
   _refActionGroup->add( Gtk::Action::create("EditRemoveTab", "Remove Tab (Outdent)"), Gtk::AccelKey("<shift>TAB"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_outdent));
   _refActionGroup->add( Gtk::Action::create("EditTrim", "Trim whitespace"), Gtk::AccelKey("<control>T"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_trim));
   _refActionGroup->add( Gtk::Action::create("EditEraseLine", "Erase line"), Gtk::AccelKey("<control>L"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_erase_line));
   _refActionGroup->add( Gtk::Action::create("EditUpper", "To upper case"), Gtk::AccelKey("<control>U"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_upper));
   _refActionGroup->add( Gtk::Action::create("EditLower", "To lower case"), Gtk::AccelKey("<control><shift>U"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_lower));
   _refActionGroup->add( Gtk::Action::create("EditComment", "Block comment"), Gtk::AccelKey("<control>K"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_comment));
   _refActionGroup->add( Gtk::Action::create("EditUncomment", "Block uncomment"), Gtk::AccelKey("<control><shift>K"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_uncomment));

   _refActionGroup->add( Gtk::Action::create("ProjectView", "Project View"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_view));
   _refActionGroup->add( Gtk::ToggleAction::create("ProjectOutput", "Compiler output"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_output));
   _refActionGroup->add( Gtk::ToggleAction::create("ProjectMessages", "Messages"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_messages));
   _refActionGroup->add( Gtk::Action::create("ProjectWatch", "Debug Watch"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_watch));
   _refActionGroup->add( Gtk::Action::create("ProjectCallstack", "Call stack"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_callstack));
   _refActionGroup->add( Gtk::Action::create("ProjectConsole", "ELENA Interactive"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_interactive));

   _refActionGroup->add( Gtk::Action::create("Search", "Find..."), Gtk::AccelKey("<control>F"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_find));
   _refActionGroup->add( Gtk::Action::create("SearchNext", "Find Next"), Gtk::AccelKey("F3"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_findnext));
   _refActionGroup->add( Gtk::Action::create("Replace", "Replace..."), Gtk::AccelKey("<control>R"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_replace));
   _refActionGroup->add( Gtk::Action::create("ReplaceNext", "Replace Next"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_replacenext));
   _refActionGroup->add( Gtk::Action::create("GoToLine", "Go to line..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_gotoline));

   _refActionGroup->add( Gtk::Action::create("ProjectInclude", "Include"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_include));
   _refActionGroup->add( Gtk::Action::create("ProjectExclude", "Exclude"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_exclude));
   _refActionGroup->add( Gtk::Action::create("ProjectCompile", "Compile"), Gtk::AccelKey("<control>F9"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_compile));
   _refActionGroup->add( Gtk::Action::create("ProjectCleanup", "Clean up"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_cleanup));
   _refActionGroup->add( Gtk::Action::create("ProjectForwards", "Forwards..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_forwards));
   _refActionGroup->add( Gtk::Action::create("ProjectOptions", "Options..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_options));


   _refActionGroup->add( Gtk::Action::create("DebugRun", "Run"), Gtk::AccelKey("F9"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_run));
   _refActionGroup->add( Gtk::Action::create("DebugStepover", "Step Over"), Gtk::AccelKey("F8"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stepover));
   _refActionGroup->add( Gtk::Action::create("DebugStepin", "Step In"), Gtk::AccelKey("F7"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stepin));
   _refActionGroup->add( Gtk::Action::create("DebugGoto", "Go To Cursor"), Gtk::AccelKey("F4"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_goto));
   _refActionGroup->add( Gtk::Action::create("DebugToggle", "Toggle Breakpoint"), Gtk::AccelKey("F5"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_toggle));
   _refActionGroup->add( Gtk::Action::create("DebugClearBreakpoints", "Clear All Breakpoints"), Gtk::AccelKey("<control><shift>F5"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_clearbps));
   _refActionGroup->add( Gtk::Action::create("DebugGotoSource", "Go To Source"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_source));
   _refActionGroup->add( Gtk::Action::create("DebugStop", "Stop Execution"), Gtk::AccelKey("<control>F2"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stop));

   _refActionGroup->add( Gtk::Action::create("ToolsEditor", "Editor Options..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_tools_editor));
   _refActionGroup->add( Gtk::Action::create("ToolsDebugger", "Debugger Options..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_tools_debugger));

   _refActionGroup->add( Gtk::Action::create("WindowNext", "Next"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_window_next));
   _refActionGroup->add( Gtk::Action::create("WindowPrevious", "Previous"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_window_prev));
   _refActionGroup->add( Gtk::Action::create("WindowWindows", "Window List..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_windows));

   _refActionGroup->add( Gtk::Action::create("HelpAPI", "ELENA API..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_help_api));
   _refActionGroup->add( Gtk::Action::create("HelpAbout", "About..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_help_about));

   loadUI(ui_info, "/MenuBar");
}

Glib::RefPtr<Gtk::Action> GTKIDEWindow :: getMenuItem(int id)
{
   switch(id) {
      case IDM_FILE_CLOSE:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileClose");
      case IDM_FILE_CLOSEALL:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileCloseAll");
      case IDM_FILE_CLOSEALLBUT:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileCloseAllButActive");
      case IDM_PROJECT_CLOSE:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/ProjectClose");
      case IDM_FILE_SAVEPROJECT:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileProjectAs");
      case IDM_FILE_SAVE:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileSave");
      case IDM_FILE_SAVEAS:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileSaveAs");
      case IDM_FILE_SAVEALL:
         return _refUIManager->get_action("/ui/MenuBar/FileMenu/FileSaveAll");
      case IDM_EDIT_UNDO:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditUndo");
      case IDM_EDIT_REDO:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditRedo");
      case IDM_EDIT_COPY:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditCopy");
      case IDM_EDIT_PASTE:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditPaste");
      case IDM_EDIT_CUT:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditCut");
      case IDM_EDIT_DELETE:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditDelete");
      case IDM_EDIT_SELECTALL:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditSelectAll");
      case IDM_EDIT_TRIM:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditTrim");
      case IDM_EDIT_ERASELINE:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditEraseLine");
//      case IDM_EDIT_DUPLICATE:
//         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditTrim");
      case IDM_EDIT_COMMENT:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditComment");
      case IDM_EDIT_UNCOMMENT:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditUncomment");
      case IDM_EDIT_UPPERCASE:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditUpper");
      case IDM_EDIT_LOWERCASE:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditLower");
      case IDM_EDIT_INDENT:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditInsertTab");
      case IDM_EDIT_OUTDENT:
         return _refUIManager->get_action("/ui/MenuBar/EditMenu/EditRemoveTab");
      case IDM_SEARCH_FIND:
         return _refUIManager->get_action("/ui/MenuBar/SearchMenu/Search");
      case IDM_SEARCH_FINDNEXT:
         return _refUIManager->get_action("/ui/MenuBar/SearchMenu/SearchNext");
      case IDM_SEARCH_REPLACE:
         return _refUIManager->get_action("/ui/MenuBar/SearchMenu/Replace");
      case IDM_SEARCH_GOTOLINE:
         return _refUIManager->get_action("/ui/MenuBar/SearchMenu/GoToLine");
      case IDM_VIEW_OUTPUT:
         return _refUIManager->get_action("/ui/MenuBar/ViewMenu/ProjectOutput");
      case IDM_VIEW_WATCH:
         return _refUIManager->get_action("/ui/MenuBar/ViewMenu/ProjectWatch");
      case IDM_VIEW_CALLSTACK:
         return _refUIManager->get_action("/ui/MenuBar/ViewMenu/ProjectCallstack");
      case IDM_VIEW_MESSAGES:
         return _refUIManager->get_action("/ui/MenuBar/ViewMenu/ProjectMessages");
      case IDM_VIEW_PROJECTVIEW:
         return _refUIManager->get_action("/ui/MenuBar/ViewMenu/ProjectView");
      case IDM_VIEW_VMCONSOLE:
         return _refUIManager->get_action("/ui/MenuBar/ViewMenu/ProjectConsole");

      case IDM_PROJECT_COMPILE:
         return _refUIManager->get_action("/ui/MenuBar/ProjectMenu/ProjectCompile");
      case IDM_PROJECT_OPTION:
         return _refUIManager->get_action("/ui/MenuBar/ProjectMenu/ProjectOptions");
      case IDM_PROJECT_FORWARDS:
         return _refUIManager->get_action("/ui/MenuBar/ProjectMenu/ProjectForwards");
      case IDM_DEBUG_RUN:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugRun");
      case IDM_DEBUG_RUNTO:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugGoto");
      case IDM_DEBUG_STEPOVER:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugStepover");
      case IDM_DEBUG_STEPINTO:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugStepin");
      case IDM_DEBUG_STOP:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugStop");
//      case IDM_DEBUG_INSPECT:
//         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/ProjectConsole");
//      case IDM_DEBUG_SWITCHHEXVIEW:
//         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/ProjectConsole");
      case IDM_DEBUG_BREAKPOINT:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugToggle");
      case IDM_DEBUG_CLEARBREAKPOINT:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugClearBreakpoints");
      case IDM_DEBUG_GOTOSOURCE:
         return _refUIManager->get_action("/ui/MenuBar/DebugMenu/DebugGotoSource");
      case IDM_PROJECT_INCLUDE:
         return _refUIManager->get_action("/ui/MenuBar/ProjectMenu/ProjectInclude");
      case IDM_PROJECT_EXCLUDE:
         return _refUIManager->get_action("/ui/MenuBar/ProjectMenu/ProjectExclude");
      case IDM_PROJECT_CLEAN:
         return _refUIManager->get_action("/ui/MenuBar/ProjectMenu/ProjectCleanup");
      default:
         return Glib::RefPtr<Gtk::Action>();
   }
}

void GTKIDEWindow :: populateToolbar()
{
   //Create the toolbar and add it to a container widget:
   Gtk::ToolButton* button = Gtk::manage(new Gtk::ToolButton());
   button->set_icon_name("document-new");

   //We can't do this until we can break the ToolButton ABI: button->set_detailed_action_name("example.new");
   gtk_actionable_set_detailed_action_name (GTK_ACTIONABLE (button->gobj()), "IDE.FileNewSource");
   _toolbar.add(*button);

   _box.pack_start(_toolbar, Gtk::PACK_SHRINK);
}

int GTKIDEWindow :: newDocument(const char* name, Document* doc)
{
   return _mainFrame.newDocument(name, doc);
}

int GTKIDEWindow :: getCurrentDocumentIndex()
{
   return _mainFrame.getCurrentIndex();
}

void GTKIDEWindow :: closeDocument(int index)
{
   _mainFrame.eraseDocumentTab(index);
}

void GTKIDEWindow :: selectDocument(int docIndex)
{
   _mainFrame.selectTab(docIndex);
}

void GTKIDEWindow :: refreshDocument()
{
   _mainFrame.refreshDocument();
}

bool GTKIDEWindow :: copyToClipboard(Document* document)
{
   int length = document->getSelectionLength();
   char* text = _ELENA_::StrFactory::allocate(length, DEFAULT_STR);

   document->copySelection(text);

   _clipboard.settext(text);

   _ELENA_::freestr(text);

   return true;
}

void GTKIDEWindow :: pasteFromClipboard(Document* document)
{
   char* text = _clipboard.gettext();
   if  (!_ELENA_::emptystr(text)) {
      document->insertLine(text, _ELENA_::getlength(text));

      _clipboard.freetext(text);
   }
}

void GTKIDEWindow :: reloadProjectView(_ProjectManager* project)
{
    _projectTree->clear();

   _ProjectManager::SourceIterator p_it = project->SourceFiles();
   int index = 0;
   while (!p_it.Eof()) {
      _ELENA_::ident_t name = *p_it;
      Gtk::TreeModel::Children children = _projectTree->children();

      int start = 0;
      int end = 0;
      while (end != -1) {
         end = name.find(start, PATH_SEPARATOR, -1);

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

bool GTKIDEWindow :: compileProject(_ProjectManager* manager, int postponedAction)
{
   _output.get_buffer()->set_text("");

   _outputProcess.setArgument(_model->project.path, _model->project.name, "project");

   _outputThread = Glib::Threads::Thread::create(
      sigc::bind(sigc::mem_fun(_outputProcess, &OutputProcess::compile), this));

   return true; //!! temporal
}

void GTKIDEWindow :: on_notification_from_output()
{
   if (_outputProcess.isStopped()) {
      _outputThread->join();
      _outputThread = NULL;
   }
   else _outputProcess.writeOut(_output);
}

void GTKIDEWindow :: on_notification_from_debugger()
{
   Glib::Threads::Mutex::Lock lock(_debugMutex);

   DebugMessage rec = _debugMessages.pop();
   switch (rec.message) {
      case dbgStart:
         _controller->onDebuggerStart();
         break;
      case dbgStep:
         _controller->onDebuggerStep(rec.strparam1, rec.strparam2, HighlightInfo(rec.nparam1, rec.nparam2, rec.nparam3));
         break;
   }
}

void GTKIDEWindow :: notifyOutput()
{
   _outputDispatcher.emit();
}

void GTKIDEWindow :: notifyCompletion(int errorCode)
{
   if (errorCode == 0) {
      _compilationSuccessDispatcher.emit();
   }
   else if (errorCode == -1) {
      _compilationWarningDispatcher.emit();
   }
   else _compilationErrorDispatcher.emit();
}

void GTKIDEWindow :: notityDebugStep(DebugMessage message)
{
   Glib::Threads::Mutex::Lock lock(_debugMutex);

   _debugMessages.push(message);
   _debugDispatcher.emit();
}

void GTKIDEWindow :: displayErrors()
{
   _ELENA_::String<char, 266> message, file;
   _ELENA_::String<char, 15> colStr, rowStr;

   Glib::ustring buffer = _output.get_buffer()->get_text();

   const char* s = buffer.c_str();

   while (s) {
      const char* err = strstr(s, ": error ");
      if (err==NULL) {
         err = strstr(s, ": warning ");
      }
      if (err==NULL)
         break;

      const char* line = err - 1;
      const char* row = NULL;
      const char* col = NULL;
      while (true) {
         if (*line=='(') {
            row = line + 1;
         }
         else if (*line==':' && col == NULL) {
            col = line + 1;
         }
         else if (*line == '\n')
            break;

         line--;
      }
      s = strchr(err, '\n');

      message.copy(err + 2, s - err- 3);
      if (row==NULL) {
         file.clear();
         colStr.clear();
         rowStr.clear();
      }
      else {
         file.copy(line + 1, row - line - 2);
         if (col != NULL) {
            rowStr.copy(row, col - row - 1);
            colStr.copy(col, err - col - 1);
         }
         else {
            rowStr.copy(row, err - row - 1);
            colStr.clear();
         }
      }

      Gtk::TreeModel::Row logRow = *(_messageList->append());
      logRow[_messageLogColumns._description] = message.str();
      logRow[_messageLogColumns._file] = file.str();
      logRow[_messageLogColumns._line] = rowStr.str();
      logRow[_messageLogColumns._column] = colStr.str();

      //break;
   }

   _controller->doShowCompilerOutput(true);
   if (_model->messages) {
      //((TabBar*)_controls[CTRL_TABBAR])->selectTabChild((Control*)_controls[CTRL_MESSAGELIST]);
   }
   else _controller->doShowMessages(true);

}

GTKIDEWindow :: GTKIDEWindow(const char* caption, _Controller* controller, Model* model)
   : SDIWindow(caption), _mainFrame(model), _outputThread(NULL), _debugMessages(DebugMessage())
{
   _controller = controller;
   _model = model;
   _skip = false;

   populateMenu();
   populateToolbar();

   _mainFrame.signal_switch_page().connect(sigc::mem_fun(*this,
           &GTKIDEWindow::on_client_change));

   // project tree
   _projectTree = Gtk::TreeStore::create(_projectTreeColumns);
   _projectView.set_model(_projectTree);

   _projectView.append_column("module", _projectTreeColumns._caption);

   _projectView.signal_row_activated().connect(sigc::mem_fun(*this,
              &GTKIDEWindow::on_projectview_row_activated));

   // message log
   _messageList = Gtk::TreeStore::create(_messageLogColumns);
   _messageLog.set_model(_messageList);

   _messageLog.append_column("description", _messageLogColumns._description);
   _messageLog.append_column("file", _messageLogColumns._file);
   _messageLog.append_column("line", _messageLogColumns._line);
   _messageLog.append_column("column", _messageLogColumns._column);

   _messageLog.get_column(0)->set_min_width(500);

   _messageLog.signal_row_activated().connect(sigc::mem_fun(*this,
              &GTKIDEWindow::on_messagelog_row_activated));

   _outputScroller.add(_output);
   //_bottomTab.append_page(_outputScroller, "Output");

   populate(&_mainFrame, &_projectView, &_bottomTab, &_statusbar);

   // set minimal sizes
   _outputScroller.set_size_request(-1, 80);
   _projectView.set_size_request(200, -1);

   //_output.set_editable(false);

   _mainFrame.textview_changed().connect(sigc::mem_fun(*this,
              &GTKIDEWindow::on_textview_changed) );

   _outputDispatcher.connect(sigc::mem_fun(*this, &GTKIDEWindow::on_notification_from_output));
   _debugDispatcher.connect(sigc::mem_fun(*this, &GTKIDEWindow::on_notification_from_debugger));
   _compilationSuccessDispatcher.connect(sigc::mem_fun(*this, &GTKIDEWindow::on_compilation_success));
   _compilationWarningDispatcher.connect(sigc::mem_fun(*this, &GTKIDEWindow::on_compilation_warning));
   _compilationErrorDispatcher.connect(sigc::mem_fun(*this, &GTKIDEWindow::on_compilation_error));
}
