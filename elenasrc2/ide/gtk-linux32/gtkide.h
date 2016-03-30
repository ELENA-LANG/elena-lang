//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

#include "gtk-linux32/gtksdi.h"
#include "gtk-linux32/gtkmenu.h"
#include "../ide.h"

namespace _GUI_
{
// --- GTKIDEView ---

enum DebugMessageType
{
   dbgNone = 0,
   dbgStart
};

struct DebugMessage
{
   DebugMessageType message;

   DebugMessage()
   {
      this->message = dbgNone;
   }
   DebugMessage(DebugMessageType message)
   {
      this->message = message;
   }
};

class MainWindow : public SDIWindow
{
   class OutputProcess
   {
      _ELENA_::Path argument;

      char buffer[512];
      int  buf_len;
      bool stopped;

      mutable Glib::Threads::Mutex _mutex;

   public:
      bool isStopped();

      void writeOut(Gtk::TextView& view);

      void compile(MainWindow* owner);

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

   ProjectTreeColumns           _projectTreeColumns;
   Glib::RefPtr<Gtk::TreeStore> _projectTree;

   _Controller*        _controller;
   Model*              _model;

   Gtk::Toolbar        _toolbar;
   Gtk::TreeView       _projectView;
   Gtk::Statusbar      _statusbar;
   Gtk::Notebook       _bottomTab;

   Gtk::ScrolledWindow _outputScroller;
   Gtk::TextView       _output;
   EditFrame           _mainFrame;
   OutputProcess       _outputProcess;

   Glib::Threads::Thread* _outputThread;
   Glib::Dispatcher       _outputDispatcher;
   Glib::Dispatcher       _debugDispatcher;

   mutable Glib::Threads::Mutex _debugMutex;
   _ELENA_::Queue<DebugMessage> _debugMessages;

protected:
   Clipboard      _clipboard;

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

   }
   void on_menu_project_messages()
   {

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

   void on_notification_from_output();
   void on_notification_from_debugger();

   void populateMenu();
   void populateToolbar();

public:
   bool copyToClipboard(Document* document);
   void pasteFrameClipboard(Document* document);

   void refreshDocument();

   int newDocument(const char* name, Document* doc);
   void closeDocument(int index);
   int getCurrentDocumentIndex();
   void selectDocument(int docIndex);

   void notifyOutput();
   void notityDebugStep(DebugMessage message);

   void reloadProjectView(_ProjectManager* project);

   bool compileProject(_ProjectManager* manager, int postponedAction);

   MainWindow(const char* caption, _Controller* controller, Model* model);
};

} // _GUI_

#endif // winideH
