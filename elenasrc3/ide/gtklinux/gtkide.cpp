//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ GTK IDE
//                                             (C)2024-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkide.h"
#include "eng/messages.h"

using namespace elena_lang;

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

const char* SOURCE_FILE_FILTER[] =
{
   "*.l",
   "ELENA source file",
   "*",
   "Any files"
};

// --- GTKIDEWindow ---

GTKIDEWindow :: GTKIDEWindow(IDEController* controller, IDEModel* model)
   : fileDialog(this, SOURCE_FILE_FILTER, 4, OPEN_FILE_CAPTION, *model->projectModel.paths.lastPath)
{
   _model = model;
   _controller = controller;

   populateMenu();
}

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

void GTKIDEWindow :: onDocumentUpdate(DocumentChangeStatus changeStatus)
{
}

void GTKIDEWindow :: onIDEStatusChange(int status)
{
   //if (test(rec->status, STATUS_FRAME_VISIBILITY_CHANGED)) {
   //   if (_model->sourceViewModel.isAssigned()) {
         //_children[_model->ideScheme.textFrameId]->show();
         //_children[_model->ideScheme.textFrameId]->setFocus();
    //  }
      //else _children[_model->ideScheme.textFrameId]->hide();
   //}
}
