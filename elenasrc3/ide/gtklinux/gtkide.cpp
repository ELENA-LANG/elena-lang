//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ GTK IDE
//                                             (C)2024-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkide.h"
#include "eng/messages.h"

using namespace elena_lang;

typedef Pair<GTKIDEWindow*, int, nullptr> FileCallbackArg;
typedef Triple<GTKIDEWindow*, CloseCallback, int, nullptr, nullptr> CloseCallbackArg;

static Glib::ustring ui_info =
        "<interface>"
        "  <menu id='MenuBar'>"
        "    <submenu>"
        "      <attribute name='label'>_File</attribute>"
        "      <submenu>"
        "         <attribute name='label'>_New</attribute>"
        "         <section>"
        "            <item>"
        "               <attribute name='label'>Source</attribute>"
        "               <attribute name='action'>win.FileNewSource</attribute>"
        "               <attribute name='accel'>&lt;Ctrl&gt;N</attribute>"
        "            </item>"
        "            <item>"
        "               <attribute name='label'>Project</attribute>"
        "               <attribute name='action'>FileNewProject</attribute>"
        "            </item>"
        "         </section>"
        "      </submenu>"
        "      <submenu>"
        "         <attribute name='label'>_Open</attribute>"
        "         <section>"
        "            <item>"
        "               <attribute name='label'>Source</attribute>"
        "               <attribute name='action'>win.FileOpenSource</attribute>"
        "               <attribute name='accel'>&lt;Ctrl&gt;O</attribute>"
        "            </item>"
        "            <item>"
        "               <attribute name='label'>Project</attribute>"
        "               <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;O</attribute>"
        "               <attribute name='action'>FileOpenProject</attribute>"
        "            </item>"
        "         </section>"
        "      </submenu>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>_Save</attribute>"
        "            <attribute name='action'>win.FileSaveSource</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;S</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Save As...</attribute>"
        "            <attribute name='action'>win.FileSaveAsSource</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Save _Project As...</attribute>"
        "            <attribute name='action'>FileProjectAs</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Save _All</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;S</attribute>"
        "            <attribute name='action'>win.FileSaveAll</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Close</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;W</attribute>"
        "            <attribute name='action'>win.FileCloseSource</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Close All</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;W</attribute>"
        "            <attribute name='action'>win.FileCloseAll</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Close Project</attribute>"
        "            <attribute name='action'>ProjectClose</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Close All But Active</attribute>"
        "            <attribute name='action'>win.FileCloseAllButActive</attribute>"
        "         </item>"
        "      </section>"
        "      <submenu>"
        "         <attribute name='label'>Recent files</attribute>"
        "         <attribute name='action'>FileRecentFiles</attribute>"
        "         <section>"
        "            <item>"
        "               <attribute name='label'>Clear history</attribute>"
        "               <attribute name='action'>FileRecentFilesClear</attribute>"
        "            </item>"
        "         </section>"
        "      </submenu>"
        "      <submenu>"
        "         <attribute name='label'>Recent projects</attribute>"
        "         <attribute name='action'>FileRecentProjects</attribute>"
        "         <section>"
        "            <item>"
        "               <attribute name='label'>Clear history</attribute>"
        "               <attribute name='action'>FileRecentProjectsClear</attribute>"
        "            </item>"
        "         </section>"
        "      </submenu>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Exit</attribute>"
        "            <attribute name='accel'>&lt;Alt&gt;F4</attribute>"
        "            <attribute name='action'>win.FileQuit</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Edit</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Undo</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;Z</attribute>"
        "            <attribute name='action'>win.EditUndo</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Redo</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;Y</attribute>"
        "            <attribute name='action'>win.EditRedo</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Cut</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;X</attribute>"
        "            <attribute name='action'>win.EditCut</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Copy</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;C</attribute>"
        "            <attribute name='action'>win.EditCopy</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Paste</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;V</attribute>"
        "            <attribute name='action'>win.EditPaste</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Delete</attribute>"
        "            <attribute name='accel'>Delete</attribute>"
        "            <attribute name='action'>win.EditDelete</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Select All</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;A</attribute>"
        "            <attribute name='action'>win.EditSelectAll</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Insert Tab (Indent)</attribute>"
        "            <attribute name='accel'>TAB</attribute>"
        "            <attribute name='action'>win.EditInsertTab</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Remove Tab (Outdent)</attribute>"
        "            <attribute name='accel'>&lt;Shift&gt;TAB</attribute>"
        "            <attribute name='action'>win.EditRemoveTab</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Trim whitespace</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;T</attribute>"
        "            <attribute name='action'>win.EditTrim</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Erase line</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;L</attribute>"
        "            <attribute name='action'>win.EditEraseLine</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>To upper case</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;U</attribute>"
        "            <attribute name='action'>win.EditUpper</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>To lower case</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;U</attribute>"
        "            <attribute name='action'>win.EditLower</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Block comment</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;K</attribute>"
        "            <attribute name='action'>win.EditComment</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Block uncomment</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;&lt;Shift&gt;K</attribute>"
        "            <attribute name='action'>win.EditUncomment</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_View</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Project View</attribute>"
        "            <attribute name='action'>ProjectView</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Compiler output</attribute>"
        "            <attribute name='action'>ProjectOutput</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Messages</attribute>"
        "            <attribute name='action'>ProjectMessages</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Debug Watch</attribute>"
        "            <attribute name='action'>ProjectWatch</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Call stack</attribute>"
        "            <attribute name='action'>ProjectCallstack</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>ELENA Interactive</attribute>"
        "            <attribute name='action'>ProjectConsole</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Search</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Find...</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;F</attribute>"
        "            <attribute name='action'>Search</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Find Next</attribute>"
        "            <attribute name='accel'>F3</attribute>"
        "            <attribute name='action'>SearchNext</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Replace...</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;R</attribute>"
        "            <attribute name='action'>Replace</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Replace Next</attribute>"
        "            <attribute name='action'>ReplaceNext</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Go to line...</attribute>"
        "            <attribute name='action'>GoToLine</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Project</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Include</attribute>"
        "            <attribute name='action'>ProjectInclude</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Exclude</attribute>"
        "            <attribute name='action'>ProjectExclude</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Compile</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;F9</attribute>"
        "            <attribute name='action'>ProjectCompile</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Clean up</attribute>"
        "            <attribute name='action'>ProjectCleanup</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Forwards...</attribute>"
        "            <attribute name='action'>ProjectForwards</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Options...</attribute>"
        "            <attribute name='action'>ProjectOptions</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Debug</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Run</attribute>"
        "            <attribute name='accel'>F9</attribute>"
        "            <attribute name='action'>DebugRun</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Step Over</attribute>"
        "            <attribute name='accel'>F8</attribute>"
        "            <attribute name='action'>DebugStepover</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Step In</attribute>"
        "            <attribute name='accel'>F7</attribute>"
        "            <attribute name='action'>DebugStepin</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Go To Cursor</attribute>"
        "            <attribute name='accel'>F4</attribute>"
        "            <attribute name='action'>DebugGoto</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Toggle Breakpoint</attribute>"
        "            <attribute name='accel'>F5</attribute>"
        "            <attribute name='action'>DebugToggle</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Clear All Breakpoints</attribute>"
        "            <attribute name='accel'>&lt;Shift&gt;F5</attribute>"
        "            <attribute name='action'>DebugClearBreakpoints</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Go to source</attribute>"
        "            <attribute name='action'>DebugGotoSource</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Stop Execution</attribute>"
        "            <attribute name='accel'>&lt;Ctrl&gt;F2</attribute>"
        "            <attribute name='action'>DebugStop</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Tools</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Editor Options...</attribute>"
        "            <attribute name='action'>ToolsEditor</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Debugger Options...</attribute>"
        "            <attribute name='action'>ToolsDebugger</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Window</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Next</attribute>"
        "            <attribute name='action'>WindowNext</attribute>"
        "         </item>"
        "         <item>"
        "            <attribute name='label'>Previous</attribute>"
        "            <attribute name='action'>WindowPrevious</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>Window List...</attribute>"
        "            <attribute name='action'>WindowWindows</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "    <submenu>"
        "      <attribute name='label'>_Help</attribute>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>ELENA API...</attribute>"
        "            <attribute name='action'>HelpAPI</attribute>"
        "         </item>"
        "      </section>"
        "      <section>"
        "         <item>"
        "            <attribute name='label'>About...</attribute>"
        "            <attribute name='action'>HelpAbout</attribute>"
        "         </item>"
        "      </section>"
        "    </submenu>"
        "  </menu>"
        "</interface>";

const char* SOURCE_FILE_FILTER[] =
{
   "*.l",
   "ELENA source file",
   "*",
   "Any files"
};

const char* PROJECT_FILE_FILTER[] =
{
   "*.prj",
   "ELENA project file",
   "*",
   "Any files"
};

// --- GTKIDEWindow::Clipboard ---

void GTKIDEWindow::Clipboard :: copyToClipboard(DocumentView* docView, bool selectionMode)
{
   size_t len = selectionMode ? docView->getSelectionLength() + 1 : docView->getCurrentLineLength() + 1;
   if (len < 511) {
      char buffer[512];
      if (selectionMode) {
         docView->copySelection(buffer);
      }
      else docView->copyCurrentLine(buffer);

      _clipboard->set_text(buffer);
   }
   else {
      DynamicString<char> buffer;
      buffer.allocate(len);

      if (selectionMode) {
         docView->copySelection((char*)buffer.str());
      }
      else docView->copyCurrentLine((char*)buffer.str());

      _clipboard->set_text(buffer.str());
   }
}

void GTKIDEWindow::Clipboard :: pasteFromClipboard()
{
   _clipboard->read_text_async(sigc::mem_fun(*this,
              &GTKIDEWindow::Clipboard::on_clipboard_received));
}

void GTKIDEWindow::Clipboard :: on_clipboard_received(Glib::RefPtr<Gio::AsyncResult>& result)
{
   auto text = _clipboard->read_text_finish(result);

   _owner->_controller->sourceController.pasteFromClipboard(_owner->_model->viewModel(), text.c_str());
}

// --- GTKIDEWindow ---

GTKIDEWindow :: GTKIDEWindow(IDEController* controller, IDEModel* model, GtkApp* app)
   : _clipboard(this),
     fileDialog(this, SOURCE_FILE_FILTER, 4, OPEN_FILE_CAPTION, *model->projectModel.paths.lastPath),
     projectDialog(this, PROJECT_FILE_FILTER, 4, OPEN_PROJECT_CAPTION, *model->projectModel.paths.lastPath),
     messageDialog(this), projectSettingsDialog(this, &model->projectModel)
{
   _closing = false;
   _projectMode = false;
   _newMode = false;

   _app = app;

   _model = model;
   _controller = controller;

   //_projectTree = Gtk::TreeStore::create(_projectTreeColumns);

   populateUI();

   set_size_request(800, 600);
}

void GTKIDEWindow :: populate(int counter, Gtk::Widget** children)
{
   SDIWindow::populate(counter, children);

//   Gtk::TreeView* projectView = (Gtk::TreeView*)_children[_model->ideScheme.projectView];

   // project tree
//   projectView->set_model(_projectTree);

//   projectView->append_column("module", _projectTreeColumns._caption);
//
//   projectView->signal_row_activated().connect(sigc::mem_fun(*this,
//              &GTKIDEWindow::on_projectview_row_activated));
}

void GTKIDEWindow :: populateUI()
{
   auto refActions = Gio::SimpleActionGroup::create();

   refActions->add_action("FileNewSource", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_new_source));
//   _app->add_action("FileNewProject", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_new_project));
   refActions->add_action("FileOpenSource", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_open_source));
//   _app->add_action("FileOpenProject", "<control><shift>O", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_open_project));
   refActions->add_action("FileSaveSource", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_save));
   refActions->add_action("FileSaveAsSource", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_saveas));
//   _app->add_action("FileProjectAs", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_saveas));
   refActions->add_action("FileSaveAll", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_saveall));
//   _app->add_action("FileClose", "<control>W", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_close));
   refActions->add_action("FileCloseSource", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_close));
   refActions->add_action("FileCloseAll", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_closeall));
//   _app->add_action("ProjectClose", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_close));
//   _app->add_action("FileCloseAllButActive", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_closeproject));
   refActions->add_action("FileCloseAllButActive", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_closeallbutactive));
//   _app->add_action("FileQuit", /*"<alt>F4", */sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_quit));
   refActions->add_action("FileQuit", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_quit));

//   _app->add_action("FileRecentFilesClear", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_clearfilehistory));
//   _app->add_action("FileRecentProjectsClear", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_clearprojecthistory));

   refActions->add_action("EditUndo", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_undo));
   refActions->add_action("EditRedo", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_redo));
   refActions->add_action("EditCut", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_cut));
   refActions->add_action("EditCopy", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_copy));
   refActions->add_action("EditPaste", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_paste));
   refActions->add_action("EditDelete", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_delete));
   refActions->add_action("EditSelectAll", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_select_all));
   refActions->add_action("EditInsertTab", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_indent));
   refActions->add_action("EditRemoveTab", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_outdent));
   refActions->add_action("EditTrim", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_trim));
   refActions->add_action("EditEraseLine", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_erase_line));
   refActions->add_action("EditUpper", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_upper));
   refActions->add_action("EditLower", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_lower));
   refActions->add_action("EditComment", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_comment));
   refActions->add_action("EditUncomment", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_edit_uncomment));

//   _app->add_action("ProjectView", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_view));
//   _app->add_action("ProjectOutput", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_output));
//   _app->add_action("ProjectMessages", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_messages));
//   _app->add_action("ProjectWatch", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_watch));
//   _app->add_action("ProjectCallstack", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_callstack));
//   _app->add_action("ProjectConsole", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_interactive));
//
//   _app->add_action("Search", "<control>F", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_find));
//   _app->add_action("SearchNext", "F3", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_findnext));
//   _app->add_action("Replace", "<control>R", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_replace));
//   _app->add_action("ReplaceNext", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_replacenext));
//   _app->add_action("GoToLine", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_gotoline));
//
//   _app->add_action("ProjectInclude", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_include));
//   _app->add_action("ProjectExclude", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_exclude));
//   _app->add_action("ProjectCompile", Gtk::AccelKey("<control>F9"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_compile));
//   _app->add_action("ProjectCleanup", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_cleanup));
//   _app->add_action("ProjectForwards", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_forwards));
//   _app->add_action("ProjectOptions", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_options));
//
//   _app->add_action("DebugRun""F9", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_run));
//   _app->add_action("DebugStepover", "F8", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stepover));
//   _app->add_action("DebugStepin", "F7", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stepin));
//   _app->add_action("DebugGoto", "F4", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_goto));
//   _app->add_action("DebugToggle", "F5", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_toggle));
//   _app->add_action("DebugClearBreakpoints", "<control><shift>F5", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_clearbps));
//   _app->add_action("DebugGotoSource", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_source));
//   _app->add_action("DebugStop", "<control>F2", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stop));
//
//   _app->add_action("ToolsEditor", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_tools_editor));
//   _app->add_action("ToolsDebugger", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_tools_debugger));
//
//   _app->add_action("WindowNext", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_window_next));
//   _app->add_action("WindowPrevious", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_window_prev));
//   _app->add_action("WindowWindows", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_windows));
//
//   _app->add_action("HelpAPI", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_help_api));
//   _app->add_action("HelpAbout", sigc::mem_fun(*this, &GTKIDEWindow::on_menu_help_about));

   insert_action_group("win", refActions);

   auto controller = Gtk::ShortcutController::create();
   controller->set_scope(Gtk::ShortcutScope::GLOBAL);
   add_controller(controller);

   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_n, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.FileNewSource")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_o, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.FileOpenSource")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_s, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.FileSaveSource")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_s, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK),
      Gtk::NamedAction::create("win.FileSaveAll")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_w, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.FileCloseSource")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_F4, Gdk::ModifierType::ALT_MASK),
      Gtk::NamedAction::create("win.FileQuit")));

   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_z, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditUndo")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_y, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditRedo")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_Delete),
      Gtk::NamedAction::create("win.EditDelete")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_a, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditSelectAll")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_Tab),
      Gtk::NamedAction::create("win.EditInsertTab")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_Tab, Gdk::ModifierType::SHIFT_MASK),
      Gtk::NamedAction::create("win.EditRemoveTab")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_l, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditEraseLine")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_t, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditTrim")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_u, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditUpper")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_u, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK),
      Gtk::NamedAction::create("win.EditLower")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_k, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditComment")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_k, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK),
      Gtk::NamedAction::create("win.EditUncomment")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_c, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditCopy")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_v, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditPaste")));
   controller->add_shortcut(Gtk::Shortcut::create(
      Gtk::KeyvalTrigger::create(GDK_KEY_x, Gdk::ModifierType::CONTROL_MASK),
      Gtk::NamedAction::create("win.EditCut")));

   loadUI(ui_info, "MenuBar");

//   //File menu:
//   _refActionGroup->add( Gtk::Action::create("FileMenu", "_File") );
//   _refActionGroup->add( Gtk::Action::create("EditMenu", "_Edit") );
//   _refActionGroup->add( Gtk::Action::create("ProjectMenu", "_Project") );
//   _refActionGroup->add( Gtk::Action::create("ViewMenu", "_View") );
//   _refActionGroup->add( Gtk::Action::create("DebugMenu", "_Debug") );
//   _refActionGroup->add( Gtk::Action::create("ToolsMenu", "_Tools") );
//   _refActionGroup->add( Gtk::Action::create("WindowMenu", "_Window") );
//   _refActionGroup->add( Gtk::Action::create("HelpMenu", "_Help") );
//   _refActionGroup->add( Gtk::Action::create("SearchMenu", "_Search") );
//
//   _refActionGroup->add( Gtk::Action::create("FileNewProject", "Project"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_new_project));
//   _refActionGroup->add( Gtk::Action::create("FileOpenProject", "Project"), Gtk::AccelKey("<control><shift>O"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_open_project));
//   _refActionGroup->add( Gtk::Action::create("FileProjectAs", "Save Project As..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_saveas));
//   _refActionGroup->add( Gtk::Action::create("ProjectClose", "Close Project"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_close));
//
//   _refActionGroup->add( Gtk::Action::create("FileRecentFiles", "Recent files") );
//   _refActionGroup->add( Gtk::Action::create("FileRecentProjects", "Recent projects") );
//   _refActionGroup->add( Gtk::Action::create("FileRecentFilesClear", "Clear history"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_clearfilehistory));
//   _refActionGroup->add( Gtk::Action::create("FileRecentProjectsClear", "Clear history"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_file_clearprojecthistory));
//
//   _refActionGroup->add( Gtk::Action::create("ProjectView", "Project View"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_view));
//   _refActionGroup->add( Gtk::ToggleAction::create("ProjectOutput", "Compiler output"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_output));
//   _refActionGroup->add( Gtk::ToggleAction::create("ProjectMessages", "Messages"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_messages));
//   _refActionGroup->add( Gtk::Action::create("ProjectWatch", "Debug Watch"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_watch));
//   _refActionGroup->add( Gtk::Action::create("ProjectCallstack", "Call stack"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_callstack));
//   _refActionGroup->add( Gtk::Action::create("ProjectConsole", "ELENA Interactive"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_interactive));
//
//   _refActionGroup->add( Gtk::Action::create("Search", "Find..."), Gtk::AccelKey("<control>F"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_find));
//   _refActionGroup->add( Gtk::Action::create("SearchNext", "Find Next"), Gtk::AccelKey("F3"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_findnext));
//   _refActionGroup->add( Gtk::Action::create("Replace", "Replace..."), Gtk::AccelKey("<control>R"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_replace));
//   _refActionGroup->add( Gtk::Action::create("ReplaceNext", "Replace Next"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_replacenext));
//   _refActionGroup->add( Gtk::Action::create("GoToLine", "Go to line..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_search_gotoline));
//
//   _refActionGroup->add( Gtk::Action::create("ProjectInclude", "Include"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_include));
//   _refActionGroup->add( Gtk::Action::create("ProjectExclude", "Exclude"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_exclude));
//   _refActionGroup->add( Gtk::Action::create("ProjectCompile", "Compile"), Gtk::AccelKey("<control>F9"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_compile));
//   _refActionGroup->add( Gtk::Action::create("ProjectCleanup", "Clean up"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_cleanup));
//   _refActionGroup->add( Gtk::Action::create("ProjectForwards", "Forwards..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_forwards));
//   _refActionGroup->add( Gtk::Action::create("ProjectOptions", "Options..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_project_options));
//
//
//   _refActionGroup->add( Gtk::Action::create("DebugRun", "Run"), Gtk::AccelKey("F9"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_run));
//   _refActionGroup->add( Gtk::Action::create("DebugStepover", "Step Over"), Gtk::AccelKey("F8"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stepover));
//   _refActionGroup->add( Gtk::Action::create("DebugStepin", "Step In"), Gtk::AccelKey("F7"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stepin));
//   _refActionGroup->add( Gtk::Action::create("DebugGoto", "Go To Cursor"), Gtk::AccelKey("F4"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_goto));
//   _refActionGroup->add( Gtk::Action::create("DebugToggle", "Toggle Breakpoint"), Gtk::AccelKey("F5"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_toggle));
//   _refActionGroup->add( Gtk::Action::create("DebugClearBreakpoints", "Clear All Breakpoints"), Gtk::AccelKey("<control><shift>F5"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_clearbps));
//   _refActionGroup->add( Gtk::Action::create("DebugGotoSource", "Go To Source"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_source));
//   _refActionGroup->add( Gtk::Action::create("DebugStop", "Stop Execution"), Gtk::AccelKey("<control>F2"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_debug_stop));
//
//   _refActionGroup->add( Gtk::Action::create("ToolsEditor", "Editor Options..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_tools_editor));
//   _refActionGroup->add( Gtk::Action::create("ToolsDebugger", "Debugger Options..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_tools_debugger));
//
//   _refActionGroup->add( Gtk::Action::create("WindowNext", "Next"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_window_next));
//   _refActionGroup->add( Gtk::Action::create("WindowPrevious", "Previous"), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_window_prev));
//   _refActionGroup->add( Gtk::Action::create("WindowWindows", "Window List..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_windows));
//
//   _refActionGroup->add( Gtk::Action::create("HelpAPI", "ELENA API..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_help_api));
//   _refActionGroup->add( Gtk::Action::create("HelpAbout", "About..."), sigc::mem_fun(*this, &GTKIDEWindow::on_menu_help_about));
}

/*Glib::RefPtr<Gtk::Action> GTKIDEWindow :: getMenuItem(ustr_t name)
{
   IdentifierString path("/ui/MenuBar/", name);

   return _refUIManager->get_action(path.str());
}*/

void GTKIDEWindow :: on_text_model_change(TextViewModelEvent event)
{
//   if (test(rec->status, STATUS_COLORSCHEME_CHANGED)) {
//      onColorSchemeChange();
//   }
//
   onDocumentUpdate(event.changeStatus);
   onIDEStatusChange(event.status);
//   if (test(rec->status, STATUS_FRAME_CHANGED)) {
//      onDocumentSelection();
//   }
}

void GTKIDEWindow :: on_textframe_change(SelectionEvent event)
{
   _controller->onDocSelection(_model, event.Index());
}

void GTKIDEWindow :: on_projectview_row_activated(const Gtk::TreeModel::Path& path,
    Gtk::TreeViewColumn*)
{
   Gtk::TreeModel::iterator iter = _projectTree->get_iter(path);
   if(iter) {
      Gtk::TreeModel::Row row = *iter;
      int index = row[_projectTreeColumns._index];
      if (index >= 0) {
         _controller->doOpenProjectSourceByIndex(_model, index);

        _children[_model->ideScheme.textFrameId]->grab_focus();
      }
   }
}

void GTKIDEWindow :: onDocumentUpdate(DocumentChangeStatus changeStatus)
{
}

void GTKIDEWindow :: onProjectChange(bool empty)
{
   Gtk::TreeView* projectView = dynamic_cast<Gtk::TreeView*>(_children[_model->ideScheme.projectView]);

   _projectTree->clear();

   int index = 0;
   if (!empty) {
      for (auto it = _model->projectModel.sources.start(); !it.eof(); ++it) {
         path_t name = *it;
         Gtk::TreeModel::Children children = _projectTree->children();

         size_t start = 0;
         size_t end = 0;
         while (end != -1) {
            end = name.findSub(start, PATH_SEPARATOR);

            IdentifierString nodeName(name + start, (end == NOTFOUND_POS ? getlength(name) : end) - start);
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
               row[_projectTreeColumns._caption] = nodeName.str();
               row[_projectTreeColumns._index] = end == NOTFOUND_POS ? index : NOTFOUND_POS;

               children = row.children();
            }
            else children = (*it).children();

            start = end + 1;
         }
         index++;
      }
   }

//   projectView->expand(root);
   //show_all_children();
}

void GTKIDEWindow :: onProjectRefresh(bool empty)
{
/*
   updateCompileMenu(!empty, !empty, false);

   enableMenuItemById(IDM_PROJECT_CLOSE, !empty, true);
   enableMenuItemById(IDM_FILE_SAVEPROJECT, !empty, false);
*/
}

void GTKIDEWindow :: onIDEStatusChange(int status)
{
   if (test(status, STATUS_PROJECT_CHANGED)) {
      onProjectChange(_model->projectModel.empty);
   }
   else if (test(status, STATUS_PROJECT_REFRESH)) {
      onProjectRefresh(_model->projectModel.empty);
   }

   //if (test(rec->status, STATUS_FRAME_VISIBILITY_CHANGED)) {
   //   if (_model->sourceViewModel.isAssigned()) {
         //_children[_model->ideScheme.textFrameId]->show();
         //_children[_model->ideScheme.textFrameId]->setFocus();
    //  }
      //else _children[_model->ideScheme.textFrameId]->hide();
   //}
}

void GTKIDEWindow :: saveFile_finish(PathString& path, int index)
{
   _controller->doSaveFile(_model, index, true, *path);
}

void GTKIDEWindow :: saveFile(int index)
{
   if (_controller->ifFileUnnamed(_model, index)) {
      saveFileAs(index);
   }
   else _controller->doSaveFile(_model, index, true);
}

void GTKIDEWindow :: saveFileAs(int index)
{
   FileCallbackArg* arg = new FileCallbackArg(this, index);

   fileDialog.saveFile((void*)arg, [](void* arg, PathString* path)
   {
      FileCallbackArg* info = (FileCallbackArg*)arg;
      if (path) {
         info->value1->saveFile_finish(*path, info->value2);
      }
      delete info;
   });
}

void GTKIDEWindow :: saveProject()
{
   // !! temporally empty
}

void GTKIDEWindow :: saveAll()
{
   for (int index = 1; index <= (int)_model->sourceViewModel.getDocumentCount(); index++) {
      if (_controller->ifFileNotSaved(_model, index) || _controller->ifFileUnnamed(_model, index))
         saveFile(index);
   }

   saveProject();
}

void GTKIDEWindow :: onFileClose(int index, FileCloseCallback callback)
{
   if (_controller->ifFileUnnamed(_model, index)) {
      CloseCallbackArg* arg = new CloseCallbackArg(this, callback, index);

      fileDialog.saveFile((void*)arg, [](void* arg, PathString* path)
      {
         CloseCallbackArg* info = (CloseCallbackArg*)arg;
         if (path) {
            info->value1->saveFile_finish(*path, info->value3);

            info->value2(info->value1, info->value3);

            delete info;
         }
         else {
            info->value1->messageDialog.question(QUESTION_CLOSE_UNSAVED, info, [](void* arg, int result)
            {
               CloseCallbackArg* info = (CloseCallbackArg*)arg;

               if (result == MessageDialogBase::Answer::Yes) {
                  info->value2(info->value1, info->value3);
               }

               delete info;
            });
         };
      });
   }
   else if (_controller->ifFileNotSaved(_model, index)) {
      CloseCallbackArg* arg = new CloseCallbackArg(this, callback, index);
      path_t path = _model->sourceViewModel.getDocumentPath(index);

      messageDialog.question(QUESTION_SAVE_FILECHANGES, path, arg, [](void* arg, int result)
      {
         CloseCallbackArg* info = (CloseCallbackArg*)arg;

         if (result == MessageDialogBase::Answer::Cancel) {

         }
         else if (result == MessageDialogBase::Answer::Yes) {
            PathString p(info->value1->_model->sourceViewModel.getDocumentPath(info->value3));

            info->value1->saveFile_finish(p, info->value3);

            info->value2(info->value1, info->value3);
         }
         else {
            info->value2(info->value1, info->value3);
         }

         delete info;
      });
   }
   else callback(this, index);
}

void GTKIDEWindow :: closeFile_finish(int index)
{
   _controller->doCloseFile(_model, index);
}

void GTKIDEWindow :: closeFile(int index)
{
   onFileClose(index, [](void* arg, int index)
   {
      GTKIDEWindow* ide = (GTKIDEWindow*)arg;

      ide->closeFile_finish(index);
   });
}

void GTKIDEWindow :: closeAll_finish()
{
   _controller->doCloseAll(_model, _projectMode);

   _closing = false;

   if (_newMode) {
      _newMode = false;

      newProject();
   }
}

void GTKIDEWindow :: closeAll_next(int index)
{
   onFileClose(index, [](void* arg, int index)
   {
      index++;

      GTKIDEWindow* ide = (GTKIDEWindow*)arg;
      if (index < (int)ide->_model->sourceViewModel.getDocumentCount()) {
         ide->closeAll_next(index);
      }
      else ide->closeAll_finish();
   });
}

void GTKIDEWindow :: closeAllButActive_finish()
{
   _controller->doCloseAllButActive(_model);
}

void GTKIDEWindow :: closeAllButActive_next(int index)
{
   onFileClose(index, [](void* arg, int index)
   {
      index++;

      GTKIDEWindow* ide = (GTKIDEWindow*)arg;
      if (index == ide->_model->sourceViewModel.getCurrentIndex()) {
         index++;
      }

      if (index < (int)ide->_model->sourceViewModel.getDocumentCount()) {
         ide->closeAllButActive_next(index);
      }
      else ide->closeAllButActive_finish();
   });
}

void GTKIDEWindow :: closeAll()
{
   if (_closing) {
      // only one closing process can be started simultaneously 
      return;
   }

   _closing = true;
   _projectMode = false;

   closeAll_next(0);
}

void GTKIDEWindow::closeProject()
{
   if (_closing) {
      // only one closing process can be started simultaneously 
      return;
   }

   _closing = true;
   _projectMode = true;
   _newMode = true;

   closeAll_next(0);
}

void GTKIDEWindow :: closeAllButActive()
{
   if (_model->sourceViewModel.getCurrentIndex() == 1) {
      closeAllButActive_next(1);
   }
   else closeAllButActive_next(0);
}

void GTKIDEWindow :: newProject()
{
   _controller->doNewProject(_model);

   projectSettingsDialog.showModal();
}