//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//		GTK: Static dialogs header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef windialogsH
#define windialogsH

#include "gtk-linux32/gtkcommon.h"
#include "../idecommon.h"

namespace _GUI_
{

//// --- FileDialog ---
//class FileDialog
//{
//private:
//   Control*      _owner;
//   const TCHAR*  _caption;
//   const TCHAR*  _filter;
//   GtkWidget*    _dialog;
//   _ELENA_::Path _filePath;
//
//   void clear()
//   {
//      if (_dialog)
//         gtk_widget_destroy(_dialog);
//
//      _dialog = NULL;
//   }
//
//   void addFilter();
//
//public:
//   static const TCHAR* SourceFilter;
//   static const TCHAR* ProjectFilter;
//
//   const TCHAR* openFile();
//   bool openFiles(_ELENA_::List<TCHAR*>& files);
//   bool saveFile(const TCHAR* defaultExt, _ELENA_::Path& path);
//
//   FileDialog(Control* owner, const TCHAR* filter, const TCHAR* caption, const TCHAR* initialDir = NULL);
//   ~FileDialog();
//};
//
//// --- MsgBox ---
//
//class MsgBox
//{
//public:
//   static bool isCancel(int result) { return result==GTK_RESPONSE_CANCEL; }
//   static bool isYes(int result) { return result==GTK_RESPONSE_YES; }
//   static bool isNo(int result) { return result==GTK_RESPONSE_NO; }
//
//   static int show(GtkWidget* owner, const TCHAR* message, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, bool withCancel = false);
//   static int showError(const TCHAR* message, const TCHAR* param);
//   static int showError(GtkWidget* owner, const TCHAR* message, const TCHAR* param);
//   static int showQuestion(GtkWidget* owner, const TCHAR* message);
//   static int showQuestion(GtkWidget* owner, const TCHAR* message, const TCHAR* param);
//   static int showQuestion(GtkWidget* owner, const TCHAR* message, const TCHAR* param1, const TCHAR* param2);
//   //static int show(HWND owner, const TCHAR* message, const TCHAR* param1, const TCHAR* param2, int type);
//};

// --- Dialog ---

//class Dialog
//{
//protected:
//   Control* _owner;

//public:
//   virtual int showModal() = 0;
//
//   Dialog(Control* owner);
//};

// --- WindowsDialog ---

class WindowsDialog //: public Dialog
{
protected:
//   GtkWidget*         _dialog;
//   GtkListStore*      _store;
//   GtkTreeViewColumn* _column;
//   GtkCellRenderer*   _renderer;
//   GtkWidget*         _scroll_window;
//   GtkWidget*         _list_view;
//
//   virtual void onCreate() = 0;
//   virtual void onOK() = 0;
//   virtual void onClose() = 0;
//
//   void addWindow(const TCHAR* docName);
//   void selectWindow(int index);
//   int getSelectedWindow();
//   int* getSelectedWindows(int& count);

public:
//   virtual int showModal();

   WindowsDialog(Control* owner);
//   virtual ~WindowsDialog();
};

//// --- EditorSettings ---
//
//class EditorSettings : public Dialog
//{
////   virtual void doCommand(int id, int command);
////   virtual void onCreate();
////   virtual void onOK();
////   virtual void onEditorHighlightSyntaxChanged();
//
//public:
//   virtual int showModal();
//
//   EditorSettings(Control* owner)
//      : Dialog(owner)
//   {
//   }
//};
//

// --- ProjectSettingsDialog ---

class ProjectSettingsDialog : public Gtk::Dialog
{
   //Gtk::Frame _project_frame;
   //Gtk::Frame _compiler_frame;
   //Gtk::Frame _linker_frame;
   //Gtk::Frame _debugger_frame;

   Gtk::Frame _projectFrame;
   Gtk::Grid  _projectGrid;
   Gtk::Label _typeLabel;
   Gtk::ComboBoxText _typeCombobox;
   Gtk::Label _namespaceLabel;
   Gtk::Entry _namespaceText;

   Gtk::Frame _compilerFrame;
   Gtk::Grid  _compilerGrid;
   Gtk::Label _warningLabel;
   Gtk::CheckButton _warningCheckbox;
   Gtk::Label _optionsLabel;
   Gtk::Entry _optionsText;

   Gtk::Frame _linkerFrame;
   Gtk::Grid  _linkerrGrid;
   Gtk::Label _targetLabel;
   Gtk::Entry _targetText;
   Gtk::Label _outputLabel;
   Gtk::Entry _outputText;

   Gtk::Frame _debuggerFrame;
   Gtk::Grid  _debuggerGrid;
   Gtk::Label _modeLabel;
   Gtk::ComboBoxText _modeCombobox;
   Gtk::Label _argumentsLabel;
   Gtk::Entry _argumentsText;

   _ProjectManager* _project;

   void setText(Gtk::Entry& control, const char* value);

   void loadTemplateList();
   void populate();

public:
   ProjectSettingsDialog(_ProjectManager* project);

   void save();
};

//// --- FindDialog ---
//
//typedef _ELENA_::List<TCHAR*> SearchHistory;
//
//class FindDialog : public Dialog
//{
//protected:
//   GtkWidget*      _dialog;
//   GtkWidget*      _search;
//   GtkWidget*      _replace;
//   GtkWidget*      _findCase;
//   GtkWidget*      _findWhole;
//
//   GtkListStore*   _searchStore;
//   GtkListStore*   _replaceStore;
//
//   bool           _replaceMode;
//   SearchOption*  _option;
//
//   SearchHistory* _searchHistory;
//   SearchHistory* _replaceHistory;
//
//   virtual void onCreate();
//   virtual void onOK();
//
//   void copyHistory(GtkListStore* store, SearchHistory* history);
//
//public:
//   virtual int showModal();
//
//   FindDialog(Control* owner, bool replaceMode, SearchOption* option, SearchHistory* searchHistory, SearchHistory* replaceHistory);
//   ~FindDialog();
//};
//
//
//// --- AboutDialog ---
//
//class AboutDialog : public Dialog
//{
////   virtual void onCreate();
////   virtual void onOK() {}
//
//public:
//   virtual int showModal();
//
//   AboutDialog(Control* owner)
//      : Dialog(owner)
//   {
//   }
//};

} // _GUI_

#endif // windialogsH
