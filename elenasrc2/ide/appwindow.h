//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      IDE main window class header
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef appwindowH
#define appwindowH

#include "ide.h"
#include "historylist.h"
#include "windowlist.h"
#include "messagelog.h"
#include "callstack.h"

#ifdef _WIN32

#include "debugcontroller.h"
#include "browser.h"
#include "winapi32\windialogs.h "

#elif _LINUX32

#include "gtk-linux32/gtkdialogs.h"

#endif

namespace _GUI_
{

#define TABCHANGED_NOTIFY  1
#define VIEWCHANGED_NOTIFY 2

// --- AppDebugController ---

#ifdef _WIN32
class AppDebugController : public _ELENA_::DebugController
{
public:
   virtual _ELENA_::_Module* loadDebugModule(const wchar16_t* reference);
};
#else
#define AppDebugController void*
#endif

// --- IDE ---

#ifdef _WIN32
typedef _ELENA_::List<_ELENA_::Breakpoint> Breakpoints;
#endif

class IDE
{
protected:
   struct MarkerInfo
   {
      HighlightInfo info;
      int           style;
      int           bandStyle;

      MarkerInfo()
      {
         this->bandStyle = -1;
      }
      MarkerInfo(HighlightInfo info, int bandStyle, int style)
      {
         this->info = info;
         this->style = style;
         this->bandStyle = bandStyle;
      }
   };

   class IDELexicalStyler : public LexicalStyler
   {
      friend class IDE;

#ifdef _WIN32
      Breakpoints*   _breakpoints;
#endif
      MarkerInfo     _current;

      bool           _bracketHighlighting;
      size_t         _openBracketPos, _closeBracketPos;

   public:
      bool checkMarker(MarkerInfo& marker, LexicalInfo& li, size_t& styleLen);

      virtual bool addMarker(HighlightInfo info, int bandStyle, int style)
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

      virtual void removeMarker(int row, int bandStyle)
      {
         if (bandStyle == STYLE_HIGHLIGHTED_BRACKET) {
            _bracketHighlighting = false;
         }
         else _current.bandStyle = -1;
      }

      size_t proceed(size_t position, LexicalInfo& info);

      IDELexicalStyler(IDE* ide, Text* text, int defaultStyle, tchar_t lookaheadState, tchar_t startState,
                        tchar_t(* makeStep)(tchar_t ch, tchar_t state), size_t(* defineStyle)(tchar_t state, size_t style))
         : LexicalStyler(text, defaultStyle, lookaheadState, startState, makeStep, defineStyle)
      {
         _bracketHighlighting = false;
#ifdef _WIN32
         _breakpoints = &ide->_breakpoints;
#endif
      }
   };

   class IDEWindowsDialog : public WindowsDialog
   {
   protected:
      IDE* _ide;

      virtual void onCreate();
      virtual void onOK();
      virtual void onClose();

   public:
      IDEWindowsDialog(IDE* ide);
   };

   friend class IDEWindowList;
   friend class IDELexicalStyler;

   Clipboard  _clipboard;

   int        _unnamedIndex;
   int        _state;
   int        _previousState;

   Point      _lastCaret;

   bool       _highlightBrackets;

   SearchOption _searchOption;

   Menu*           _appMenu;
   WindowList      _windowList;
   RecentList      _recentFiles;
   RecentList      _recentProjects;

   SDIWindow*      _appWindow;
   ToolBar*        _appToolBar;
   StatusBar*      _statusBar;
   EditFrame*      _mainFrame;
   TabBar*         _outputBar;
   MessageLog*     _messageList;
   CallStackLog*   _callStackList;
   Control*        _output;
#ifdef _WIN32
   ContextBrowser* _contextBrowser;
#endif

#ifdef _WIN32
   _ELENA_::List<_ELENA_::Breakpoint> _breakpoints;

   AppDebugController* _debugController;
#endif

   virtual void onIDEInit();

   void onCursorChange();
   void onFrameChange(FrameState state);
   void onFrameChange()
   {
      onFrameChange(_mainFrame->getState());
   }

   void onUIChange(int state);
   void onChange();
   void onDocIncluded();

   void onFileOpen();
   void onFileClose();
   void onProjectOpen();
   virtual void onProjectClose();

   bool onDebugAction(int action, bool stepMode);

   void onRowChange(Document* doc, int row);

   void exit();

   bool openFile(const tchar_t* path);
   void renameFileAs(int index, const tchar_t* newPath, const tchar_t* oldPath, bool included);
   bool openProject(const tchar_t* path);
   bool closeFile(int index);
   bool closeAll(bool closeProject = true);

   void setCaption(const tchar_t* projectName);

   void showProjectSettings();
   void showProjectForwards();

   bool startDebugger(bool stepMode);
   bool isOutaged(bool noWarning);
   void runToCursor();
   void refreshDebugStatus();

   bool toggleBreakpoint(const tchar_t* module, const tchar_t* path, size_t row, Document* doc);

   void doCreateFile();
   void doCreateTempFile(const tchar_t* name);
   void doOpenFile();
   bool doSave(bool saveAsMode);
   bool doSave(int docIndex, bool saveAsMode);
   bool doCloseFile();

   void doCreateProject();
   void doOpenProject();
   bool doSaveProject(bool saveAsMode);
   bool doCloseProject();

   bool doSaveAll(bool forced);
   void doCloseAllButActive();
   void doSelectFile(int optionID);
   void doSelectProject(int optionID);
   void doSelectWindow(int optionID);
   void doClearFileHistory()
   {
      _recentFiles.clear();
   }
   void doClearProjectHistory()
   {
      _recentProjects.clear();
      Settings::defaultProject.clear();
   }
   void doExit()
   {
      exit();
      onChange();
   }

   void doUndo();
   void doRedo();
   bool doEditCopy();
   void doEditPaste();
   void doEditDelete();
   void doIndent()
   {
      _mainFrame->indent();
   }
   void doOutdent()
   {
      _mainFrame->outdent();
   }
   void doSelectAll()
   {
      _mainFrame->selectAll();
      onFrameChange();
   }
   void doTrim()
   {
      _mainFrame->trim();
      onFrameChange();
   }
   void doDuplicateLine()
   {
      _mainFrame->duplicateLine();
   }
   void doEraseLine()
   {
      _mainFrame->eraseLine();
   }
   void doComment()
   {
      _mainFrame->commentBlock();
   }
   void doUnComment()
   {
      _mainFrame->uncommentBlock();
   }
   void doUpperCase()
   {
      _mainFrame->toUppercase();
   }
   void doLowerCase()
   {
      _mainFrame->toLowercase();
   }
   void doSwap()
   {
      _mainFrame->swap();
   }
   void doShowDebugWatch()
   {
#ifdef _WIN32
      doShowDebugWatch(!_contextBrowser->isVisible());
#endif
   }

   void doSelectWindow();
   void doSwitchTab(bool forward);
   void doSetEditorSettings();
   void doSetDebuggerSettings();
   void doFind();
   void doFindNext();
   void doReplace();
   void doShowAbout();
   void doGoToLine();

   void doShowCompilerOutput(bool checked, bool forced = false);
   void doShowMessages(bool checked, bool forced = false);
   void doShowDebugWatch(bool visible);
   void doShowCallStack(bool visible, bool forced = false);

   bool doCompileProject(int postponedAction);
   void doCompileProject()
   {
      if (!_ELENA_::test(_state, uiIDEBusy)) {
         doCompileProject(0);
      }
   }

   void doDebugRun()
   {
#ifdef _WIN32
      if (!_ELENA_::test(_state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_RUN, false)) {
            _debugController->run();
         }
      }
#endif
   }
   void doDebugRunTo()
   {
      if (!_ELENA_::test(_state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_RUNTO, true)) {
            runToCursor();
         }
      }
   }

   void doNextStatement()
   {
#ifdef _WIN32
      if (!_ELENA_::test(_state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_STEPINTO, true)) {
            _debugController->stepOverLine();
         }
      }
#endif
   }
   void doStepOver()
   {
#ifdef _WIN32
      if (!_ELENA_::test(_state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_STEPINTO, true)) {
            _debugController->stepOver();
         }
      }
#endif
   }
   void doStepInto()
   {
#ifdef _WIN32
      if (!_ELENA_::test(_state, uiIDEBusy)) {
         if (onDebugAction(IDM_DEBUG_STEPOVER, true)) {
            _debugController->stepInto();
         }
      }
#endif
   }

   void doDebugStop()
   {
#ifdef _WIN32
      _debugController->stop();
#endif
   }

   void doDebugInspect()
   {
#ifdef _WIN32
      _contextBrowser->browse(_debugController);
#endif
   }

   void doDebugSwitchHexMode()
   {
#ifdef _WIN32
      Settings::hexNumberMode = !Settings::hexNumberMode;
      _contextBrowser->refresh(_debugController);
#endif
   }

   void doGotoSource()
   {
#ifdef _WIN32
      _debugController->showCurrentModule();
      _mainFrame->setFocus();
#endif
   }

   void doHighlightBrackets(Document* doc);

   void doInclude();
   void doExclude();

   virtual bool compileProject(int postponedAction) = 0;

   virtual void openHelp() = 0;

   //bool highlightMessage(HighlightInfo hi, int bandStyle, int style);

public:
   virtual void onCompilationStart();
   virtual void onCompilationEnd(const tchar_t* message, bool successful);
   virtual void onAutoCompilationEnd()
   {
      _state &= ~uiAutoRecompile;
   }

   virtual void onDebuggerStart();
   virtual void onDebuggerStep(const wchar16_t* ns, const tchar_t* source, HighlightInfo info);
   virtual void onDebuggerCheckPoint(const tchar_t* message);
   virtual void onDebuggerStop(bool broken);

   virtual bool onClose();

   void start();

   void loadRecentFiles(_ELENA_::IniConfigFile& file)
   {
      _recentFiles.load(file, RECENTFILES_SECTION);
      _recentFiles.refresh();
   }
   void loadRecentProjects(_ELENA_::IniConfigFile& file)
   {
      _recentProjects.load(file, RECENTRPOJECTS_SECTION);
      _recentProjects.refresh();
   }
   bool loadModule(const tchar_t* name, const tchar_t* sourcePath);
   bool loadTemporalModule(const tchar_t* name, int param);

   void saveRecentFiles(_ELENA_::IniConfigFile& file)
   {
      _recentFiles.save(file, RECENTFILES_SECTION);
   }
   void saveRecentProjects(_ELENA_::IniConfigFile& file)
   {
      _recentProjects.save(file, RECENTRPOJECTS_SECTION);
   }

   void highlightMessage(MessageBookmark* bookmark, int bandStyle);
   void toggleBreakpoint();
   void clearBreakpoints();

   IDE(AppDebugController* debugController);
};

} // _GUI_

#endif // appwindowH
