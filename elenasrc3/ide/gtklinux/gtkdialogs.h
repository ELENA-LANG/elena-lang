//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		GTK: Static dialogs header
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKDIALOGS_H
#define GTKDIALOGS_H

#include "controller.h"
#include "gtklinux/gtkcommon.h"
//#include "editcontrol.h"
//#include "ideproject.h"
//#include "ideview.h"

namespace elena_lang
{
//   class MsgBox
//   {
//   public:
//      static int show(HWND owner, const wchar_t* message, int type);
//
//      static int showQuestion(HWND owner, const wchar_t* message);
//
//      static bool isCancel(int result) { return result == IDCANCEL; }
//      static bool isYes(int result) { return result == IDYES; }
//      static bool isNo(int result) { return result == IDNO; }
//   };

   // --- FileDialog ---
   class FileDialog : public FileDialogBase
   {
      Gtk::Window* _owner;

      const char*  _initialDir;
      const char*  _caption;

      const char** _filter;
      int          _filterCounter;
//
//      OPENFILENAME _struct;
//      wchar_t      _fileName[MAX_PATH * 8];        // ??
//      int          _defaultFlags;
//
   public:
//      static const wchar_t* ProjectFilter;
//      static const wchar_t* SourceFilter;

      bool openFile(PathString& path) override;
      bool openFiles(List<path_t, freepath>& files) override;
      bool saveFile(path_t ext, PathString& path) override;

      FileDialog(Gtk::Window* owner, const char** filter, int filterCounter, const char* caption,
         const char* initialDir = nullptr);
   };

   class MessageDialog : public MessageDialogBase
   {
      Gtk::Window* _owner;

      int show(const char* message, Gtk::MessageType messageType, Gtk::ButtonsType buttonTypes, bool withCancel);

   public:
      Answer question(text_str message, text_str param) override;
      Answer question(text_str message) override;

      void info(text_str message) override;

      MessageDialog(Gtk::Window* owner)
      {
         _owner = owner;
      }
   };

   class ProjectSettings : public Gtk::Dialog, public ProjectSettingsBase
   {
      Gtk::Frame _projectFrame;
      Gtk::Grid  _projectGrid;

      Gtk::Frame _compilerFrame;
      Gtk::Grid  _compilerGrid;

      Gtk::Frame _linkerFrame;
      Gtk::Grid  _linkerrGrid;

      Gtk::Frame _debuggerFrame;
      Gtk::Grid  _debuggerGrid;

      ProjectModel* _model;

//      void loadTemplateList();
//      void loadProfileList();

      void populate();
//      void onOK() override;

   public:
//      bool showModal() override;

      ProjectSettings(ProjectModel* model);
   };

//   class EditorSettings : public WinDialog, public EditorSettingsBase
//   {
//      TextViewModelBase* _model;
//
//      void onCreate() override;
//      void onOK() override;
//
//   public:
//      bool showModal() override;
//
//      EditorSettings(HINSTANCE instance, WindowBase* owner, TextViewModelBase* model);
//   };
//
//   class IDESettings : public WinDialog, public IDESettingsBase
//   {
//      IDEModel* _model;
//
//      void onCreate() override;
//      void onOK() override;
//
//   public:
//      bool showModal() override;
//
//      IDESettings(HINSTANCE instance, WindowBase* owner, IDEModel* model);
//   };
//
//   class DebuggerSettings : public WinDialog, public DebuggerSettingsBase
//   {
//      ProjectModel* _model;
//
//      void onCreate() override;
//      void onOK() override;
//
//   public:
//      bool showModal() override;
//
//      DebuggerSettings(HINSTANCE instance, WindowBase* owner, ProjectModel* model);
//   };
//
//   class FindDialog : public WinDialog, public FindDialogBase
//   {
//      FindModel* _model;
//      bool       _replaceMode;
//
//      void copyHistory(int id, SearchHistory* history);
//
//      void onCreate() override;
//      void onOK() override;
//
//   public:
//      bool showModal() override;
//
//      FindDialog(HINSTANCE instance, WindowBase* owner, bool replaceMode, FindModel* model);
//   };
//
//   // --- GoToLineDialog ---
//
//   class GoToLineDialog : public WinDialog, public GotoDialogBase
//   {
//      int _lineNumber;
//
//      void onCreate() override;
//      void onOK() override;
//
//   public:
//      bool showModal(int& row) override;
//
//      GoToLineDialog(HINSTANCE instance, WindowBase* owner);
//   };
//
//   class WindowListDialog : public WinDialog, public WindowListDialogBase
//   {
//      TextViewModel* _model;
//      int            _selectedIndex;
//
//      void doCommand(int id, int command) override;
//
//      void onListChange();
//
//      int getSelectedWindow();
//
//   public:
//      SelectResult selectWindow() override;
//
//      void onCreate() override;
//      void onOK() override;
//
//      WindowListDialog(HINSTANCE instance, WindowBase* owner, TextViewModel* model);
//   };
//
//   class AboutDialog : public WinDialog
//   {
//   public:
//      void onCreate() override;
//      void onOK() override;
//
//      AboutDialog(HINSTANCE instance, WindowBase* owner);
//   };

}

#endif
