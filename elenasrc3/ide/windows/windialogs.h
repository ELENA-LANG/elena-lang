//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		Win32: Static dialogs header
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINDIALOGS_H
#define WINDIALOGS_H

#include "controller.h"
#include "editcontrol.h"
#include "ideproject.h"

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

   class MessageDialog : public MessageDialogBase
   {
      WindowBase* _owner;

   public:
      Answer question(text_str message, text_str param) override;

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

      void addComboBoxItem(int id, const wchar_t* text);
      void setComboBoxIndex(int id, int index);
      int  getComboBoxIndex(int id);

      void setText(int id, const wchar_t* text);
      void getText(int id, wchar_t** text, int length);
      void setTextLimit(int id, int maxLength);

   public:
      static BOOL CALLBACK DialogProc(HWND hwnd, size_t message, WPARAM wParam, LPARAM lParam);

      int show();

      WinDialog(HINSTANCE instance, WindowBase* owner)
      {
         _instance = instance;
         _owner = owner;
      }
   };

   class ProjectSettings : public WinDialog, public ProjectSettingsBase
   {
      ProjectModel* _model;

      void loadTemplateList();

      void onCreate() override;
      void onOK() override;

   public:
      bool showModal() override;

      ProjectSettings(HINSTANCE instance, WindowBase* owner, ProjectModel* model);
   };

}

#endif

