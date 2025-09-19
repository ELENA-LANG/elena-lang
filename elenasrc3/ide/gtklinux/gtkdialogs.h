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
#include "ideview.h"

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

   typedef void(*FileDialogListCallback)(void*, PathList*);
   typedef void(*FileDialogCallback)(void*, PathString*);
   typedef void(*MessageCallback)(void* arg, int result);

   // --- FileDialog ---
   class FileDialog// : public FileDialogBase
   {
      Gtk::Window* _owner;

      const char*  _initialDir;
      const char*  _caption;

      const char** _filter;
      int          _filterCounter;

      PathString   _defExtension;

      FileDialogListCallback  _listCallback;
      FileDialogCallback      _callback;
      void*                   _callbackArg;

      void prepareDialog(Glib::RefPtr<Gtk::FileDialog> dialog);

      void on_list_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result,
          const Glib::RefPtr<Gtk::FileDialog>& dialog);
      void on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result,
          const Glib::RefPtr<Gtk::FileDialog>& dialog);

   public:
      bool openFile(PathString& path)/* override*/;
      void openFiles(void* arg, FileDialogListCallback callback);
      void saveFile(void* arg, FileDialogCallback callback);

      FileDialog(Gtk::Window* owner, const char** filter, int filterCounter, const char* caption,
         const char* initialDir = nullptr);
   };

   class MessageDialog
   {
      Gtk::Window*    _owner;
      //Glib::RefPtr<Gtk::AlertDialog> _dialog;

      MessageCallback _callback;
      void*           _callbackArg;

      void on_question_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result,
         const Glib::RefPtr<Gtk::AlertDialog>& dialog);

      void show(const char* message, Gtk::MessageType messageType, Gtk::ButtonsType buttonTypes, bool withCancel);

   public:
      void question(text_str message, text_str param, void* arg, MessageCallback callback);
      void question(text_str message, void* arg, MessageCallback callback);

      void info(text_str message);

      MessageDialog(Gtk::Window* owner)
      {
         _owner = owner;
         //_dialog = nullptr;
         _callback = nullptr;
         _callbackArg = nullptr;
      }
   };

   class ProjectSettings : public Gtk::Window
   {
      Gtk::Box          _box;

      Gtk::Frame        _projectFrame;
      Gtk::Grid         _projectGrid;
      Gtk::Label        _typeLabel;
      Gtk::ComboBoxText _typeCombobox;
      Gtk::Label        _namespaceLabel;
      Gtk::Entry        _namespaceText;
      Gtk::Label        _profileLabel;
      Gtk::ComboBoxText _profileCombobox;

      Gtk::Frame        _compilerFrame;
      Gtk::Grid         _compilerGrid;
      Gtk::Label        _strictTypeLabel;
      Gtk::CheckButton  _strictTypeCheckbox;
      Gtk::Label        _optionsLabel;
      Gtk::Entry        _optionsText;
      Gtk::Label        _warningLabel;
      Gtk::ComboBoxText _warningCombobox;

      Gtk::Frame        _linkerFrame;
      Gtk::Grid         _linkerrGrid;
      Gtk::Label        _targetLabel;
      Gtk::Entry        _targetText;
      Gtk::Label        _outputLabel;
      Gtk::Entry        _outputText;

      Gtk::Frame        _debuggerFrame;
      Gtk::Grid         _debuggerGrid;
      Gtk::Label        _modeLabel;
      Gtk::ComboBoxText _modeCombobox;
      Gtk::Label        _argumentsLabel;
      Gtk::Entry        _argumentsText;

      Gtk::Box          _footer;
      Gtk::Button       _buttonOK;
      Gtk::Button       _button_Cancel;

      ProjectModel*     _model;

      void loadTemplateList();
      void loadProfileList();

      void populate();
      void save();

      void onOK();
      void onCancel();

   public:
      void showModal();

      ProjectSettings(Gtk::Window* owner, ProjectModel* model);
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
