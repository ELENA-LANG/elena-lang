//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      IDE main window class implementation
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#include "appwindow.h"
#include "text.h"
#include "sourcedoc.h"
#include "settings.h"

using namespace _GUI_;

#define OPENING_BRACKET   _T("({[")
#define CLOSING_BRACKET   _T(")}]")

// --- Lexical DFA Table ---

const text_c lexStart        = 'a';
const text_c lexCommentStart = 'b';
const text_c lexKeyword = 'c';
const text_c lexOperator = 'd';
const text_c lexBrackets = 'e';
const text_c lexObject = 'f';
const text_c lexCloseBracket = 'g';
const text_c lexStick = 'h';
const text_c lexDigit = 'i';
const text_c lexHint = 'j';
const text_c lexMessage = 'k';
const text_c lexLookahead = 'l';
const text_c lexLineComment = 'm';
const text_c lexComment = 'n';
const text_c lexComment2 = 'o';
const text_c lexQuote = 'p';
const text_c lexQuote2 = 'q';
const text_c lexHint2 = 'r';

const text_c* lexDFA[] =
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

text_c makeStep(text_c ch, text_c state)
{
   return (unsigned)ch < 128 ? lexDFA[state - lexStart][ch] : lexDFA[state - lexStart][127];
}

_ELENA_::pos_t defineStyle(text_c state, _ELENA_::pos_t style)
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

inline size_t evaluateLength(_GUI_::TextBookmark& bookmark, int column)
{
   _GUI_::TextBookmark bm = bookmark;

   bm.moveTo(column, bm.getRow());

   return bm.getPosition() - bookmark.getPosition();
}

bool IDEController::IDELexicalStyler :: checkMarker(MarkerInfo& marker, LexicalInfo& li, _ELENA_::pos_t& styleLen)
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

_ELENA_::pos_t IDEController::IDELexicalStyler :: proceed(_ELENA_::pos_t position, LexicalInfo& li)
{
   _ELENA_::pos_t styleLen = 0xFF; // !! temporal

   if (_current.bandStyle != -1 && checkMarker(_current, li, styleLen)) {
      // set marker attribute if the breakpoint is set on the same line
      if (li.newLine) {
         Breakpoints::Iterator it = _breakpoints->start();
         Document* doc = ((Document::Reader*)&li)->_doc;
         while (!it.Eof()) {
            if ((*it).row == li.row && (*it).param == doc) {
               li.marker = true;

               break;
            }
            it++;
         }
      }
      return styleLen;
   }
   else {
      if (li.newLine) {
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
      }

      // highlight bracket
      if (_bracketHighlighting) {
         _ELENA_::pos_t position = li.bm.getPosition();
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

// --- IDEController ---

void IDEController::start(_View* view, _DebugListener* listener, Model* model)
{
   _view = view;
   _model = model;
   _project.assign(model);
   _debugController.assignListener(listener);
   _debugController.assignSourceManager(&_project);

   onIDEInit();

   view->start(model->appMaximized);

   Text::TabSize = model->tabSize;

   // open default project
   if (!model->defaultProject.isEmpty())
      openProject(model->defaultProject.c_str());

   // open default files
   _ELENA_::List<_ELENA_::path_c*>::Iterator it = model->defaultFiles.start();
   while (!it.Eof()) {
      openFile(*it);

      it++;
   }

   onChange();
}

bool IDEController :: openProject(_ELENA_::path_t path)
{
   if (!closeAll())
      return false;

   if (!_project.open(path)) {
      _view->error(IDE_MSG_INVALID_PROJECT, path);
      return false;
   }

   if (_model->lastProjectRemember)
      _model->defaultProject.copy(path);

   if (_model->lastPathRemember)
      _model->paths.lastPath.copySubPath(path);

   if (_model->autoProjectLoad) {
      _ELENA_::Path sourcePath;
      _ProjectManager::SourceIterator it = _project.SourceFiles();
      while (!it.Eof()) {
         sourcePath.copy((*it).c_str());
         Paths::resolveRelativePath(sourcePath, _model->project.path.c_str());

         if (!openFile(sourcePath.c_str()))
            _view->error(ERROR_CANNOT_OPEN_FILE, TextString(*it));

         it++;
      }
   }
   onProjectOpen();

   return true;
}

void IDEController :: selectProjectFile(int index)
{
   _ProjectManager::SourceIterator it = _project.SourceFiles();
   while (index > 0) {
      index--;
      it++;
   }

   _ELENA_::Path sourcePath(*it);
   Paths::resolveRelativePath(sourcePath, _model->project.path.c_str());

   if (!openFile(sourcePath.c_str())) {
      _view->error(ERROR_CANNOT_OPEN_FILE, TextString(*it));
   }
   else onChange();
}

bool IDEController :: openFile(_ELENA_::path_t path)
{
   int index = _model->getDocumentIndex(path);
   if (index >= 0) {
      _view->selectDocument(index);

      return true;
   }

#ifdef _WIN32
   _GUI_::Text* text = new _GUI_::Text(_GUI_::eolCRLF);
#else
   _GUI_::Text* text = new _GUI_::Text(_GUI_::eolLF);
#endif

   bool opened = text->load(path, _model->defaultEncoding, _model->autoDetecting);
   if (opened) {
      _GUI_::Document* doc = new SourceDoc(text, new IDELexicalStyler(this, text, STYLE_DEFAULT, lexLookahead, lexStart, makeStep, defineStyle), _model->defaultEncoding);

      _ELENA_::ReferenceNs module;
      _ELENA_::Path modulePath(path);
      _project.retrieveName(modulePath, module);

      _ELENA_::String<text_c, 80> caption;
      size_t len = strlen(module);
      if (len > 50)
         len = 50;

      _ELENA_::Convertor::copy(caption, module, len, len);
      caption[len] = 0;

      caption.append(':');
      caption.append(path + path.findLast(PATH_SEPARATOR) + 1);
      //caption.append(modulePath);

      int index = _view->newDocument(caption, doc);

      _model->mappings.add(path, index);
      _model->documents.add(doc);

	   // check if the file belongs to the project
	   if (_project.isIncluded(path)) {
         markDocumentAsIncluded(index);
	   }

      _view->addToWindowList(path);

      onFileOpen();
      return true;
   }
   else return false;
}

void IDEController :: exit()
{
   if(closeAll())
      _view->exit();
}

bool IDEController :: closeFile(int index)
{
   if (index == -1)
      index = _view->getCurrentDocumentIndex();

   text_t path = _model->getDocumentPath(index);

   if (_model->currentDoc->status.modifiedMode) {
      _View::Answer result = _view->question(QUESTION_SAVE_FILECHANGES, path);
      if (result == _View::Cancel) {
         return false;
      }
      else if (result == _View::Yes) {
         if (!doSave(false))
            return false;
      }
   }
   _view->removeFromWindowList(path);
   _model->removeDocument(index);
   _view->closeDocument(index);

   onFileClose();
   onChange();

   return true;
}

bool IDEController :: closeAll(bool closeProject)
{
   if (_ELENA_::test(_model->state, uiIDEBusy) && !_view->confirm(ERROR_CANNOT_CLOSE_COMPILING)) {
      return false;
   }

   while (_model->documents.Count() > 0) {
      if (!doCloseFile())
         return false;
   }

   return closeProject ? doCloseProject() : true;
}

void IDEController :: removeAllDocumentMarker(int bandStyle)
{
   Documents::Iterator it = _model->documents.start();
   while (!it.Eof()) {
      (*it)->removeMarker(-1, bandStyle);

      it++;
   }

   if (_model->currentDoc)
      _view->refresh();
}

void IDEController :: setCaption(text_t projectName)
{
   _ELENA_::String<text_c, 0x100> title(APP_NAME);
   if (!_ELENA_::emptystr(projectName)) {
      title.append(_T(" - ["));
      title.append(projectName);
      title.append(_T("]"));
   }

   _view->setCaption(title);
}

void IDEController :: clearBreakpoints()
{
   _breakpoints.clear();

   if (_debugController.isStarted())
      _debugController.clearBreakpoints();

   removeAllDocumentMarker(STYLE_BREAKPOINT);
}

void IDEController :: cleanUpProject()
{
   // clean exe file
   if (!_ELENA_::emptystr(_project.getTarget()))
   {
      _ELENA_::Path targetFile(_model->project.path.c_str(), _project.getTarget());

      _view->removeFile(targetFile.c_str());
   }
   // clean module files
   _ELENA_::Path rootPath(_model->project.path.c_str(), _project.getOutputPath());
   for (_ProjectManager::SourceIterator it = _project.SourceFiles(); !it.Eof(); it++) {
      _ELENA_::Path source(rootPath.c_str(), *it);

      _ELENA_::Path module;
      module.copySubPath(source.c_str());

      _ELENA_::ReferenceNs name(_project.getPackage());
      name.pathToName(module.c_str());          // get a full name

      // remove module
      module.copy(rootPath.c_str());
      module.nameToPath(name, _T("nl"));
      _view->removeFile(module.c_str());

      // remove debug info module
      module.changeExtension(_T("dnl"));
      _view->removeFile(module.c_str());
   }
}

void IDEController :: renameFileAs(int index, _ELENA_::path_t newPath, _ELENA_::path_t oldPath, bool included)
{
   _model->mappings.erase(oldPath);
   _model->mappings.add(newPath, index);

   _view->renameDocument(index, _ELENA_::FileName(newPath));

   if (included) {
      _project.excludeSource(oldPath);
      _project.includeSource(newPath);

      nameProjectDocument(index, newPath);
   }
   _view->activateFrame();
}

void IDEController :: saveDocument(_ELENA_::path_t path, int index)
{
   Document* doc = _model->getDocument(index);
   if (doc) {
      doc->save(path);
   }
}

void IDEController :: markDocumentAsIncluded(int index)
{
   Document* doc = _model->getDocument(index);
   if (doc) {
      doc->status.included = true;
   }
}

void IDEController :: markDocumentAsExcluded(int index)
{
   Document* doc = _model->getDocument(index);
   if (doc) {
      doc->status.included = false;
   }
}

void IDEController :: addDocumentMarker(int index, HighlightInfo info, int bandStyle, int style)
{
   Document* doc = _model->getDocument(index);

   doc->addMarker(info, bandStyle, style);

   _view->refresh();
}

void IDEController :: highlightMessage(MessageBookmark* bookmark, int bandStyle)
{
   if (bookmark) {
      _ELENA_::ident_t module = bookmark->module;

      //_ELENA_::ReferenceNs name;
      //Project::retrieveName(bookmark->file, name);

      _ELENA_::Path docPath;
      if (bookmark->module == NULL || module.compare(_project.getPackage())) {
         docPath.copy(_model->project.path.c_str());
         docPath.combine(bookmark->file);
      }
      else {
         docPath.copy(_model->paths.packageRoot.c_str());
         docPath.combine(module, module.find('\'', _ELENA_::getlength(bookmark->module)));
         docPath.combine(bookmark->file);

         openFile(docPath.c_str());
      }

      HighlightInfo hi(bookmark->col - 1, bookmark->row - 1, 0, 0);

      removeAllDocumentMarker(bandStyle);

      if(openFile(docPath.c_str())) {
         addDocumentMarker(-1, hi, bandStyle, bandStyle);
         onChange();

         _model->state |= uiHighlight;
      }
   }
}

void IDEController :: doSetProjectSettings()
{
   _view->configProject(&_project);

   _project.refresh();

   Settings::onNewProjectTemplate(_model, &_project);
}

void IDEController :: doSetEditorSettings()
{
   if (_view->configEditor(_model))
   {
      Text::TabSize = _model->tabSize;

      _view->reloadSettings();
   }
}

void IDEController :: doSetDebuggerSettings()
{
   if (_view->configDebugger(_model)) {
      Settings::onNewProjectTemplate(_model, &_project);
   }
}

void IDEController :: doSetProjectForwards()
{
   _view->configurateForwards(&_project);

   _project.refresh();
}

bool IDEController :: findText(SearchOption& option)
{
   if (_model->currentDoc) {
      if (_model->currentDoc->findLine(option.text, option.matchCase, option.wholeWord)) {
         _view->refresh();

         return true;
      }
   }
   return false;
}

bool IDEController :: replaceText(SearchOption& option)
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->insertLine(option.newText, _ELENA_::getlength(option.newText));

      _view->refresh();

      return true;
   }
   else return false;
}

void IDEController :: doCreateFile(text_t name, _GUI_::Document* doc)
{
   int index = _view->newDocument(name, doc);

   _model->mappings.add(_ELENA_::path_t(name), index);
   _model->documents.add(doc);

   _view->addToWindowList(name);

   onFileOpen();
   onChange();
}

void IDEController :: doCreateFile()
{
   _ELENA_::String<text_c, 30> name(_T("unnamed"));
   name.appendInt(_model->unnamedIndex++);

#ifdef _WIN32
   _GUI_::Text* text = new _GUI_::Text(_GUI_::eolCRLF);
#else
   _GUI_::Text* text = new _GUI_::Text(_GUI_::eolLF);
#endif

   text->create();

   _GUI_::Document* doc = new SourceDoc(text, new IDELexicalStyler(this, text, STYLE_DEFAULT, lexLookahead, lexStart, makeStep, defineStyle), _model->defaultEncoding);
   doc->status.unnamed = true;

   doCreateFile(name, doc);
}

void IDEController :: doCreateTempFile(text_t name)
{
#ifdef _WIN32
   _GUI_::Text* text = new _GUI_::Text(_GUI_::eolCRLF);
#else
   _GUI_::Text* text = new _GUI_::Text(_GUI_::eolLF);
#endif

   text->create();
   
   _GUI_::Document* doc = new SourceDoc(text, new IDELexicalStyler(this, text, STYLE_DEFAULT, lexLookahead, lexStart, makeStep, defineStyle), _model->defaultEncoding);
   doc->status.readOnly = true;
   doc->setHighlightMode(false);

   doCreateFile(name, doc);
}

void IDEController :: doOpenFile()
{
   _ELENA_::List<text_c*> files(NULL, _ELENA_::freestr);
   if (_view->selectFiles(_model, files)) {
      _ELENA_::List<text_c*>::Iterator it = files.start();
      while (!it.Eof()) {
         if (openFile(*it))
            _view->addToRecentFileList(*it);

         it++;
      }
      onChange();
   }
}

void IDEController::doOpenFile(_ELENA_::path_t path)
{
   if (openFile(path)) {
      _view->addToRecentFileList(path);

      onChange();
   }
}

void IDEController :: doExit()
{
   exit();

   onChange();
}

bool findBracket(Text* text, TextBookmark& bookmark, text_c starting, text_c ending, bool forward)
{
   // define the upper / lower border of bracket search
   int frameY = 0;
   if (forward)
      frameY = text->getRowCount();

   int counter = 0;
   while (true) {
      text_c ch = text->getChar(bookmark);
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

void IDEController :: doHighlightBrackets(Document* doc)
{
   if (!doc)
      return;

   Text* text = doc->getText();
   TextBookmark caret = doc->getCurrentTextBookmark();

   text_c current_ch = text->getChar(caret);

   size_t pos = text_str(OPENING_BRACKET).find(current_ch, -1);
   if (pos != NOTFOUND_POS) {
      //Point frame = doc->getFrame();
      //Point size = doc->getSize();

      _model->state |= uiBracketBold;

      size_t openBracketPos = caret.getPosition();
      size_t closeBracketPos = NOTFOUND_POS;

      if (findBracket(text, caret, OPENING_BRACKET[pos], CLOSING_BRACKET[pos], true)) {
         closeBracketPos = caret.getPosition();
      }

      if (doc->addMarker(HighlightInfo((int)openBracketPos, (int)closeBracketPos, -1, -1), STYLE_HIGHLIGHTED_BRACKET, STYLE_HIGHLIGHTED_BRACKET, false))
         _view->refresh();

      return;
   }
   text_str closing(CLOSING_BRACKET);
   pos = closing.find(current_ch, -1);

   if (closing.find(current_ch, -1) != -1) {
      _model->state |= uiBracketBold;

      size_t openBracketPos = NOTFOUND_POS;
      size_t closeBracketPos = caret.getPosition();

      if (findBracket(text, caret, CLOSING_BRACKET[pos], OPENING_BRACKET[pos], false)) {
         openBracketPos = caret.getPosition();
      }

      if (doc->addMarker(HighlightInfo((int)closeBracketPos, (int)openBracketPos, -1, -1), STYLE_HIGHLIGHTED_BRACKET, STYLE_HIGHLIGHTED_BRACKET, false))
         _view->refresh();

      return;
   }

   if(_ELENA_::test(_model->state, uiBracketBold)) {
      _model->state &= ~uiBracketBold;

      doc->removeMarker(-1, STYLE_HIGHLIGHTED_BRACKET);
      _view->refresh();
   }
}

bool IDEController :: doCloseFile()
{
   if (_model->currentDoc) {
      return closeFile(-1);
   }
   else return true;
}

bool IDEController :: doCloseFile(int index)
{
   return closeFile(index);
}

bool IDEController :: doCloseProject()
{
   if (_model->project.changed) {
      _View::Answer result = _view->question(QUESTION_SAVE_CHANGES);
      if (result == _View::Cancel) {
         return false;
      }
      else if (result == _View::Yes) {
         doSaveProject(false);
      }
   }
   _project.reset();

   onProjectClose();

   return true;
}

void IDEController :: doInclude()
{
   if (_model->isProjectUnnamed()) {
      if (!doSaveProject(false))
         return;
   }

   int index = _view->getCurrentDocumentIndex();
   if (index != -1) {
      markDocumentAsIncluded(index);

      _ELENA_::path_t path = _model->getDocumentPath(index);
      _project.includeSource(path);

      _view->reloadProjectView(&_project);
   }
   onDocIncluded();
}

void IDEController :: doExclude()
{
   int index = _view->getCurrentDocumentIndex();
   if (index != -1) {
      markDocumentAsExcluded(index);

      _ELENA_::path_t path = _model->getDocumentPath(index);
      _project.excludeSource(path);

      _view->reloadProjectView(&_project);
   }
   onDocIncluded();
}

bool IDEController :: doSaveProject(bool saveAsMode)
{
   if (saveAsMode || _model->project.name.isEmpty()) {
      _ELENA_::Path path;
      if (_view->saveProject(_model, path)) {
         _project.rename(path.c_str());

         setCaption(_model->project.name);
      }
	   else return false;
   }
   _project.save(_model->project.extension.c_str(), _model->saveWithBOM);

   if (_ELENA_::test(_model->state, uiProjectNotSaved)) {
      _model->state &= ~uiProjectNotSaved;

      onProjectOpen();
   }

   onChange();

   return true;
}

bool IDEController :: doSave(bool saveAsMode)
{
   if (_model->isDocumentReadOnly() || !_model->currentDoc)
      return false;

   if (_model->project.name.isEmpty()) {
      if (!doSaveProject(false))
         return false;

      if (!doSave(_view->getCurrentDocumentIndex(), saveAsMode))
         return false;
   }
   else {
      if (!doSave(_view->getCurrentDocumentIndex(), saveAsMode))
         return false;
   }

   if (_model->isProjectChanged()) {
      _view->reloadProjectView(&_project);
   }
   onChange();

   return true;
}

void IDEController :: nameProjectDocument(int index, _ELENA_::path_t path)
{
   _ELENA_::ReferenceNs module;
   _ELENA_::Path modulePath(path);
   _project.retrieveName(modulePath, module);

   _ELENA_::String<text_c, 80> caption;
   size_t len = strlen(module);
   if (len > 50)
      len = 50;

   _ELENA_::Convertor::copy(caption, module, len, len);
   caption[len] = 0;

   caption.append(':');
   caption.append(path + path.findLast(PATH_SEPARATOR) + 1);

   _view->renameDocument(index, caption);
}

bool IDEController :: doSave(int docIndex, bool saveAsMode)
{
   Document* doc = _model->getDocument(docIndex);

   bool modified = doc->status.modifiedMode;
   bool unnamed = doc->status.unnamed;
   bool included = doc->status.included;
   _ELENA_::Path oldPath(_model->getDocumentPath(docIndex));
   if (unnamed || saveAsMode) {
      _ELENA_::Path newPath;
      if (_view->saveFile(_model, newPath)) {
         renameFileAs(docIndex, newPath.c_str(), oldPath.c_str(), included);
      }
      else return false;

      if(unnamed && !included) {
         if (_view->confirm(QUESTION_INCLUDE_FILE1, newPath, QUESTION_INCLUDE_FILE2)) {
            markDocumentAsIncluded(docIndex);
            _project.includeSource(newPath.c_str());

            nameProjectDocument(docIndex, newPath.c_str());

            if (doc == _model->currentDoc) {
               onDocIncluded();
            }
         }
      }
      saveDocument(newPath.c_str(), docIndex);
   }
   else saveDocument(oldPath.c_str(), docIndex);

   if (modified)
      _view->markDocumentTitle(docIndex, false);

   return true;
}

bool IDEController :: doSaveAll(bool forced)
{
   if (_model->isDocumentReadOnly())
      return true;

   if (_model->isProjectUnnamed()) {
      if (!doSaveProject(false))
         return false;
   }
   for (int index = 0 ; index < (int)_model->mappings.Count() ; index++) {
      if (_model->isDocumentModified(index) || _model->isDocumentUnnamed(index)) {
         if (forced || _model->isDocumentUnnamed(index)) {
            doSave(index, false);
         }
         else {
            _ELENA_::path_t path = _model->getDocumentPath(index);
            _View::Answer result = _view->question(QUESTION_SAVE_FILECHANGES, path);
            if (result == _View::Cancel) {
               onChange();

               return false;
            }
            else if (result == _View::Yes) {
               saveDocument(path, index);
               _view->markDocumentTitle(index, false);
            }
         }
      }
   }
   if (_model->isProjectChanged()) {
      if (!forced && !_model->isProjectUnnamed()) {
         if (_view->confirm(QUESTION_SAVE_CHANGES)) {
            doSaveProject(false);
         }
      }
      else doSaveProject(false);
   }
   onChange();
   _view->reloadProjectView(&_project);

   return true;
}

void IDEController :: doCreateProject()
{
   if (!closeAll())
      return;

   onProjectOpen();

   doSetProjectSettings();

   _model->state |= uiProjectNotSaved;

   onProjectOpen();
   onChange();
}

void IDEController :: doOpenProject()
{
   _ELENA_::Path path;
   if (_view->selectProject(_model, path)) {
      if (openProject(path.c_str()))
         _view->addToRecentProjectList(path);

      onChange();
   }
}

void IDEController :: doOpenProject(_ELENA_::path_t path)
{
   if (openProject(path)) {
      _view->addToRecentProjectList(path);

      onChange();
   }
}

void IDEController :: doCloseAll(bool closeProject)
{
   closeAll(closeProject);

   onChange();
}

void IDEController :: doCloseAllButActive()
{
   int index = _view->getCurrentDocumentIndex();
   for (int i = 0; i < index; i++) {
      _view->selectDocument(0);
      if (!closeFile(0)) {
         onChange();

         return;
      }
   }
   int count = _model->mappings.Count();
   for (int i = 1; i < count; i++) {
      _view->selectDocument(1);
      if (!closeFile(1)) {
         onChange();

         return;
      }
   }
   onChange();
}

void IDEController :: doUndo()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->undo();

      _view->refresh();
   }
}

void IDEController :: doRedo()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->redo();

      _view->refresh();
   }
}

bool IDEController :: doEditCopy()
{
   if (_model->currentDoc && _model->currentDoc->hasSelection()) {
      return _view->copyToClipboard(_model->currentDoc);
   }
   else return false;
}

void IDEController :: doEditPaste()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _view->pasteFromClipboard(_model->currentDoc);
      _view->refresh();
   }
}

void IDEController :: doEditDelete()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->eraseChar(false);
      _view->refresh();
   }
}

void IDEController :: indent(Document* doc)
{
   if (_model->tabCharUsing) {
      doc->tabbing('\t', 1, true);
   }
   else {
      if (!doc->hasSelection()) {
         size_t shift = _ELENA_::calcTabShift(doc->getCaret().x, _model->tabSize);
         doc->insertChar(' ', shift);
      }
      else doc->tabbing(' ', _model->tabSize, true);
   }
   _view->refresh();
}

void IDEController :: outdent(Document* doc)
{
   if (doc->hasSelection()) {
      if (_model->tabCharUsing) {
         doc->tabbing('\t', 1, false);
      }
      else doc->tabbing(' ', _model->tabSize, false);
   }
   _view->refresh();
}

void IDEController :: doIndent()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      indent(_model->currentDoc);
   }
}

void IDEController :: doOutdent()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      outdent(_model->currentDoc);
   }
}

void IDEController :: doSelectAll()
{
   if (_model->currentDoc) {
      _model->currentDoc->moveFirst(false);
      _model->currentDoc->moveLast(true);
   }
   _view->refresh();
}

void IDEController :: doTrim()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->trim();

      _view->refresh();
   }
}

void IDEController :: doDuplicateLine()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->duplicateLine();

      _view->refresh();
   }
}

void IDEController :: doEraseLine()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->eraseLine();

      _view->refresh();
   }
}

void IDEController :: doComment()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->commentBlock();

      _view->refresh();
   }
}

void IDEController :: doUnComment()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->uncommentBlock();

      _view->refresh();
   }
}

void IDEController :: doUpperCase()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->toUppercase();

      _view->refresh();
   }
}

void IDEController :: doLowerCase()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->toLowercase();

      _view->refresh();
   }
}

void IDEController :: doSwap()
{
   if (_model->currentDoc && !_model->currentDoc->status.readOnly) {
      _model->currentDoc->swap();

      _view->refresh();
   }
}

void IDEController :: doFind()
{
   if (_view->find(_model, &_searchOption, &_model->searchHistory)) {
      Settings::addSearchHistory(_model, _searchOption.text);

      if (findText(_searchOption)) {
         _view->enableMenuItemById(IDM_SEARCH_FINDNEXT, true, false);
      }
      else _view->error(NOT_FOUND_TEXT);
   }
}

void IDEController :: doFindNext()
{
   if (!findText(_searchOption)) {
      _view->error(NOT_FOUND_TEXT);
   }
}

void IDEController :: doReplace()
{
   if (_view->replace(_model, &_searchOption, &_model->searchHistory, &_model->replaceHistory)) {
      Settings::addSearchHistory(_model, _searchOption.text);
      Settings::addReplaceHistory(_model, _searchOption.newText);

      bool found = false;
      while (findText(_searchOption)) {
         found = true;

         _View::Answer result = _view->question(REPLACE_TEXT);
         if (result == _View::Cancel) {
            break;
         }
         else if (result == _View::Yes) {
            if(!replaceText(_searchOption))
               break;
         }
      }
      if (!found)
         _view->error(NOT_FOUND_TEXT);
   }
}

void IDEController :: doGoToLine()
{
   Document* doc = _model->currentDoc;
   if (doc) {
      Point caret = doc->getCaret();

      int row = caret.y + 1;
      if (_view->gotoLine(row)) {
         caret.y = row - 1;

         doc->setCaret(caret, false);
         _view->refresh();
      }
   }
}

void IDEController :: doSwitchTab(bool forward)
{
   int index = _view->getCurrentDocumentIndex();
   if (forward) {
      index += 1;
      if (index == _model->mappings.Count())
         index = 0;
   }
   else {
      if (index == 0) {
         index = _model->mappings.Count() - 1;
      }
      else index--;
   }
   _view->selectDocument(index);
}

void IDEController :: doSelectWindow()
{
   if (_view->selectWindow(_model, this)) {
      onChange();
   }
}

void IDEController :: doSelectWindow(int index)
{
   _view->selectDocument(index);
}

void IDEController :: doSelectWindow(text_t path)
{
   _view->selectDocument(_model->getDocumentIndex(path));
}

void IDEController :: doShowAbout()
{
   _view->about(_model);
}

bool IDEController :: doCompileProject(int postponedAction)
{
   onCompilationStart();

   // exit if the operation was canceled
   if(!doSaveAll(false)) {
      onCompilationEnd(ERROR_CANCELED, false);

      return false;
   }

   if (postponedAction) {
      _model->state |= uiAutoRecompile;
   }
   if (!_view->compileProject(&_project, postponedAction)) {
      onCompilationEnd(ERROR_COULD_NOT_START, false);

      return false;
   }

   return true;
}

void IDEController :: doShowProjectView(bool checked, bool forced)
{
   if (_model->projectView != checked || forced) {
      _model->projectView = checked;

      _view->checkMenuItemById(IDM_VIEW_PROJECTVIEW, _model->projectView);

      if (checked) {
         _view->openProjectView();
      }
      else _view->closeProjectView();

      _view->refresh(false);
   }
}

void IDEController :: doShowCompilerOutput(bool checked, bool forced)
{
   if (_model->compilerOutput != checked || forced) {
      _model->compilerOutput = checked;

      _view->checkMenuItemById(IDM_VIEW_OUTPUT, _model->compilerOutput);

      if (checked) {
         _view->openOutput();
      }
      else _view->closeOutput();

      _view->refresh(false);
   }
}

void IDEController :: doShowVMConsole(bool checked, bool forced)
{
   if (_model->vmConsole != checked || forced) {
      _model->vmConsole = checked;

      _view->checkMenuItemById(IDM_VIEW_VMCONSOLE, _model->vmConsole);

      if (checked) {
         _view->openVMConsole(&_project);
      }
      else _view->closeVMConsole();

      _view->refresh(false);
   }
}

void IDEController :: doShowCallStack(bool checked, bool forced)
{
   if (_model->callStack != checked || forced) {
      _model->callStack = checked;

      _view->checkMenuItemById(IDM_VIEW_CALLSTACK, _model->callStack);

      if (checked) {
         _view->openCallList();

         if (_debugController.isStarted())
            _view->refreshDebugWindows(&_debugController);
      }
      else _view->closeCallList();

      _view->refresh(false);
   }
}

void IDEController :: doShowMessages(bool checked, bool forced)
{
   if (_model->messages != checked || forced) {
      _model->messages = checked;

      _view->checkMenuItemById(IDM_VIEW_MESSAGES, _model->messages);

      if (checked) {
         _view->openMessageList();
      }
      else _view->closeMessageList();

      _view->refresh(false);
   }
}

void IDEController :: doShowDebugWatch(bool visible)
{
   _view->checkMenuItemById(IDM_VIEW_WATCH, visible);

   if (visible) {
      _view->openDebugWatch();
   }
   else _view->closeDebugWatch();

   _view->refresh(false);
}

void IDEController :: onIDEInit()
{
   doShowProjectView(_model->projectView, true);
   doShowCallStack(_model->callStack, true);
   doShowCompilerOutput(_model->compilerOutput, true);
   doShowVMConsole(_model->vmConsole, true);
   doShowMessages(_model->messages, true);

   _view->showStatus(0, EDITOR_READY);

//   // check debugger settings
//   if (Settings::debugTape)
//      _debugController->allowTapeDebug();

   doShowDebugWatch(false);
}

void IDEController :: onUIChange()
{
   bool hasDocument = _ELENA_::test(_model->state, uiFrameShown);
   bool hasProject = _ELENA_::test(_model->state, uiProjectActive);
   bool busy = _ELENA_::test(_model->state, uiIDEBusy);
   bool debugging = _ELENA_::test(_model->state, uiDebugging);

   bool hasDocumentPrev = _ELENA_::test(_model->previousState, uiFrameShown);
   bool hasProjectPrev = _ELENA_::test(_model->previousState, uiProjectActive);
   bool busyPrev = _ELENA_::test(_model->previousState, uiIDEBusy);
   bool debuggingPrev = _ELENA_::test(_model->previousState, uiDebugging);

   if (hasDocument != hasDocumentPrev) {
      _view->enableMenuItemById(IDM_FILE_SAVE, hasDocument, true);
      _view->enableMenuItemById(IDM_FILE_SAVEAS, hasDocument, false);
      _view->enableMenuItemById(IDM_FILE_SAVEALL, hasDocument, true);
      _view->enableMenuItemById(IDM_FILE_CLOSE, hasDocument, true);
      _view->enableMenuItemById(IDM_FILE_CLOSEALL, hasDocument, false);
      _view->enableMenuItemById(IDM_FILE_CLOSEALLBUT, hasDocument, false);

      _view->enableMenuItemById(IDM_EDIT_SELECTALL, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_TRIM, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_ERASELINE, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_DUPLICATE, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_INDENT, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_OUTDENT, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_SWAP, hasDocument, false);
      _view->enableMenuItemById(IDM_EDIT_PASTE, hasDocument, true);

      _view->enableMenuItemById(IDM_SEARCH_FIND, hasDocument, false);
      _view->enableMenuItemById(IDM_SEARCH_FINDNEXT, hasDocument, false);
      _view->enableMenuItemById(IDM_SEARCH_GOTOLINE, hasDocument, false);
      _view->enableMenuItemById(IDM_SEARCH_REPLACE, hasDocument, false);
      _view->enableMenuItemById(IDM_DEBUG_BREAKPOINT, hasDocument, false);

      _view->enableMenuItemById(IDM_WINDOW_NEXT, hasDocument, false);
      _view->enableMenuItemById(IDM_WINDOW_PREVIOUS, hasDocument, false);
      _view->enableMenuItemById(IDM_WINDOW_WINDOWS, hasDocument, false);

      if (!hasDocument) {
         onDocIncluded();

         _view->enableMenuItemById(IDM_EDIT_UNDO, false, true);
         _view->enableMenuItemById(IDM_EDIT_REDO, false, true);
         _view->enableMenuItemById(IDM_EDIT_COPY, false, true);
         _view->enableMenuItemById(IDM_EDIT_CUT, false, true);
         _view->enableMenuItemById(IDM_EDIT_DELETE, false, false);
         _view->enableMenuItemById(IDM_EDIT_UPPERCASE, false, false);
         _view->enableMenuItemById(IDM_EDIT_LOWERCASE, false, false);
         _view->enableMenuItemById(IDM_EDIT_COMMENT, false, false);
         _view->enableMenuItemById(IDM_EDIT_UNCOMMENT, false, false);
      }
      else onFrameChange();
   }

   if (hasProject != hasProjectPrev) {
      _view->enableMenuItemById(IDM_FILE_SAVEPROJECT, hasProject, false);
      _view->enableMenuItemById(IDM_DEBUG_CLEARBREAKPOINT, hasProject, false);
      _view->enableMenuItemById(IDM_PROJECT_CLOSE, hasProject, true);
   }

   if (hasProject != hasProjectPrev || busyPrev != busy || debuggingPrev != debugging) {
      bool compileEnabled = hasProject && !busy && !debugging;

      _view->enableMenuItemById(IDM_PROJECT_COMPILE, compileEnabled, false);
      _view->enableMenuItemById(IDM_PROJECT_FORWARDS, compileEnabled, false);
      _view->enableMenuItemById(IDM_PROJECT_OPTION, compileEnabled, false);
      _view->enableMenuItemById(IDM_PROJECT_CLEAN, compileEnabled, false);

      bool runEnabled = hasProject && !busy;
      _view->enableMenuItemById(IDM_DEBUG_RUN, runEnabled, true);
      _view->enableMenuItemById(IDM_DEBUG_RUNTO, runEnabled, false);
      _view->enableMenuItemById(IDM_DEBUG_STEPOVER, runEnabled, true);
      _view->enableMenuItemById(IDM_DEBUG_STEPINTO, runEnabled, true);

      bool stopEnabled = hasProject && debugging;
      _view->enableMenuItemById(IDM_DEBUG_STOP, stopEnabled, true);

      bool debugEnabled = hasProject && !busy && debugging;
      _view->enableMenuItemById(IDM_DEBUG_GOTOSOURCE, debugEnabled, true);
      _view->enableMenuItemById(IDM_DEBUG_INSPECT, debugEnabled, false);
   }

   _model->previousState = _model->state;
}

void IDEController :: onDocIncluded()
{
	if (_model->currentDoc != NULL) {
      _view->enableMenuItemById(IDM_PROJECT_INCLUDE, !_model->currentDoc->status.included, false);
      _view->enableMenuItemById(IDM_PROJECT_EXCLUDE, _model->currentDoc->status.included, false);
	}
	else {
      _view->enableMenuItemById(IDM_PROJECT_INCLUDE, false, false);
      _view->enableMenuItemById(IDM_PROJECT_EXCLUDE, false, false);
	}
}

void IDEController :: onChange()
{
   onUIChange();
   onFrameChange();
}

void IDEController :: onFrameChange()
{
   FrameState state = _model->getFrameState();

   bool hasDocument = (state != editEmpty);
   bool hasSelection = hasDocument && _ELENA_::test(state, editHasSelection);
   bool canUndo = hasDocument && _ELENA_::test(state, editCanUndo);
   bool canRedo = hasDocument && _ELENA_::test(state, editCanRedo);
   bool modeChange = hasDocument && _ELENA_::test(state, editModeChanged);

   // update menu options
   _view->enableMenuItemById(IDM_EDIT_COPY, hasSelection, true);
   _view->enableMenuItemById(IDM_EDIT_CUT, hasSelection, true);
   _view->enableMenuItemById(IDM_EDIT_DELETE, hasSelection, false);
   _view->enableMenuItemById(IDM_EDIT_UNDO, canUndo, true);
   _view->enableMenuItemById(IDM_EDIT_REDO, canRedo, true);
   _view->enableMenuItemById(IDM_EDIT_UPPERCASE, hasDocument, false);
   _view->enableMenuItemById(IDM_EDIT_LOWERCASE, hasDocument, false);
   _view->enableMenuItemById(IDM_EDIT_COMMENT, hasSelection, false);
   _view->enableMenuItemById(IDM_EDIT_UNCOMMENT, hasSelection, false);

   // update status bar
   if (hasDocument) {
      if (_ELENA_::test(_model->state, uiHighlight)) {
         _model->state &= ~uiHighlight;

         removeAllDocumentMarker(STYLE_TRACE_LINE);
      }

      Point caret = _model->currentDoc->getCaret();

      if (_model->currentDoc->status.rowDifference != 0) {
         if (_model->lastCaret.x == 0) {
            onRowChange(caret.y - 2);
         }
         else onRowChange(caret.y - 1);
      }

      if (_model->lastCaret != caret) {
         _ELENA_::String<text_c, 30> line(_T("Ln "));
         line.appendInt(caret.y + 1);
         line.append(_T(" Col "));
         line.appendInt(caret.x + 1);

         _view->showStatus(1, line);

         _model->lastCaret = caret;
      }

      if (modeChange) {
         if (_ELENA_::test(state, editModifiedMode)) {
            _view->showStatus(2, EDITOR_MODIFIED);

            _view->markDocumentTitle(_view->getCurrentDocumentIndex(), true);
         }
         else _view->showStatus(2, DEFAULT_TEXT);

         if (_ELENA_::test(state, editOverwriteMode)) {
            _view->showStatus(3, _T("OVR"));
         }
         else _view->showStatus(3, _T("INS"));
      }
   }
}

void IDEController :: onRowChange(int row)
{
   int rowChange = _model->currentDoc->status.rowDifference;
   _model->currentDoc->status.rowDifference = 0;

   bool changed = false;
   // shift breakpoints if the row number was changed
   Breakpoints::Iterator it = _breakpoints.start();
   while (!it.Eof()) {
      if ((*it).row > row && (*it).param == _model->currentDoc) {
         (*it).row += rowChange;
         changed = true;

         break;
      }
      it++;
   }
   if (changed) {
      // to force repaint
      _model->currentDoc->status.frameChanged = true;

      _view->refresh();
   }
}

void IDEController :: onFileOpen()
{
   if (!_ELENA_::test(_model->state, uiFrameShown)) {
      _view->showFrame();

      _model->state |= uiFrameShown;

      _view->showStatus(2, DEFAULT_TEXT);
      _view->showStatus(3, DEFAULT_TEXT);
   }

   onDocIncluded();
}

void IDEController::onFileClose()
{
   if (_view->getCurrentDocumentIndex() == -1) {
      _view->hideFrame();

      _model->state &= ~uiFrameShown;

      _model->unnamedIndex = 0;
   }
}

void IDEController :: onCursorChange()
{
   if (_model->highlightBrackets) {
      doHighlightBrackets(_model->currentDoc);
   }
}

void IDEController :: onProjectOpen()
{
   setCaption(_model->project.name);

   _view->reloadProjectView(&_project);

   _model->state |= uiProjectActive;
}

void IDEController :: onProjectClose()
{
   setCaption(NULL);

   clearBreakpoints();

//   // !! temporal check
//   if (_messageList)
//      _messageList->clear();

   _model->state &= ~uiProjectActive;

   _view->reloadProjectView(&_project);

//   ((Output*)_output)->clear();
}

bool IDEController :: onClose()
{
   if (_model->vmConsole)
      _view->closeVMConsole();

   bool result = closeAll();

   onChange();

   return result;
}

void IDEController :: onCompilationStart()
{
   _view->switchToOutput();

//  !! _debugger.clear();
   _view->clearMessageList();

   _model->state &= ~uiHighlight;
   removeAllDocumentMarker(STYLE_TRACE_LINE);

   _model->state |= uiIDEBusy;

   onUIChange();

   _view->showStatus(0, PROJECT_COMPILING);
}

void IDEController :: onCompilationEnd(text_t message, bool successful)
{
   if (!successful) {
      _model->state &= ~(uiIDEBusy | uiAutoRecompile);
   }
   else _model->state &= ~uiIDEBusy;

   onUIChange();

   _view->showStatus(0, message);
}

bool IDEController :: onDebugAction(int action, bool stepMode)
{
   if (_ELENA_::test(_model->state, uiIDEBusy))
      return false;

   removeAllDocumentMarker(STYLE_TRACE_LINE);
   _view->showStatus(0, NULL);

   if (!_debugController.isStarted()) {
      bool recompile = _model->autoRecompile && !_ELENA_::test(_model->state, uiAutoRecompile);
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
   return true;
}

void IDEController :: onDebuggerStart()
{
   _model->state |= uiDebugging;

   _view->openDebugWatch();
   _view->checkMenuItemById(IDM_VIEW_WATCH, true);

   _model->setReadOnlyMode(true);

   onChange();
}

bool IDEController :: startDebugger(bool stepMode)
{
   const char* target = _project.getTarget();
   const char* arguments = _project.getArguments();

   if (!_ELENA_::emptystr(target)) {
      _ELENA_::Path exePath(_model->project.path.c_str(), target);

      // provide the whole command line including the executable path and name
      _ELENA_::Path commandLine(exePath);
      commandLine.append(_T(" "));
      commandLine.append(TextString(arguments).str());

      bool mode = _project.getDebugMode() != 0;
      if (mode) {
         if (!_debugController.start(exePath.c_str(), commandLine.c_str(), mode, _breakpoints)) {
            _view->error(ERROR_DEBUG_FILE_NOT_FOUND_COMPILE);

            return false;
         }
         else return true;
      }
      else if(stepMode) {
         _view->error(ERROR_DEBUG_FILE_NOT_FOUND_SETTING);

         return false;
      }
      else {
         if (!_debugController.start(exePath.c_str(), commandLine.c_str(), false, _breakpoints)) {
            _view->error(ERROR_RUN_NEED_RECOMPILE);

            return false;
         }
         else return true;
      }
   }
   else {
      _view->error(ERROR_RUN_NEED_TARGET);

      return false;
   }
}

bool IDEController :: isOutaged(bool noWarning)
{
   if (_model->isAnyDocumentModified()) {
      if (!noWarning)
         _view->error(ERROR_RUN_OUT_OF_DATE);

      return false;
   }

   _ELENA_::Path rootPath(_model->project.path.c_str(), _project.getOutputPath());
   for (_ProjectManager::SourceIterator it = _project.SourceFiles(); !it.Eof(); it++) {
      _ELENA_::Path source(rootPath.c_str(), *it);

      _ELENA_::Path module;
      module.copySubPath(*it);

      _ELENA_::ReferenceNs name(_project.getPackage());
      name.pathToName(module.c_str());          // get a full name

      while (name.Length() != 0) {
         module.copy(rootPath.c_str());
         module.nameToPath(name, _T("nl"));

         if (!_ELENA_::Path::ifExist(module.c_str())) {
            name.trimProperName();
         }
         else break;
      }

      if (name.Length() > 0) {
         DateTime sourceDT = DateTime::getFileTime(source);
         DateTime moduleDT = DateTime::getFileTime(module);

         if (sourceDT > moduleDT) {
            if (!noWarning)
               _view->error(ERROR_RUN_OUT_OF_DATE);

            return false;
         }
      }
      else return false;
   }

   return true;
}

void IDEController :: runToCursor()
{
   Document* doc = _model->currentDoc;
   if (doc) {
      _ELENA_::Path path(_model->getDocumentPath(_view->getCurrentDocumentIndex()));
      Point caret = doc->getCaret();

      _ELENA_::ReferenceNs module;
      _project.retrieveName(path, module);

      _debugController.runToCursor(module, path.c_str(), 0, caret.y);
   }
}

// --- SourceManager ---

void IDEController::ProjectManager :: retrievePath(_ELENA_::ident_t name, _ELENA_::Path & path, _ELENA_::path_t extension)
{
   _ELENA_::ident_t  package = getPackage();

      // if it is the root package
   if (package.compare(name)) {
      path.copy(_model->project.path.c_str());
      path.combine(getOutputPath());
      path.nameToPath(name, extension);
   }
   // if the class belongs to the project package
   else if (name.compare(package, _ELENA_::getlength(package)) && (name[_ELENA_::getlength(package)] == '\'')) {
      path.copy(_model->project.path.c_str());
      path.combine(getOutputPath());

      path.nameToPath(name, extension);
   }
   else {
      // if file doesn't exist use package root
      path.copy(_model->paths.libraryRoot.c_str());

      path.nameToPath(name, extension);
   }
}

void IDEController :: onDebuggerStop(bool broken)
{
   _model->state &= ~uiDebugging;

   _view->closeDebugWatch();
   _view->checkMenuItemById(IDM_VIEW_WATCH, false);

   _view->showStatus(0, broken ? PROGRAM_BROKEN : PROGRAM_STOPPED);

   removeAllDocumentMarker(STYLE_TRACE_LINE);
   _model->setReadOnlyMode(false);

   _debugController.release();

   //// close temporal document
   //// !! probably more generic solution should be used
   //int tempDocIndex = _model->getDocumentIndex(_ELENA_::Path(TAPE_SYMBOL).str());
   //if (tempDocIndex >= 0) {
   //   _model->removeDocument(tempDocIndex);
   //   _view->closeDocument(tempDocIndex);
   //}      

   onChange();
}

void IDEController :: onDebuggerStep(text_t ns, text_t source, HighlightInfo info)
{
   if (!loadModule(ns, source)) {
      _view->error(ERROR_MOUDLE_NOT_FOUND);

      return;
   }

   addDocumentMarker(-1, info, STYLE_TRACE_LINE, STYLE_TRACE);

   _view->refreshDebugWindows(&_debugController);
}

void IDEController :: onDebuggerAssemblyStep(text_t name, HighlightInfo info)
{
   loadTemporalModule(name, info.disp);

   // HOTFIX : disp is a tape buffer offset
   info.disp = 0;

   addDocumentMarker(-1, info, STYLE_TRACE_LINE, STYLE_TRACE);

   _view->refreshDebugWindows(&_debugController);
}

void IDEController :: onDebuggerCheckPoint(text_t message)
{
   _view->showStatus(0, message);
}

bool IDEController :: loadModule(text_t ns, text_t source)
{
   if (!_debugController.isStarted())
      return false;

   // HOTFIX : if it is template code
   size_t pos = text_str(source).findLast('\'');
   if (pos != NOTFOUND_POS) {
      text_c templateNs[_ELENA_::IDENTIFIER_LEN];
      size_t dummy = pos;
      _ELENA_::Convertor::copy(templateNs, source, (size_t)pos, dummy);
      templateNs[pos] = 0;

      return loadModule(templateNs, source + pos + 1);
   }
   else if (_ELENA_::NamespaceName::isIncluded(_project.getPackage(), _ELENA_::IdentifierString(ns))) {
      _ELENA_::Path path(_model->project.path);
      path.combine(source);
#ifdef _WIN32
      // HOTFIX : to ignore case for win32
      path.lower();
#endif

      openFile(path.c_str());
   }
   else {
      _ELENA_::Path path(_model->paths.packageRoot);
      path.combine(ns, text_str(ns).find('\'', _ELENA_::getlength(ns)));
      path.combine(source);
#ifdef _WIN32
      // HOTFIX : to ignore case for win32
      path.lower();
#endif

      openFile(path.c_str());
   }

   return true;
}

bool IDEController :: loadTemporalModule(text_t name, int param)
{
//   if (!_debugController->isStarted())
//      return false;

   int index = _model->getDocumentIndex(name);

   // if the temporal module is not yet opened - create it
   if (index == -1) {
      doCreateTempFile(name);

      index = _model->getDocumentIndex(name);
   }
   else _view->selectDocument(index);

   Document* doc = _model->getDocument(index);

   // clear all
   doc->moveFirst(false);
   doc->moveLast(true);
   doc->eraseChar(false);

   // insert temporal content
   text_t source = _debugController.getTemporalSource(param);
   doc->insertLine(source, _ELENA_::getlength(source));

   doc->status.modifiedMode = false;

   return true;
}

bool IDEController :: toggleBreakpoint(_ELENA_::ident_t module, _ELENA_::ident_t path, int row, Document* doc)
{
   _ELENA_::List<_ELENA_::Breakpoint>::Iterator it = _breakpoints.start();
   while (!it.Eof()) {
      if (doc == (*it).param && row==(*it).row) {
         if (_debugController.isStarted()) {
            _debugController.toggleBreakpoint(*it, false);
         }
         _breakpoints.cut(it);
         return false;
      }
      it++;
   }
   _ELENA_::Breakpoint breakpoint(module, path, row, doc);
   _breakpoints.add(breakpoint);

   if (_debugController.isStarted()) {
      _debugController.toggleBreakpoint(breakpoint, true);
   }

   return true;
}

void IDEController :: toggleBreakpoint()
{
   int index = _view->getCurrentDocumentIndex();
   Document* doc = _model->getDocument(index);
   if (doc) {
      _ELENA_::Path path(_model->getDocumentPath(index));

      Point caret = doc->getCaret();

      _ELENA_::ReferenceNs module;
      _project.retrieveName(path, module);

      toggleBreakpoint(module, _ELENA_::IdentifierString(path), caret.y, doc);

      // forcee full repaint
      doc->status.frameChanged = true;
      _view->refresh();
   }
}

// --- Projects ---

const char* IDEController::ProjectManager :: getPackage()
{
   return _model->project.xmlConfig.getSetting(IDE_PACKAGE_XMLSETTING);
}

const char* IDEController::ProjectManager :: getTemplate()
{
   return _model->project.xmlConfig.getSetting(IDE_TEMPLATE_XMLSETTING);
}

const char* IDEController::ProjectManager::getOptions()
{
   return _model->project.xmlConfig.getSetting(IDE_COMPILER_XMLOPTIONS);
}

const char* IDEController::ProjectManager::getTarget()
{
   return _model->project.xmlConfig.getSetting(IDE_EXECUTABLE_XMLSETTING);
}

const char* IDEController::ProjectManager::getOutputPath()
{
   return _model->project.xmlConfig.getSetting(IDE_OUTPUT_XMLSETTING);
}

const char* IDEController::ProjectManager::getArguments()
{
   return _model->project.xmlConfig.getSetting(IDE_ARGUMENT_XMLSETTING);
}

int IDEController::ProjectManager::getDebugMode()
{
   _ELENA_::ident_t mode = _model->project.xmlConfig.getSetting(IDE_DEBUGINFO_XMLSETTING);
   if (mode.compare("-1")) {
      return -1;
   }
   else if (mode.compare("-2")) {
      return -2;
   }
   else return 0;
}

bool IDEController::ProjectManager::getBoolSetting(const char* name)
{
   _ELENA_::ident_t value = _model->project.xmlConfig.getSetting(name);

   return value.compare("-1");
}

void IDEController::ProjectManager::setSectionOption(const char* option, const char* value)
{
   if (!_ELENA_::emptystr(value)) {
      _model->project.xmlConfig.setSetting(option, value);
   }
   else _model->project.xmlConfig.setSetting(option, DEFAULT_STR);

   _model->project.changed = true;
}

void IDEController::ProjectManager::setTarget(const char* target)
{
   setSectionOption(IDE_EXECUTABLE_XMLSETTING, target);
}

void IDEController::ProjectManager::setArguments(const char* arguments)
{
   setSectionOption(IDE_ARGUMENT_XMLSETTING, arguments);
}

void IDEController::ProjectManager::setTemplate(const char* templateName)
{
   setSectionOption(IDE_TEMPLATE_XMLSETTING, templateName);
}

void IDEController::ProjectManager::setOutputPath(const char* path)
{
   setSectionOption(IDE_OUTPUT_XMLSETTING, path);
}

void IDEController::ProjectManager::setOptions(const char* options)
{
   setSectionOption(IDE_COMPILER_XMLOPTIONS, options);
}

void IDEController::ProjectManager::setPackage(const char* package)
{
   setSectionOption(IDE_PACKAGE_XMLSETTING, package);
}

void IDEController::ProjectManager::setDebugMode(int mode)
{
   if (mode != 0) {
      _model->project.xmlConfig.setSetting(IDE_DEBUGINFO_XMLSETTING, mode);
   }
   else _model->project.xmlConfig.setSetting(IDE_DEBUGINFO_XMLSETTING, NULL);

   _model->project.changed = true;
}

void IDEController::ProjectManager::setBoolSetting(const char* key, bool value)
{
   setSectionOption(key, value ? "-1" : "0");
}

void IDEController::ProjectManager :: reloadSources()
{
   _sources.clear();

   _ELENA_::Map<_ELENA_::ident_t, _ELENA_::_ConfigFile::Node> list;
   _model->project.xmlConfig.select(IDE_FILES_XMLSECTION, list);

   for (_ELENA_::_ConfigFile::Nodes::Iterator it = list.start(); !it.Eof(); it++) {
      _ELENA_::_ConfigFile::Nodes files;
      (*it).select(ELC_INCLUDE, files);

      for (_ELENA_::_ConfigFile::Nodes::Iterator file_it = files.start(); !file_it.Eof(); file_it++) {
         _sources.add((*file_it).Content());
      }
   }
}

void IDEController::ProjectManager :: reloadForwards()
{
   _forwards.clear();

   _ELENA_::Map<_ELENA_::ident_t, _ELENA_::_ConfigFile::Node> list;
   _model->project.xmlConfig.select(IDE_FORWARDS_XMLSECTION, list);

   for (_ELENA_::_ConfigFile::Nodes::Iterator it = list.start(); !it.Eof(); it++) {
      _ELENA_::ident_t key = (*it).Attribute("key");
      _ELENA_::ident_t content = (*it).Content();

      _forwards.add(key, content);
   }
}

void IDEController::ProjectManager :: refresh()
{
   const char* projectTemplate = getTemplate();

   // reload package root
   _ELENA_::path_t templatePath = _model->packageRoots.get(projectTemplate);
   if (_ELENA_::emptystr(templatePath))
      templatePath = _model->packageRoots.get("default");

   _model->paths.packageRoot.copy(templatePath);
   Paths::resolveRelativePath(_model->paths.packageRoot, _model->paths.appPath.c_str());

   // reload library root
   templatePath = _model->libraryRoots.get(projectTemplate);
   if (_ELENA_::emptystr(templatePath))
      templatePath = _model->libraryRoots.get("default");

   _model->paths.libraryRoot.copy(templatePath);
   Paths::resolveRelativePath(_model->paths.libraryRoot, _model->paths.appPath.c_str());

   // reload source list
   reloadSources();

   // reload forward list
   reloadForwards();
}

bool IDEController::ProjectManager :: open(_ELENA_::path_t path)
{
   _model->project.xmlConfig.clear();

   if (!_model->project.xmlConfig.load(path, _ELENA_::feUTF8)) {
      //_model->project.type = ctIni; // to reset default

      return false;
   }
      
   rename(path);
   refresh();

   _model->project.changed = false;
   return true;
}

void IDEController::ProjectManager::reset()
{
   _model->project.xmlConfig.clear();

   _model->project.name.clear();
   _model->project.path.clear();

   refresh();

   // should be the last to prevent being marked as changed
   _model->project.changed = false;
}

void IDEController::ProjectManager :: save(_ELENA_::path_t extension, bool withBOM)
{
   _ELENA_::Path cfgPath(_model->project.path);
   cfgPath.combine(_model->project.name.c_str());
   cfgPath.appendExtension(extension);

   _model->project.xmlConfig.save(cfgPath.c_str(), _ELENA_::feUTF8, withBOM);

   _model->project.changed = false;
}

void IDEController::ProjectManager::rename(_ELENA_::path_t path)
{
   _model->project.name.copyName(path);
   _model->project.path.copySubPath(path);
   _model->project.extension.copyExtension(path);

   Paths::resolveRelativePath(_model->project.path, _model->paths.defaultPath.c_str());

   _model->project.changed = true;
}

void IDEController::ProjectManager :: retrieveName(_ELENA_::Path& path, _ELENA_::ReferenceNs & name)
{
   _ELENA_::path_t root = _model->project.path.c_str();
   size_t rootLength = _ELENA_::getlength(root);

   _ELENA_::Path fullPath;
   fullPath.copySubPath(path.c_str());
   Paths::resolveRelativePath(fullPath, root);
   fullPath.lower();

   if (!_ELENA_::emptystr(root) && fullPath.str().compare(root, rootLength)) {
      name.copy(getPackage());
      if (_ELENA_::getlength(fullPath) > rootLength)
         name.pathToName(fullPath + rootLength + 1);

      path.copy(path + rootLength + 1);
   }
   else {
      root = _model->paths.packageRoot.str();
      rootLength = _ELENA_::getlength(root);

      if (!_ELENA_::emptystr(root) && _ELENA_::Path::comparePaths(fullPath.c_str(), root, rootLength)) {
         name.pathToName(fullPath + rootLength + 1);

         // skip the root path + root namespace
         size_t rootNs = path.str().find(rootLength + 1, PATH_SEPARATOR, -1);

         path.copy(path + rootNs + 1);
      }
      else {
         _ELENA_::FileName fileName(fullPath.c_str());

         name.copy(_ELENA_::IdentifierString(fileName));
      }
   }
}

bool IDEController::ProjectManager :: isIncluded(_ELENA_::path_t path)
{
   _ELENA_::Path current;
   _ELENA_::Path relPath(path);
   Paths::makeRelativePath(relPath, _model->project.path.c_str());

   _ProjectManager::SourceIterator it = SourceFiles();
   while (!it.Eof()) {
      current.copy((*it).c_str());

      if (relPath.str().compare(current, _ELENA_::getlength(current))) {
         return true;
      }
      it++;
   }
   return false;
}

void IDEController::ProjectManager :: includeSource(_ELENA_::path_t path)
{
   _ELENA_::Path relPath(path);
   Paths::makeRelativePath(relPath, _model->project.path.c_str());

   _ELENA_::IdentifierString str(relPath.c_str());
      
   _model->project.xmlConfig.appendSetting(IDE_SOURCE_ELEMENT, str.c_str());

   reloadSources();

   _model->project.changed = true;
}

void IDEController::ProjectManager::excludeSource(_ELENA_::path_t path)
{
   _ELENA_::Path relPath(path);
   Paths::makeRelativePath(relPath, _model->project.path.c_str());

   _ELENA_::IdentifierString str(relPath.c_str());
   _model->project.xmlConfig.removeSetting(IDE_SOURCE_ELEMENT, str.c_str());

   reloadSources();

   _model->project.changed = true;
}

void IDEController::ProjectManager :: clearForwards()
{
   _model->project.xmlConfig.setSetting(IDE_FORWARDS_ROOT, DEFAULT_STR);

   _model->project.changed = true;
}

void IDEController::ProjectManager::addForward(const char* name, const char* reference)
{
   _model->project.xmlConfig.appendSetting(IDE_FORWARDS_ELEMENT, name, reference);

   _model->project.changed = true;

   reloadForwards();
}
