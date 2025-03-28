//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		Win32: Static dialogs header
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINDIALOGS_H
#define WINDIALOGS_H

#include "controller.h"
#include "editcontrol.h"
#include "ideproject.h"
#include "ideview.h"

namespace elena_lang
{
   class MsgBox
   {
   public:
      static int show(HWND owner, const wchar_t* message, int type);

      static int showQuestion(HWND owner, const wchar_t* message);

      static bool isCancel(int result) { return result == IDCANCEL; }
      static bool isYes(int result) { return result == IDYES; }
      static bool isNo(int result) { return result == IDNO; }
   };

   // --- FileDialog ---
   class FileDialog : public FileDialogBase
   {
      WindowBase*  _owner;

      OPENFILENAME _struct;
      wchar_t      _fileName[MAX_PATH * 8];        // ??
      int          _defaultFlags;

   public:
      static const wchar_t* ProjectFilter;
      static const wchar_t* SourceFilter;

      bool openFile(PathString& path) override;
      bool openFiles(List<path_t, freepath>& files) override;
      bool saveFile(path_t ext, PathString& path) override;

      FileDialog(HINSTANCE instance, WindowBase* owner, const wchar_t* filter, const wchar_t* caption,
         const wchar_t* initialDir = nullptr);
   };

   // --- FontDialog ---
   class FontDialog : public FontDialogBase
   {
      WindowBase* _owner;

      CHOOSEFONT  _cf;
      LOGFONT     _lf;

   public:
      bool selectFont(FontInfo& fontInfo) override;

      FontDialog(HINSTANCE instance, WindowBase* owner);
   };

   class MessageDialog : public MessageDialogBase
   {
      WindowBase* _owner;

   public:
      Answer question(text_str message, text_str param) override;
      Answer question(text_str message) override;

      void info(text_str message) override;

      MessageDialog(WindowBase* owner)
      {
         _owner = owner;
      }
   };

   class WinDialog
   {
   protected:
      HINSTANCE   _instance;
      HWND        _handle;

      WindowBase* _owner;
      int         _dialogId;

      virtual void onCreate() = 0;
      virtual void onOK() = 0;

      virtual void doCommand(int id, int command);

      void enable(int id, bool enabled);

      void addComboBoxItem(int id, const wchar_t* text);
      void setComboBoxIndex(int id, int index);
      int  getComboBoxIndex(int id);
      void clearComboBoxItem(int id);

      void addListItem(int id, const wchar_t* text);
      int  getListSelCount(int id);
      int  getListIndex(int id);

      void setText(int id, const wchar_t* text);
      void setIntText(int id, int value);
      void getText(int id, wchar_t** text, int length);
      int  getIntText(int id);
      void setTextLimit(int id, int maxLength);

      void setCheckState(int id, bool value);
      void setUndefinedCheckState(int id);
      bool getCheckState(int id);
      bool isUndefined(int id);

   public:
      static BOOL CALLBACK DialogProc(HWND hwnd, size_t message, WPARAM wParam, LPARAM lParam);

      int show();

      WinDialog(HINSTANCE instance, WindowBase* owner, int dialogId)
         : _handle(nullptr)
      {
         _instance = instance;
         _owner = owner;
         _dialogId = dialogId;
      }
   };

   class ProjectSettings : public WinDialog, public ProjectSettingsBase
   {
      ProjectModel* _model;

      void loadTemplateList();
      void loadProfileList();

      void onCreate() override;
      void onOK() override;

   public:
      bool showModal() override;

      ProjectSettings(HINSTANCE instance, WindowBase* owner, ProjectModel* model);
   };

   class EditorSettings : public WinDialog, public EditorSettingsBase
   {
      TextViewModelBase* _model;

      void onCreate() override;
      void onOK() override;

   public:
      bool showModal() override;

      EditorSettings(HINSTANCE instance, WindowBase* owner, TextViewModelBase* model);
   };

   class IDESettings : public WinDialog, public IDESettingsBase
   {
      IDEModel* _model;

      void onCreate() override;
      void onOK() override;

      void loadFontList();

   public:
      bool showModal() override;

      IDESettings(HINSTANCE instance, WindowBase* owner, IDEModel* model);
   };

   class DebuggerSettings : public WinDialog, public DebuggerSettingsBase
   {
      ProjectModel* _model;

      void onCreate() override;
      void onOK() override;

   public:
      bool showModal() override;

      DebuggerSettings(HINSTANCE instance, WindowBase* owner, ProjectModel* model);
   };

   class FindDialog : public WinDialog, public FindDialogBase
   {
      FindModel* _model;
      bool       _replaceMode;

      void copyHistory(int id, SearchHistory* history);

      void onCreate() override;
      void onOK() override;

   public:
      bool showModal() override;

      FindDialog(HINSTANCE instance, WindowBase* owner, bool replaceMode, FindModel* model);
   };

   // --- GoToLineDialog ---

   class GoToLineDialog : public WinDialog, public GotoDialogBase
   {
      int _lineNumber;

      void onCreate() override;
      void onOK() override;

   public:
      bool showModal(int& row) override;

      GoToLineDialog(HINSTANCE instance, WindowBase* owner);
   };

   class WindowListDialog : public WinDialog, public WindowListDialogBase
   {
      TextViewModel* _model;
      int            _selectedIndex;

      void doCommand(int id, int command) override;

      void onListChange();

      int getSelectedWindow();

   public:
      SelectResult selectWindow() override;

      void onCreate() override;
      void onOK() override;

      WindowListDialog(HINSTANCE instance, WindowBase* owner, TextViewModel* model);
   };

   class AboutDialog : public WinDialog
   {
   public:
      void onCreate() override;
      void onOK() override;

      AboutDialog(HINSTANCE instance, WindowBase* owner);
   };

}

#endif

