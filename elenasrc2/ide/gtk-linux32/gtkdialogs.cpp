//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//    GTK: Static dialog implementations
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkdialogs.h"

using namespace _GUI_;

//const TCHAR* FileDialog :: SourceFilter = "ELENA source file\0*.l\0All types\0*.*\0\0";
//const TCHAR* FileDialog :: ProjectFilter = "ELENA Project file\0*.prj\0All types\0*.*\0\0";
//
//// --- FileDialog ---
//
//FileDialog :: FileDialog(Control* owner, const TCHAR* filter, const TCHAR* caption, const TCHAR* initialDir)
//{
//   _filePath.copy(initialDir);
//   _dialog = NULL;
//   _caption = caption;
//   _filter = filter;
//   _owner = owner;
//}
//
//FileDialog :: ~FileDialog()
//{
//   if (_dialog)
//      gtk_widget_destroy(_dialog);
//}
//
//bool FileDialog :: openFiles(_ELENA_::List<TCHAR*>& files)
//{
//   clear();
//
//   _dialog = gtk_file_chooser_dialog_new(_caption,
//     				      GTK_WINDOW(_owner->getHandle()),
//     				      GTK_FILE_CHOOSER_ACTION_OPEN,
//     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
//     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
//     				      NULL);
//
//   gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(_dialog), TRUE);
//
//   if (!_ELENA_::emptystr(_filePath))
//      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(_dialog), _filePath);
//
//   addFilter();
//
//   if (gtk_dialog_run(GTK_DIALOG(_dialog)) == GTK_RESPONSE_ACCEPT) {
//      GSList* file_names = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (_dialog));
//      for (GSList* current=file_names; current; current=current->next) {
//         files.add(_ELENA_::String::clone((char*)current->data));
//
//         g_free(current->data);
//      }
//      g_slist_free(file_names);
//
//      return true;
//   }
//   else return false;
//}
//
//void FileDialog :: addFilter()
//{
//   const TCHAR* current = _filter;
//   while (!_ELENA_::emptystr(current)) {
//      GtkFileFilter *filter = gtk_file_filter_new();
//
//      gtk_file_filter_set_name(filter, current);
//      current += _ELENA_::getlength(current) + 1;
//
//      gtk_file_filter_add_pattern(filter, current);
//      current += _ELENA_::getlength(current) + 1;
//
//      gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(_dialog), filter);
//   }
//}
//
//const TCHAR* FileDialog :: openFile()
//{
//   clear();
//
//   _dialog = gtk_file_chooser_dialog_new(_caption,
//     				      GTK_WINDOW(_owner->getHandle()),
//     				      GTK_FILE_CHOOSER_ACTION_OPEN,
//     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
//     				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
//     				      NULL);
//
//   if (!_ELENA_::emptystr(_filePath))
//      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(_dialog), _filePath);
//
//   addFilter();
//
//   if (gtk_dialog_run(GTK_DIALOG(_dialog)) == GTK_RESPONSE_ACCEPT) {
//      char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (_dialog));
//
//      _filePath.copy(filename);
//
//      g_free(filename);
//
//      return _filePath;
//   }
//   else return NULL;
//}
//
//bool FileDialog :: saveFile(const TCHAR* defaultExt, _ELENA_::Path& path)
//{
//   clear();
//
//   _dialog = gtk_file_chooser_dialog_new(_caption,
//     				      GTK_WINDOW(_owner->getHandle()),
//     				      GTK_FILE_CHOOSER_ACTION_SAVE,
//     				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
//     				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
//     				      NULL);
//
//   addFilter();
//
//   if (!_ELENA_::emptystr(_filePath))
//      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(_dialog), _filePath);
//
//   if (gtk_dialog_run(GTK_DIALOG(_dialog)) == GTK_RESPONSE_ACCEPT) {
//      char* filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (_dialog));
//
//      path.copy(filename);
//
//      g_free(filename);
//
//      return true;
//   }
//   else return false;
//}
//
//// --- MessageBox ---
//
//int MsgBox :: showError(const TCHAR* message, const TCHAR* param)
//{
//   return showError(NULL, message, param);
//}
//
//int MsgBox :: showError(GtkWidget* owner, const TCHAR* message, const TCHAR* param)
//{
//   _ELENA_::String string(message);
//   string.append(param);
//
//   return show(owner, string, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE);
//}
//
//int MsgBox :: showQuestion(GtkWidget* owner, const TCHAR* message)
//{
//   return show(owner, message, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_YES_NO, true);
//}
//
//int MsgBox :: showQuestion(GtkWidget* owner, const TCHAR* message, const TCHAR* param)
//{
//   _ELENA_::String string(message);
//   string.append(param);
//
//   return show(owner, string, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_YES_NO, true);
//}
//
//int MsgBox :: showQuestion(GtkWidget* owner, const TCHAR* message, const TCHAR* param1, const TCHAR* param2)
//{
//   _ELENA_::String string(message);
//   string.append(param1);
//   string.append(param2);
//
//   return show(owner, string, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_YES_NO, true);
//}
//
//int MsgBox :: show(GtkWidget* owner, const TCHAR* message, GtkDialogFlags flags, GtkMessageType type, GtkButtonsType buttons, bool withCancel)
//{
//   GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(owner), flags, type, buttons, message);
//
//   if (withCancel)
//      gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
//
//   int retVal = gtk_dialog_run(GTK_DIALOG(dialog));
//   gtk_widget_destroy(dialog);
//
//   return retVal;
//}
//
//// --- Dialogs ---
//
//Dialog :: Dialog(Control* owner)
//{
//   _owner = owner;
//}

// --- WindowsDialog ---

WindowsDialog :: WindowsDialog(Control* owner)
 //  : Dialog(owner)
{
//   _dialog = gtk_dialog_new();
//   gtk_window_set_transient_for(GTK_WINDOW(_dialog), GTK_WINDOW(owner->getHandle()));
//   gtk_window_set_modal(GTK_WINDOW(_dialog), TRUE);
//
//   gtk_window_set_title(GTK_WINDOW(_dialog), "Window List");
//   gtk_window_set_default_size(GTK_WINDOW(_dialog), 200, 200);
//
//   _store = gtk_list_store_new(1, G_TYPE_STRING);
//
//   _renderer = gtk_cell_renderer_text_new ();
//   _column = gtk_tree_view_column_new_with_attributes("Window", _renderer,
//                                                      "text", 0, NULL);
//
//   _scroll_window = gtk_scrolled_window_new(NULL, NULL);
//   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(_scroll_window),
//                                  GTK_POLICY_AUTOMATIC,
//                                  GTK_POLICY_ALWAYS);
//
//   gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), _scroll_window,
//                        TRUE, TRUE, 0);
//
//   _list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(_store));
//   gtk_container_add(GTK_CONTAINER(_scroll_window), _list_view);
//
//   gtk_tree_view_append_column(GTK_TREE_VIEW (_list_view), _column);
//
//   gtk_tree_view_set_headers_visible(GTK_TREE_VIEW (_list_view), TRUE);
//
//   GtkAdjustment* vscroll = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(_scroll_window));
//   gtk_tree_view_set_vadjustment(GTK_TREE_VIEW (_list_view), vscroll);
//
//   GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (_list_view));
//   gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
//
//   gtk_dialog_add_button(GTK_DIALOG(_dialog), "Activate", -1);
//   gtk_dialog_add_button(GTK_DIALOG(_dialog), "Cancel", 0);
//   gtk_dialog_add_button(GTK_DIALOG(_dialog), "Close Window(s)", -2);
}
/*
int WindowsDialog :: showModal()
{
   onCreate();

   gtk_widget_show_all(_scroll_window);

   int code = gtk_dialog_run(GTK_DIALOG(_dialog));
   if (code == -1) {
      onOK();
   }
   else if (code == -2)
      onClose();

   return code;
}

void WindowsDialog :: addWindow(const TCHAR* docName)
{
   GtkTreeIter list_iterator;

   gtk_list_store_append(GTK_LIST_STORE(_store), &list_iterator);
   gtk_list_store_set(GTK_LIST_STORE(_store), &list_iterator,
                        0, docName, -1);
}

void WindowsDialog :: selectWindow(int index)
{
   GtkTreeIter iter;
   gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(_store), &iter, NULL, index);

   GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (_list_view));
   gtk_tree_selection_select_iter(selection, &iter);
}

int WindowsDialog :: getSelectedWindow()
{
   GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (_list_view));
   int selected = -1;

   GtkTreeIter iter;
   gtk_tree_model_get_iter_first(GTK_TREE_MODEL(_store), &iter);
   int index = 0;
   do {
      if (gtk_tree_selection_iter_is_selected(selection, &iter)) {
         selected = index;
         break;
      }

      index++;
   }
   while (gtk_tree_model_iter_next(GTK_TREE_MODEL(_store), &iter));

   return selected;
}

int* WindowsDialog :: getSelectedWindows(int& count)
{
   GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (_list_view));
   count = gtk_tree_selection_count_selected_rows(selection);
   int* selected = new int[count];

   GtkTreeIter iter;
   gtk_tree_model_get_iter_first(GTK_TREE_MODEL(_store), &iter);
   int index = 0;
   int selected_index = 0;
   do {
      if (gtk_tree_selection_iter_is_selected(selection, &iter)) {
         selected[selected_index++] = index;
      }

      index++;
   }
   while (gtk_tree_model_iter_next(GTK_TREE_MODEL(_store), &iter));

   return selected;
}

WindowsDialog :: ~WindowsDialog()
{
   gtk_widget_destroy(_dialog);
   gtk_widget_destroy(_scroll_window);
   gtk_tree_view_column_clear(_column);
   gtk_list_store_clear(GTK_LIST_STORE(_store));
   gtk_widget_destroy(_list_view);
}

// ---- EditorSettings ---

int EditorSettings :: showModal()
{
   return -2;
}

// --- FindDialog ---

void FindDialog :: copyHistory(GtkListStore* store, SearchHistory* history)
{
   GtkTreeIter list_iterator;

   SearchHistory::Iterator it = history->start();
   while (!it.Eof()) {
      const TCHAR* item = *it;

      gtk_list_store_append(GTK_LIST_STORE(store), &list_iterator);
      gtk_list_store_set(GTK_LIST_STORE(store), &list_iterator,
                        0, item, -1);

      it++;
   }
}

FindDialog :: FindDialog(Control* owner, bool replaceMode, SearchOption* option, SearchHistory* searchHistory, SearchHistory* replaceHistory)
   : Dialog(owner)
{
   _replaceMode = replaceMode;
   _option = option;

   _searchHistory = searchHistory;
   _replaceHistory = replaceHistory;

   _searchStore = gtk_list_store_new(1, G_TYPE_STRING);

   _dialog = gtk_dialog_new();
   gtk_window_set_transient_for(GTK_WINDOW(_dialog), GTK_WINDOW(owner->getHandle()));
   gtk_window_set_modal(GTK_WINDOW(_dialog), TRUE);

   gtk_window_set_default_size(GTK_WINDOW(_dialog), 400, 100);

   if (replaceMode) {
      gtk_window_set_title(GTK_WINDOW(_dialog), "Replace");

      _replaceStore = gtk_list_store_new(1, G_TYPE_STRING);
   }
   else {
      gtk_window_set_title(GTK_WINDOW(_dialog), "Find");
   }

   GtkWidget* hbox = gtk_hbox_new(FALSE, 0);

   GtkWidget* label = gtk_label_new ("Find What:");

   gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
   gtk_label_set_width_chars(GTK_LABEL(label), 14);
   gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);

   _search = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(_searchStore));
   gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(_search), 0); // !! test if it is required
   GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
   gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(_search), renderer, TRUE );
   gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT(_search), renderer, "text", 0, NULL );

   gtk_box_pack_start (GTK_BOX (hbox), _search, TRUE, TRUE, 0);

   gtk_widget_show(hbox);
   gtk_widget_show(label);
   gtk_widget_show(_search);

   gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), hbox, TRUE, FALSE, 0);

   if (replaceMode) {
      GtkWidget* hbox2 = gtk_hbox_new(FALSE, 0);

      GtkWidget* label2 = gtk_label_new ("Replace With:");
      gtk_label_set_width_chars(GTK_LABEL(label2), 14);

      gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);
      gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, TRUE, 0);

      _replace = gtk_combo_box_new_with_model_and_entry(GTK_TREE_MODEL(_replaceStore));

      GtkCellRenderer* renderer2 = gtk_cell_renderer_text_new();
      gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(_replace), renderer2, TRUE );
      gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT(_replace), renderer2, "text", 0, NULL );

      gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(_replace), 0); // !! test if it is required

      gtk_box_pack_start (GTK_BOX (hbox2), _replace, TRUE, TRUE, 0);

      gtk_widget_show(hbox2);
      gtk_widget_show(label2);
      gtk_widget_show(_replace);

      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), hbox2, TRUE, FALSE, 0);
   }

   _findCase = gtk_check_button_new_with_label("Match a case");
   _findWhole = gtk_check_button_new_with_label("Match whole word");

   gtk_widget_show(_findCase);
   gtk_widget_show(_findWhole);

   gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), _findCase, TRUE, FALSE, 0);
   gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), _findWhole, TRUE, FALSE, 0);

   gtk_dialog_add_button(GTK_DIALOG(_dialog), replaceMode ? "Replace" : "Find Next", -1);
   gtk_dialog_add_button(GTK_DIALOG(_dialog), "Close", 0);

   gtk_widget_show(_dialog);
}

void FindDialog :: onCreate()
{
   gtk_entry_set_text (GTK_ENTRY(gtk_bin_get_child(GTK_BIN(_search))), _option->text);
   if (_replaceMode) {
      gtk_entry_set_text (GTK_ENTRY(gtk_bin_get_child(GTK_BIN(_replace))), _option->newText);
   }

   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_findCase), _option->matchCase);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_findWhole), _option->wholeWord);

   if (_searchHistory) {
      copyHistory(_searchStore, _searchHistory);
   }

   if (_replaceHistory) {
      copyHistory(_replaceStore, _replaceHistory);
   }
}

void FindDialog :: onOK()
{
   const TCHAR* s = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(_search))));
   _option->text.copy(s);

   if (_replaceMode) {
      s = gtk_entry_get_text (GTK_ENTRY(gtk_bin_get_child(GTK_BIN(_replace))));
      _option->newText.copy(s);
   }
   _option->matchCase = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(_findCase)) == TRUE;
   _option->wholeWord = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(_findWhole)) == TRUE;
}

int FindDialog :: showModal()
{
   onCreate();

   //gtk_widget_show_all(_scroll_window);

   int code = gtk_dialog_run(GTK_DIALOG(_dialog));
   if (code == -1) {
      onOK();
   }

   return code;
}

FindDialog :: ~FindDialog()
{
   gtk_widget_destroy(_dialog);
   gtk_list_store_clear(GTK_LIST_STORE(_searchStore));
}

// --- AboutDialog ---

int AboutDialog :: showModal()
{
   return -2;
}
*/

// --- ProjectSettingsDialog ---

ProjectSettingsDialog :: ProjectSettingsDialog(_ProjectManager* project)
   : _projectFrame("Project"), _compilerFrame("Compiler"),
     _linkerFrame("Linker"), _debuggerFrame("Debugger"),
     _typeLabel("Type"), _namespaceLabel("Namespace"),
     _warningLabel("Warn about unresolved references"), _optionsLabel("Additional options"),
     _targetLabel("Target file name"), _outputLabel("Output path"),
     _modeLabel("Debug mode"), _argumentsLabel("Command arguments")
{
   _project = project;

   Gtk::Box *box = get_vbox();

   box->pack_start(_projectFrame, Gtk::PACK_SHRINK);

   _projectFrame.add(_projectGrid);
   _projectGrid.set_row_homogeneous(true);
   _projectGrid.set_column_homogeneous(true);
   _projectGrid.attach(_typeLabel, 0, 0, 1, 1);
   _projectGrid.attach(_typeCombobox, 1, 0, 1, 1);
   _projectGrid.attach(_namespaceLabel, 0, 1, 1, 1);
   _projectGrid.attach(_namespaceText, 1, 1, 1, 1);

   box->pack_start(_compilerFrame);

   _compilerFrame.add(_compilerGrid);
   _compilerGrid.set_row_homogeneous(true);
   _compilerGrid.set_column_homogeneous(true);
   _compilerGrid.attach(_warningCheckbox, 0, 0, 1, 1);
   _compilerGrid.attach(_warningLabel, 1, 0, 1, 1);
   _compilerGrid.attach(_optionsLabel, 0, 1, 1, 1);
   _compilerGrid.attach(_optionsText, 1, 1, 1, 1);

   box->pack_start(_linkerFrame);

   _linkerFrame.add(_linkerrGrid);
   _linkerrGrid.set_row_homogeneous(true);
   _linkerrGrid.set_column_homogeneous(true);
   _linkerrGrid.attach(_targetLabel, 0, 0, 1, 1);
   _linkerrGrid.attach(_targetText, 1, 0, 1, 1);
   _linkerrGrid.attach(_outputLabel, 0, 1, 1, 1);
   _linkerrGrid.attach(_outputText, 1, 1, 1, 1);

   box->pack_start(_debuggerFrame);

   _debuggerFrame.add(_debuggerGrid);
   _debuggerGrid.set_row_homogeneous(true);
   _debuggerGrid.set_column_homogeneous(true);
   _debuggerGrid.attach(_modeLabel, 0, 0, 1, 1);
   _debuggerGrid.attach(_modeCombobox, 1, 0, 1, 1);
   _debuggerGrid.attach(_argumentsLabel, 0, 1, 1, 1);
   _debuggerGrid.attach(_argumentsText, 1, 1, 1, 1);

   add_button("OK", Gtk::RESPONSE_OK);
   add_button("Cancel", Gtk::RESPONSE_CANCEL);

   populate();

   show_all_children();
}

void ProjectSettingsDialog :: setText(Gtk::Entry& control, const char* value)
{
   if (!_ELENA_::emptystr(value)) {
      control.set_text(value);
   }
}

void ProjectSettingsDialog :: loadTemplateList()
{
   _ELENA_::Path configPath("/etc/elena/elc.config");

   _ELENA_::IniConfigFile config;
   if (!config.load(configPath.c_str(), _ELENA_::feUTF8))
      return;

   const char* curTemplate = _project->getTemplate();

   int current = 0;
   for (_ELENA_::ConfigCategoryIterator it = config.getCategoryIt("templates") ; !it.Eof() ; it++, current++) {
      _typeCombobox.append(it.key().c_str());

      if (it.key().compare(curTemplate))
         _typeCombobox.set_active(current);
   }
}

void ProjectSettingsDialog :: populate()
{
   setText(_namespaceText, _project->getPackage());
   setText(_targetText, _project->getTarget());
   setText(_outputText, _project->getOutputPath());
   setText(_argumentsText, _project->getArguments());
   setText(_optionsText, _project->getOptions());

   _modeCombobox.append("Disabled");
   _modeCombobox.append("Enabled");

   int mode = _project->getDebugMode();
   if (mode != 0) {
      _modeCombobox.set_active(1);
   }
   else _modeCombobox.set_active(1);

   _warningCheckbox.set_active(_project->getBoolSetting("warn:unresolved"));

   loadTemplateList();
}

void ProjectSettingsDialog :: save()
{
   Glib::ustring path = _targetText.get_text();
   if (!path.empty()) {
      _project->setTarget(path.c_str());
   }
   else _project->setTarget(NULL);

   path = _argumentsText.get_text();
   _project->setArguments(path.c_str());

   path = _outputText.get_text();
   _project->setOutputPath(path.c_str());

   path = _optionsText.get_text();
   _project->setOptions(path.c_str());

   Glib::ustring name = _namespaceText.get_text();
   _project->setPackage(name.c_str());

   if (_typeCombobox.get_active_id() != -1) {
      name = _typeCombobox.get_active_text();
      _project->setTemplate(name.c_str());
   }

   int index = _modeCombobox.get_active_row_number();
   if (index == 1) {
      _project->setDebugMode(-1);
   }
   else _project->setDebugMode(0);

   _project->setBoolSetting("warn:unresolved", _warningCheckbox.get_active());
}
