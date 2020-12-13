//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE common
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef idecommonH
#define idecommonH

#include "guicommon.h"
#include "document.h"
#include "config.h"
#include "debugging.h"

#ifdef _WIN32

#include "eng\messages.h"
#include "winapi\winideconst.h"

#elif _LINUX32

#include "eng/messages.h"
#include "gtk-linux32/gtkideconst.h"

#endif

namespace _GUI_
{

// --- Project settings ---
#define ELC_INCLUDE                             "include"

#define IDE_TEMPLATE_XMLSETTING                 "configuration/project/template"
#define IDE_PACKAGE_XMLSETTING                  "configuration/project/namespace"
#define IDE_COMPILER_XMLOPTIONS                 "configuration/project/options"
#define IDE_EXECUTABLE_XMLSETTING               "configuration/project/executable"
#define IDE_OUTPUT_XMLSETTING                   "configuration/project/output"
#define IDE_ARGUMENT_XMLSETTING                 "configuration/project/arguments"
#define IDE_DEBUGINFO_XMLSETTING                "configuration/project/debuginfo"

#define IDE_FILES_XMLSECTION                    "configuration/files/*"
#define IDE_FORWARDS_XMLSECTION                 "configuration/forwards/*"

#define IDE_SOURCE_ELEMENT                      "configuration/files/module/include"

#define IDE_FORWARDS_ROOT                       "configuration/forwards"
#define IDE_FORWARDS_ELEMENT                    "configuration/forwards/forward"


// --- ELENA IDE Styles ---
#define SCHEME_COUNT                            2

#define STYLE_KEYWORD                           3
#define STYLE_COMMENT                           4
#define STYLE_OPERATOR                          5
#define STYLE_MESSAGE                           6
#define STYLE_NUMBER                            7
#define STYLE_STRING                            8
#define STYLE_HINT                              9  // !! not used
#define STYLE_ERROR_LINE                        10
#define STYLE_TRACE_LINE                        11
#define STYLE_TRACE                             12
#define STYLE_BREAKPOINT                        13
#define STYLE_HIGHLIGHTED_BRACKET               14
#define STYLE_MAX                               14

// --- UIState ---

enum IDEState
{
   uiEmpty = 0x00,
   uiFrameShown = 0x01,
   uiProjectActive = 0x02,
   uiIDEBusy = 0x04,
   uiDebugging = 0x08,
   uiAutoRecompile = 0x10,
   uiHighlight = 0x20,
   uiBracketBold = 0x40,
   uiProjectNotSaved = 0x80
};

// --- FrameState ---

enum FrameState
{
   editEmpty           = 0x00,
   editHasDocument     = 0x01,
   editHasSelection    = 0x02,
   editCanUndo         = 0x04,
   editCanRedo         = 0x08,
   editModifiedMode    = 0x10,
   editModeChanged     = 0x20,
   editOverwriteMode   = 0x40
};

// --- Model ---
typedef _ELENA_::String<text_c, 255> SearchText;
typedef _ELENA_::List<Document*> Documents;
typedef _ELENA_::Map<_ELENA_::path_t, int> DocMapping;
typedef _ELENA_::Map<const char*, _ELENA_::path_c*> PathMapping;
typedef _ELENA_::List<text_c*> SearchHistory;

struct SearchOption
{
   SearchText text;
   SearchText newText;
   bool       matchCase;
   bool       wholeWord;

   SearchOption()
   {
      matchCase = wholeWord = false;
   }
};

// --- MessageBookmark ---

struct MessageBookmark
{
   char*             module;
   _ELENA_::path_c*  file;
   int               col, row;

   MessageBookmark(_ELENA_::path_t file, text_str col, text_str row)
   {
      this->module = NULL;
      this->file = file.clone();
      this->col = col.toInt();
      this->row = row.toInt();
   }

   MessageBookmark(_ELENA_::ident_t module, _ELENA_::path_t file, int col, int row)
   {
      this->module = module.clone();
      this->file = file.clone();
      this->col = col;
      this->row = row;
   }

   ~MessageBookmark()
   {
      _ELENA_::freestr(module);
      _ELENA_::freestr(file);
   }
};

//enum ConfigType
//{
//   ctIni = 0,
//   ctXml = 1
//};

struct ProjectScope
{
   bool                   changed;

   _ELENA_::XmlConfigFile xmlConfig;

   _ELENA_::FileName      name;
   _ELENA_::FileName      extension;
   _ELENA_::Path          path;

   ProjectScope()
   {
      changed = false;
   }
};

struct PathScope
{
   _ELENA_::Path appPath;
   _ELENA_::Path defaultPath;
   _ELENA_::Path packageRoot;
   _ELENA_::Path libraryRoot;
   _ELENA_::Path lastPath;
};

// --- _DebugListener ---

class _DebugListener
{
public:
   virtual void onStop(bool failed) = 0;

   virtual void onStart() = 0;
   //virtual void onLoadModule(const wchar16_t* name, const wchar16_t* path) = 0;
   virtual void onLoadTape(_ELENA_::ident_t name, int row, int disp, int length) = 0;
   virtual void onStep(_ELENA_::ident_t ns, _ELENA_::ident_t source, int row, int disp, int length) = 0;
   virtual void onCheckPoint(text_t message) = 0;
   virtual void onNotification(text_t message, size_t address, int code) = 0;

   virtual void onDebuggerHook() = 0;
};

// --- _LibraryManager ---

class _ProjectManager
{
public:
   typedef _ELENA_::List<_ELENA_::ident_t>::Iterator SourceIterator;
   typedef _ELENA_::Map<_ELENA_::ident_t, _ELENA_::ident_t>::Iterator ForwardIterator;

   virtual _ELENA_::path_t getAppPath() = 0;

   virtual SourceIterator SourceFiles() = 0;

   virtual int getDebugMode() = 0;

   virtual void retrievePath(_ELENA_::ident_t name, _ELENA_::Path & path, _ELENA_::path_t extension) = 0;

   virtual const char* getPackage() = 0;
   virtual const char* getTemplate() = 0;
   virtual const char* getOptions() = 0;
   virtual const char* getTarget() = 0;
   virtual const char* getOutputPath() = 0;
   virtual const char* getArguments() = 0;

   virtual bool getBoolSetting(const char* name) = 0;
   virtual void setBoolSetting(const char* key, bool value) = 0;

   virtual void setTarget(const char* target) = 0;
   virtual void setArguments(const char* target) = 0;
   virtual void setOutputPath(const char* path) = 0;
   virtual void setOptions(const char* options) = 0;
   virtual void setPackage(const char* package) = 0;
   virtual void setTemplate(const char* target) = 0;
   virtual void setDebugMode(int mode) = 0;

   virtual ForwardIterator Forwards() = 0;
   virtual void clearForwards() = 0;
   virtual void addForward(const char* name, const char* reference) = 0;
};

class Model
{
public:
   PathScope    paths;
   ProjectScope project;

   _ELENA_::Path                   defaultProject;
   _ELENA_::List<_ELENA_::path_c*> defaultFiles;

   SearchHistory  searchHistory;
   SearchHistory  replaceHistory;

   PathMapping    packageRoots;
   PathMapping    libraryRoots;

   // documents
   int            unnamedIndex;
   Document*      currentDoc;
   Documents      documents;
   DocMapping     mappings;

   // layout
   int  state;
   int  previousState;
   bool appMaximized;
   bool tabWithAboveScore;
   bool compilerOutput;
   bool callStack;
   bool messages;
   bool projectView;
   bool vmConsole;

   // policy
   bool lastProjectRemember;
   bool lastPathRemember;
   bool autoRecompile;
   bool autoProjectLoad;
   bool saveWithBOM;

   // editor
   Point  lastCaret;
   int    defaultEncoding;
   bool   autoDetecting;
   int    tabSize;
   bool   tabCharUsing;
   bool   lineNumberVisible;
   bool   highlightSyntax;
   bool   highlightBrackets;
   bool   hexNumberMode;
   int    scheme;
   int    font_size;

   //   static bool debugTape;

   int getDocumentIndex(_ELENA_::path_t path)
   {
      return mappings.get(path);
   }

   _ELENA_::path_t getDocumentPath(int index)
   {
      return _ELENA_::retrieveKey(mappings.start(), index, DEFAULT_TEXT);
   }

   Document* getDocument(int index)
   {
      if (index == -1) {
         return currentDoc;
      }
      else return *documents.get(index);
   }

   bool isDocumentModified(int index)
   {
      Document* doc = getDocument(index);

      return doc ? doc->status.modifiedMode : false;
   }

   bool isAnyDocumentModified()
   {
      Documents::Iterator it = documents.start();
      while (!it.Eof()) {
         if ((*it)->status.modifiedMode)
            return true;

         it++;
      }
      return false;
   }

   void setReadOnlyMode(bool mode)
   {
      Documents::Iterator it = documents.start();
      while (!it.Eof()) {
         (*it)->status.readOnly = mode;

         it++;
      }
   }

   bool isDocumentUnnamed(int index)
   {
      Document* doc = getDocument(index);

      return doc ? doc->status.unnamed : false;
   }

   void removeDocument(int index)
   {
      documents.cut(documents.get(index));

      _ELENA_::path_t path = _ELENA_::retrieveKey(mappings.start(), index, (_ELENA_::path_t)NULL);
      mappings.erase(path);

      DocMapping::Iterator it = mappings.start();
      int i = 0;
      while (!it.Eof()) {
         *it = i;
         it++;
         i++;
      }
   }

   bool isDocumentReadOnly() const
   {
      return (currentDoc != NULL) && currentDoc->status.readOnly;
   }

   bool isProjectUnnamed() const
   {
      return project.name.isEmpty();
   }

   bool isProjectChanged() const
   {
      return project.changed;
   }

   FrameState getFrameState()
   {
      int states = editEmpty;

      if (currentDoc != NULL) {
         states = editHasDocument;

         // check document statesqwe
         if (currentDoc->hasSelection())
            states |= editHasSelection;
         if (currentDoc->canUndo())
            states |= editCanUndo;
         if (currentDoc->canRedo())
            states |= editCanRedo;
         if (currentDoc->status.modifiedMode)
            states |= editModifiedMode;
         if (currentDoc->status.isModeChanged())
            states |= editModeChanged;
         if (currentDoc->status.overwriteMode)
            states |= editOverwriteMode;
      }
      return (FrameState)states;
   }

   Model()
      : documents(NULL, _ELENA_::freeobj), mappings(-1), defaultFiles((_ELENA_::path_c*)NULL, _ELENA_::freestr),
         packageRoots((_ELENA_::path_c*)NULL, _ELENA_::freestr), libraryRoots((_ELENA_::path_c*)NULL, _ELENA_::freestr),
         searchHistory((text_c*)NULL, _ELENA_::freestr), replaceHistory((text_c*)NULL, _ELENA_::freestr)
   {
      state = uiEmpty;
      previousState = 0xFFFFFFFF;

      currentDoc = NULL;
      unnamedIndex = 0;

      appMaximized = true;
      autoDetecting = true;
      tabSize = 4;
      tabCharUsing = false;
      lineNumberVisible = true;
      highlightSyntax = true;
      highlightBrackets = true;
      defaultEncoding = _ELENA_::feUTF8;
      scheme = 0;
      font_size = 10;

      lastCaret.x = -1;
      lastCaret.y = -1;

      lastProjectRemember = true;
      lastPathRemember = true;
      autoProjectLoad = false;
      saveWithBOM = false;

      projectView = true;
      compilerOutput = true;
      callStack = true;
      messages = true;
      tabWithAboveScore = true;
      autoRecompile = true;
//      //debugTape = false;
      hexNumberMode = true;
//      //testMode = false;
      vmConsole = false;
   }
};

// --- Controller Interface ---
class _Controller
{
public:
   virtual void onDocIncluded() = 0;
   virtual void onCursorChange() = 0;
   virtual void onFrameChange() = 0;
   virtual bool onClose() = 0;
   virtual void onCompilationEnd(text_t message, bool successful) = 0;
   virtual void onAutoCompilationEnd() = 0;
   virtual void onDebuggerStart() = 0;
   virtual void onDebuggerStep(text_t ns, text_t source, HighlightInfo info) = 0;
   virtual void onDebuggerAssemblyStep(text_t name, HighlightInfo info) = 0;
   virtual void onDebuggerCheckPoint(text_t message) = 0;
   virtual void onDebuggerStop(bool broken) = 0;
   virtual void doDebugInspect() = 0;

   virtual void doCreateProject() = 0;
   virtual void doCreateFile() = 0;
   virtual void doOpenProject() = 0;
   virtual void doOpenProject(_ELENA_::path_t path) = 0;
   virtual void doOpenFile() = 0;
   virtual void doOpenFile(_ELENA_::path_t path) = 0;
   virtual bool doSave(bool saveAsMode) = 0;
   virtual bool doSaveProject(bool saveAsMode) = 0;
   virtual bool doSaveAll(bool forced) = 0;
   virtual bool doCloseFile() = 0;
   virtual bool doCloseFile(int index) = 0;
   virtual void doCloseAll(bool closeProject) = 0;
   virtual void doCloseAllButActive() = 0;
   virtual void doSwitchTab(bool forward) = 0;
   virtual void doSelectWindow() = 0;
   virtual void doSelectWindow(int index) = 0;
   virtual void doSelectWindow(text_t path) = 0;

   virtual void doUndo() = 0;
   virtual void doRedo() = 0;
   virtual bool doEditCopy() = 0;
   virtual void doEditPaste() = 0;
   virtual void doEditDelete() = 0;
   virtual void doIndent() = 0;
   virtual void doOutdent() = 0;
   virtual void doSelectAll() = 0;
   virtual void doTrim() = 0;
   virtual void doDuplicateLine() = 0;
   virtual void doEraseLine() = 0;
   virtual void doComment() = 0;
   virtual void doUnComment() = 0;
   virtual void doUpperCase() = 0;
   virtual void doLowerCase() = 0;
   virtual void doSwap() = 0;

   virtual void doFind() = 0;
   virtual void doFindNext() = 0;
   virtual void doReplace() = 0;
   virtual void doGoToLine() = 0;

   virtual void doInclude() = 0;
   virtual void doExclude() = 0;

   virtual void doSetEditorSettings() = 0;
   virtual void doSetProjectSettings() = 0;
   virtual void doSetDebuggerSettings() = 0;
   virtual void doSetProjectForwards() = 0;
   virtual void doShowAbout() = 0;

   virtual void doExit() = 0;

   virtual void cleanUpProject() = 0;
   virtual void doCompileProject() = 0;
   virtual void doStepOver() = 0;
   virtual void doStepInto() = 0;
   virtual void doDebugStop() = 0;
   virtual void doDebugRunTo() = 0;

   virtual void doShowCompilerOutput(bool checked, bool forced = false) = 0;
   virtual void doShowVMConsole(bool checked, bool forced = false) = 0;
   virtual void doShowProjectView(bool checked, bool forced = false) = 0;
   virtual void doShowMessages(bool checked, bool forced = false) = 0;
   virtual void doShowDebugWatch(bool visible) = 0;
   virtual void doShowCallStack(bool checked, bool forced = false) = 0;
   virtual void doBrowseWatch(void* node) = 0;
   virtual void doBrowseWatch() = 0;
   virtual void doDebugSwitchHexMode() = 0;
   virtual void doGotoSource() = 0;

   virtual void highlightMessage(MessageBookmark* bookmark, int bandStyle) = 0;
   virtual void selectProjectFile(int index) = 0;
   virtual void refreshDebuggerInfo() = 0;

   virtual void doDebugRun() = 0;
   virtual void onDebuggerVMHook() = 0;

   virtual void toggleBreakpoint() = 0;
   virtual void clearBreakpoints() = 0;
};

// --- View ---
class _View
{
public:
   enum Answer
   {
      Yes, No, Cancel
   };

   virtual void start(bool maximized) = 0;
   virtual void exit() = 0;
   virtual void refresh(bool onlyFrame = true) = 0;
   virtual void reloadSettings() = 0;

   virtual bool configProject(_ProjectManager* project) = 0;
   virtual bool configEditor(Model* model) = 0;
   virtual bool configDebugger(Model* model) = 0;
   virtual bool configurateForwards(_ProjectManager* project) = 0;
   virtual bool about(Model* model) = 0;

   virtual bool saveProject(Model* model, _ELENA_::Path& path) = 0;
   virtual bool selectProject(Model* model, _ELENA_::Path& path) = 0;
   virtual bool saveFile(Model* model, _ELENA_::Path& newPath) = 0;
   virtual bool selectFiles(Model* model, _ELENA_::List<text_c*>& selected) = 0;
   virtual bool find(Model* model, SearchOption* option, SearchHistory* searchHistory) = 0;
   virtual bool replace(Model* model, SearchOption* option, SearchHistory* searchHistory, SearchHistory* replaceHistory) = 0;
   virtual bool gotoLine(int& row) = 0;
   virtual bool selectWindow(Model* model, _Controller* controller) = 0;

   virtual bool copyToClipboard(Document* doc) = 0;
   virtual void pasteFromClipboard(Document* doc) = 0;

   virtual void error(text_t message) = 0;
   virtual void error(text_t message, text_t param) = 0;
   virtual bool confirm(text_t message) = 0;
   virtual bool confirm(text_t message, text_t param1, text_t param2) = 0;
   virtual Answer question(text_t message, text_t param) = 0;
   virtual Answer question(text_t message) = 0;

   virtual int newDocument(text_t name, Document* doc) = 0;
   virtual int getCurrentDocumentIndex() = 0;
   virtual void renameDocument(int index, text_t name) = 0;
   virtual void selectDocument(int index) = 0;
   virtual void closeDocument(int index) = 0;

   virtual void setCaption(text_t caption) = 0;
   virtual void activateFrame() = 0;
   virtual void showFrame() = 0;
   virtual void showStatus(int index, text_t message) = 0;
   virtual void hideFrame() = 0;

   virtual void enableMenuItemById(int id, bool doEnable, bool toolBarItemAvailable) = 0;
   virtual void checkMenuItemById(int id, bool doEnable) = 0;

   virtual void markDocumentTitle(int docIndex, bool changed) = 0;

   virtual void addToWindowList(text_t path) = 0;
   virtual void removeFromWindowList(text_t path) = 0;
   virtual void addToRecentFileList(text_t path) = 0;
   virtual void addToRecentProjectList(text_t path) = 0;

   virtual void removeFile(_ELENA_::path_t name) = 0;

   virtual void openOutput() = 0;
   virtual void closeOutput() = 0;
   virtual void switchToOutput() = 0;

   virtual void openVMConsole(_ProjectManager* project) = 0;
   virtual void closeVMConsole() = 0;

   virtual void openMessageList() = 0;
   virtual void clearMessageList() = 0;
   virtual void closeMessageList() = 0;

   virtual void openDebugWatch() = 0;
   virtual void closeDebugWatch() = 0;

   virtual void openProjectView() = 0;
   virtual void closeProjectView() = 0;

   virtual void openCallList() = 0;
   virtual void closeCallList() = 0;

   virtual bool compileProject(_ProjectManager* project, int postponedAction) = 0;
//   virtual void resetDebugWindows() = 0;
   virtual void refreshDebugWindows(_ELENA_::_DebugController* debugController) = 0;
   virtual void browseWatch(_ELENA_::_DebugController* debugController, void* node) = 0;
   virtual void browseWatch(_ELENA_::_DebugController* debugController) = 0;

   virtual void reloadProjectView(_ProjectManager* project) = 0;

   virtual ~_View() {}
};

} // _GUI_

#endif // idecommonH
