//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		Win32: Static dialogs header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef windialogsH
#define windialogsH

#include "winapi32\wincommon.h"
#include "..\idecommon.h"

namespace _GUI_
{

// --- FileDialog ---
class FileDialog
{
public:
   static wchar_t* ProjectFilter;
   static wchar_t* SourceFilter;

private:
   OPENFILENAME _struct;
   wchar_t      _fileName[MAX_PATH * 8];        // ??
   int          _defaultFlags;

public:
   const wchar_t* openFile();
   bool openFiles(_ELENA_::List<wchar_t*>& files);
   bool saveFile(const wchar_t* defaultExt, _ELENA_::Path& path);

   FileDialog(Control* owner, const wchar_t* filter, const wchar_t* caption, const wchar_t* initialDir = NULL);
};

// --- MsgBox ---

class MsgBox
{
public:
   static int show(HWND owner, const wchar_t* message, int type);
   static int showError(const wchar_t* message, const wchar_t* param);
   static int showError(HWND owner, const wchar_t* message, const wchar_t* param);
   static int showQuestion(HWND owner, const wchar_t* message);
   static int showQuestion(HWND owner, const wchar_t* message, const wchar_t* param);
   static int showQuestion(HWND owner, const wchar_t* message, const wchar_t* param1, const wchar_t* param2);

   static bool isCancel(int result) { return result==IDCANCEL; }
   static bool isYes(int result) { return result==IDYES; }
   static bool isNo(int result) { return result==IDNO; }
   //static int show(HWND owner, const wchar_t* message, const wchar_t* param1, const wchar_t* param2, int type);
};

// --- Dialog ---

class Dialog
{
protected:
   HWND     _handle;
   Control* _owner;

   virtual int _getDialogID() const = 0;

   virtual void onCreate() = 0;
   virtual void onOK() = 0;

   virtual void doCommand(int id, int command);

   void enable(int id, bool enabled);

   void getText(int id, wchar_t** text, int length);
   int  getIntText(int id);
   bool getCheckState(int id);
   int  getComboBoxIndex(int id);
   int  getListCount(int id);
   void getListItem(int id, int index, wchar_t** text);
   int  getListIndex(int id);
   int  getListSelCount(int id);
   void getListSelected(int id, int count, int* selected);

   void setText(int id, const wchar_t* text);
   void setIntText(int id, int value);
   void setCheckState(int id, bool value);
   void setComboBoxIndex(int id, int index);
   void setListIndex(int id, int index);
   void setListSelected(int id, int index, bool toggle);
   void setTextLimit(int id, int maxLength);

   void addComboBoxItem(int id, const wchar_t* text);
   void addListItem(int id, const wchar_t* text);

   void insertListItem(int id, int index, const wchar_t* text);
   void removeListItem(int id, int index);

public:
   static BOOL CALLBACK DialogProc(HWND hwnd, size_t message, WPARAM wParam, LPARAM lParam);

   virtual int showModal();

   Dialog(Control* owner);
};

// --- ProjectSettingsDialog ---

class ProjectSettingsDialog : public Dialog
{
   _ProjectManager* _project;

   virtual int _getDialogID() const { return IDD_SETTINGS; }

   void loadTemplateList();

   virtual void onCreate();
   virtual void onOK();

public:   
   ProjectSettingsDialog(Control* owner, _ProjectManager* project) : Dialog(owner)
   {
      _project = project;
   }
};

// --- ProjectForwardsDialog ---

class ProjectForwardsDialog : public Dialog
{
   _ProjectManager* _project;

   bool  _changed;
   int   _current;

   bool validateItem(wchar_t* &text);

   void addItem();
   void getItem();
   void editItem();
   void deleteItem();

   virtual int _getDialogID() const { return IDD_FORWARDS; }

   virtual void onCreate();
   virtual void onOK();

   virtual void doCommand(int id, int command);

public:   
   ProjectForwardsDialog(Control* owner, _ProjectManager* project)
      : Dialog(owner) 
   { 
      _changed = false; 
	   _current = -1;

      _project = project;
   }
};

// --- WindowsDialog ---

class WindowsDialog : public Dialog
{
protected:
   virtual void onClose() = 0;
   virtual void onListChange();

   virtual int _getDialogID() const { return IDD_WINDOWS; }

   virtual void doCommand(int id, int command);

   void addWindow(const wchar_t* docName)
   {
      addListItem(IDC_WINDOWS_LIST, docName);
   }

   void selectWindow(int index)
   {
      setListSelected(IDC_WINDOWS_LIST, index, true);
   }

   int getSelectedWindow()
   {
      return getListIndex(IDC_WINDOWS_LIST);
   }
   int* getSelectedWindows(int& count)
   {
      count = getListSelCount(IDC_WINDOWS_LIST);
      int* selected = (int*)malloc(sizeof(INT) * count);
      getListSelected(IDC_WINDOWS_LIST, count, selected);

      return selected;
   }

public:
   WindowsDialog(Control* owner);
};

// --- GoToLineDialog ---

class GoToLineDialog : public Dialog
{
   int _number;

   virtual int _getDialogID() const { return IDD_GOTOLINE; }

   virtual void onCreate();
   virtual void onOK();

public:
   int getLineNumber() const { return _number; }

   GoToLineDialog(Control* owner, int number)
      : Dialog(owner) 
   {
      _number = number;
   }
};

// --- EditorSettings ---

class EditorSettings : public Dialog
{
   Model* _model;

   virtual int _getDialogID() const { return IDD_EDITOR_SETTINGS; }

   virtual void doCommand(int id, int command);
   virtual void onCreate();
   virtual void onOK();
   virtual void onEditorHighlightSyntaxChanged();

public:
   EditorSettings(Control* owner, Model* model)
      : Dialog(owner)
   {
      _model = model;
   }
};

// --- DebuggerSettings ---

class DebuggerSettings : public Dialog
{
   Model* _model;

   virtual int _getDialogID() const { return IDD_DEBUGGER_SETTINGS; }

   virtual void onCreate();
   virtual void onOK();

public:
   DebuggerSettings(Control* owner, Model* model)
      : Dialog(owner)
   {
      _model = model;
   }
};

// --- FindDialog ---

class FindDialog : public Dialog
{
   bool           _replaceMode;
   SearchOption*  _option;

   SearchHistory* _searchHistory;
   SearchHistory* _replaceHistory;
   
   void copyHistory(int id, SearchHistory* history);

   virtual int _getDialogID() const { return _replaceMode ? IDD_EDITOR_REPLACE : IDD_EDITOR_FIND; }

   virtual void onCreate();
   virtual void onOK();

public:
   FindDialog(Control* owner, bool replaceMode, SearchOption* option, SearchHistory* searchHistory, SearchHistory* replaceHistory)
      : Dialog(owner) 
   {
      _replaceMode = replaceMode;
      _option = option;

      _searchHistory = searchHistory;
      _replaceHistory = replaceHistory;
   }
};

// --- AboutDialog ---

class AboutDialog : public Dialog
{
   virtual int _getDialogID() const { return IDD_ABOUT; }

   virtual void onCreate();
   virtual void onOK() {}

public:
   AboutDialog(Control* owner)
      : Dialog(owner) 
   { 
   }
};

} // _GUI_

#endif // windialogsH

