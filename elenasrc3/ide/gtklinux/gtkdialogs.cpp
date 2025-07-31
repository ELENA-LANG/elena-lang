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
   _listCallback = nullptr;
}

bool FileDialog :: openFile(PathString& path)
{
//   Gtk::FileChooserDialog dialog(_caption, Gtk::FileChooser::Action::OPEN);
//   dialog.set_transient_for(*_owner);
//
//   if (!emptystr(_initialDir))
//      dialog.set_current_folder (_initialDir);
//
//   dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
//   dialog.add_button("_Open", Gtk::RESPONSE_OK);
//
//   for (int i = 0; i < _filterCounter; i += 2) {
//      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
//
//      filter_l->set_name(_filter[i + 1]);
//      filter_l->add_pattern(_filter[i]);
//      dialog.add_filter(filter_l);
//   }
//
//   int result = dialog.run();
//   if (result == Gtk::RESPONSE_OK) {
//      std::string filename = dialog.get_filename();
//
//      path.copy(filename.c_str());
//
//      return true;
//   }
   return false;
}

void FileDialog :: on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result,
  const Glib::RefPtr<Gtk::FileDialog>& dialog)
{
    PathList files(nullptr);

    auto file = dialog->open_finish(result);
    files.add(StrUtil::clone(file->get_path().c_str()));

   _listCallback(_callbackArg, &files);

   _listCallback = nullptr;
   _callbackArg = nullptr;
}

bool FileDialog :: openFiles(void* arg, FileDialogCallback callback)
{
   assert(_listCallback == nullptr);

   _listCallback = callback;
   _callbackArg = arg;

   auto dialog = Gtk::FileDialog::create();

   auto filters = Gio::ListStore<Gtk::FileFilter>::create();
   for (int i = 0; i < _filterCounter; i += 2) {
      auto filter = Gtk::FileFilter::create();
      filter->set_name(_filter[i + 1]);
      filter->add_pattern(_filter[i]);

      filters->append(filter);
   }

   dialog->set_filters(filters);

//   dialog.set_transient_for(*_owner);
//
   if (!emptystr(_initialDir)) {
      auto file = Gio::File::create_for_path(_initialDir);
      dialog->set_initial_folder(file);
   }

   dialog->open(sigc::bind(sigc::mem_fun(
      *this, &FileDialog::on_file_dialog_finish), dialog));
//
//   dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
//   dialog.add_button("_Open", Gtk::RESPONSE_OK);
//
//   for (int i = 0; i < _filterCounter; i += 2) {
//      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
//
//      filter_l->set_name(_filter[i + 1]);
//      filter_l->add_pattern(_filter[i]);
//      dialog.add_filter(filter_l);
//   }
//
//   int result = dialog.run();
//   if (result == Gtk::RESPONSE_OK) {
//      std::string filename = dialog.get_filename();
//
//      files.add(StrUtil::clone(filename.c_str()));
//
//      return true;
//   }
   return false;
}

bool FileDialog :: saveFile(path_t ext, PathString& path)
{
//   Gtk::FileChooserDialog dialog(_caption, Gtk::FILE_CHOOSER_ACTION_SAVE);
//   dialog.set_transient_for(*_owner);
//
//   if (!emptystr(_initialDir))
//      dialog.set_current_folder (_initialDir);
//
//   dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
//   dialog.add_button("_Save", Gtk::RESPONSE_OK);
//
//   for (int i = 0; i < _filterCounter; i += 2) {
//      Glib::RefPtr<Gtk::FileFilter> filter_l = Gtk::FileFilter::create();
//
//      filter_l->set_name(_filter[i + 1]);
//      filter_l->add_pattern(_filter[i]);
//      dialog.add_filter(filter_l);
//   }
//
//   int result = dialog.run();
//   if (result == Gtk::RESPONSE_OK) {
//      std::string filename = dialog.get_filename();
//
//      path.copy(filename.c_str());
//
//      return true;
//   }
   return false;
}

// --- MessageDialog ---

int MessageDialog :: show(const char* message, Gtk::MessageType messageType, Gtk::ButtonsType buttonTypes, bool withCancel)
{
//   Gtk::MessageDialog dialog(message, false, messageType, buttonTypes, true);
//   dialog.set_transient_for(*_owner);
//
////   if (withCancel)
////      dialog.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
//
//   return dialog.run();


    return 0; // !! temporal
}

MessageDialogBase::Answer MessageDialog :: question(text_str message, text_str param)
{
   IdentifierString questionStr(message, param);

   return question(*questionStr);
}

MessageDialogBase::Answer MessageDialog :: question(text_str message)
{
   int retVal = show(message, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO, true);

/*   if (retVal == Gtk::ResponseType::RESPONSE_YES) {
      return Answer::Yes;
   }
   else if (retVal == Gtk::ResponseType::RESPONSE_CANCEL) {
      return Answer::Cancel;
   }
   else*/ return Answer::No;
}

void MessageDialog :: info(text_str message)
{
//   show(message, Gtk::MessageType::MESSAGE_INFO, Gtk::ButtonsType::BUTTONS_OK, false);
}

// --- ProjectSettings ---

ProjectSettings :: ProjectSettings(ProjectModel* model)
   : _projectFrame("Project"), _typeLabel("Type"), _namespaceLabel("Namespace"), _profileLabel("Profile"),
     _compilerFrame("Compiler"), _strictTypeLabel("Strict type enforcing"), _optionsLabel("Additional options"), _warningLabel("Warning level"),
     _linkerFrame("Linker"), _targetLabel("Target file name"), _outputLabel("Output path"),
     _debuggerFrame("Debugger"), _modeLabel("Debug mode"), _argumentsLabel("Command arguments")
{
//   _model = model;
//
//   Gtk::Box *box = get_vbox();
//
//   box->pack_start(_projectFrame, Gtk::PACK_SHRINK);
//
//   _projectFrame.add(_projectGrid);
//   _projectGrid.set_row_homogeneous(true);
//   _projectGrid.set_column_homogeneous(true);
//   _projectGrid.attach(_typeLabel, 0, 0, 1, 1);
//   _projectGrid.attach(_typeCombobox, 1, 0, 1, 1);
//   _projectGrid.attach(_namespaceLabel, 0, 1, 1, 1);
//   _projectGrid.attach(_namespaceText, 1, 1, 1, 1);
//   _projectGrid.attach(_profileLabel, 0, 2, 1, 1);
//   _projectGrid.attach(_profileCombobox, 1, 2, 1, 1);
//
//   box->pack_start(_compilerFrame);
//
//   _compilerFrame.add(_compilerGrid);
//   _compilerGrid.set_row_homogeneous(true);
//   _compilerGrid.set_column_homogeneous(true);
//   _compilerGrid.attach(_strictTypeLabel, 0, 0, 1, 1);
//   _compilerGrid.attach(_strictTypeCheckbox, 1, 0, 1, 1);
//   _compilerGrid.attach(_optionsLabel, 0, 1, 1, 1);
//   _compilerGrid.attach(_optionsText, 1, 1, 1, 1);
//   _compilerGrid.attach(_warningLabel, 0, 2, 1, 1);
//   _compilerGrid.attach(_warningCombobox, 1, 2, 1, 1);
//
//   box->pack_start(_linkerFrame);
//
//   _linkerFrame.add(_linkerrGrid);
//   _linkerrGrid.set_row_homogeneous(true);
//   _linkerrGrid.set_column_homogeneous(true);
//   _linkerrGrid.attach(_targetLabel, 0, 0, 1, 1);
//   _linkerrGrid.attach(_targetText, 1, 0, 1, 1);
//   _linkerrGrid.attach(_outputLabel, 0, 1, 1, 1);
//   _linkerrGrid.attach(_outputText, 1, 1, 1, 1);
//
//   box->pack_start(_debuggerFrame);
//
//   _debuggerFrame.add(_debuggerGrid);
//   _debuggerGrid.set_row_homogeneous(true);
//   _debuggerGrid.attach(_modeLabel, 0, 0, 1, 1);
//   _debuggerGrid.attach(_modeCombobox, 1, 0, 1, 1);
//   _debuggerGrid.attach(_argumentsLabel, 0, 1, 1, 1);
//   _debuggerGrid.attach(_argumentsText, 1, 1, 1, 1);
//
//   add_button("OK", Gtk::RESPONSE_OK);
//   add_button("Cancel", Gtk::RESPONSE_CANCEL);
//
//   _modeCombobox.append("Disabled");
//   _modeCombobox.append("Enabled");
//
//   _warningCombobox.append(" None");
//   _warningCombobox.append(" Level 1");
//   _warningCombobox.append(" Level 2");
//   _warningCombobox.append(" Level 3");
//
//   show_all_children();
}

void ProjectSettings :: loadTemplateList()
{
   _typeCombobox.remove_all();

   int selected = 0;
   int current = 0;
   for (auto it = _model->projectTypeList.start(); !it.eof(); ++it) {
      ustr_t key = *it;
      if (_model->templateName.compare(key)) {
         selected = current;
      }

      _typeCombobox.append(key.str());
      current++;
   }

   _typeCombobox.set_active(selected);
}

void ProjectSettings :: loadProfileList()
{
   _profileCombobox.remove_all();

   int selected = 0;
   int current = 0;
   for (auto it = _model->profileList.start(); !it.eof(); ++it) {
      ustr_t key = *it;
      if (_model->profile.compare(key)) {
         selected = current;
      }

      _profileCombobox.append(key.str());
      current++;
   }

   _profileCombobox.set_active(selected);
}

inline void setText(Gtk::Entry& control, const char* value)
{
   if (!emptystr(value)) {
      control.set_text(value);
   }
   else control.set_text("");
}

void ProjectSettings :: populate()
{
   setText(_namespaceText, _model->package.str());

   setText(_optionsText, _model->options.str());

   setText(_targetText, _model->target.str());

   setText(_outputText, _model->outputPath.str());

   setText(_argumentsText, _model->debugArguments.str());

   //int mode = _project->getDebugMode();
   //if (mode != 0) {
      _modeCombobox.set_active(1);
   //}
   //else _modeCombobox.set_active(0);

   _strictTypeCheckbox.set_active();

   if (_model->strictType == FLAG_UNDEFINED) {
      _strictTypeCheckbox.set_inconsistent(true);
   }
   else {
      _strictTypeCheckbox.set_inconsistent(false);
      _strictTypeCheckbox.set_active(_model->strictType == -1);
   }

   _warningCombobox.set_active(_model->warningLevel);

   loadTemplateList();
   loadProfileList();
}

void ProjectSettings :: save()
{
   Glib::ustring name = _namespaceText.get_text();
   if (!name.empty()) {
      _model->package.copy(name.c_str());
   }
   else _model->package.clear();

   if (_typeCombobox.get_active_row_number() != -1) {
      name = _typeCombobox.get_active_text();
      _model->templateName.copy(name.c_str());
   }

   if (_profileCombobox.get_active_row_number() != -1) {
      name = _profileCombobox.get_active_text();
      _model->profile.copy(name.c_str());
   }

   Glib::ustring path = _targetText.get_text();
   if (!path.empty()) {
      _model->target.copy(path.c_str());
   }
   else _model->target.clear();

   path = _outputText.get_text();
   _model->outputPath.copy(path.c_str());

   path = _argumentsText.get_text();
   _model->debugArguments.copy(path.c_str());

   path = _optionsText.get_text();
   if (!path.empty()) {
      _model->options.copy(path.c_str());
   }
   else _model->options.clear();

   if (_strictTypeCheckbox.get_inconsistent()) {
      _model->strictType = FLAG_UNDEFINED;
   }
   else if (_strictTypeCheckbox.get_active()) {
      _model->strictType = -1;
   }
   else _model->strictType = 0;

   _model->warningLevel = _warningCombobox.get_active_row_number();

   if (!_model->singleSourceProject)
      _model->notSaved = true;
}

bool ProjectSettings :: showModal()
{
//   populate();

   bool retVal = /*run() == Gtk::RESPONSE_OK*/false;
   if (retVal)
      save();

   close();

   return retVal;
}

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
