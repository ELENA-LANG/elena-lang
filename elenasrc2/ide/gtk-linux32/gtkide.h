//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

#include "gtk-linux32/gtksdi.h"
#include "../ide.h"

namespace _GUI_
{
// --- GTKIDEView ---

class MainWindow : public SDIWindow
{
   _Controller*   _controller;
   Model*         _model;

   Gtk::Toolbar*   _toolbar;
   Gtk::TreeView*  _projectView;
   Gtk::Statusbar* _statusbar;
   Gtk::Notebook*  _bottomTab;
   EditFrame*      _mainFrame;

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

   void populateMenu();
   void populateToolbar();

public:
   bool copyToClipboard(Document* document);
   void pasteFrameClipboard(Document* document);

   void refreshDocument();

   int newDocument(const char* name, Document* doc);
   void closeDocument(int index);
   int getCurrentDocumentIndex();

   MainWindow(const char* caption, _Controller* controller, Model* model);
};

////// --- Win32ContextBrowser ---
////
////class Win32ContextBrowser : public ContextBrowser
////{
////   ContextMenu _menu;
////
////public:
////   TreeViewItem hitTest(short x, short y);
////
////   void showContextMenu(HWND owner, short x, short y);
////
////   Win32ContextBrowser(Control* owner);
////};
////
////// --- AppDebugController ---
////
////class Win32AppDebugController : public AppDebugController
////{
////   Window* _receptor;
////
////   void _notify(int code);
////   void _notify(int code, const TCHAR* message, int param = 0);
////   void _notify(int code, const TCHAR* message, int param1, int param2);
////   void _notify(int code, const TCHAR* source, HighlightInfo info);
////
////public:
////   void assign(Window* receptor)
////   {
////      _receptor = receptor;
////   }
////
////   virtual void onStart()
////   {
////      _notify(IDE_DEBUGGER_START);
////   }
////
////   virtual void onStop(bool failed)
////   {
////      _notify(failed ? IDE_DEBUGGER_BREAK : IDE_DEBUGGER_STOP);
////   }
////
////   virtual void onStep(const TCHAR* source, int row, int disp, int length)
////   {
////      _notify(IDE_DEBUGGER_STEP, source, HighlightInfo(row, disp, length));
//   }
//
//   void onCheckPoint(const TCHAR* message)
//   {
//      _notify(IDE_DEBUGGER_CHECKPOINT, message);
//   }
//
//   void onNotification(const TCHAR* message, size_t address, int code)
//   {
//      _notify(IDM_DEBUGGER_EXCEPTION, message, address, code);
//   }
//
//   virtual void onLoadModule(const TCHAR* name)
//   {
//      _notify(IDE_DEBUGGER_LOADMODULE, name);
//   }
//
//   Win32AppDebugController()
//   {
//      _receptor = NULL;
//   }
//};

} // _GUI_

#endif // winideH
