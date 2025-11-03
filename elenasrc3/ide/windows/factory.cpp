//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"

#include "windows/winide.h"
#include "windows/wintextview.h"
#include "windows/wintextframe.h"
#include "windows/winidestatusbar.h"
#include "windows/winsplitter.h"
#include "windows/winoutput.h"
#include "windows/winmessagelog.h"
#include "windows/wintreeview.h"
#include "windows/wincontextbrowser.h"
#include "windows/winmenu.h"
#include "windows/wintoolbar.h"

#include "Resource.h"

#include <shlwapi.h>
#include <tchar.h>

using namespace elena_lang;

#ifdef _M_IX86

#define CLI_PATH     "elena-cli.exe"
#define ELT_CLI_PATH "elt-cli.exe"

#else

#define CLI_PATH     "elena64-cli.exe"
#define ELT_CLI_PATH "elt64-cli.exe"

#endif // DEBUG

#define MAX_LOADSTRING 100

// Global Variables:
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szSDI[MAX_LOADSTRING];                    // the main window class name
WCHAR szTextView[MAX_LOADSTRING];               // the main window class name
WCHAR szHSplitter[MAX_LOADSTRING];              // the hsplitter class name
WCHAR szVSplitter[MAX_LOADSTRING];              // the vsplitter window class name
WCHAR szCompilerOutput[MAX_LOADSTRING];         // the compiler output caption
WCHAR szErrorList[MAX_LOADSTRING];              // the compiler output caption
WCHAR szWatch[MAX_LOADSTRING];                  // the debug auto watch caption
WCHAR szVMOutput[MAX_LOADSTRING];               // the vm terminal output caption

#define CONTEXT_MENU_INSPECT                    _T("Inspect\tCtrl+I")
#define CONTEXT_MENU_SHOWHEX                    _T("Show as hexadecimal")

#define CONTEXT_MENU_SHOWHEX                    _T("Show as hexadecimal")
#define CONTEXT_MENU_CLOSE                      _T("Close\tCtrl+W")
#define CONTEXT_MENU_CUT                        _T("Cut\tCtrl+X")
#define CONTEXT_MENU_COPY                       _T("Copy\tCtrl+C")
#define CONTEXT_MENU_PASTE                      _T("Paste\tCtrl+V")
#define CONTEXT_MENU_TOGGLE                     _T("Toggle Breakpoint\tF5")
#define CONTEXT_MENU_RUNTO                      _T("Run to Cursor\tF4")
#define CONTEXT_MENU_INSPECT                    _T("Inspect\tCtrl+I")

// !! temporally
#define IDE_CHARSET_ANSI                        ANSI_CHARSET
#define IDE_CHARSET_DEFAULT                     1

// --- Styles ---
StyleInfo defaultStyles[STYLE_MAX + 1] = {
   {Color(0), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0), Color(Canvas::Chrome()), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0), Color(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0x60, 0x60, 0x60), Color(0x0, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0xFF, 0x0, 0x0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0xFF, 0x0, 0x0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0, 0, 0xFF), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0x40, 0x80, 0x80), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0, 0x80, 0), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0x80, 0x40), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0, 0x80, 0x80), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
};

StyleInfo classicStyles[STYLE_MAX + 1] = {
   {Color(0xFF, 0xFF, 0), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(Canvas::Chrome()), Color(0, 0, 68), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0x60, 0x60, 0x60), Color(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0x60, 0x60, 0x60), Color(0x0, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0xFF, 0x0, 0x0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xC0, 0xC0, 0xC0), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xC0, 0xC0, 0xC0), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0, 0xFF, 0x80), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0, 0xFF, 0xFF), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0xFF, 0), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
};

StyleInfo darkStyles[STYLE_MAX + 1] = {
   {Color(0xFF, 0xFF, 0xFF), Color(50, 50, 50), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(164, 164, 164), Color(64, 64, 64), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0x60, 0x60, 0x60), Color(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0xEF, 0xEF, 0xEF), Color(64, 128, 128), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0xFF, 0x0, 0x0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0x80, 0x0, 0x0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(84, 255, 209), Color(50, 50, 50), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(160, 160, 160), Color(50, 50, 50), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(87, 166, 74), Color(50, 50, 50), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(181, 230, 168), Color(50, 50, 50), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(214, 157, 133), Color(50, 50, 50), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0x27, 0x2D, 0x60), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
};

constexpr auto STYLE_SCHEME_COUNT = 3;

MenuInfo browserContextMenuInfo[3] = {
      {IDM_DEBUG_INSPECT, CONTEXT_MENU_INSPECT},
      {0, nullptr},
      {IDM_DEBUG_SWITCHHEXVIEW, CONTEXT_MENU_SHOWHEX}
};

MenuInfo contextMenuInfo[8] = {
   {IDM_FILE_CLOSE, CONTEXT_MENU_CLOSE},
   {0, NULL},
   {IDM_EDIT_CUT, CONTEXT_MENU_CUT},
   {IDM_EDIT_COPY, CONTEXT_MENU_COPY},
   {IDM_EDIT_PASTE, CONTEXT_MENU_PASTE},
   {0, NULL},
   {IDM_DEBUG_BREAKPOINT, CONTEXT_MENU_TOGGLE},
   {IDM_DEBUG_RUNTO, CONTEXT_MENU_RUNTO},
};

size_t AppToolBarButtonNumber = 19;

ToolBarButton AppToolBarButtons[] =
{
   {IDM_FILE_NEW, IDR_FILENEW},
   {IDM_FILE_OPEN, IDR_FILEOPEN},
   {IDM_FILE_SAVE, IDR_FILESAVE},
   {IDM_FILE_SAVEALL, IDR_SAVEALL},
   {IDM_FILE_CLOSE, IDR_CLOSEFILE},
   {IDM_PROJECT_CLOSE, IDR_CLOSEALL},
   {0, IDR_SEPARATOR},
   {IDM_EDIT_CUT, IDR_CUT},
   {IDM_EDIT_COPY, IDR_COPY},
   {IDM_EDIT_PASTE, IDR_PASTE},
   {0, IDR_SEPARATOR},
   {IDM_EDIT_UNDO, IDR_UNDO},
   {IDM_EDIT_REDO, IDR_REDO},
   {0, IDR_SEPARATOR},
   {IDM_DEBUG_RUN, IDR_RUN},
   {IDM_DEBUG_STEPINTO, IDR_STEPINTO},
   {IDM_DEBUG_STEPOVER, IDR_STEPOVER},
   {IDM_DEBUG_STOP, IDR_STOP},
   {IDM_DEBUG_GOTOSOURCE, IDR_GOTO},
};

ToolBarButton AppToolBarButtonsLarge[] =
{
   {IDM_FILE_NEW, IDR_FILENEW_L},
   {IDM_FILE_OPEN, IDR_FILEOPEN_L},
   {IDM_FILE_SAVE, IDR_FILESAVE_L},
   {IDM_FILE_SAVEALL, IDR_SAVEALL_L},
   {IDM_FILE_CLOSE, IDR_CLOSEFILE_L},
   {IDM_PROJECT_CLOSE, IDR_CLOSEALL_L},
   {0, IDR_SEPARATOR},
   {IDM_EDIT_CUT, IDR_CUT_L},
   {IDM_EDIT_COPY, IDR_COPY_L},
   {IDM_EDIT_PASTE, IDR_PASTE_L},
   {0, IDR_SEPARATOR},
   {IDM_EDIT_UNDO, IDR_UNDO_L},
   {IDM_EDIT_REDO, IDR_REDO_L},
   {0, IDR_SEPARATOR},
   {IDM_DEBUG_RUN, IDR_RUN_L},
   {IDM_DEBUG_STEPINTO, IDR_STEPINTO_L},
   {IDM_DEBUG_STEPOVER, IDR_STEPOVER_L},
   {IDM_DEBUG_STOP, IDR_STOP_L},
   {IDM_DEBUG_GOTOSOURCE, IDR_GOTO_L},
};

inline void canonicalize(PathString& path)
{
   wchar_t p[MAX_PATH];

   ::PathCanonicalize(p, path.str());

   path.copy(p);
}

// --- IDEFactory ---

PathSettings IDEFactory::_pathSettings;

IDEFactory :: IDEFactory(HINSTANCE instance, IDEModel* ideModel, 
   IDEController* controller,
   GUISettinngs   settings)
{
   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;
   _schemes[2] = darkStyles;
   _settings = settings;

   _instance = instance;
   _model = ideModel;
   _controller = controller;

   _model->projectModel.paths.compilerPath.copy(CLI_PATH);
   _model->projectModel.paths.vmTerminalPath.copy(ELT_CLI_PATH);

   _model->projectModel.paths.libraryRoot.copy(*_model->projectModel.paths.appPath);
#ifdef _M_IX86   
   _model->projectModel.paths.libraryRoot.combine("..\\lib60\\");      // !! temporal
#else
   _model->projectModel.paths.libraryRoot.combine("..\\lib60_64\\");      // !! temporal
#endif

   _model->projectModel.paths.librarySourceRoot.copy(*_model->projectModel.paths.appPath);
   _model->projectModel.paths.librarySourceRoot.combine("..\\src60\\");// !! temporal

   canonicalize(_model->projectModel.paths.librarySourceRoot);
   canonicalize(_model->projectModel.paths.libraryRoot);
}

void IDEFactory :: initPathSettings(IDEModel* ideModel)
{
   wchar_t appPath[MAX_PATH];
   ::GetModuleFileName(NULL, appPath, MAX_PATH);
   ::PathRemoveFileSpec(appPath);

   _pathSettings.appPath.copy(appPath);

   ideModel->projectModel.paths.appPath.copy(*_pathSettings.appPath);
}

void IDEFactory :: registerClasses()
{
   // Initialize global strings
   LoadStringW(_instance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_IDE, szSDI, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_TEXTVIEW, szTextView, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_HSPLITTER, szHSplitter, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_VSPLITTER, szVSplitter, MAX_LOADSTRING);

   SDIWindow::registerSDIWindow(_instance, szSDI, LoadIcon(_instance, MAKEINTRESOURCE(IDI_IDE)), MAKEINTRESOURCEW(IDC_IDE), LoadIcon(_instance, MAKEINTRESOURCE(IDI_SMALL)));
   TextViewWindow::registerTextViewWindow(_instance, szTextView);
   Splitter::registerSplitterWindow(_instance, szHSplitter, false);
   Splitter::registerSplitterWindow(_instance, szVSplitter, true);
}

ControlPair IDEFactory :: createTextControl(WindowBase* owner, NotifierBase* notifier)
{
   auto viewModel = _model->viewModel();

   // initialize view styles
   reloadStyles(viewModel);

   // initialize UI components
   TextViewWindow* view = new TextViewWindow(notifier, _model->viewModel(), 
      &_controller->sourceController, &_styles, [](NotifierBase* notifier, int x, int y, bool hasSelection)
      {
         ContextMenuEvent event(EVENT_TEXT_CONTEXTMENU, x, y, hasSelection);

         notifier->notify(&event);
      }, [](NotifierBase* notifier)
         {
            SimpleEvent event(EVENT_TEXT_MARGINLICK);

            notifier->notify(&event);
         });

   TextViewFrame* frame = new TextViewFrame(notifier, _settings.withTabAboverscore, view, 
      _model->viewModel(), [](NotifierBase* notifier, int index)
      {
         SelectionEvent event = { EVENT_TEXTFRAME_SELECTION_CHANGED, index };

         notifier->notify(&event);
      });

   view->create(_instance, szTextView, owner, 0);
   frame->createControl(_instance, owner);

   return { frame, view };
}

void IDEFactory :: reloadStyles(TextViewModelBase* viewModel)
{
   // update font size
   for (int j = 0; j <= STYLE_MAX; j++) {
      defaultStyles[j].size = viewModel->fontInfo.size;
      defaultStyles[j].faceName = *viewModel->fontInfo.name;

      classicStyles[j].size = viewModel->fontInfo.size;
      classicStyles[j].faceName = *viewModel->fontInfo.name;

      darkStyles[j].size = viewModel->fontInfo.size;
      darkStyles[j].faceName = *viewModel->fontInfo.name;
   }

   _styles.assign(STYLE_MAX + 1, _schemes[viewModel->schemeIndex], viewModel->fontInfo.size + 5, 20, &_fontFactory);
}

void IDEFactory :: styleControl(GUIControlBase* control)
{
   ControlBase* ctrl = static_cast<ControlBase*>(control);

   auto style = _styles.getStyle(STYLE_DEFAULT);

   ctrl->setColor(0, style->background);
   ctrl->setColor(1, style->foreground);
}

ControlBase* IDEFactory :: createStatusbar(WindowBase* owner)
{
   StatusBar* statusBar = new IDEStatusBar(_model);

   statusBar->createControl(_instance, owner);
   statusBar->show();

   return statusBar;
}

ControlBase* IDEFactory :: createTabBar(WindowBase* owner, NotifierBase* notifier)
{
   TabBar* tabBar = new TabBar(notifier, _settings.withTabAboverscore, 200);

   tabBar->createControl(_instance, owner);

   return tabBar;
}

ControlBase* IDEFactory :: createSplitter(WindowBase* owner, ControlBase* client, bool vertical, NotifierBase* notifier)
{
   Splitter* splitter = new Splitter(notifier, client, vertical, [](NotifierBase* notifier)
      {
         LayoutEvent event(STATUS_LAYOUT_CHANGED);

         notifier->notify(&event);
      });

   splitter->create(_instance, 
      vertical ? szVSplitter : szHSplitter,
     owner);

   return splitter;
}

ControlBase* IDEFactory :: createVmConsoleControl(ControlBase* owner, ProcessBase* outputProcess)
{
   VMConsoleInteractive* vmConsole = new VMConsoleInteractive(outputProcess);

   vmConsole->createControl(_instance, owner);

   outputProcess->attachListener(vmConsole);

   return vmConsole;
}

ControlBase* IDEFactory :: createCompilerOutput(ControlBase* owner, ProcessBase* outputProcess, NotifierBase* notifier)
{
   CompilerOutput* output = new CompilerOutput(notifier, [](NotifierBase* notifier, int exitCode, int postponedAction)
      {
         CompletionEvent event = { EVENT_COMPILATION_END, exitCode, postponedAction };

         notifier->notify(&event);
      });

   output->createControl(_instance, owner);

   outputProcess->attachListener(output);

   return output;
}

ControlBase* IDEFactory :: createErrorList(ControlBase* owner, NotifierBase* notifier)
{
   MessageLog* log = new MessageLog(notifier, [](NotifierBase* notifier, int index)
      {
         SelectionEvent event = { EVENT_ERRORLIST_SELECTION, index };

         notifier->notify(&event);
      });
   log->createControl(_instance, owner);

   return log;
}

ControlBase* IDEFactory :: createProjectView(ControlBase* owner, NotifierBase* notifier)
{
   TreeView* projectView = new TreeView(300, 50, notifier, true,
      [](NotifierBase* notifier, size_t param)
      {
         ParamSelectionEvent event = { EVENT_PROJECTVIEW_SELECTION_CHANGED, param };

         notifier->notify(&event);
      });
   projectView->createControl(_instance, owner);

   styleControl(projectView);

   return projectView;
}

ControlBase* IDEFactory :: createDebugBrowser(ControlBase* owner, NotifierBase* notifier)
{
   ContextBrowser* browser = new ContextBrowser(&_model->contextBrowserModel, 300, 50, notifier,
      [](NotifierBase* notifier, size_t item, size_t param)
      {
         BrowseEvent event = { EVENT_BROWSE_CONTEXT, item, param };

         notifier->notify(&event);
      });
   browser->createControl(_instance, owner);
   browser->hide();

   return browser;
}

GUIControlBase* IDEFactory :: createMenu(ControlBase* owner)
{
   RootMenu* menu = new RootMenu(::GetMenu(owner->handle()));

   return menu;
}

GUIControlBase* IDEFactory :: createDebugContextMenu(ControlBase* owner)
{
   ContextMenu* menu = new ContextMenu();

   menu->create(3, browserContextMenuInfo);

   return menu;
}

GUIControlBase* IDEFactory :: createEditorContextMenu(ControlBase* owner)
{
   ContextMenu* menu = new ContextMenu();

   menu->create(8, contextMenuInfo);

   return menu;
}

GUIControlBase* IDEFactory :: createToolbar(ControlBase* owner, bool largeMode)
{
   ToolBar* toolBar = new ToolBar(largeMode ? 32 : 16);

   toolBar->createControl(_instance, owner, largeMode ? AppToolBarButtonsLarge : AppToolBarButtons, AppToolBarButtonNumber);
   toolBar->show();

   return toolBar;
}

void IDEFactory :: initializeScheme(int frameTextIndex, int tabBar, int compilerOutput, int errorList, 
   int projectView, int contextBrowser, int menu, int statusBar, int debugContextMenu, int vmConsoleControl, 
   int toolBarControl, int contextEditor, int textIndex)
{
   LoadStringW(_instance, IDC_COMPILER_OUTPUT, szCompilerOutput, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_COMPILER_MESSAGES, szErrorList, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_COMPILER_WATCH, szWatch, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_COMPILER_VMOUTPUT, szVMOutput, MAX_LOADSTRING);

   _model->ideScheme.textFrameId = frameTextIndex;
   _model->ideScheme.resultControl = tabBar;
   _model->ideScheme.compilerOutputControl = compilerOutput;
   _model->ideScheme.errorListControl = errorList;
   _model->ideScheme.projectView = projectView;
   _model->ideScheme.debugWatch = contextBrowser;
   _model->ideScheme.menu = menu;
   _model->ideScheme.statusBar = statusBar;
   _model->ideScheme.debugContextMenu = debugContextMenu;
   _model->ideScheme.vmConsoleControl = vmConsoleControl;
   _model->ideScheme.toolBarControl = toolBarControl;
   _model->ideScheme.editorContextMenu = contextEditor;
   _model->ideScheme.textControlId = textIndex;

   _model->ideScheme.captions.add(compilerOutput, szCompilerOutput);
   _model->ideScheme.captions.add(errorList, szErrorList);
   _model->ideScheme.captions.add(contextBrowser, szWatch);
   _model->ideScheme.captions.add(vmConsoleControl, szVMOutput);
}

GUIApp* IDEFactory :: createApp()
{
   WindowApp* app = new WindowApp(_instance, MAKEINTRESOURCE(IDC_IDE), &IDENotificationFormatter::getInstance());

   registerClasses();

   return app;
}

GUIControlBase* IDEFactory :: createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess, 
   ProcessBase* vmConsoleProcess)
{
   GUIControlBase* children[16];
   int counter = 0;

   int textIndex = counter++;
   int bottomBox = counter++;
   int statusBarIndex = counter++;
   int vsplitter = counter++;
   int tabBar = counter++;
   int compilerOutput = counter++;
   int errorList = counter++;
   int projectView = counter++;
   int hsplitter = counter++;
   int browser = counter++;
   int menu = counter++;
   int debugContextMenu = counter++;
   int vmConsoleControl = counter++;
   int toolBarControl = counter++;
   int contextEditor = counter++;
   int editIndex = counter++;

   SDIWindow* sdi = new IDEWindow(szTitle, _controller, _model, _instance, this);
   sdi->create(_instance, szSDI, nullptr, WS_EX_ACCEPTFILES);

   VerticalBox* vb = new VerticalBox(false, 1);

   auto textCtrls = createTextControl(sdi, notifier);

   children[textIndex] = textCtrls.value1;
   children[bottomBox] = vb;
   children[tabBar] = createTabBar(sdi, notifier);
   children[hsplitter] = createSplitter(sdi, (ControlBase*)children[tabBar], false, notifier);
   children[statusBarIndex] = createStatusbar(sdi);
   children[compilerOutput] = createCompilerOutput((ControlBase*)children[tabBar], outputProcess, notifier);
   children[errorList] = createErrorList((ControlBase*)children[tabBar], notifier);
   children[browser] = createDebugBrowser((ControlBase*)children[tabBar], notifier);
   children[projectView] = createProjectView(sdi, notifier);
   children[vsplitter] = createSplitter(sdi, (ControlBase*)children[projectView], true, notifier);
   children[menu] = createMenu(sdi);
   children[debugContextMenu] = createDebugContextMenu(sdi);
   children[vmConsoleControl] = createVmConsoleControl((ControlBase*)children[tabBar], vmConsoleProcess);
   children[toolBarControl] = createToolbar(sdi, _settings.withLargeToolbar);
   children[contextEditor] = createEditorContextMenu(sdi);
   children[editIndex] = textCtrls.value2;

   vb->append(children[hsplitter]);
   vb->append(children[statusBarIndex]);

   initializeScheme(textIndex, tabBar, compilerOutput, errorList, projectView, browser, menu, statusBarIndex,
      debugContextMenu, vmConsoleControl, toolBarControl, contextEditor, editIndex);

   sdi->populate(counter, children);
   sdi->setLayout(textIndex, toolBarControl, bottomBox, -1, vsplitter);

   styleControl(sdi);

   return sdi;
}
