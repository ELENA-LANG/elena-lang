//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                             (C)2024-2025, by Aleksey Rakov
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

// --- GTKIDEView ---

class GTKIDEWindow : public SDIWindow
{
protected:
   class Clipboard : public ClipboardBase
   {
      Glib::RefPtr<Gdk::Clipboard> _clipboard;

      GTKIDEWindow* _owner;
      Glib::ustring _strData;

   public:
      bool copyToClipboard(DocumentView* docView, bool selectionMode) override;
      void pasteFromClipboard(DocumentChangeStatus& status, DocumentView* docView) override;

      Clipboard(GTKIDEWindow* owner)
      {
         _owner = owner;

         _clipboard = owner->get_clipboard();
      }
   };

   GtkApp*                      _app;

   IDEModel*                    _model;
   IDEController*               _controller;

   Clipboard                    _clipboard;

   ProjectTreeColumns           _projectTreeColumns;
   Glib::RefPtr<Gtk::TreeStore> _projectTree;

   // dialogs
   FileDialog                   fileDialog;
   FileDialog                   projectDialog;
   MessageDialog                messageDialog;

   ProjectSettings              projectSettingsDialog;

   void populateUI();

   //Glib::RefPtr<Gtk::Action> getMenuItem(ustr_t name) override;

   bool copyToClipboard()
   {
      return _controller->sourceController.copyToClipboard(_model->viewModel(), &_clipboard);
   }

   // event signals
   void on_menu_file_new_source()
   {
      _controller->doNewFile(_model);
   }
   void on_menu_file_new_project()
   {
      _controller->doNewProject(fileDialog, projectDialog, messageDialog, projectSettingsDialog, _model);
   }
   void on_menu_file_open_source()
   {
      _controller->doOpenFile(fileDialog, _model);
      //_recentFileList.reload();
      //_recentProjectList.reload();
   }
   void on_menu_file_open_project()
   {
      _controller->doOpenProject(fileDialog, projectDialog, messageDialog, _model);
      //_recentProjectList.reload();
   }
   void on_menu_file_quit()
   {
      if(_controller->doExit(fileDialog, projectDialog, messageDialog, _model)) {
         _app->quit();
      }
   }
   void on_menu_file_save()
   {
      _controller->doSaveFile(fileDialog, _model, false, true);
   }
   void on_menu_file_saveas()
   {
      _controller->doSaveFile(fileDialog, _model, true, true);
   }
   void on_menu_project_saveas()
   {
      _controller->doSaveProject(projectDialog, _model, true);
   }
   void on_menu_file_saveall()
   {
      _controller->doSaveAll(fileDialog, projectDialog, _model);
   }
   void on_menu_file_close()
   {
      _controller->doCloseFile(fileDialog, messageDialog, _model);
   }
   void on_menu_file_closeall()
   {
      _controller->doCloseAll(fileDialog, projectDialog, messageDialog, _model, false);
   }
   void on_menu_file_closeproject()
   {
      _controller->doCloseProject(fileDialog, projectDialog, messageDialog, _model);
   }
   void on_menu_file_closeallbutactive()
   {
      _controller->doCloseAllButActive(fileDialog, messageDialog, _model);
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
      _controller->sourceController.pasteFromClipboard(_model->viewModel(), &_clipboard);
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
      //_controller->doCompileProject();
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
      _controller->doChangeProject(projectSettingsDialog, _model);
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
      checkMenuItemById("ViewMenu/ProjectView", visible);
   }
   void on_menu_project_output()
   {
//      if (!_skip) {
//         _controller->doShowCompilerOutput(!_model->compilerOutput);
//      }
//      else _skip = false;
   }
   void on_menu_project_messages()
   {
//      if (!_skip) {
//         _controller->doShowMessages(!_model->messages);
//      }
//      else _skip = false;
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
      //_controller->doDebugRun();
   }
   void on_menu_debug_next()
   {
      //_controller->doStepOver();
   }
   void on_menu_debug_stepover()
   {
      //_controller->doStepOver();
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

   void onDocumentUpdate(DocumentChangeStatus changeStatus);
   void onProjectChange(bool empty);
   void onProjectRefresh(bool empty);
   void onIDEStatusChange(int status);

public:
   void populate(int counter, Gtk::Widget** children);

   void on_text_model_change(TextViewModelEvent event);
   void on_textframe_change(SelectionEvent event);

   GTKIDEWindow(/*const char* caption, */IDEController* controller, IDEModel* model, GtkApp* app);
};

} // _GUI_

#endif // winideH
