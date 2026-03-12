//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                             (C)2024-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKIDE_H
#define GTKIDE_H

#include "gtklinux/gtksdi.h"
#include "gtklinux/gtkdialogs.h"
#include "idecontroller.h"

namespace elena_lang
{

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

// --- GTKIDEView ---

typedef void(*FileCloseCallback)(void* arg, int index);

enum class CloseMode : int
{
   None        = 0,
   ProjectMask = 1,

   NewProject  = 1,
   OpenProject = 3,
   ExitApp     = 5,
};

class GTKIDEWindow : public SDIWindow
{
protected:
   class Clipboard //: public ClipboardBase
   {
      Glib::RefPtr<Gdk::Clipboard> _clipboard;

      GTKIDEWindow* _owner;
      Glib::ustring _strData;

      void on_clipboard_received(Glib::RefPtr<Gio::AsyncResult>& result);

   public:
      void copyToClipboard(DocumentView* docView, bool selectionMode);
      void pasteFromClipboard();

      Clipboard(GTKIDEWindow* owner)
      {
         _owner = owner;

         _clipboard = owner->get_clipboard();
      }
   };

   class EventLog : public ErrorLogBase
   {
      GTKIDEWindow*  _owner;

   public:
      void addMessage(text_str message, text_str file, text_str row, text_str col) override;

      MessageLogInfo getMessage(int index) override { return {}; } // !! is not used
      MessageLogInfo getMessage(const Gtk::TreeModel::Path& path);

      void clearMessages() override;

      EventLog(GTKIDEWindow* owner)
         : _owner(owner)
      {
      }
   };

   friend class EventLog;

   GtkApp*                      _app;

   IDEModel*                    _model;
   IDEController*               _controller;

   Clipboard                    _clipboard;

   ProjectTreeColumns           _projectTreeColumns;
   Glib::RefPtr<Gtk::TreeStore> _projectTree;

   MessageLogColumns            _messageLogColumns;
   Glib::RefPtr<Gtk::TreeStore> _messageList;

   // dialogs
   FileDialog                   fileDialog;
   FileDialog                   projectDialog;
   MessageDialog                messageDialog;

   ProjectSettings              projectSettingsDialog;

   bool                         _closing;
   CloseMode                    _mode;

   // menu items
   Glib::RefPtr<Gio::SimpleAction>  _projectViewMenuItem;
   Glib::RefPtr<Gio::SimpleAction>  _errorListMenuItem;
   Glib::RefPtr<Gio::SimpleAction>  _outputMenuItem;
   Glib::RefPtr<Gio::SimpleAction>  _compileMenuItem;

   Glib::RefPtr<Gio::SimpleAction>  _runMenuItem;
   Glib::RefPtr<Gio::SimpleAction>  _stepOverMenuItem;
   Glib::RefPtr<Gio::SimpleAction>  _stopMenuItem;

   void toggleResultTab(int controlIndex, bool visible);

   void populateUI();

   //Glib::RefPtr<Gtk::Action> getMenuItem(ustr_t name) override;

   void newProject();
   void closeProject(bool newMode);
   void openProject();
   void openProject_finish(path_t path);

   void doDebugAction(DebugAction action, bool withoutPostponeAction);

   bool copyToClipboard()
   {
      auto docView = _model->sourceViewModel.DocView();

      _clipboard.copyToClipboard(docView, docView->hasSelection());

      return true;
   }

   void pasteFromClipboard()
   {
      _clipboard.pasteFromClipboard();
   }

   // event signals
   void on_menu_file_new_source()
   {
      _controller->doNewFile(_model);
   }
   void on_menu_file_new_project()
   {
      closeProject(true);
   }

   void on_menu_file_open_source()
   {
      fileDialog.openFiles((void*)this, [](void* arg, PathList* list)
      {
         static_cast<GTKIDEWindow*>(arg)->on_menu_file_open_source_finish(list);
      });
   }
   void on_menu_file_open_source_finish(PathList* files)
   {
      _controller->doOpenFile(_model, *files);
      //_recentFileList.reload();
      //_recentProjectList.reload();
   }

   void on_menu_file_open_project()
   {
      projectDialog.openFile((void*)this, [](void* arg, PathString* path)
      {
         if (path) {
            static_cast<GTKIDEWindow*>(arg)->openProject();
         }
      });
   }
   void on_menu_file_quit()
   {
      closeAndExit();
   }
   void on_menu_file_save()
   {
      saveFile(-1);
   }
   void on_menu_file_saveas()
   {
      saveFileAs(-1);
   }
   void on_menu_project_saveas()
   {
      saveProject();
   }
   void on_menu_file_saveall()
   {
      saveAll();
   }
   void on_menu_file_close()
   {
      closeFile(-1);
   }
   void on_menu_file_closeall()
   {
      closeAll();
   }
   void on_menu_file_closeproject()
   {
      closeProject(false);
   }
   void on_menu_file_closeallbutactive()
   {
      closeAllButActive();
   }

   void on_menu_edit_undo()
   {
      _controller->sourceController.undo(_model->viewModel());
   }
   void on_menu_edit_redo()
   {
      _controller->sourceController.redo(_model->viewModel());
   }
   void on_menu_edit_cut()
   {
      if (copyToClipboard())
         on_menu_edit_delete();
   }
   void on_menu_edit_copy()
   {
      copyToClipboard();
   }
   void on_menu_edit_paste()
   {
      pasteFromClipboard();
   }
   void on_menu_edit_delete()
   {
      _controller->sourceController.deleteText(_model->viewModel());
   }
   void on_menu_edit_select_all()
   {
      _controller->sourceController.selectAll(_model->viewModel());
   }
   void on_menu_edit_indent()
   {
      _controller->doIndent(_model);
   }
   void on_menu_edit_outdent()
   {
      _controller->doOutdent(_model);
   }
   void on_menu_edit_trim()
   {
      _controller->sourceController.trim(_model->viewModel());
   }
   void on_menu_edit_erase_line()
   {
      _controller->sourceController.eraseLine(_model->viewModel());
   }
   void on_menu_edit_upper()
   {
      _controller->sourceController.upperCase(_model->viewModel());
   }
   void on_menu_edit_lower()
   {
      _controller->sourceController.lowerCase(_model->viewModel());
   }
   void on_menu_edit_comment()
   {
      _controller->sourceController.insertBlockText(_model->viewModel(), "//", 2);
   }
   void on_menu_edit_uncomment()
   {
      _controller->sourceController.deleteBlockText(_model->viewModel(), "//", 2);
   }
   void on_menu_project_include()
   {
      //_controller->doInclude();
   }
   void on_menu_project_exclude()
   {
      //_controller->doExclude();
   }
   void on_menu_project_compile()
   {
      _controller->doCompileProject(_model);
   }
   void on_menu_project_cleanup()
   {
      //_controller->cleanUpProject();
   }
   void on_menu_project_forwards()
   {
      //_controller->doSetProjectForwards();
   }
   void on_menu_project_options()
   {
      //_controller->doChangeProject(projectSettingsDialog, _model);
   }
   void on_menu_file_clearfilehistory()
   {
   }
   void on_menu_file_clearprojecthistory()
   {
   }
   void on_menu_project_view()
   {
      bool visible = toggleVisibility(_model->ideScheme.projectView);
      checkMenuItemById(_projectViewMenuItem, visible);
   }
   void on_menu_project_output()
   {
      bool visible = toggleVisibility(_model->ideScheme.compilerOutputControl);
      checkMenuItemById(_outputMenuItem, visible);

      toggleResultTab(_model->ideScheme.compilerOutputControl, visible);
   }
   void on_menu_project_messages()
   {
      bool visible = toggleVisibility(_model->ideScheme.errorListControl);
      checkMenuItemById(_errorListMenuItem, visible);

      toggleResultTab(_model->ideScheme.errorListControl, visible);
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
      if (_model->autoSave)
         saveAll();

      doDebugAction(DebugAction::Run, false);
   }
   void on_menu_debug_next()
   {
      //_controller->doStepOver();
   }
   void on_menu_debug_stepover()
   {
      if (_model->autoSave)
         saveAll();

      doDebugAction(DebugAction::StepOver, false);
   }
   void on_menu_debug_stepin()
   {
      //_controller->doStepInto();
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
      _controller->doDebugStop(_model);
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
        Gtk::TreeViewColumn*);
   void on_errorlist_row_activated(const Gtk::TreeModel::Path& path,
        Gtk::TreeViewColumn*);

   void updateCompileMenu(bool compileEnable, bool debugEnable, bool stopEnable);

   void onDocumentUpdate(DocumentChangeStatus changeStatus);
   void onProjectChange(bool empty);
   void onProjectRefresh(bool empty);
   void onIDEStatusChange(int status);
   void onErrorHighlight(const Gtk::TreeModel::Path& path);

   void onDebugStep();
   void onDebuggerSourceNotFound();
   void onDebugEnd();

   void onComilationStart();
   void onCompilationEnd(int exitCode, int postponedAction);

   void saveFile(int index);
   void saveFileAs(int index);
   void saveFile_finish(PathString& path, int index);
   void saveAll();
   void saveProject();
   void saveProject_finish(PathString& path);

   void onFileClose(int index, FileCloseCallback callback);

   void closeFile_finish(int index);
   void closeFile(int index);

   void closeAll_next(int index);
   void closeAll_finish();
   void closeAll();
   void closeAndExit();

   void closeAllButActive_finish();
   void closeAllButActive_next(int index);
   void closeAllButActive();

public:
   void populate(int counter, Gtk::Widget** children);

   void on_text_model_change(TextViewModelEvent event);
   void on_textframe_change(SelectionEvent event);
   void on_compilation_end(CompletionEvent event);

   GTKIDEWindow(/*const char* caption, */IDEController* controller, IDEModel* model, GtkApp* app);
};

} // _GUI_

#endif // winideH
