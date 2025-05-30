//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//    GTK Static dialog implementations
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtkdialogs.h"

//#include "Resource.h"
//#include "eng/messages.h"

using namespace elena_lang;

//// --- MsgBox ---
//
//int MsgBox :: show(HWND owner, const wchar_t* message, int type)
//{
//   return ::MessageBox(owner, message, APP_NAME, type);
//}
//
//int MsgBox :: showQuestion(HWND owner, const wchar_t* message)
//{
//   return show(owner, message, MB_YESNOCANCEL | MB_ICONQUESTION);
//}

// --- FileDialog ---

//const wchar_t* FileDialog::ProjectFilter = _T("ELENA Project file\0*.prj\0All types\0*.*\0\0");
//const wchar_t* FileDialog::SourceFilter = _T("ELENA source file\0*.l\0All types\0*.*\0\0");
//
FileDialog :: FileDialog(Gtk::Window* owner, const char** filter, int filterCounter,
   const char* caption, const char* initialDir)
{
   _owner = owner;
   _caption = caption;
   _filter = filter;
   _filterCounter = filterCounter;
   _initialDir = initialDir;
}

bool FileDialog :: openFile(PathString& path)
{
   Gtk::FileChooserDialog dialog(_caption, Gtk::FILE_CHOOSER_ACTION_OPEN);
   dialog.set_transient_for(*_owner);

   if (!emptystr(_initialDir))
      dialog.set_current_folder (_initialDir);

   dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
   dialog.add_button("_Open", Gtk::RESPONSE_OK);

   for (int i = 0; i < _filterCounter; i += 2) {
      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();

      filter_l->set_name(_filter[i + 1]);
      filter_l->add_pattern(_filter[i]);
      dialog.add_filter(filter_l);
   }

   int result = dialog.run();
   if (result == Gtk::RESPONSE_OK) {
      std::string filename = dialog.get_filename();

      path.copy(filename.c_str());

      return true;
   }
   return false;
}

bool FileDialog :: openFiles(List<path_t, freepath>& files)
{
   Gtk::FileChooserDialog dialog(_caption, Gtk::FILE_CHOOSER_ACTION_OPEN);
   dialog.set_transient_for(*_owner);

   if (!emptystr(_initialDir))
      dialog.set_current_folder (_initialDir);

   dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
   dialog.add_button("_Open", Gtk::RESPONSE_OK);

   for (int i = 0; i < _filterCounter; i += 2) {
      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();

      filter_l->set_name(_filter[i + 1]);
      filter_l->add_pattern(_filter[i]);
      dialog.add_filter(filter_l);
   }

   int result = dialog.run();
   if (result == Gtk::RESPONSE_OK) {
      std::string filename = dialog.get_filename();

      files.add(StrUtil::clone(filename.c_str()));

      return true;
   }
   return false;
}

bool FileDialog :: saveFile(path_t ext, PathString& path)
{
   Gtk::FileChooserDialog dialog(_caption, Gtk::FILE_CHOOSER_ACTION_SAVE);
   dialog.set_transient_for(*_owner);

   if (!emptystr(_initialDir))
      dialog.set_current_folder (_initialDir);

   dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
   dialog.add_button("_Save", Gtk::RESPONSE_OK);

   for (int i = 0; i < _filterCounter; i += 2) {
      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();

      filter_l->set_name(_filter[i + 1]);
      filter_l->add_pattern(_filter[i]);
      dialog.add_filter(filter_l);
   }

   int result = dialog.run();
   if (result == Gtk::RESPONSE_OK) {
      std::string filename = dialog.get_filename();

      path.copy(filename.c_str());

      return true;
   }
   return false;
}

// --- MessageDialog ---

int MessageDialog :: show(const char* message, Gtk::MessageType messageType, Gtk::ButtonsType buttonTypes, bool withCancel)
{
   Gtk::MessageDialog dialog(message, false, messageType, buttonTypes, true);
   dialog.set_transient_for(*_owner);

   if (withCancel)
      dialog.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);

   return dialog.run();
}

MessageDialogBase::Answer MessageDialog :: question(text_str message, text_str param)
{
   IdentifierString questionStr(message, param);

   return question(*questionStr);
}

MessageDialogBase::Answer MessageDialog :: question(text_str message)
{
   int retVal = show(message, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);

   if (retVal == Gtk::ResponseType::RESPONSE_YES) {
      return Answer::Yes;
   }
   else if (retVal == Gtk::ResponseType::RESPONSE_CANCEL) {
      return Answer::Cancel;
   }
   else return Answer::No;
}

void MessageDialog :: info(text_str message)
{
   show(message, Gtk::MessageType::MESSAGE_INFO, Gtk::ButtonsType::BUTTONS_OK, false);
}

//// --- WinDialog ---
//
//BOOL CALLBACK WinDialog::DialogProc(HWND hWnd, size_t message, WPARAM wParam, LPARAM lParam)
//{
//   WinDialog* dialog = (WinDialog*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
//   switch (message) {
//      case WM_INITDIALOG:
//         dialog = (WinDialog*)lParam;
//         dialog->_handle = hWnd;
//         ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)lParam);
//
//         dialog->onCreate();
//
//         return 0;
//      case WM_COMMAND:
//         dialog->doCommand(LOWORD(wParam), HIWORD(wParam));
//         return TRUE;
//      default:
//         return FALSE;
//   }
//}
//
//void WinDialog :: doCommand(int id, int command)
//{
//   switch (id) {
//      case IDOK:
//         onOK();
//         ::EndDialog(_handle, IDOK);
//         break;
//      case IDCANCEL:
//         ::EndDialog(_handle, IDCANCEL);
//         break;
//   }
//}
//
//int WinDialog :: show()
//{
//   return (int)::DialogBoxParam(_instance, MAKEINTRESOURCE(_dialogId),
//      _owner->handle(), (DLGPROC)DialogProc, (LPARAM)this);
//}
//
//void WinDialog :: clearComboBoxItem(int id)
//{
//   LRESULT counter = ::SendDlgItemMessage(_handle, id, CB_GETCOUNT, 0, 0);
//   while (counter > 0) {
//      ::SendDlgItemMessage(_handle, id, CB_DELETESTRING, 0, 0);
//
//      counter--;
//   }
//}
//
//void WinDialog :: addComboBoxItem(int id, const wchar_t* text)
//{
//   ::SendDlgItemMessage(_handle, id, CB_ADDSTRING, 0, (LPARAM)text);
//}
//
//void WinDialog :: setComboBoxIndex(int id, int index)
//{
//   ::SendDlgItemMessage(_handle, id, CB_SETCURSEL, index, 0);
//}
//
//int WinDialog :: getComboBoxIndex(int id)
//{
//   return (int)::SendDlgItemMessage(_handle, id, CB_GETCURSEL, 0, 0);
//}
//
//void WinDialog :: addListItem(int id, const wchar_t* text)
//{
//   ::SendDlgItemMessage(_handle, id, LB_ADDSTRING, 0, (LPARAM)text);
//}
//
//int WinDialog :: getListSelCount(int id)
//{
//   return (int)::SendDlgItemMessage(_handle, id, LB_GETSELCOUNT, 0, 0);
//}
//
//int WinDialog :: getListIndex(int id)
//{
//   return (int)::SendDlgItemMessage(_handle, id, LB_GETCURSEL, 0, 0);
//}
//
//void WinDialog :: setText(int id, const wchar_t* text)
//{
//   ::SendDlgItemMessage(_handle, id, WM_SETTEXT, 0, (LPARAM)text);
//}
//
//void WinDialog :: setIntText(int id, int value)
//{
//   String<text_c, 15> s;
//   s.appendInt(value);
//
//   ::SendDlgItemMessage(_handle, id, WM_SETTEXT, 0, (LPARAM)(s.str()));
//}
//
//void WinDialog :: setTextLimit(int id, int maxLength)
//{
//   ::SendDlgItemMessage(_handle, id, EM_SETLIMITTEXT, maxLength, 0);
//}
//
//void WinDialog :: getText(int id, wchar_t** text, int length)
//{
//   ::SendDlgItemMessage(_handle, id, WM_GETTEXT, length, (LPARAM)text);
//}
//
//int WinDialog :: getIntText(int id)
//{
//   wchar_t s[13];
//
//   ::SendDlgItemMessage(_handle, id, WM_GETTEXT, 12, (LPARAM)s);
//
//   return StrConvertor::toInt(s, 10);
//}
//
//void WinDialog :: setCheckState(int id, bool value)
//{
//   ::SendDlgItemMessage(_handle, id, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
//}
//
//void WinDialog :: setUndefinedCheckState(int id)
//{
//   ::SendDlgItemMessage(_handle, id, BM_SETCHECK, BST_INDETERMINATE, 0);
//}
//
//bool WinDialog :: getCheckState(int id)
//{
//   return test((int)::SendDlgItemMessage(_handle, id, BM_GETCHECK, 0, 0), BST_CHECKED);
//}
//
//bool WinDialog :: isUndefined(int id)
//{
//   return test((int)::SendDlgItemMessage(_handle, id, BM_GETCHECK, 0, 0), BST_INDETERMINATE);
//}
//
//void WinDialog :: enable(int id, bool enabled)
//{
//   ::EnableWindow(::GetDlgItem(_handle, id), enabled ? TRUE : FALSE);
//}
//
//// --- ProjectSettings ---
//
//ProjectSettings :: ProjectSettings(HINSTANCE instance, WindowBase* owner, ProjectModel* model)
//   : WinDialog(instance, owner, IDD_SETTINGS)
//{
//   _model = model;
//}
//
//void ProjectSettings :: loadTemplateList()
//{
//   int selected = 0;
//   int current = 0;
//   for (auto it = _model->projectTypeList.start(); !it.eof(); ++it) {
//      ustr_t key = *it;
//      if (_model->templateName.compare(key)) {
//         selected = current;
//      }
//
//      WideMessage caption(key);
//      addComboBoxItem(IDC_SETTINGS_TEPMPLATE, *caption);
//      current++;
//   }
//
//   setComboBoxIndex(IDC_SETTINGS_TEPMPLATE, selected);
//}
//
//void ProjectSettings :: loadProfileList()
//{
//   int selected = 0;
//   int current = 0;
//   for (auto it = _model->profileList.start(); !it.eof(); ++it) {
//      ustr_t key = *it;
//      if (_model->profile.compare(key)) {
//         selected = current;
//      }
//
//      WideMessage caption(key);
//      addComboBoxItem(IDC_SETTINGS_PROFILE, *caption);
//      current++;
//   }
//
//   setComboBoxIndex(IDC_SETTINGS_PROFILE, selected);
//}
//
//void ProjectSettings :: onCreate()
//{
//   setTextLimit(IDC_SETTINGS_PACKAGE, IDENTIFIER_LEN);
//
//   WideMessage caption(*_model->package);
//   setText(IDC_SETTINGS_PACKAGE, caption.str());
//
//   WideMessage optionCaption(*_model->options);
//   setText(IDC_SETTINGS_OPTIONS, optionCaption.str());
//
//   WideMessage targetCaption(*_model->target);
//   setText(IDC_SETTINGS_TARGET, targetCaption.str());
//
//   setText(IDC_SETTINGS_OUTPUT, *_model->outputPath);
//
//   setText(IDC_SETTINGS_ARGUMENT, *_model->debugArguments);
//
//   addComboBoxItem(IDC_SETTINGS_DEBUG, _T("Disabled"));
//   addComboBoxItem(IDC_SETTINGS_DEBUG, _T("Enabled"));
//
//   //int mode = _project->getDebugMode();
//   //if (mode != 0) {
//      setComboBoxIndex(IDC_SETTINGS_DEBUG, 1);
//   //}
//   //else setComboBoxIndex(IDC_SETTINGS_DEBUG, 0);
//
//   if (_model->strictType == FLAG_UNDEFINED) {
//      setUndefinedCheckState(IDC_SETTINGS_STRICTTYPE);
//   }
//   else setCheckState(IDC_SETTINGS_STRICTTYPE, _model->strictType == -1);
//
//   loadTemplateList();
//   loadProfileList();
//}
//
//void ProjectSettings :: onOK()
//{
//   wchar_t name[IDENTIFIER_LEN + 1];
//
//   if (getComboBoxIndex(IDC_SETTINGS_TEPMPLATE) != -1) {
//      getText(IDC_SETTINGS_TEPMPLATE, (wchar_t**)(&name), IDENTIFIER_LEN);
//
//      IdentifierString value(name);
//      _model->templateName.copy(*value);
//   }
//
//   if (getComboBoxIndex(IDC_SETTINGS_PROFILE) != -1) {
//      getText(IDC_SETTINGS_PROFILE, (wchar_t**)(&name), IDENTIFIER_LEN);
//
//      IdentifierString value(name);
//      _model->profile.copy(*value);
//   }
//
//   getText(IDC_SETTINGS_PACKAGE, (wchar_t**)(&name), IDENTIFIER_LEN);
//   if (getlength(name) > 0) {
//      IdentifierString value(name);
//      _model->package.copy(*value);
//   }
//   else _model->package.clear();
//
//   getText(IDC_SETTINGS_OPTIONS, (wchar_t**)(&name), IDENTIFIER_LEN);
//   if (getlength(name) > 0) {
//      IdentifierString value(name);
//      _model->options.copy(*value);
//   }
//   else _model->options.clear();
//
//   getText(IDC_SETTINGS_TARGET, (wchar_t**)(&name), IDENTIFIER_LEN);
//   if (getlength(name) > 0) {
//      IdentifierString value(name);
//      _model->target.copy(*value);
//   }
//   else _model->target.clear();
//
//   getText(IDC_SETTINGS_OUTPUT, (wchar_t**)(&name), IDENTIFIER_LEN);
//   _model->outputPath.copy(name);
//
//   getText(IDC_SETTINGS_ARGUMENT, (wchar_t**)(&name), IDENTIFIER_LEN);
//   _model->debugArguments.copy(name);
//
//   if (isUndefined(IDC_SETTINGS_STRICTTYPE)) {
//      _model->strictType = FLAG_UNDEFINED;
//   }
//   else if (getCheckState(IDC_SETTINGS_STRICTTYPE)) {
//      _model->strictType = -1;
//   }
//   else _model->strictType = 0;
//
//   if (!_model->singleSourceProject)
//      _model->notSaved = true;
//}
//
//bool ProjectSettings :: showModal()
//{
//   return show() == IDOK;
//}
//
//// --- EditorSettings ---
//
//EditorSettings :: EditorSettings(HINSTANCE instance, WindowBase* owner, TextViewModelBase* model)
//   : WinDialog(instance, owner, IDD_EDITOR_SETTINGS)
//{
//   _model = model;
//}
//
//void EditorSettings::onCreate()
//{
//   addComboBoxItem(IDC_EDITOR_COLORSCHEME, _T("Default"));
//   addComboBoxItem(IDC_EDITOR_COLORSCHEME, _T("Classic"));
//   addComboBoxItem(IDC_EDITOR_COLORSCHEME, _T("Dark"));
//
//   setComboBoxIndex(IDC_EDITOR_COLORSCHEME, _model->schemeIndex);
//
//   setCheckState(IDC_EDITOR_HIGHLIGHSYNTAXFLAG, _model->highlightSyntax);
//   setCheckState(IDC_EDITOR_LINENUMBERFLAG, _model->lineNumbersVisible);
//
//   // populate font size combo box
//   String<wchar_t, 4> size;
//   for (int i = 8; i < 25; i++) {
//      size.appendInt(i);
//      addComboBoxItem(IDC_EDITOR_FONTSIZE, size.str());
//      size.clear();
//   }
//
//   setComboBoxIndex(IDC_EDITOR_FONTSIZE, _model->fontSize - 8);
//}
//
//void EditorSettings :: onOK()
//{
//   int index = getComboBoxIndex(IDC_EDITOR_COLORSCHEME);
//   if (index != -1)
//      _model->schemeIndex = index;
//
//   int fontSize = getComboBoxIndex(IDC_EDITOR_COLORSCHEME) + 8;
//   if (_model->fontSize != fontSize) {
//      _model->fontSize = fontSize;
//   }
//
//   bool value = getCheckState(IDC_EDITOR_HIGHLIGHSYNTAXFLAG);
//   if (_model->highlightSyntax != value)
//      _model->setHighlightMode(value);
//
//   _model->lineNumbersVisible = getCheckState(IDC_EDITOR_LINENUMBERFLAG);
//}
//
//bool EditorSettings :: showModal()
//{
//   return show() == IDOK;
//}
//
//// --- IDESettings ---
//
//IDESettings :: IDESettings(HINSTANCE instance, WindowBase* owner, IDEModel* model)
//   : WinDialog(instance, owner, IDD_IDE_SETTINGS)
//{
//   _model = model;
//}
//
//void IDESettings :: onCreate()
//{
//   setCheckState(IDC_IDE_REMEMBERPATH, _model->rememberLastPath);
//   setCheckState(IDC_IDE_REMEMBERPROJECT, _model->rememberLastProject);
//   setCheckState(IDC_IDE_PERSIST_CONSOLE, _model->projectModel.withPersistentConsole);
//   setCheckState(IDC_IDE_APPMAXIMIZED, _model->appMaximized);
//   setCheckState(IDC_IDE_AUTORECOMPILE, _model->projectModel.autoRecompile);
//   setCheckState(IDC_IDE_AUTOSAVE, _model->autoSave);
//}
//
//void IDESettings :: onOK()
//{
//   _model->rememberLastPath = getCheckState(IDC_IDE_REMEMBERPATH);
//   _model->rememberLastProject = getCheckState(IDC_IDE_REMEMBERPROJECT);
//   _model->appMaximized = getCheckState(IDC_IDE_APPMAXIMIZED);
//   _model->projectModel.withPersistentConsole = getCheckState(IDC_IDE_PERSIST_CONSOLE);
//   _model->projectModel.autoRecompile = getCheckState(IDC_IDE_AUTORECOMPILE);
//   _model->autoSave = getCheckState(IDC_IDE_AUTOSAVE);
//}
//
//bool IDESettings :: showModal()
//{
//   return show() == IDOK;
//}
//
//// --- DebuggerSettings ---
//
//DebuggerSettings :: DebuggerSettings(HINSTANCE instance, WindowBase* owner, ProjectModel* model)
//   : WinDialog(instance, owner, IDD_DEBUGGER_SETTINGS)
//{
//   _model = model;
//}
//
//void DebuggerSettings :: onCreate()
//{
//   setText(IDC_DEBUGGER_SRCPATH, *_model->paths.librarySourceRoot);
//   setText(IDC_DEBUGGER_LIBPATH, *_model->paths.libraryRoot);
//}
//
//void DebuggerSettings :: onOK()
//{
//   wchar_t value[IDENTIFIER_LEN + 1];
//
//   getText(IDC_DEBUGGER_SRCPATH, (wchar_t**)(&value), IDENTIFIER_LEN);
//   _model->paths.librarySourceRoot.copy(value);
//
//   getText(IDC_DEBUGGER_LIBPATH, (wchar_t**)(&value), IDENTIFIER_LEN);
//   _model->paths.libraryRoot.copy(value);
//}
//
//bool DebuggerSettings::showModal()
//{
//   return show() == IDOK;
//}
//
//// --- FindDialog ---
//
//FindDialog :: FindDialog(HINSTANCE instance, WindowBase* owner, bool replaceMode, FindModel* model)
//   : WinDialog(instance, owner, replaceMode ? IDD_EDITOR_REPLACE : IDD_EDITOR_FIND)
//{
//   _replaceMode = replaceMode;
//   _model = model;
//}
//
//void FindDialog :: copyHistory(int id, SearchHistory* history)
//{
//   clearComboBoxItem(id);
//
//   SearchHistory::Iterator it = history->start();
//   while (!it.eof()) {
//      addComboBoxItem(id, *it);
//
//      ++it;
//   }
//}
//
//bool FindDialog :: showModal()
//{
//   return show() == IDOK;
//}
//
//void FindDialog :: onCreate()
//{
//   setText(IDC_FIND_TEXT, _model->text.str());
//   if (_replaceMode) {
//      setText(IDC_REPLACE_TEXT, _model->newText.str());
//   }
//   setCheckState(IDC_FIND_CASE, _model->matchCase);
//   setCheckState(IDC_FIND_WHOLE, _model->wholeWord);
//
//   copyHistory(IDC_FIND_TEXT, &_model->searchHistory);
//
//   //if (_replaceHistory) {
//   //   copyHistory(IDC_REPLACE_TEXT, _replaceHistory);
//   //}
//}
//
//void FindDialog :: onOK()
//{
//   wchar_t s[200];
//
//   getText(IDC_FIND_TEXT, (wchar_t**)(&s), 255);
//   _model->text.copy(s);
//
//   if (_replaceMode) {
//      getText(IDC_REPLACE_TEXT, (wchar_t**)(&s), 255);
//      _model->newText.copy(s);
//   }
//
//   _model->matchCase = getCheckState(IDC_FIND_CASE);
//   _model->wholeWord = getCheckState(IDC_FIND_WHOLE);
//}
//
//// --- GoToLineDialog ---
//
//GoToLineDialog :: GoToLineDialog(HINSTANCE instance, WindowBase* owner)
//   : WinDialog(instance, owner, IDD_GOTOLINE)
//{
//}
//
//void GoToLineDialog :: onCreate()
//{
//   setIntText(IDC_GOTOLINE_LINENUMBER, _lineNumber);
//}
//
//void GoToLineDialog :: onOK()
//{
//   _lineNumber = getIntText(IDC_GOTOLINE_LINENUMBER);
//}
//
//bool GoToLineDialog :: showModal(int& row)
//{
//   _lineNumber = row;
//
//   if (show() == IDOK) {
//      row = _lineNumber;
//
//      return true;
//   }
//
//   return false;
//}
//
//// --- WindowListDialog ---
//
//WindowListDialog :: WindowListDialog(HINSTANCE instance, WindowBase* owner, TextViewModel* model)
//   : WinDialog(instance, owner, IDD_WINDOWS)
//{
//   _model = model;
//   _selectedIndex = -1;
//}
//
//void WindowListDialog :: onListChange()
//{
//   enable(IDOK, (getListSelCount(IDC_WINDOWS_LIST) == 1));
//}
//
//void WindowListDialog :: doCommand(int id, int command)
//{
//   switch (id) {
//      case IDC_WINDOWS_LIST:
//         if (command == LBN_SELCHANGE) {
//            onListChange();
//         }
//         break;
//      case IDC_WINDOWS_CLOSE:
//         _selectedIndex = getSelectedWindow();
//         ::EndDialog(_handle, -2);
//         break;
//      default:
//         WinDialog::doCommand(id, command);
//         break;
//   }
//}
//
//int WindowListDialog :: getSelectedWindow()
//{
//   return getListIndex(IDC_WINDOWS_LIST);
//}
//
//void WindowListDialog :: onOK()
//{
//   _selectedIndex = getSelectedWindow();
//}
//
//void WindowListDialog :: onCreate()
//{
//   int count = _model->getDocumentCount();
//   for (int i = 1; i <= count; i++) {
//      path_t path = _model->getDocumentPath(i);
//
//      addListItem(IDC_WINDOWS_LIST, path);
//   }
//}
//
//WindowListDialogBase::SelectResult WindowListDialog :: selectWindow()
//{
//   int retVal = show();
//
//   if (retVal == IDOK) {
//      return { _selectedIndex + 1, Mode::Activate };
//   }
//   if (retVal == -2) {
//      return { _selectedIndex + 1, Mode::Close };
//   }
//
//   return { 0, Mode::None };
//}
//
//// --- AboutDialog ---
//
//AboutDialog :: AboutDialog(HINSTANCE instance, WindowBase* owner)
//   : WinDialog(instance, owner, IDD_ABOUT)
//{
//}
//
//void AboutDialog :: onCreate()
//{
//   setText(IDC_ABOUT_LICENCE_TEXT, MIT_LICENSE);
//   setText(IDC_ABOUT_HOME, ELENA_HOMEPAGE);
//}
//
//void AboutDialog :: onOK()
//{
//}
