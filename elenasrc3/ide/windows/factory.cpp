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
#include "Resource.h"

using namespace elena_lang;

#define MAX_LOADSTRING 100

// Global Variables:
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szSDI[MAX_LOADSTRING];                    // the main window class name
WCHAR szTextView[MAX_LOADSTRING];               // the main window class name

// !! temporally
#define IDE_CHARSET_ANSI                        ANSI_CHARSET
#define IDE_CHARSET_DEFAULT                     1

// --- Styles ---
StyleInfo defaultStyles[STYLE_MAX + 1] = {
   {Color(0), Color(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0), Color(Canvas::Chrome()), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0), Color(0xC0, 0xC0, 0xC0), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   {Color(0x60, 0x60, 0x60), Color(0x0, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0, 0, 0xFF), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0x80, 0), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0x40, 0x80, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0, 0x00, 0x80), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0xFF, 0x80, 0x40), Colour(0xFF, 0xFF, 0xFF), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
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
   //{Colour(0xFF, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xC0, 0xC0, 0xC0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xC0, 0xC0, 0xC0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0, 0xFF, 0x80), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0x80, 0xFF, 0xFF), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0, 0, 0x80), Colour(0xC0, 0xC0, 0xC0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0x60, 0x60, 0x60), Colour(0x0, 0xFF, 0xFF), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, true, false},
   //{Colour(0xFF, 0xFF, 0xFF), Colour(0xFF, 0x0, 0x0), TEXT("Courier New"), IDE_CHARSET_ANSI, 10, false, false},
   //{Colour(0xFF, 0xFF, 0), Colour(0, 0, 0x80), _T("Courier New"), IDE_CHARSET_ANSI, 10, true, false}
};

// --- IDEFactory ---

IDEFactory :: IDEFactory(HINSTANCE instance, int cmdShow, IDEModel* ideModel, 
   IDEController* controller,
   GUISettinngs   settings)
{
   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;
   _settings = settings;

   _instance = instance;
   _cmdShow = cmdShow;
   _model = ideModel;
   _controller = controller;
}

void IDEFactory :: registerClasses()
{
   // Initialize global strings
   LoadStringW(_instance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_IDE, szSDI, MAX_LOADSTRING);
   LoadStringW(_instance, IDC_TEXTVIEW, szTextView, MAX_LOADSTRING);

   SDIWindow::registerSDIWindow(_instance, szSDI, LoadIcon(_instance, MAKEINTRESOURCE(IDI_IDE)), MAKEINTRESOURCEW(IDC_IDE), LoadIcon(_instance, MAKEINTRESOURCE(IDI_SMALL)));
   TextViewWindow::registerTextViewWindow(_instance, szTextView);
}

ControlBase* IDEFactory :: createTextControl(WindowBase* owner, NotifierBase* notifier)
{
   auto viewModel = _model->viewModel();

   // initialize view styles
   _styles.assign(STYLE_MAX + 1, _schemes[/*model->scheme*/0], viewModel->fontSize + 5, 20, &_fontFactory);

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

void IDEFactory :: initializeModel()
{
   //// !! temporal
   //auto viewModel = _model->viewModel();

   //PathString path("C:\\Alex\\ELENA\\tests60\\sandbox\\sandbox.l");

   //_controller->sourceController.openDocument(viewModel, "sandbox.l", *path, FileEncoding::UTF8);
   //_controller->sourceController.selectDocument(viewModel, "sandbox.l");
}

GUIApp* IDEFactory :: createApp()
{
   WindowApp* app = new WindowApp(_instance, _cmdShow, MAKEINTRESOURCE(IDC_IDE));

   registerClasses();

   return app;
}

GUIControlBase* IDEFactory :: createMainWindow(NotifierBase* notifier)
{
   ControlBase* children[2];
   int counter = 0;

   int textIndex = counter++;
   int statusBarIndex = counter++;

   SDIWindow* sdi = new IDEWindow(szTitle, _controller, _model, _instance, 
      textIndex);
   sdi->create(_instance, szSDI, nullptr);

   children[textIndex] = createTextControl(sdi, notifier);
   children[statusBarIndex] = createStatusbar(sdi);

   sdi->populate(counter, children);
   sdi->setLayout(textIndex, -1, statusBarIndex, -1, -1);

   initializeModel();

   return sdi;
}
