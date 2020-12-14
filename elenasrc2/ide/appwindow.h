//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      IDE main window class header
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef appwindowH
#define appwindowH

#include "ide.h"
//#include "debugging.h"
#include "debugcontroller.h"

namespace _GUI_
{

typedef _ELENA_::List<_ELENA_::Breakpoint> Breakpoints;

// --- IDE ---

class IDEController : public _Controller
{
   class ProjectManager : public _ProjectManager
   {
      Model* _model;

      _ELENA_::List<_ELENA_::ident_t>  _sources;
      _ELENA_::Map<_ELENA_::ident_t, _ELENA_::ident_t>  _forwards;

      void reloadSources();

      void reloadForwards();

   public:
      virtual void retrievePath(_ELENA_::ident_t name, _ELENA_::Path & path, _ELENA_::path_t extension);

      virtual _ELENA_::path_t getAppPath() { return _model->paths.appPath.str(); }

      virtual const char* getPackage();
      virtual const char* getTemplate();
      virtual const char* getOptions();
      virtual const char* getTarget();
      virtual const char* getOutputPath();
      virtual const char* getArguments();

      virtual int getDebugMode();

      virtual bool getBoolSetting(const char* name);

      virtual SourceIterator SourceFiles()
      {
         return _sources.start();
      }

      virtual ForwardIterator Forwards()
      {
         return _forwards.start();
      }

      void setSectionOption(const char* option, const char* value);
      virtual void setBoolSetting(const char* key, bool value);

      virtual void setTarget(const char* target);
      virtual void setArguments(const char* target);
      virtual void setOutputPath(const char* path);
      virtual void setOptions(const char* options);
      virtual void setPackage(const char* package);
      virtual void setTemplate(const char* target);
      virtual void setDebugMode(int mode);

      bool open(_ELENA_::path_t path);
      void refresh();
      void reset();
      void save(_ELENA_::path_t extension, bool withBOM);

      void rename(_ELENA_::path_t path);

      void retrieveName(_ELENA_::Path& path, _ELENA_::ReferenceNs & name);

      bool isIncluded(_ELENA_::path_t path);
      void includeSource(_ELENA_::path_t path);
      void excludeSource(_ELENA_::path_t path);

      virtual void clearForwards();
      virtual void addForward(const char* name, const char* reference);

      void assign(Model* model)
      {
         _model = model;
      }

      ProjectManager()
      {
         _model = NULL;
      }
   };

   _ELENA_::List<_ELENA_::Breakpoint> _breakpoints;
   _ELENA_::DebugController           _debugController;

   _View*       _view;
   Model*       _model;

   SearchOption   _searchOption;
   ProjectManager _project;

   struct MarkerInfo
   {
      HighlightInfo  info;
      _ELENA_::pos_t style;
      _ELENA_::pos_t bandStyle;

      MarkerInfo()
      {
         this->bandStyle = -1;
      }
      MarkerInfo(HighlightInfo info, _ELENA_::pos_t bandStyle, _ELENA_::pos_t style)
      {
         this->info = info;
         this->style = style;
         this->bandStyle = bandStyle;
      }
   };

   class IDELexicalStyler : public LexicalStyler
   {
      friend class IDEController;

      Breakpoints*   _breakpoints;
      MarkerInfo     _current;

      bool           _bracketHighlighting;
      int            _openBracketPos, _closeBracketPos;

   public:
      bool checkMarker(MarkerInfo& marker, LexicalInfo& li, _ELENA_::pos_t& styleLen);

      virtual bool addMarker(HighlightInfo info, _ELENA_::pos_t bandStyle, _ELENA_::pos_t style)
      {
         if (bandStyle == STYLE_TRACE_LINE || bandStyle == STYLE_ERROR_LINE) {
            _current.info = info;
            _current.style = style;
            _current.bandStyle = bandStyle;

            return true;
         }
         // in case if bracket highlighting, HighlightInfo contains the absolute positions (col - begin, row - end)
         else if (bandStyle == STYLE_HIGHLIGHTED_BRACKET) {
            if (!_bracketHighlighting || _openBracketPos != info.col || _closeBracketPos != info.row) {
               _bracketHighlighting = true;
               _openBracketPos = info.col;
               _closeBracketPos = info.row;

               return true;
            }
         }
         return false;
      }

      virtual void removeMarker(int row, _ELENA_::pos_t bandStyle)
      {
         if (bandStyle == STYLE_HIGHLIGHTED_BRACKET) {
            _bracketHighlighting = false;
         }
         else _current.bandStyle = -1;
      }

      _ELENA_::pos_t proceed(_ELENA_::pos_t position, LexicalInfo& info);

      IDELexicalStyler(IDEController* ide, Text* text, _ELENA_::pos_t defaultStyle, text_c lookaheadState, text_c startState,
         text_c(*makeStep)(text_c ch, text_c state), _ELENA_::pos_t(*defineStyle)(text_c state, _ELENA_::pos_t style))
         : LexicalStyler(text, defaultStyle, lookaheadState, startState, makeStep, defineStyle)
      {
         _bracketHighlighting = false;
         _breakpoints = &ide->_breakpoints;
      }
   };

   bool toggleBreakpoint(_ELENA_::ident_t module, _ELENA_::ident_t path, int row, Document* doc);

   void setCaption(text_t projectName);

   void markDocumentAsIncluded(int index);
   void markDocumentAsExcluded(int index);
   void removeAllDocumentMarker(int style);
   void renameFileAs(int index, _ELENA_::path_t newPath, _ELENA_::path_t oldPath, bool included);
   void saveDocument(_ELENA_::path_t path, int index);

   bool openProject(_ELENA_::path_t path);
   bool openFile(_ELENA_::path_t path);
   bool closeFile(int index);
   bool closeAll(bool closeProject = true);
   void exit();
   void indent(Document* doc);
   void outdent(Document* doc);

   bool findText(SearchOption& option);
   bool replaceText(SearchOption& option);

   virtual void cleanUpProject();
   virtual void toggleBreakpoint();

   void runToCursor();

   virtual void doSetProjectSettings();
   virtual void doSetEditorSettings();
   virtual void doSetDebuggerSettings();
   virtual void doSetProjectForwards();
   virtual void doShowAbout();

   virtual void doCreateProject();
   virtual bool doCloseFile();
   virtual bool doCloseFile(int index);
   virtual void doOpenProject();
   virtual void doOpenProject(_ELENA_::path_t path);
   virtual void doCloseAll(bool closeProject);
   virtual void doCloseAllButActive();
   virtual bool doSave(bool saveAsMode);
   virtual bool doSaveProject(bool saveAsMode);
   virtual bool doSaveAll(bool forced);
   virtual void doOpenFile();
   virtual void doOpenFile(_ELENA_::path_t path);

   virtual void doUndo();
   virtual void doRedo();
   virtual bool doEditCopy();
   virtual void doEditPaste();
   virtual void doEditDelete();
   virtual void doIndent();
   virtual void doOutdent();
   virtual void doSelectAll();
   virtual void doTrim();
   virtual void doDuplicateLine();
   virtual void doEraseLine();
   virtual void doComment();
   virtual void doUnComment();
   virtual void doUpperCase();
   virtual void doLowerCase();
   virtual void doSwap();

   virtual void doFind();
   virtual void doFindNext();
   virtual void doReplace();
   virtual void doGoToLine();
   virtual void doSwitchTab(bool forward);
   virtual void doSelectWindow();
   virtual void doSelectWindow(int index);
   virtual void doSelectWindow(text_t path);

   virtual void doInclude();
   virtual void doExclude();

   void doCreateFile(text_t name, _GUI_::Document* doc);

   void doHighlightBrackets(Document* doc);
   bool doCloseProject();
   void doCreateFile();
   void doCreateTempFile(text_t name);
   bool doSave(int docIndex, bool saveAsMode);
   void doExit();

   bool doCompileProject(int postponedAction);
   virtual void doCompileProject()
   {
      if (!_ELENA_::test(_model->state, uiIDEBusy)) {
         doCompileProject(0);
      }
   }

   virtual void doShowCompilerOutput(bool checked, bool forced = false);
   virtual void doShowVMConsole(bool checked, bool forced = false);
   virtual void doShowProjectView(bool checked, bool forced = false);
   virtual void doShowMessages(bool checked, bool forced = false);
   virtual void doShowCallStack(bool visible, bool forced = false);
   virtual void doShowDebugWatch(bool visible);

   virtual void onFrameChange();
   virtual void onCursorChange();

   void onIDEInit();
   void onUIChange();
   virtual void onDocIncluded();
   void onChange();
   void onRowChange(int row);
   void onFileOpen();
   void onFileClose();
   void onProjectOpen();
   void onProjectClose();
   virtual bool onClose();

   void onCompilationStart();
   virtual void onCompilationEnd(text_t message, bool successful);
   virtual void onAutoCompilationEnd()
   {
      _model->state &= ~uiAutoRecompile;
   }
   bool onDebugAction(int action, bool stepMode);

   virtual void refreshDebuggerInfo()
   {
      if (_debugController.isStarted()) {
         _view->refreshDebugWindows(&_debugController);
      }
   }

   virtual void doStepOver()
   {
      if (!_ELENA_::test(_model->state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_STEPINTO, true)) {
            _debugController.stepOver();
         }
      }
   }
   virtual void doStepInto()
   {
      if (!_ELENA_::test(_model->state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_STEPOVER, true)) {
            _debugController.stepInto();
         }
      }
   }
   virtual void doDebugRunTo()
   {
      if (!_ELENA_::test(_model->state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_RUNTO, true)) {
            runToCursor();
         }
      }
   }
   virtual void doDebugStop()
   {
      _debugController.stop();
   }

   virtual void doDebugInspect()
   {
      _view->browseWatch(&_debugController);
   }

   virtual void onDebuggerStart();
   virtual void onDebuggerStep(text_t ns, text_t source, HighlightInfo info);
   virtual void onDebuggerCheckPoint(text_t message);
   virtual void onDebuggerStop(bool broken);
   virtual void onDebuggerAssemblyStep(text_t name, HighlightInfo info);

   void addDocumentMarker(int index, HighlightInfo info, int bandStyle, int style);
   virtual void highlightMessage(MessageBookmark* bookmark, int bandStyle);

   bool startDebugger(bool stepMode);
   bool isOutaged(bool noWarning);

   virtual void doDebugRun()
   {
      if (!_ELENA_::test(_model->state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_RUN, false)) {
            _debugController.run();
         }
      }
   }

   virtual void onDebuggerVMHook()
   {
      _debugController.loadBreakpoints(_breakpoints);
   }

   virtual void doBrowseWatch(void* node)
   {
      _view->browseWatch(&_debugController, node);
   }
   virtual void doBrowseWatch()
   {
      _view->browseWatch(&_debugController);
   }

   virtual void doDebugSwitchHexMode()
   {
      _model->hexNumberMode = !_model->hexNumberMode;
      _view->refreshDebugWindows(&_debugController);
   }

   virtual void doGotoSource()
   {
      if (_debugController.isStarted()) {
         _debugController.showCurrentModule();
         _view->showFrame();
      }
   }

   virtual void selectProjectFile(int index);

   bool loadModule(text_t ns, text_t source);
   bool loadTemporalModule(text_t name, int param);

   void nameProjectDocument(int index, _ELENA_::path_t path);

public:
   void start(_View* view, _DebugListener* listener, Model* model);

   virtual void clearBreakpoints();

   IDEController()
   {
      _view = NULL;
      _model = NULL;
   }
};

} // _GUI_

#endif // appwindowH
