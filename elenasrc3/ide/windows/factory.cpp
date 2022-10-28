//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2022, by Aleksey Rakov
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
#include "Resource.h"

#include <shlwapi.h>

using namespace elena_lang;

#ifdef _M_IX86

#define CLI_PATH "elena-cli.exe"

#else

#define CLI_PATH "elena64-cli.exe"

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
   //{Color(0xFF, 0x80, 0x40), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0, 0xFF), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0x80, 0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0x40, 0x80, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0, 0x00, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0, 0x80, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0x80, 0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0x60, 0x60, 0x60), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false}
};

StyleInfo classicStyles[STYLE_MAX + 1] = {
   {Color(0xFF, 0xFF, 0), Color(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0), Color(Canvas::Chrome()), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0x60, 0x60, 0x60), Color(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0x60, 0x60, 0x60), Color(0x0, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   {Color(0xFF, 0xFF, 0xFF), Color(0xFF, 0x0, 0x0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xFF, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xC0, 0xC0, 0xC0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xC0, 0xC0, 0xC0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0, 0xFF, 0x80), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0, 0x80), Colour(0xC0, 0xC0, 0xC0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0x60, 0x60, 0x60), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false}
};

constexpr auto STYLE_SCHEME_COUNT = 2;

// --- IDEFactory ---

IDEFactory :: IDEFactory(HINSTANCE instance, IDEModel* ideModel, 
   IDEController* controller,
   GUISettinngs   settings)
{
   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;
   _settings = settings;

   _instance = instance;
   _model = ideModel;
   _controller = controller;

   wchar_t appPath[MAX_PATH];
   ::GetModuleFileName(NULL, appPath, MAX_PATH);
   ::PathRemoveFileSpec(appPath);

   _pathSettings.appPath.copy(appPath);

   _model->projectModel.paths.appPath.copy(*_pathSettings.appPath);
   _model->projectModel.paths.compilerPath.copy(CLI_PATH);
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

ControlBase* IDEFactory :: createTextControl(WindowBase* owner, NotifierBase* notifier)
{
   auto viewModel = _model->viewModel();

   // update font size
   for (int j = 0; j < STYLE_MAX; j++) {
      defaultStyles[j].size = viewModel->fontSize;
      classicStyles[j].size = viewModel->fontSize;
   }

   // initialize view styles
   _styles.assign(STYLE_MAX + 1, _schemes[viewModel->schemeIndex], viewModel->fontSize + 5, 20, &_fontFactory);

   // initialize UI components
   TextViewWindow* view = new TextViewWindow(_model->viewModel(), &_controller->sourceController, &_styles);
   TextViewFrame* frame = new TextViewFrame(notifier, _settings.withTabAboverscore, view, _model->viewModel());

   view->create(_instance, szTextView, owner);
   frame->createControl(_instance, owner);

   // !! temporal
   frame->show();

   return frame;
}

ControlBase* IDEFactory :: createStatusbar(WindowBase* owner)
{
   StatusBar* statusBar = new IDEStatusBar(_model);

   statusBar->createControl(_instance, owner);

   statusBar->show(); // !! temporal

   return statusBar;
}

ControlBase* IDEFactory :: createTabBar(WindowBase* owner, NotifierBase* notifier)
{
   TabBar* tabBar = new TabBar(notifier, _settings.withTabAboverscore, 200);

   tabBar->createControl(_instance, owner);

   tabBar->show(); // !! temporal

   return tabBar;
}

ControlBase* IDEFactory :: createSplitter(WindowBase* owner, ControlBase* client, bool vertical, NotifierBase* notifier, int notifyCode)
{
   Splitter* splitter = new Splitter(notifier, notifyCode, client, vertical);

   splitter->create(_instance, 
      vertical ? szVSplitter : szHSplitter,
     owner);

   return splitter;
}

ControlBase* IDEFactory :: createCompilerOutput(ControlBase* owner, ProcessBase* outputProcess, NotifierBase* notifier)
{
   CompilerOutput* output = new CompilerOutput(notifier, NOTIFY_COMPILATION_RESULT);

   output->createControl(_instance, owner);

   outputProcess->attachListener(output);

   return output;
}

ControlBase* IDEFactory :: createErrorList(ControlBase* owner, NotifierBase* notifier)
{
   MessageLog* log = new MessageLog(notifier, NOTIFY_ERROR_HIGHLIGHT_ROW);
   log->createControl(_instance, owner);

   return log;
}

ControlBase* IDEFactory :: createProjectView(ControlBase* owner)
{
   TreeView* projectView = new TreeView(300, 50, true);
   projectView->createControl(_instance, owner);

   return projectView;
}

void IDEFactory :: initializeScheme(int frameTextIndex, int tabBar, int compilerOutput, int errorList, int projectView)
{
   LoadStringW(_instance, IDC_COMPILER_OUTPUT, szCompilerOutput, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_COMPILER_MESSAGES, szErrorList, MAX_LOADSTRING);

   _model->ideScheme.textFrameId = frameTextIndex;
   _model->ideScheme.resultControl = tabBar;
   _model->ideScheme.compilerOutputControl = compilerOutput;
   _model->ideScheme.errorListControl = errorList;
   _model->ideScheme.projectView = projectView;

   _model->ideScheme.captions.add(compilerOutput, szCompilerOutput);
   _model->ideScheme.captions.add(errorList, szErrorList);

   if(_model->projectModel.lastOpenFiles.count() > 0) {
      path_t path = _model->projectModel.lastOpenFiles.get(1);

      _controller->openFile(_model, path);
   }
}

GUIApp* IDEFactory :: createApp()
{
   WindowApp* app = new WindowApp(_instance, MAKEINTRESOURCE(IDC_IDE));

   registerClasses();

   return app;
}

GUIControlBase* IDEFactory :: createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess)
{
   GUIControlBase* children[9];
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

   SDIWindow* sdi = new IDEWindow(szTitle, _controller, _model, _instance);
   sdi->create(_instance, szSDI, nullptr);

   VerticalBox* vb = new VerticalBox(false, 1);

   children[textIndex] = createTextControl(sdi, notifier);
   children[bottomBox] = vb;
   children[tabBar] = createTabBar(sdi, notifier);
   children[vsplitter] = createSplitter(sdi, (ControlBase*)children[tabBar], false, notifier, 
      NOTIFY_LAYOUT_CHANGED);
   children[statusBarIndex] = createStatusbar(sdi);
   children[compilerOutput] = createCompilerOutput((ControlBase*)children[tabBar], outputProcess, notifier);
   children[errorList] = createErrorList((ControlBase*)children[tabBar], notifier);
   children[projectView] = createProjectView(sdi);
   children[hsplitter] = createSplitter(sdi, (ControlBase*)children[projectView], true, notifier,
      NOTIFY_LAYOUT_CHANGED);

   vb->append(children[vsplitter]);
   vb->append(children[statusBarIndex]);

   sdi->populate(counter, children);
   sdi->setLayout(textIndex, -1, bottomBox, -1, hsplitter);

   initializeScheme(textIndex, tabBar, compilerOutput, errorList, projectView);

   return sdi;
}
