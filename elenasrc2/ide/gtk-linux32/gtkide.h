//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

#include "gtk-linux32/gtksdi.h"
#include "gtk-linux32/gtkmenu.h"
#include "../gtk-linux32/gtkdialogs.h"
#include "../ide.h"

namespace _GUI_
{
// --- GTKIDEView ---

enum DebugMessageType
{
   dbgNone = 0,
   dbgStart,
   dbgStep
};

struct DebugMessage
{
   DebugMessageType message;

   const char* strparam1;
   const char* strparam2;
   int         nparam1;
   int         nparam2;
   int         nparam3;

   DebugMessage()
   {
      this->message = dbgNone;
      strparam1 = strparam2 = NULL;
      nparam1 = nparam2 = nparam3 = 0;
   }
   DebugMessage(DebugMessageType message)
   {
      this->message = message;
      strparam1 = strparam2 = NULL;
      nparam1 = nparam2 = nparam3 = 0;
   }
   DebugMessage(DebugMessageType message, const char* s1, const char* s2, int n1, int n2, int n3)
   {
      this->message = message;
      this->strparam1 = s1;
      this->strparam2 = s2;
      this->nparam1 = n1;
      this->nparam2 = n2;
      this->nparam3 = n3;
   }
};

class GTKIDEWindow : public SDIWindow, public _View, public _DebugListener
{
   class OutputProcess
   {
      _ELENA_::Path argument;

      char buffer[512];
      int  buf_len;
      bool stopped;
      int  exitCode;

      mutable Glib::Threads::Mutex _mutex;

   public:
      bool isStopped();

      void writeOut(Gtk::TextView& view);

      void compile(GTKIDEWindow* owner);

      void setArgument(const char* path, const char* name, const char* extension)
      {
         argument.copy("-c");
         argument.append(path);
         argument.combine(name);
         argument.appendExtension(extension);
      }

      OutputProcess()
      {
         buf_len = 0;
         stopped = true;
         exitCode = 0;
      }
   };

   class ProjectTreeColumns : public Gtk::TreeModel::ColumnRecord
   {
   public:
      Gtk::TreeModelColumn<Glib::ustring> _caption;
      Gtk::TreeModelColumn<int>           _index;

      ProjectTreeColumns()
      {
          add(_caption);
          add(_index);
      }
   };

   class MessageLogColumns : public Gtk::TreeModel::ColumnRecord
   {
   public:
      Gtk::TreeModelColumn<Glib::ustring> _description;
      Gtk::TreeModelColumn<Glib::ustring> _file;
      Gtk::TreeModelColumn<Glib::ustring> _line;
      Gtk::TreeModelColumn<Glib::ustring> _column;

      MessageLogColumns()
      {
          add(_description);
          add(_file);
          add(_line);
          add(_column);
      }
   };

   class StatusLine
   {
      _ELENA_::String<char, 127> line1;
      _ELENA_::String<char, 20>  line2;
      _ELENA_::String<char, 20>  line3;
      _ELENA_::String<char, 20>  line4;

   public:
      void set(int index, text_t message);
      void show(Gtk::Statusbar& statusbar);
   };

   ProjectTreeColumns           _projectTreeColumns;
   Glib::RefPtr<Gtk::TreeStore> _projectTree;

   MessageLogColumns            _messageLogColumns;
   Glib::RefPtr<Gtk::TreeStore> _messageList;

   _Controller*        _controller;
   Model*              _model;

   Gtk::Toolbar        _toolbar;
   Gtk::TreeView       _projectView;
   Gtk::TreeView       _messageLog;
   Gtk::Statusbar      _statusbar;
   Gtk::Notebook       _bottomTab;

   Gtk::ScrolledWindow _outputScroller;
   Gtk::TextView       _output;
   EditFrame           _mainFrame;
   OutputProcess       _outputProcess;

   Glib::Threads::Thread* _outputThread;
   Glib::Dispatcher       _outputDispatcher;
   Glib::Dispatcher       _debugDispatcher;
   Glib::Dispatcher       _compilationSuccessDispatcher;
   Glib::Dispatcher       _compilationWarningDispatcher;
   Glib::Dispatcher       _compilationErrorDispatcher;

   mutable Glib::Threads::Mutex _debugMutex;
   _ELENA_::Queue<DebugMessage> _debugMessages;

protected:
   Clipboard      _clipboard;
   StatusLine     _statusInfo;

   bool _skip; // HOTFIX : to prevent infinite checkmenuitem call

   void displayErrors();

   // event signals
   void on_menu_file_new_source()
   {
      _controller->doCreateFile();
   }
   void on_menu_file_new_project()
   {
      _controller->doCreateProject();
   }
   void on_menu_file_open_source()
   {
      _controller->doOpenFile();
   }
   void on_menu_file_open_project()
   {
      _controller->doOpenProject();
   }
   void on_menu_file_quit()
   {
      _controller->doExit();
   }
   void on_menu_file_save()
   {
      _controller->doSave(false);
   }
   void on_menu_file_saveas()
   {
      _controller->doSave(true);
   }
   void on_menu_project_saveas()
   {
      _controller->doSaveProject(true);
   }
   void on_menu_file_saveall()
   {
      _controller->doSave(true);
   }
   void on_menu_file_close()
   {
      _controller->doCloseFile();
   }
   void on_menu_file_closeall()
   {
      _controller->doCloseAll(false);
   }
   void on_menu_file_closeproject()
   {
      _controller->doCloseAll(true);
   }
   void on_menu_file_closeallbutactive()
   {
      _controller->doCloseAllButActive();
   }

   void on_menu_edit_undo()
   {
      _controller->doUndo();
   }
   void on_menu_edit_redo()
   {
      _controller->doRedo();
   }
   void on_menu_edit_cut()
   {
      if (_controller->doEditCopy())
         _controller->doEditDelete();
   }
   void on_menu_edit_copy()
   {
      _controller->doEditCopy();
   }
   void on_menu_edit_paste()
   {
      _controller->doEditPaste();
   }
   void on_menu_edit_delete()
   {
      _controller->doEditDelete();
   }
   void on_menu_edit_select_all()
   {
      _controller->doSelectAll();
   }
   void on_menu_edit_indent()
   {
      _controller->doIndent();
   }
   void on_menu_edit_outdent()
   {
      _controller->doOutdent();
   }
   void on_menu_edit_trim()
   {
      _controller->doTrim();
   }
   void on_menu_edit_erase_line()
   {
      _controller->doEraseLine();
   }
   void on_menu_edit_upper()
   {
      _controller->doUpperCase();
   }
   void on_menu_edit_lower()
   {
      _controller->doLowerCase();
   }
   void on_menu_edit_comment()
   {
      _controller->doComment();
   }
   void on_menu_edit_uncomment()
   {
      _controller->doUnComment();
   }
   void on_menu_project_include()
   {
      _controller->doInclude();
   }
   void on_menu_project_exclude()
   {
      _controller->doExclude();
   }
   void on_menu_project_compile()
   {
      _controller->doCompileProject();
   }
   void on_menu_project_cleanup()
   {
      _controller->cleanUpProject();
   }
   void on_menu_project_forwards()
   {
      _controller->doSetProjectForwards();
   }
   void on_menu_project_options()
   {
      _controller->doSetProjectSettings();
   }
   void on_menu_file_clearfilehistory()
   {

   }
   void on_menu_file_clearprojecthistory()
   {

   }
   void on_menu_project_view()
   {

   }
   void on_menu_project_output()
   {
      if (!_skip) {
         _controller->doShowCompilerOutput(!_model->compilerOutput);
      }
      else _skip = false;
   }
   void on_menu_project_messages()
   {
      if (!_skip) {
         _controller->doShowMessages(!_model->messages);
      }
      else _skip = false;
   }
   void on_menu_project_watch()
   {

   }
   void on_menu_project_callstack()
   {

   }
   void on_menu_project_interactive()
   {

   }
   void on_menu_search_find()
   {

   }
   void on_menu_search_findnext()
   {

   }
   void on_menu_search_replace()
   {

   }
   void on_menu_search_replacenext()
   {

   }
   void on_menu_search_gotoline()
   {
   }
   void on_menu_debug_run()
   {
      _controller->doDebugRun();
   }
   void on_menu_debug_next()
   {
      _controller->doStepOver();
   }
   void on_menu_debug_stepover()
   {
      _controller->doStepOver();
   }
   void on_menu_debug_stepin()
   {
      _controller->doStepInto();
   }
   void on_menu_debug_goto()
   {

   }
   void on_menu_debug_toggle()
   {

   }
   void on_menu_debug_clearbps()
   {

   }
   void on_menu_debug_source()
   {

   }
   void on_menu_debug_stop()
   {

   }
   void on_menu_tools_editor()
   {

   }
   void on_menu_tools_debugger()
   {

   }
   void on_menu_window_next()
   {

   }
   void on_menu_window_prev()
   {

   }
   void on_menu_windows()
   {

   }
   void on_menu_help_api()
   {

   }
   void on_menu_help_about()
   {

   }

   void on_projectview_row_activated(const Gtk::TreeModel::Path& path,
        Gtk::TreeViewColumn*)
   {
      Gtk::TreeModel::iterator iter = _projectTree->get_iter(path);
      if(iter) {
         Gtk::TreeModel::Row row = *iter;
         int index = row[_projectTreeColumns._index];
         if (index >= 0)
            _controller->selectProjectFile(index);
      }
   }

   void on_messagelog_row_activated(const Gtk::TreeModel::Path& path,
        Gtk::TreeViewColumn*)
   {
      Gtk::TreeModel::iterator iter = _messageList->get_iter(path);
      if(iter) {
         Gtk::TreeModel::Row row = *iter;

         Glib::ustring file = row[_messageLogColumns._file];
         Glib::ustring col = row[_messageLogColumns._column];
         Glib::ustring line = row[_messageLogColumns._line];

         MessageBookmark bookmark(file.c_str(), col.c_str(), line.c_str());

         _controller->highlightMessage(&bookmark, STYLE_ERROR_LINE);
      }
   }

   void on_notification_from_output();
   void on_notification_from_debugger();
   void on_compilation_success()
   {
      _controller->onCompilationEnd(SUCCESSFULLY_COMPILED, true);
   }
   void on_compilation_warning()
   {
      displayErrors();
      _controller->onCompilationEnd(COMPILED_WITH_WARNINGS, true);
   }
   void on_compilation_error()
   {
      _controller->cleanUpProject();
      displayErrors();
      _controller->onCompilationEnd(COMPILED_WITH_ERRORS, false);
   }

   void on_textview_changed()
   {
      _controller->onCursorChange();
      _controller->onFrameChange();
   }

   void on_client_change(Widget* page, guint page_number)
   {
      on_textview_changed();
   }

   void populateMenu();
   void populateToolbar();

   Glib::RefPtr<Gtk::Action> getMenuItem(int id);

public:
   void refreshDocument();

   int newDocument(const char* name, Document* doc);
   void closeDocument(int index);
   int getCurrentDocumentIndex();
   void selectDocument(int docIndex);

   void notifyOutput();
   void notifyCompletion(int errorCode);
   void notityDebugStep(DebugMessage message);

   virtual void reloadProjectView(_ProjectManager* project);

   virtual bool compileProject(_ProjectManager* manager, int postponedAction);

   virtual void start(bool maximized)
   {
      show();

      if (maximized)
         maximize();
   }

   virtual void exit()
   {
      SDIWindow::exit();
   }

   virtual void refresh(bool onlyFrame)
   {
      if (onlyFrame) {
         refreshDocument();
      }
      else queue_draw();
   }

   virtual bool saveProject(Model* model, _ELENA_::Path& path)
   {
      Gtk::FileChooserDialog dialog(SAVEAS_PROJECT_CAPTION, Gtk::FILE_CHOOSER_ACTION_SAVE);
      dialog.set_transient_for(*this);

      if (!_ELENA_::emptystr(model->project.path))
         dialog.set_current_folder((const char*)model->project.path);

      dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
      dialog.add_button("_Save", Gtk::RESPONSE_OK);

      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
      filter_l->set_name("ELENA project file");
      filter_l->add_pattern("*.project");
      dialog.add_filter(filter_l);

      Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
      filter_any->set_name("Any files");
      filter_any->add_pattern("*");
      dialog.add_filter(filter_any);

      int result = dialog.run();
      if (result == Gtk::RESPONSE_OK) {
         std::string filename = dialog.get_filename();

         path.copy(filename.c_str());

         if(!_ELENA_::Path::checkExtension(path.str()))
            path.append(".project");

         return true;
      }
      else return false;
   }

   virtual bool saveFile(Model* model, _ELENA_::Path& newPath)
   {
      Gtk::FileChooserDialog dialog(SAVEAS_FILE_CAPTION, Gtk::FILE_CHOOSER_ACTION_SAVE);
      dialog.set_transient_for(*this);

      if (!_ELENA_::emptystr(model->project.path))
         dialog.set_current_folder((const char*)model->project.path);

      dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
      dialog.add_button("_Open", Gtk::RESPONSE_OK);

      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
      filter_l->set_name("ELENA source file");
      filter_l->add_pattern("*.l");
      dialog.add_filter(filter_l);

      Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
      filter_any->set_name("Any files");
      filter_any->add_pattern("*");
      dialog.add_filter(filter_any);

      int result = dialog.run();
      if (result == Gtk::RESPONSE_OK) {
         std::string filename = dialog.get_filename();

         newPath.copy(filename.c_str());

         return true;
      }
      else return false;
   }

   virtual bool selectFiles(Model* model, _ELENA_::List<text_c*>& selected)
   {
      Gtk::FileChooserDialog dialog(OPEN_FILE_CAPTION, Gtk::FILE_CHOOSER_ACTION_OPEN);
      dialog.set_transient_for(*this);

      if (!_ELENA_::emptystr(model->paths.lastPath))
         dialog.set_current_folder ((const char*)model->paths.lastPath);

      dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
      dialog.add_button("_Open", Gtk::RESPONSE_OK);

      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
      filter_l->set_name("ELENA source file");
      filter_l->add_pattern("*.l");
      dialog.add_filter(filter_l);

      Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
      filter_any->set_name("Any files");
      filter_any->add_pattern("*");
      dialog.add_filter(filter_any);

      int result = dialog.run();
      if (result == Gtk::RESPONSE_OK) {
         std::string filename = dialog.get_filename();

         selected.add(_ELENA_::StrFactory::clone(filename.c_str()));

         return true;
      }
      else return false;
   }

   virtual bool selectProject(Model* model, _ELENA_::Path& path)
   {
      Gtk::FileChooserDialog dialog(OPEN_PROJECT_CAPTION, Gtk::FILE_CHOOSER_ACTION_OPEN);
      dialog.set_transient_for(*this);

      if (!_ELENA_::emptystr(model->paths.lastPath))
         dialog.set_current_folder ((const char*)model->paths.lastPath);

      dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
      dialog.add_button("_Open", Gtk::RESPONSE_OK);

      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
      filter_l->set_name("ELENA project file");
      filter_l->add_pattern("*.project");
      dialog.add_filter(filter_l);

      Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
      filter_any->set_name("Any files");
      filter_any->add_pattern("*");
      dialog.add_filter(filter_any);

      int result = dialog.run();
      if (result == Gtk::RESPONSE_OK) {
         std::string filename = dialog.get_filename();

         path.copy(filename.c_str());

         return true;
      }
      else return false;
   }

   virtual void error(text_t message)
   {
      Gtk::MessageDialog dialog(*this, message, false, Gtk::MESSAGE_ERROR);
      dialog.run();
   }

   virtual void error(text_t message, text_t param)
   {
      _ELENA_::String<char, 255> string(message);
      string.append(param);

      Gtk::MessageDialog dialog(*this, (const char*)string, false, Gtk::MESSAGE_ERROR);
      dialog.run();
   }

   virtual bool confirm(text_t message, text_t param1, text_t param2)
   {
      _ELENA_::String<char, 255> string(message);
      string.append(param1);
      string.append(param2);

      Gtk::MessageDialog dialog(*this, (const char*)string, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      int Answer=dialog.run();

      // Process user choice
      switch(Answer)
      {
         case(Gtk::RESPONSE_YES):
            return true;
         default:
            return false;
      }
   }

   virtual bool confirm(text_t message)
   {
      Gtk::MessageDialog dialog(*this, message, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
      int Answer=dialog.run();

      // Process user choice
      switch(Answer)
      {
         case(Gtk::RESPONSE_YES):
            return true;
         default:
            return false;
      }
   }

   virtual _View::Answer question(text_t message)
   {
      Gtk::MessageDialog dialog(*this, message, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
      dialog.add_button("Yes", Gtk::RESPONSE_YES);
      dialog.add_button("No", Gtk::RESPONSE_NO);
      dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

      int Answer=dialog.run();

      // Process user choice
      switch(Answer)
      {
         case(Gtk::RESPONSE_YES):
            return _View::Yes;
         case(Gtk::RESPONSE_CANCEL):
            return _View::Cancel;
         default:
            return _View::No;
      }
   }

   virtual Answer question(text_t message, text_t param)
   {
      _ELENA_::String<char, 255> string(message);
      string.append(param);

      Gtk::MessageDialog dialog(*this, (const char*)string, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
      dialog.add_button("Yes", Gtk::RESPONSE_YES);
      dialog.add_button("No", Gtk::RESPONSE_NO);
      dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

      int Answer=dialog.run();

      // Process user choice
      switch(Answer)
      {
         case(Gtk::RESPONSE_YES):
            return _View::Yes;
         case(Gtk::RESPONSE_CANCEL):
            return _View::Cancel;
         default:
            return _View::No;
      }
   }

   virtual void renameDocument(int index, text_t name)
   {
     // appWindow.renameDocument(index, name);
   }

   virtual void showFrame()
   {
     // appWindow.showFrame();
   }

   virtual void activateFrame()
   {
     // appWindow.activateFrame();
   }

   virtual void hideFrame()
   {
    //  appWindow.hideFrame();
   }

   virtual void showStatus(int index, text_t message)
   {
      _statusInfo.set(index, message);

      _statusInfo.show(_statusbar);
   }

   virtual void setCaption(text_t caption)
   {
      //appWindow.setCaption(caption);
   }

   virtual void enableMenuItemById(int id, bool doEnable, bool toolBarItemAvailable)
   {
      Glib::RefPtr<Gtk::Action> menuItem = getMenuItem(id);
      if (menuItem)
         menuItem->set_sensitive(doEnable);

//      appWindow.getMenu()->enableItemById(id, doEnable);
//      if (toolBarItemAvailable)
//         appWindow.getToolBar()->enableItemById(id, doEnable);
   }

   virtual void checkMenuItemById(int id, bool doEnable)
   {
      Glib::RefPtr<Gtk::Action> menuItem = getMenuItem(id);

      Glib::RefPtr<Gtk::ToggleAction> toggleItem =
         Glib::RefPtr<Gtk::ToggleAction>::cast_static(menuItem);

      if (toggleItem->get_active() != doEnable) {
         _skip = true;
         toggleItem->set_active(doEnable);
      }
   }

   virtual void markDocumentTitle(int docIndex, bool changed)
   {
      //appWindow.markDocumentTitle(docIndex, changed);
   }

   virtual void addToWindowList(text_t path)
   {
      //appWindow.addToWindowList(path);
   }

   virtual void removeFromWindowList(text_t path)
   {
      //appWindow.removeFromWindowList(path);
   }

   virtual void addToRecentFileList(text_t path)
   {
    //  appWindow.addToRecentFileList(path);
   }

   virtual void addToRecentProjectList(text_t path)
   {
   //   appWindow.addToRecentProjectList(path);
   }

   virtual bool configProject(_ProjectManager* project)
   {
      ProjectSettingsDialog dlg(project);
      if (dlg.run() == Gtk::RESPONSE_OK) {
         dlg.save();

         return true;
      }
      else return false;
   }

   virtual bool configEditor(Model* model)
   {
//      EditorSettings dlg(&appWindow, model);
//
//      return dlg.showModal() != 0;
//
      return false; // !!
   }

   virtual bool configDebugger(Model* model)
   {
//      DebuggerSettings dlg(&appWindow, model);
//
//      return dlg.showModal() != 0;

      return false; // !!
   }

   virtual bool configurateForwards(_ProjectManager* project)
   {
//      ProjectForwardsDialog dlg(&appWindow, project);
//
//      return dlg.showModal() != 0;

      return false; // !!
   }

   virtual bool about(Model* model)
   {
//      AboutDialog dlg(&appWindow);
//
//      return dlg.showModal() != 0;

      return false; // !!
   }

   virtual bool copyToClipboard(Document* document);

   virtual void pasteFromClipboard(Document* document);

   virtual bool find(Model* model, SearchOption* option, SearchHistory* searchHistory)
   {
//      FindDialog dialog(&appWindow, false, option, searchHistory, NULL);
//
//      return dialog.showModal();

      return false; // !!
   }

   virtual bool replace(Model* model, SearchOption* option, SearchHistory* searchHistory, SearchHistory* replaceHistory)
   {
//      FindDialog dialog(&appWindow, true, option, searchHistory, replaceHistory);
//
//      return dialog.showModal();

      return false; // !!
   }

   virtual bool gotoLine(int& row)
   {
//      GoToLineDialog dlg(&appWindow, row);
//      if (dlg.showModal()) {
//         row = dlg.getLineNumber();
//
//         return true;
//      }
      /*else */return false;
   }

   virtual bool selectWindow(Model* model, _Controller* controller)
   {
//      IDEWindowsDialog dialog(&appWindow, controller, model);
//
//      if (dialog.showModal() == -2) {
//         return true;
//      }
/*      else */return false;
   }

   virtual void reloadSettings()
   {
      //appWindow.reloadSettings();
   }

   virtual void removeFile(_ELENA_::ident_t name)
   {
      //remove(name);
   }

   virtual void switchToOutput()
   {
      //appWindow.switchToOutput();
   }

   virtual void openOutput()
   {
      _bottomTab.append_page(_outputScroller, "Output");
      _bottomTab.show_all_children();
   }

   virtual void closeOutput()
   {
      _bottomTab.remove_page(_outputScroller);
   }

   virtual void openMessageList()
   {
      _bottomTab.append_page(_messageLog, "Messages");
      _bottomTab.show_all_children();
   }

   virtual void closeMessageList()
   {
      _bottomTab.remove_page(_messageLog);
   }

   virtual void openDebugWatch()
   {
      //appWindow.openDebugWatch();
   }

   virtual void closeDebugWatch()
   {
      //appWindow.closeDebugWatch();
   }

   virtual void openProjectView()
   {
      //appWindow.openDebugWatch();
   }

   virtual void closeProjectView()
   {
      //appWindow.closeDebugWatch();
   }

   virtual void openCallList()
   {
      //appWindow.openCallList();
   }

   virtual void closeCallList()
   {
      //appWindow.closeCallList();
   }

   virtual void clearMessageList()
   {
      _messageList->clear();
   }

   virtual void openVMConsole(_ProjectManager* project)
   {

   }

   virtual void closeVMConsole()
   {

   }

   virtual void resetDebugWindows()
   {
      //appWindow.resetDebugWindows();
   }

   virtual void refreshDebugWindows(_ELENA_::_DebugController* debugController)
   {
      //appWindow.refreshDebugWindows(debugController);
   }

   virtual void browseWatch(_ELENA_::_DebugController* debugController, void* watchNode)
   {
      //appWindow.browseWatch(debugController, watchNode);
   }

   virtual void browseWatch(_ELENA_::_DebugController* debugController)
   {
      //appWindow.browseWatch(debugController);
   }

   virtual void onStop(bool failed)
   {
      //appWindow._notify(failed ? IDE_DEBUGGER_BREAK : IDE_DEBUGGER_STOP);
   }

   void onCheckPoint(const char* message)
   {
      //appWindow._notify(IDE_DEBUGGER_CHECKPOINT, message);
   }

   void onNotification(const char* message, size_t address, int code)
   {
      //appWindow._notify(IDM_DEBUGGER_EXCEPTION, message, address, code);
   }

   virtual void onStep(_ELENA_::ident_t ns, _ELENA_::ident_t source, int row, int disp, int length)
   {
      notityDebugStep(DebugMessage(dbgStep, ns, source, row, disp, length));
   }

   virtual void onDebuggerHook()
   {
      //appWindow._notify(IDE_DEBUGGER_HOOK);
   }

   virtual void onStart()
   {
      notityDebugStep(DebugMessage(dbgStart));
   }

   GTKIDEWindow(const char* caption, _Controller* controller, Model* model);
};

} // _GUI_

#endif // winideH
