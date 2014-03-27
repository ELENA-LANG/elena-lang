//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      IDE main window class implementation
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "appwindow.h"
#include "text.h"
#include "module.h"

using namespace _GUI_;

#define EMPTY_STRING      _T("")

#define OPENING_BRACKET   _T("({[")
#define CLOSING_BRACKET   _T(")}]")

// --- Lexical DFA Table ---

const tchar_t lexStart        = 'a';
const tchar_t lexCommentStart = 'b';
const tchar_t lexKeyword      = 'c';
const tchar_t lexOperator     = 'd';
const tchar_t lexBrackets     = 'e';
const tchar_t lexObject       = 'f';
const tchar_t lexCloseBracket = 'g';
const tchar_t lexStick        = 'h';
const tchar_t lexDigit        = 'i';
const tchar_t lexHint         = 'j';
const tchar_t lexMessage      = 'k';
const tchar_t lexLookahead    = 'l';
const tchar_t lexLineComment  = 'm';
const tchar_t lexComment      = 'n';
const tchar_t lexComment2     = 'o';
const tchar_t lexQuote        = 'p';
const tchar_t lexQuote2       = 'q';
const tchar_t lexHint2        = 'r';

const tchar_t* lexDFA[] =
{
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfjgdddddliiiiiiiiiiddddddacccccccccccccccccccccccccceaedcaccccccccccccccccccccccccccehedc"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadfegdddddlffffffffffddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadfegdddddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadaegdddddliiiiiiiiiiddddddakkkkkkkkkkkkkkkkkkkkkkkkkkeaedkakkkkkkkkkkkkkkkkkkkkkkkkkkehedk"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpcaadfegdddddliiiiiiiiiiddddddaiiiiiikkkkkkkkkkkkkkkkkkkkeaedkaiiiiiikikkkikkkkkikkkkkkkkehedk"),
     _T("ajjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjrjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpcaadkegdddddlkkkkkkkkkkddddddakkkkkkkkkkkkkkkkkkkkkkkkkkeaedkakkkkkkkkkkkkkkkkkkkkkkkkkkehedk"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaddpcaadfagnddddmaaaaaaaaaaddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
     _T("ammmmmmmmmammammmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm"),
     _T("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnonnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"),
     _T("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnbnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn"),
     _T("ppppppppppppppppppppppppppppppppppqppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"),
     _T("aaaaaaaaaakaakaaaaaaaaaaaaaaaaaakdpaaadaegdddddliiiiiiiiiiddddddakkkkkkkkkkkkkkkkkkkkkkkkkkeaedkakkkkkkkkkkkkkkkkkkkkkkkkkkehedk"),
     _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaadpcaadfjgddjddliiiiiiiiiiddddddaffffffffffffffffffffffffffeaedfaffffffffffffffffffffffffffehedf"),
};

tchar_t makeStep(tchar_t ch, tchar_t state)
{
   return (tchar_t)ch < 128 ? lexDFA[state - lexStart][ch] : lexDFA[state - lexStart][127];
}

size_t defineStyle(tchar_t state, size_t style)
{
   switch (state) {
      case lexStart:
      case lexObject:
         return STYLE_DEFAULT;
      case lexKeyword:
         return STYLE_KEYWORD;
      case lexMessage:
         return STYLE_MESSAGE;
      case lexOperator:
      case lexBrackets:
      case lexStick:
      case lexCloseBracket:
      case lexLookahead:
         return STYLE_OPERATOR;
      case lexLineComment:
      case lexComment:
      case lexComment2:
      case lexCommentStart:
      case lexHint:                  // !! temporal, do hint needs its own style
      case lexHint2:
         return STYLE_COMMENT;
      case lexDigit:
         return STYLE_NUMBER;
      case lexQuote:
      case lexQuote2:
         return STYLE_STRING;
      default:
         return style;
   }
}

// --- IDELexicalStyler ---

inline size_t evaluateLength(_GUI_::TextBookmark& bookmark, size_t column)
{
   _GUI_::TextBookmark bm = bookmark;

   bm.moveTo(column, bm.getRow());

   return bm.getPosition() - bookmark.getPosition();
}

bool IDE::IDELexicalStyler :: checkMarker(MarkerInfo& marker, LexicalInfo& li, size_t& styleLen)
{
   if (marker.info.row == li.row) {
      int col = marker.info.col;

      Point caret = li.bm.getCaret();
      if (caret.x < col) {
         styleLen =  evaluateLength(li.bm, col);
         li.style = marker.bandStyle;
      }
      else if (caret.x >= col && caret.x < col + marker.info.length) {
         styleLen = marker.info.length - evaluateLength(li.bm, col);
         li.style = marker.style;
      }
      else li.style = marker.bandStyle;

      li.bandStyle = true;

      return true;
   }
   return false;
}

size_t IDE::IDELexicalStyler :: proceed(size_t position, LexicalInfo& li)
{
   size_t styleLen = 0xFF; // !! temporal

   if (_current.bandStyle != -1 && checkMarker(_current, li, styleLen)) {
      // set marker attribute if the breakpoint is set on the same line
      if (li.newLine) {
         #ifdef _WIN32
         Breakpoints::Iterator it = _breakpoints->start();
         Document* doc = ((Document::Reader*)&li)->_doc;
         while (!it.Eof()) {
            if ((*it).row == li.row && (*it).param == doc) {
               li.marker = true;

               break;
            }
            it++;
         }
         #endif
      }
      return styleLen;
   }
   else {
      if (li.newLine) {
         #ifdef _WIN32
         Breakpoints::Iterator it = _breakpoints->start();
         Document* doc = ((Document::Reader*)&li)->_doc;
         while (!it.Eof()) {
            if ((*it).row == li.row && (*it).param == doc) {
               li.marker = true;
               li.bandStyle = true;
               li.style = STYLE_BREAKPOINT;

               return styleLen;
            }
            it++;
         }
         #endif
      }

      // highlight bracket
      if (_bracketHighlighting) {
         size_t position = li.bm.getPosition();
         if (position == _openBracketPos || position == _closeBracketPos) {
            li.style = STYLE_HIGHLIGHTED_BRACKET;

            return 1;
         }
         styleLen = LexicalStyler::proceed(position, li);
         // to allow bracket highlighting foregoing by another operator
         if(li.style == STYLE_OPERATOR) {
            if (_ELENA_::isbetween(position, styleLen, _openBracketPos)) {
               styleLen = _openBracketPos - position;
            }
            else if (_ELENA_::isbetween(position, styleLen, _closeBracketPos))
               styleLen = _closeBracketPos - position;
         }
         return styleLen;
      }
      else return LexicalStyler::proceed(position, li);
   }
}

// --- IDEWindowsDialog ---

IDE::IDEWindowsDialog :: IDEWindowsDialog(IDE* ide)
   : WindowsDialog(ide->_appWindow)
{
   _ide = ide;
}

void IDE::IDEWindowsDialog :: onCreate()
{
   //!! temporal
   #ifdef _WIN32
   for (int i = 0 ; i < _ide->_mainFrame->getDocumentCount() ; i++) {
      addWindow(_ide->_mainFrame->getDocumentPath(i));
   }
   selectWindow(_ide->_mainFrame->getCurrentIndex());
   #endif
}

void IDE::IDEWindowsDialog :: onOK()
{
   //!! temporal
   #ifdef _WIN32
   _ide->_mainFrame->selectTab(getSelectedWindow());
   #endif
}

void IDE::IDEWindowsDialog :: onClose()
{
   //!! temporal
   #ifdef _WIN32
   int count = 0;
   int* selected = getSelectedWindows(count);

   int offset = 0;
   for (int i = 0 ; i < count; i++) {
      if(!_ide->closeFile(selected[i] - offset))
         break;

      offset++;
   }
   free(selected);
   #endif
}

// --- AppDebugController ---

#ifdef _WIN32
_ELENA_::_Module* AppDebugController :: loadDebugModule(const wchar16_t* reference)
{
   _ELENA_::NamespaceName name(reference);
   _ELENA_::Path          path;

   Project::retrievePath(name, path, _T("dnl"));

   _ELENA_::Module* module = (_ELENA_::Module*)_modules.get(name);
   if (module == NULL) {
      module = new _ELENA_::Module();

      _ELENA_::FileReader reader(path, _ELENA_::feRaw, false);
      _ELENA_::LoadResult result = module->load(reader);
      if (result != _ELENA_::lrSuccessful) {
         delete module;

         return NULL;
      }
      _modules.add(name, module);
   }
   return module;
}
#endif

// --- IDE ---

IDE :: IDE(AppDebugController* debugController)
   : _recentFiles(10, IDM_FILE_FILES),
   _recentProjects(10, IDM_FILE_PROJECTS),
   _windowList(10, IDM_WINDOW_WINDOWS)
{
   _unnamedIndex = 0;
   _state = uiEmpty;
   _previousState = 0xFFFFFFFF;
#ifdef _WIN32
   _debugController = debugController;
#endif

   _lastCaret.x = -1;
   _lastCaret.y = -1;
}

void IDE :: onIDEInit()
{
   #ifdef _WIN32
   _appMenu->checkItemById(IDM_VIEW_OUTPUT, Settings::compilerOutput);

   if (!Settings::compilerOutput) {
      _outputBar->hide();
   }
   // !! temporal
   else if(_outputBar) _outputBar->show();

   _statusBar->setText(0, EDITOR_READY);

   _highlightBrackets = Settings::highlightBrackets;

   // check debugger settings
   if (Settings::debugTape)
      _debugController->allowTapeDebug();

   doShowDebugWatch(false);
   #endif
}

void IDE :: start()
{
   Text::TabSize = Settings::tabSize;
   _mainFrame->reloadSettings();

   // open default project
   if (!Settings::defaultProject.isEmpty())
      openProject(Settings::defaultProject);

   // open default files
   _ELENA_::List<tchar_t*>::Iterator it = Settings::defaultFiles.start();
   while (!it.Eof()) {
      openFile(*it);

      it++;
   }

   onChange();
}

void IDE :: exit()
{
   if(closeAll())
      _appWindow->exit();
}

bool IDE :: openFile(const tchar_t* path)
{
   //!!temporal
   #ifdef _WIN32
   int index = _mainFrame->getDocumentIndex(path);
   if (index >= 0) {
      _mainFrame->selectDocument(path);

      return true;
   }

   _GUI_::Text* text = new _GUI_::Text();
   bool opened = text->load(path, Settings::defaultEncoding, Settings::autoDetecting);
   if (opened) {
      _mainFrame->openDocument(path, text,
         new IDELexicalStyler(this, text, STYLE_DEFAULT, lexLookahead, lexStart, makeStep, defineStyle), Settings::defaultEncoding);

	   // check if the file belongs to the project
	   if (Project::isIncluded(path)) {
         _mainFrame->markDocumentAsIncluded();
	   }

      _windowList.add(path);

      onFileOpen();
      return true;
   }
   else return false;
   #else
   return false;
   #endif
}

bool IDE :: openProject(const tchar_t* path)
{
   if (!closeAll())
      return false;

//!!temporal
#ifdef _WIN32
   if (!Project::open(path)) {
      MsgBox::showError(_appWindow->getHandle(), IDE_MSG_INVALID_PROJECT, path);
      return false;
   }
#endif

   if (Settings::lastProjectRemember)
      Settings::defaultProject.copy(path);

   if (Settings::lastPathRemember)
      Paths::lastPath.copyPath(path);

   _ELENA_::Path sourcePath;
   _ELENA_::ConfigCategoryIterator it = Project::SourceFiles();
   while (!it.Eof()) {
      sourcePath.copy(it.key());
      Paths::resolveRelativePath(sourcePath, Project::getPath());

//!!temporal
#ifdef _WIN32
      if (!openFile(sourcePath))
         MsgBox::showError(_appWindow->getHandle(), ERROR_CANNOT_OPEN_FILE, _ELENA_::ConstantIdentifier(it.key()));
#endif

      it++;
   }
   onProjectOpen();

   return true;
}

bool IDE :: closeFile(int index)
{
//!!temporal
#ifdef _WIN32
   const tchar_t* path = _mainFrame->getDocumentPath(index);

   if (_mainFrame->isDocumentModified()) {
      int result = MsgBox::showQuestion(_appWindow->getHandle(), QUESTION_SAVE_FILECHANGES, path);
      if (MsgBox::isCancel(result)) {
         return false;
      }
      else if (MsgBox::isYes(result)) {
         if (!doSave(false))
            return false;
      }
   }
   _windowList.remove(path);

   _mainFrame->closeDocument(index);

   onFileClose();
#endif
   return true;
}

bool IDE :: closeAll(bool closeProject)
{
//!!temporal
#ifdef _WIN32
   if (_ELENA_::test(_state, uiIDEBusy)) {
      int result = MsgBox::showQuestion(_appWindow->getHandle(), ERROR_CANNOT_CLOSE_COMPILING);
      if (MsgBox::isNo(result)) {
         return false;
      }
   }

   while (_mainFrame->getDocumentCount() > 0) {
      if (!doCloseFile())
         return false;
   }
#endif

   return closeProject ? doCloseProject() : true;
}

void IDE :: setCaption(const tchar_t* projectName)
{
   _ELENA_::String<tchar_t, 0x100> title(APP_NAME);
   if (!_ELENA_::emptystr(projectName)) {
      title.append(_T(" - ["));
      title.append(projectName);
      title.append(_T("]"));
   }

//!!temporal
#ifdef _WIN32
   _appWindow->setCaption(title);
#endif
}

void IDE :: renameFileAs(int index, const tchar_t* newPath, const tchar_t* oldPath, bool included)
{
//!!temporal
#ifdef _WIN32
   _mainFrame->renameDocument(index, newPath);

   if (included) {
      Project::excludeSource(oldPath);
      Project::includeSource(newPath);
   }
   _mainFrame->setFocus();
#endif
}

bool IDE :: startDebugger(bool stepMode)
{
//!!temporal
#ifdef _WIN32
   const char* target = Project::getTarget();
   const char* arguments = Project::getArguments();

   if (!_ELENA_::emptystr(target)) {
      _ELENA_::Path exePath(Project::getPath(), target);

      // provide the whole command line including the executable path and name
      _ELENA_::Path commandLine(exePath);
      commandLine.append(_T(" "));
      commandLine.append(arguments);

      _ELENA_::DebugMode mode = (_ELENA_::DebugMode)Project::getDebugMode();
      if (mode != _ELENA_::dbmNone) {
         if (!_debugController->start(exePath, commandLine, mode, _breakpoints)) {
            MsgBox::showError(_appWindow->getHandle(), ERROR_DEBUG_FILE_NOT_FOUND_COMPILE, NULL);

            return false;
         }
         else return true;
      }
      else if(stepMode) {
         MsgBox::showError(_appWindow->getHandle(), ERROR_DEBUG_FILE_NOT_FOUND_SETTING, NULL);

         return false;
      }
      else {
         if (!_debugController->start(exePath, commandLine, _ELENA_::dbmNone, _breakpoints)) {
            MsgBox::showError(_appWindow->getHandle(), ERROR_RUN_NEED_RECOMPILE, NULL);

            return false;
         }
         else return true;
      }
   }
   else {
      MsgBox::showError(_appWindow->getHandle(), ERROR_RUN_NEED_TARGET, NULL);

      return false;
   }
#else
   return false;
#endif
}

bool IDE :: loadModule(const tchar_t* ns, const tchar_t* source)
{
//!!temporal
#ifdef _WIN32
   if (!_debugController->isStarted())
      return false;

   if (_ELENA_::ConstantIdentifier::compare(ns, Project::getPackage())) {
      _ELENA_::Path path(Project::getPath(), source);

      openFile(path);
   }
   else {
      _ELENA_::Path path((const tchar_t*)Paths::packageRoot);
      path.combine(ns, _ELENA_::StringHelper::find(ns, '\'', _ELENA_::getlength(ns)));
      path.combine(source);

      openFile(path);
   }
#endif
   return true;
}

bool IDE :: loadTemporalModule(const tchar_t* name, int param)
{
//!!temporal
#ifdef _WIN32
   if (!_debugController->isStarted())
      return false;

   int index = _mainFrame->getDocumentIndex(name);

   // if the temporal module is not yet opened - create it
   if (index == -1) {
      doCreateTempFile(name);

      index = _mainFrame->getDocumentIndex(name);
   }
   Document* doc = _mainFrame->getDocument(index);

   // clear all
   doc->moveFirst(false);
   doc->moveLast(true);
   doc->eraseChar(false);

   // insert temporal content
   const tchar_t* source = _debugController->getTemporalSource(param);
   doc->insertLine(source, _ELENA_::getlength(source));

   doc->status.modifiedMode = false;
#endif
   return true;
}

void IDE :: runToCursor()
{
//!!temporal
#ifdef _WIN32
   int index = _mainFrame->getCurrentIndex();
   Document* doc = _mainFrame->getDocument(index);
   if (doc) {
      _ELENA_::Path path(_mainFrame->getDocumentPath(index));
      Point caret = doc->getCaret();

      _ELENA_::ReferenceNs module;
      Project::retrieveName(path, module);

      _debugController->runToCursor(module, path, 0, caret.y);
   }
#endif
}

void IDE :: refreshDebugStatus()
{
#ifdef _WIN32
   if (Settings::testMode)  {
      _ELENA_::String<tchar_t, 15> address(_T("@"));
      address.appendHex(_debugController->getEIP());
      _statusBar->setText(4, address);
   }
#endif
}

bool IDE :: isOutaged(bool noWarning)
{
#ifdef _WIN32
   if (_mainFrame->isAnyModified()) {
      if (!noWarning)
         MsgBox::showError(_appWindow->getHandle(), ERROR_RUN_OUT_OF_DATE, NULL);

      return false;
   }

   _ELENA_::ConstantIdentifier src("l");
   _ELENA_::ConstantIdentifier ext("nl");
   _ELENA_::Path rootPath(Project::getPath(), Project::getOutputPath());
   for (_ELENA_::ConfigCategoryIterator it = Project::SourceFiles() ; !it.Eof() ; it++) {
      _ELENA_::Path source(rootPath, it.key());

      _ELENA_::Path module;
      module.copyPath(it.key());

      _ELENA_::ReferenceNs name(Project::getPackage());
      name.pathToName(module);          // get a full name

      module.copy(rootPath);
      module.nameToPath(name, ext);

      DateTime sourceDT = DateTime::getFileTime(source);
      DateTime moduleDT = DateTime::getFileTime(module);

      if (sourceDT > moduleDT) {
         if (!noWarning)
            MsgBox::showError(_appWindow->getHandle(), ERROR_RUN_OUT_OF_DATE, NULL);

         return false;
      }
   }

   return true;
#else
   return false; // !! temporal
#endif
}

void IDE :: highlightMessage(MessageBookmark* bookmark)
{
   //!!temporal
#ifdef _WIN32
   if (bookmark) {
      _ELENA_::Path docPath(Project::getPath());
      docPath.combine(bookmark->file);

      //_ELENA_::ReferenceNs name;
      //Project::retrieveName(bookmark->file, name);

      HighlightInfo hi(bookmark->col - 1, bookmark->row - 1, 0, 0);

      _mainFrame->removeAllDocumentMarker(STYLE_ERROR_LINE);

      int index = _mainFrame->getDocumentIndex(docPath);
      if (index != -1) {
         _mainFrame->setFocus();
         _mainFrame->selectDocument(index);

         _mainFrame->addDocumentMarker(index, hi, STYLE_ERROR_LINE, STYLE_ERROR_LINE);
         _mainFrame->refreshDocument();

         _state |= uiHighlight;
      }
   }
   #endif
}

bool IDE :: toggleBreakpoint(const tchar_t* module, const tchar_t* path, size_t row, Document* doc)
{
//!!temporal
#ifdef _WIN32
   _ELENA_::List<_ELENA_::Breakpoint>::Iterator it = _breakpoints.start();
   while (!it.Eof()) {
      if (doc == (*it).param && row==(*it).row) {
         if (_debugController->isStarted()) {
            _debugController->toggleBreakpoint(*it, false);
         }
         _breakpoints.cut(it);
         return false;
      }
      it++;
   }
   _ELENA_::Breakpoint breakpoint(module, path, row, doc);
   _breakpoints.add(breakpoint);

   if (_debugController->isStarted()) {
      _debugController->toggleBreakpoint(breakpoint, true);
   }
#endif
   return true;
}

void IDE :: toggleBreakpoint()
{
//!!temporal
#ifdef _WIN32
   int index = _mainFrame->getCurrentIndex();
   Document* doc = _mainFrame->getDocument(index);
   if (doc) {
      _ELENA_::Path path(_mainFrame->getDocumentPath(index));
      Point caret = doc->getCaret();

      _ELENA_::ReferenceNs module;
      Project::retrieveName(path, module);

      toggleBreakpoint(module, path, caret.y, doc);

      // forcee full repaint
      doc->status.frameChanged = true;
      _mainFrame->refreshDocument();
   }
#endif
}

void IDE :: clearBreakpoints()
{
#ifdef _WIN32
   _breakpoints.clear();

   if (_debugController && _debugController->isStarted())
      _debugController->clearBreakpoints();

   _mainFrame->removeAllDocumentMarker(STYLE_BREAKPOINT);
#endif
}

void IDE :: doCreateFile()
{
   _ELENA_::String<tchar_t, 30> path(_T("unnamed"));
   path.appendInt(_unnamedIndex++);

   _GUI_::Text* text = new _GUI_::Text();
   text->create();

   Document* doc = _mainFrame->openDocument(path, text,
      new IDELexicalStyler(this, text, STYLE_DEFAULT, lexLookahead, lexStart, makeStep, defineStyle), Settings::defaultEncoding);

   doc->status.unnamed = true;

   _windowList.add(path);

   onFileOpen();
   onChange();
}

void IDE :: doCreateTempFile(const tchar_t* name)
{
   _GUI_::Text* text = new _GUI_::Text();
   text->create();

   Document* doc = _mainFrame->openDocument(name, text,
      new IDELexicalStyler(this, text, STYLE_DEFAULT, lexLookahead, lexStart, makeStep, defineStyle), Settings::defaultEncoding);

   doc->status.readOnly = true;
   doc->setHighlightMode(false);

   _windowList.add(name);

   onFileOpen();
   onChange();
}

void IDE :: doCreateProject()
{
   if (!closeAll())
      return;

   onProjectOpen();

   showProjectSettings();

   onChange();
}

bool IDE :: doCloseFile()
{
   int index = _mainFrame->getCurrentDocumentIndex();
   if (index != -1) {
      return closeFile(index);
   }
   else return true;
}

bool IDE :: doCloseProject()
{
//!!temporal
#ifdef _WIN32
   if (Project::isChanged()) {
      int result = MsgBox::showQuestion(_appWindow->getHandle(), QUESTION_SAVE_CHANGES);
      if (MsgBox::isCancel(result)) {
         return false;
      }
      else if (MsgBox::isYes(result)) {
         doSaveProject(false);
      }
   }
   Project::reset();

   onProjectClose();
#endif
   return true;
}

void IDE :: doCloseAllButActive()
{
//!!temporal
#ifdef _WIN32
   int index = _mainFrame->getCurrentIndex();
   for (int i = 0 ; i < index ; i++) {
      _mainFrame->selectDocument(0);
      if (!closeFile(0))
         return;
   }
   int count = _mainFrame->getDocumentCount();
   for (int i = 1 ; i < count ; i++) {
      _mainFrame->selectDocument(1);
      if (!closeFile(1))
         return;
   }
#endif
}

void IDE :: doSelectFile(int optionID)
{
   const tchar_t* path = _recentFiles.get(optionID);
   if(openFile(path)) {
      _recentFiles.add(path);

      onChange();
   }
}

void IDE :: doSelectProject(int optionID)
{
   const tchar_t* path = _recentProjects.get(optionID);
   if(openProject(path)) {
      _recentProjects.add(path);

      onChange();
   }
}

void IDE :: doSelectWindow(int optionID)
{
//!!temporal
#ifdef _WIN32
   _mainFrame->selectDocument(_windowList.get(optionID));
#endif
}

void IDE :: doOpenFile()
{
//!!temporal
#ifdef _WIN32
   FileDialog dialog(_appWindow, FileDialog::SourceFilter, OPEN_FILE_CAPTION, Paths::lastPath);

   _ELENA_::List<tchar_t*> files(NULL, _ELENA_::freestr);
   if (dialog.openFiles(files)) {
      _ELENA_::List<tchar_t*>::Iterator it = files.start();
      while (!it.Eof()) {
         if (openFile(*it))
            _recentFiles.add(*it);

         it++;
      }
      onChange();
   }
#endif
}

void IDE :: doOpenProject()
{
//!!temporal
#ifdef _WIN32
   FileDialog dialog(_appWindow, FileDialog::ProjectFilter, OPEN_PROJECT_CAPTION, Paths::lastPath);
   const tchar_t* path = dialog.openFile();
   if (path) {
      if (openProject(path))
         _recentProjects.add(path);

      onChange();
   }
#endif
}

bool IDE :: doSave(bool saveAsMode)
{
//!!temporal
#ifdef _WIN32
   if (_mainFrame->isReadOnly())
      return false;

   if (Project::isUnnamed() && _mainFrame->isDocumentIncluded()) {
      if (!doSaveProject(false))
         return false;

      if (!doSave(_mainFrame->getCurrentIndex(), saveAsMode))
         return false;
   }
   else {
      if (!doSave(_mainFrame->getCurrentIndex(), saveAsMode))
         return false;
   }
#endif
   return true;
}

bool IDE :: doSave(int docIndex, bool saveAsMode)
{
//!!temporal
#ifdef _WIN32
   bool modified = _mainFrame->isDocumentModified(docIndex);
   bool unnamed = _mainFrame->isDocumentUnnamed(docIndex);
   bool included = _mainFrame->isDocumentIncluded(docIndex);
   _ELENA_::Path oldPath(_mainFrame->getDocumentPath(docIndex));
   if (unnamed || saveAsMode) {
      FileDialog dialog(_appWindow, FileDialog::SourceFilter, SAVEAS_FILE_CAPTION, Project::getPath());

      _ELENA_::Path newPath;
	   if (dialog.saveFile(_T("l"), newPath)) {
         renameFileAs(docIndex, newPath, oldPath, included);
      }
      else return false;

      if(unnamed && !included) {
         int result = MsgBox::showQuestion(_appWindow->getHandle(), QUESTION_INCLUDE_FILE1, newPath, QUESTION_INCLUDE_FILE2);
         if (MsgBox::isYes(result)) {
			   _mainFrame->markDocumentAsIncluded(docIndex);
            Project::includeSource(newPath);
         }
      }
      _mainFrame->saveDocument(newPath, docIndex);
   }
   else _mainFrame->saveDocument(oldPath, docIndex);

   if (modified)
      _mainFrame->markDocument(docIndex, false);
#endif
   return true;
}

bool IDE :: doSaveAll(bool forced)
{
//!!temporal
#ifdef _WIN32
   if (_mainFrame->isReadOnly())
      return true;

   if (Project::isUnnamed()) {
      if (!doSaveProject(false))
         return false;
   }
   for (int index = 0 ; index < _mainFrame->getDocumentCount() ; index++) {
      if (_mainFrame->isDocumentModified(index) || _mainFrame->isDocumentUnnamed(index)) {
         if (forced || _mainFrame->isDocumentUnnamed(index)) {
            doSave(index, false);
         }
         else {
            const wchar_t* path = _mainFrame->getDocumentPath(index);
            int result = MsgBox::showQuestion(_appWindow->getHandle(), QUESTION_SAVE_FILECHANGES, path);
            if (MsgBox::isCancel(result)) {
               return false;
            }
            else if (MsgBox::isYes(result)) {
               _mainFrame->saveDocument(path, index);
               _mainFrame->markDocument(index, false);
            }
         }
      }
   }
   if (Project::isChanged()) {
      if (!forced && !Project::isUnnamed()) {
         int result = MsgBox::showQuestion(_appWindow->getHandle(), QUESTION_SAVE_CHANGES);
         if (MsgBox::isYes(result)) {
            doSaveProject(false);
         }
      }
      else doSaveProject(false);
   }
#endif

   return true;
}

bool IDE :: doSaveProject(bool saveAsMode)
{
//!!temporal
#ifdef _WIN32
   if (saveAsMode || Project::isUnnamed()) {
      FileDialog dialog(_appWindow, FileDialog::ProjectFilter, SAVEAS_PROJECT_CAPTION, Project::getPath());
      _ELENA_::Path       path;

      if (dialog.saveFile(_T("prj"), path)) {
         Project::rename(path);

         setCaption(Project::getName());
      }
	   else return false;
   }
   Project::save();
#endif
   return true;
}

bool IDE :: doEditCopy()
{
//!!temporal
#ifdef _WIN32
   if (_clipboard.begin(_appWindow)) {
      bool result = _mainFrame->copyClipboard(_clipboard);

      _clipboard.end();

      return result;
   }
   else return false;
#else
return false;
#endif
}

void IDE :: doEditPaste()
{
//!!temporal
#ifdef _WIN32
   if (_clipboard.begin(_appWindow)) {
      _mainFrame->pasteClipboard(_clipboard);

      _clipboard.end();
   }
#endif
}

void IDE :: doEditDelete()
{
   _mainFrame->eraseSelection();
}

void IDE :: doUndo()
{
   _mainFrame->undo();
}

void IDE :: doRedo()
{
   _mainFrame->redo();
}

void IDE :: doSelectWindow()
{
//!!temporal
#ifdef _WIN32
   IDEWindowsDialog dialog(this);

   if (dialog.showModal()==-2) {
      onChange();
   }
#endif
}

void IDE :: doSetEditorSettings()
{
//!!temporal
#ifdef _WIN32
   EditorSettings dlg(_appWindow);

   if (dlg.showModal()) {
      Text::TabSize = Settings::tabSize;
      _mainFrame->reloadSettings();
   }
#endif
}

void IDE :: doSetDebuggerSettings()
{
//!!temporal
#ifdef _WIN32
   DebuggerSettings dlg(_appWindow);

   if (dlg.showModal()) {
      Settings::onNewProjectTemplate();
   }
#endif
}

void IDE :: doSwitchTab(bool forward)
{
//!!temporal
#ifdef _WIN32

   int index = _mainFrame->getCurrentIndex();
   if (forward) {
      index += 1;
      if (index == _mainFrame->getDocumentCount())
         index = 0;
   }
   else {
      if (index == 0) {
         index = _mainFrame->getDocumentCount() - 1;
      }
      else index--;
   }
   _mainFrame->selectDocument(index);
#endif
}

void IDE :: doFind()
{
//!!temporal
#ifdef _WIN32
   FindDialog dialog(_appWindow, false, &_searchOption, &Settings::searchHistory, NULL);

   if (dialog.showModal()) {
      Settings::addSearchHistory(_searchOption.text);

      if (_mainFrame->findText(_searchOption)) {
         _appMenu->enableItemById(IDM_SEARCH_FINDNEXT, true);
      }
      else MsgBox::showError(_appWindow->getHandle(), NOT_FOUND_TEXT, NULL);
   }
#endif
}

void IDE :: doFindNext()
{
//!!temporal
#ifdef _WIN32
   if (!_mainFrame->findText(_searchOption)) {
      MsgBox::showError(_appWindow->getHandle(), NOT_FOUND_TEXT, NULL);
   }
#endif
}

void IDE :: doReplace()
{
//!!temporal
#ifdef _WIN32
   FindDialog dialog(_appWindow, true, &_searchOption, &Settings::searchHistory, &Settings::replaceHistory);

   if (dialog.showModal()) {
      Settings::addSearchHistory(_searchOption.text);
      Settings::addReplaceHistory(_searchOption.newText);

      bool found = false;
      while (_mainFrame->findText(_searchOption)) {
         found = true;

         int result = MsgBox::showQuestion(_appWindow->getHandle(), REPLACE_TEXT);
         if (MsgBox::isCancel(result)) {
            break;
         }
         else if (MsgBox::isYes(result)) {
            if(!_mainFrame->replaceText(_searchOption))
               break;
         }
      }
      if (!found)
         MsgBox::showError(_appWindow->getHandle(), NOT_FOUND_TEXT, NULL);
   }
#endif
}

void IDE :: doGoToLine()
{
   #ifdef _WIN32 // !! temporal
   Document* doc = _mainFrame->getDocument(_mainFrame->getCurrentIndex());
   if (doc) {
      Point caret = doc->getCaret();

      GoToLineDialog dlg(_appWindow, caret.y + 1);
      if (dlg.showModal()) {
         caret.y = dlg.getLineNumber() - 1;

         doc->setCaret(caret, false);
         _mainFrame->refreshDocument();
      }
   }
   #endif
}

void IDE :: doShowAbout()
{
//!!temporal
#ifdef _WIN32
   AboutDialog dlg(_appWindow);

   dlg.showModal();
#endif
}

bool IDE :: doCompileProject(int postponedAction)
{
//!!temporal
#ifdef _WIN32
   onCompilationStart();

   // exit if the operation was canceled
   if(!doSaveAll(false)) {
      onCompilationEnd(ERROR_CANCELED, false);

      return false;
   }

   if (postponedAction) {
      _state |= uiAutoRecompile;
   }
   if (!compileProject(postponedAction)) {
      onCompilationEnd(ERROR_COULD_NOT_START, false);

      return false;
   }

#endif

   return true;
}

void IDE :: doShowCompilerOutput(bool checked)
{
//!!temporal
#ifdef _WIN32
   if (Settings::compilerOutput != checked) {
      Settings::compilerOutput = checked;

      _appMenu->checkItemById(IDM_VIEW_OUTPUT, Settings::compilerOutput);

      if (checked) {
         _outputBar->show();
      }
      else _outputBar->hide();

      _appWindow->refresh();
   }
#endif
}

void IDE :: doShowDebugWatch(bool visible)
{
//!!temporal
#ifdef _WIN32
   _appMenu->checkItemById(IDM_VIEW_WATCH, visible);

   // !! temporal
   if (!_contextBrowser)
      return;

   if (visible) {
      _contextBrowser->show();
   }
   else _contextBrowser->hide();

   _appWindow->refresh();
#endif
}

bool findBracket(Text* text, TextBookmark& bookmark, tchar_t starting, tchar_t ending, bool forward)
{
   // define the upper / lower border of bracket search
   int frameY = 0;
   if (forward)
      frameY = text->getRowCount();

   int counter = 0;
   while (true) {
      tchar_t ch = text->getChar(bookmark);
      if (ch == starting)
         counter++;
      else if (ch == ending) {
         counter--;
         if (counter==0)
            return true;
      }

      if (forward) {
         if (!bookmark.moveOn(1) || (bookmark.getRow() > frameY))
            break;
      }
      else {
         if (!bookmark.moveOn(-1) || bookmark.getRow() < frameY)
            break;
      }
   }
   return false;
}

void IDE :: doHighlightBrackets(Document* doc)
{
   if (!doc)
      return;

   Text* text = doc->getText();
   TextBookmark caret = doc->getCurrentTextBookmark();

   tchar_t current_ch = text->getChar(caret);

   int pos = _ELENA_::StringHelper::find(OPENING_BRACKET, current_ch, -1);
   if (pos != -1) {
      Point frame = doc->getFrame();
      Point size = doc->getSize();

      _state |= uiBracketBold;

      int openBracketPos = caret.getPosition();
      int closeBracketPos = -1;

      if (findBracket(text, caret, OPENING_BRACKET[pos], CLOSING_BRACKET[pos], true)) {
         closeBracketPos = caret.getPosition();
      }

      if (doc->addMarker(HighlightInfo(openBracketPos, closeBracketPos, -1, -1), STYLE_HIGHLIGHTED_BRACKET, STYLE_HIGHLIGHTED_BRACKET, false))
         _mainFrame->refreshDocument();

      return;
   }
   pos = _ELENA_::StringHelper::find(CLOSING_BRACKET, current_ch, -1);

   if (_ELENA_::StringHelper::find(CLOSING_BRACKET, current_ch, -1) != -1) {
      _state |= uiBracketBold;

      int openBracketPos = -1;
      int closeBracketPos = caret.getPosition();

      if (findBracket(text, caret, CLOSING_BRACKET[pos], OPENING_BRACKET[pos], false)) {
         openBracketPos = caret.getPosition();
      }

      if (doc->addMarker(HighlightInfo(closeBracketPos, openBracketPos, -1, -1), STYLE_HIGHLIGHTED_BRACKET, STYLE_HIGHLIGHTED_BRACKET, false))
         _mainFrame->refreshDocument();

      return;
   }

   if(_ELENA_::test(_state, uiBracketBold)) {
      _state &= ~uiBracketBold;

      doc->removeMarker(-1, STYLE_HIGHLIGHTED_BRACKET);
      _mainFrame->refreshDocument();
   }
}

void IDE :: doInclude()
{
   if (Project::isUnnamed()) {
      if (!doSaveProject(false))
         return;
   }

   if (_mainFrame->getCurrentDocumentIndex() != -1) {
      _mainFrame->markDocumentAsIncluded();

      const tchar_t* path = _mainFrame->getDocumentPath(-1);
      Project::includeSource(path);
   }
   onDocIncluded();
}

void IDE :: doExclude()
{
   if (_mainFrame->getCurrentDocumentIndex() != -1) {
      _mainFrame->markDocumentAsExcluded();

      const tchar_t* path = _mainFrame->getDocumentPath(-1);
      Project::excludeSource(path);
   }
   onDocIncluded();
}

void IDE :: showProjectSettings()
{
   #ifdef _WIN32 // !! temporal
   ProjectSettingsDialog dlg(_appWindow);

   dlg.showModal();

   Settings::onNewProjectTemplate();
   #endif
}

void IDE :: showProjectForwards()
{
   #ifdef _WIN32 // !! temporal
   ProjectForwardsDialog dlg(_appWindow);

   dlg.showModal();
   #endif
}

bool IDE :: onClose()
{
   bool result = closeAll();

   onChange();

   return result;
}

void IDE :: onProjectOpen()
{
   setCaption(Project::getName());

   _state |= uiProjectActive;
}

void IDE :: onProjectClose()
{
   setCaption(NULL);

   clearBreakpoints();

   // !! temporal check
   if (_messageList)
      _messageList->clear();

   _state &= ~uiProjectActive;
}

void IDE :: onFileOpen()
{
   if (!_ELENA_::test(_state, uiFrameShown)) {
      _mainFrame->show();
      _mainFrame->setFocus();

      _state |= uiFrameShown;

   #ifdef _WIN32
      _statusBar->setText(2, EMPTY_STRING);
      _statusBar->setText(3, EMPTY_STRING);
   #endif
   }
   #ifdef _WIN32
   onDocIncluded();
   #endif
}

void IDE :: onFileClose()
{
   #ifdef _WIN32
   if (_mainFrame->getCurrentIndex()==-1) {
      _mainFrame->hide();

      _state &= ~uiFrameShown;

      _unnamedIndex = 0;
   }
#endif
}

void IDE :: onChange()
{
   onUIChange(_state);
   onFrameChange(_mainFrame->getState());
}

void IDE :: onCursorChange()
{
   if (_highlightBrackets) {
      doHighlightBrackets(_mainFrame->getDocument(-1));
   }
}

void IDE :: onRowChange(Document* doc, int row)
{
   int rowChange = doc->status.rowDifference;
   doc->status.rowDifference = 0;

   bool changed = false;
#ifdef _WIN32
   // shift breakpoints if the row number was changed
   Breakpoints::Iterator it = _breakpoints.start();
   while (!it.Eof()) {
      if ((*it).row > row && (*it).param == doc) {
         (*it).row += rowChange;
         changed = true;

         break;
      }
      it++;
   }
#endif
   if (changed) {
      // to force repaint
      doc->status.frameChanged = true;

      _mainFrame->refreshDocument();
   }
}

void IDE :: onDocIncluded()
{
   #ifdef _WIN32
	Document* doc = _mainFrame->getDocument(-1);
	if (doc != NULL) {
      _appMenu->enableItemById(IDM_PROJECT_INCLUDE, !doc->status.included);
      _appMenu->enableItemById(IDM_PROJECT_EXCLUDE, doc->status.included);
	}
	else {
      _appMenu->enableItemById(IDM_PROJECT_INCLUDE, false);
      _appMenu->enableItemById(IDM_PROJECT_EXCLUDE, false);
	}
	#endif
}

void IDE :: onFrameChange(FrameState state)
{
   #ifdef _WIN32
   bool hasDocument = (state != editEmpty);
   bool hasSelection = hasDocument && _ELENA_::test(state, editHasSelection);
   bool canUndo = hasDocument && _ELENA_::test(state, editCanUndo);
   bool canRedo = hasDocument && _ELENA_::test(state, editCanRedo);
   bool modeChange = hasDocument && _ELENA_::test(state, editModeChanged);

   // update menu options
   _appMenu->enableItemById(IDM_EDIT_COPY, hasSelection);
   _appMenu->enableItemById(IDM_EDIT_CUT, hasSelection);
   _appMenu->enableItemById(IDM_EDIT_DELETE, hasSelection);
   _appMenu->enableItemById(IDM_EDIT_UNDO, canUndo);
   _appMenu->enableItemById(IDM_EDIT_REDO, canRedo);
   _appMenu->enableItemById(IDM_EDIT_UPPERCASE, hasDocument);
   _appMenu->enableItemById(IDM_EDIT_LOWERCASE, hasDocument);
   _appMenu->enableItemById(IDM_EDIT_COMMENT, hasSelection);
   _appMenu->enableItemById(IDM_EDIT_UNCOMMENT, hasSelection);

   // update toolbar options
   _appToolBar->enableItemById(IDM_EDIT_CUT, hasSelection);
   _appToolBar->enableItemById(IDM_EDIT_COPY, hasSelection);
   _appToolBar->enableItemById(IDM_EDIT_UNDO, canUndo);
   _appToolBar->enableItemById(IDM_EDIT_REDO, canUndo);

   // update status bar
   if (hasDocument) {
      if (_ELENA_::test(_state, uiHighlight)) {
         _state &= ~uiHighlight;

         _mainFrame->removeAllDocumentMarker(STYLE_TRACE_LINE);
      }

      Document* doc = _mainFrame->getDocument(-1);
      Point caret = doc->getCaret();

      if (doc->status.rowDifference != 0) {
         if (_lastCaret.x == 0) {
            onRowChange(doc, caret.y - 2);
         }
         else onRowChange(doc, caret.y - 1);
      }

      if (_lastCaret != caret) {
         _ELENA_::String<tchar_t, 30> line(_T("Ln "));
         line.appendInt(caret.y + 1);
         line.append(_T(" Col "));
         line.appendInt(caret.x + 1);

         _statusBar->setText(1, line);

         _lastCaret = caret;
      }

      if (modeChange) {
         if (_ELENA_::test(state, editModifiedMode)) {
            _statusBar->setText(2, EDITOR_MODIFIED);

            _mainFrame->markDocument(_mainFrame->getCurrentIndex(), true);
         }
         else _statusBar->setText(2, EMPTY_STRING);

         if (_ELENA_::test(state, editOverwriteMode)) {
            _statusBar->setText(3, _T("OVR"));
         }
         else _statusBar->setText(3, _T("INS"));
      }
   }
   #endif
}

void IDE :: onUIChange(int state)
{
   #ifdef _WIN32
   bool hasDocument = _ELENA_::test(state, uiFrameShown);
   bool hasProject = _ELENA_::test(state, uiProjectActive);
   bool busy = _ELENA_::test(state, uiIDEBusy);
   bool debugging = _ELENA_::test(state, uiDebugging);

   bool hasDocumentPrev = _ELENA_::test(_previousState, uiFrameShown);
   bool hasProjectPrev = _ELENA_::test(_previousState, uiProjectActive);
   bool busyPrev = _ELENA_::test(_previousState, uiIDEBusy);
   bool debuggingPrev = _ELENA_::test(_previousState, uiDebugging);

   if (hasDocument != hasDocumentPrev) {
      _appMenu->enableItemById(IDM_FILE_SAVE, hasDocument);
      _appMenu->enableItemById(IDM_FILE_SAVEAS, hasDocument);
      _appMenu->enableItemById(IDM_FILE_SAVEALL, hasDocument);
      _appMenu->enableItemById(IDM_FILE_CLOSE, hasDocument);
      _appMenu->enableItemById(IDM_FILE_CLOSEALL, hasDocument);
      _appMenu->enableItemById(IDM_FILE_CLOSEALLBUT, hasDocument);

      _appToolBar->enableItemById(IDM_FILE_SAVE, hasDocument);
      _appToolBar->enableItemById(IDM_FILE_SAVEALL, hasDocument);
      _appToolBar->enableItemById(IDM_FILE_CLOSE, hasDocument);

      _appMenu->enableItemById(IDM_EDIT_SELECTALL, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_TRIM, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_ERASELINE, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_DUPLICATE, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_INDENT, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_OUTDENT, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_SWAP, hasDocument);
      _appMenu->enableItemById(IDM_EDIT_PASTE, hasDocument);

      _appToolBar->enableItemById(IDM_EDIT_PASTE, hasDocument);

      _appMenu->enableItemById(IDM_SEARCH_FIND, hasDocument);
      _appMenu->enableItemById(IDM_SEARCH_FINDNEXT, hasDocument);
      _appMenu->enableItemById(IDM_SEARCH_GOTOLINE, hasDocument);
      _appMenu->enableItemById(IDM_SEARCH_REPLACE, hasDocument);
      _appMenu->enableItemById(IDM_DEBUG_BREAKPOINT, hasDocument);

      _appMenu->enableItemById(IDM_WINDOW_NEXT, hasDocument);
      _appMenu->enableItemById(IDM_WINDOW_PREVIOUS, hasDocument);
      _appMenu->enableItemById(IDM_WINDOW_WINDOWS, hasDocument);

      if (!hasDocument) {
         onDocIncluded();

         _appMenu->enableItemById(IDM_EDIT_UNDO, false);
         _appMenu->enableItemById(IDM_EDIT_REDO, false);
         _appMenu->enableItemById(IDM_EDIT_COPY, false);
         _appMenu->enableItemById(IDM_EDIT_CUT, false);
         _appMenu->enableItemById(IDM_EDIT_DELETE, false);
         _appMenu->enableItemById(IDM_EDIT_UPPERCASE, false);
         _appMenu->enableItemById(IDM_EDIT_LOWERCASE, false);
         _appMenu->enableItemById(IDM_EDIT_COMMENT, false);
         _appMenu->enableItemById(IDM_EDIT_UNCOMMENT, false);

         _appToolBar->enableItemById(IDM_EDIT_CUT, false);
         _appToolBar->enableItemById(IDM_EDIT_COPY, false);
         _appToolBar->enableItemById(IDM_EDIT_UNDO, false);
         _appToolBar->enableItemById(IDM_EDIT_REDO, false);
      }
      else onFrameChange(_mainFrame->getState());
   }

   if (hasProject != hasProjectPrev) {
      _appMenu->enableItemById(IDM_FILE_SAVEPROJECT, hasProject);
      _appMenu->enableItemById(IDM_DEBUG_CLEARBREAKPOINT, hasProject);
      _appMenu->enableItemById(IDM_PROJECT_CLOSE, hasProject);

      _appToolBar->enableItemById(IDM_PROJECT_CLOSE, hasProject);
   }

   if (hasProject != hasProjectPrev || busyPrev != busy || debuggingPrev != debugging) {
      bool compileEnabled = hasProject && !busy && !debugging;

      _appMenu->enableItemById(IDM_PROJECT_COMPILE, compileEnabled);
      _appMenu->enableItemById(IDM_PROJECT_FORWARDS, compileEnabled);
      _appMenu->enableItemById(IDM_PROJECT_OPTION, compileEnabled);
      _appMenu->enableItemById(IDM_PROJECT_CLEAN, compileEnabled);

      bool runEnabled = hasProject && !busy;
      _appMenu->enableItemById(IDM_DEBUG_RUN, runEnabled);
      _appMenu->enableItemById(IDM_DEBUG_RUNTO, runEnabled);
      _appMenu->enableItemById(IDM_DEBUG_STEPOVER, runEnabled);
      _appMenu->enableItemById(IDM_DEBUG_STEPINTO, runEnabled);
      _appMenu->enableItemById(IDM_DEBUG_NEXTSTATEMENT, runEnabled);

      _appToolBar->enableItemById(IDM_DEBUG_RUN, runEnabled);
      _appToolBar->enableItemById(IDM_DEBUG_STEPINTO, runEnabled);
      _appToolBar->enableItemById(IDM_DEBUG_STEPOVER, runEnabled);

      bool stopEnabled = hasProject && debugging;
      _appMenu->enableItemById(IDM_DEBUG_STOP, stopEnabled);

      _appToolBar->enableItemById(IDM_DEBUG_STOP, stopEnabled);

      bool debugEnabled = hasProject && !busy && debugging;
      _appMenu->enableItemById(IDM_DEBUG_GOTOSOURCE, debugEnabled);
      _appMenu->enableItemById(IDM_DEBUG_INSPECT, debugEnabled);

      _appToolBar->enableItemById(IDM_DEBUG_GOTOSOURCE, debugEnabled);
   }

   _previousState = _state;
   #endif
}

void IDE :: onCompilationStart()
{
   #ifdef _WIN32

   _outputBar->selectTab(0);

//  !! _debugger.clear();
   _messageList->clear();

   _state &= ~uiHighlight;
   _mainFrame->removeAllDocumentMarker(STYLE_TRACE_LINE);

   _state |= uiIDEBusy;

   onUIChange(_state);

   _statusBar->setText(0, PROJECT_COMPILING);
#endif
}

void IDE :: onCompilationEnd(const tchar_t* message, bool successful)
{
   if (!successful) {
      _state &= ~(uiIDEBusy | uiAutoRecompile);
   }
   else _state &= ~uiIDEBusy;

   onUIChange(_state);

   _statusBar->setText(0, message);
}

bool IDE :: onDebugAction(int action, bool stepMode)
{
   #ifdef _WIN32
   if (_ELENA_::test(_state, uiIDEBusy))
      return false;

   _mainFrame->removeAllDocumentMarker(STYLE_TRACE_LINE);
   _statusBar->setText(0, NULL);

   if (!_debugController->isStarted()) {
      bool recompile = Settings::autoRecompile && !_ELENA_::test(_state, uiAutoRecompile);
      if (!isOutaged(recompile)) {
         if (recompile) {
            if(!doCompileProject(action))
               return false;
         }
         return false;
      }
      if (!startDebugger(stepMode))
         return false;
   }
#endif
   return true;
}

void IDE :: onDebuggerStart()
{
   _state |= uiDebugging;

   doShowDebugWatch(true);

   _mainFrame->setReadOnlyMode(true);

   onChange();
}

void IDE :: onDebuggerStop(bool broken)
{
   #ifdef _WIN32
   _state &= ~uiDebugging;

   doShowDebugWatch(false);

   _statusBar->setText(0, broken ? PROGRAM_BROKEN : PROGRAM_STOPPED);

   _mainFrame->removeAllDocumentMarker(STYLE_TRACE_LINE);
   _mainFrame->setReadOnlyMode(false);
   _contextBrowser->reset();

   _debugController->release();

   //// close temporal document
   //// !! probably more generic solution should be used
   //int tempDocIndex = _mainFrame->getDocumentIndex(_ELENA_::ConstantIdentifier(TAPE_SYMBOL));
   //if (tempDocIndex >= 0)
   //   _mainFrame->closeDocument(tempDocIndex);

   onChange();
#endif
}

void IDE :: onDebuggerStep(const wchar16_t* ns, const tchar_t* source, HighlightInfo info)
{
   #ifdef _WIN32
   if (!loadModule(ns, source)) {
      MsgBox::showError(_appWindow->getHandle(), ERROR_MOUDLE_NOT_FOUND, NULL);

      return;
   }

   _mainFrame->addDocumentMarker(-1, info, STYLE_TRACE_LINE, STYLE_TRACE);
   _mainFrame->refreshDocument();

   _contextBrowser->refresh(_debugController);

   refreshDebugStatus();
#endif
}

void IDE :: onDebuggerCheckPoint(const tchar_t* message)
{
   _statusBar->setText(0, message);
}

