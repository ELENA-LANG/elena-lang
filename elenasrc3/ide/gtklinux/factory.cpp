//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "gtklinux/gtkide.h"
#include "gtklinux/gtktextframe.h"
#include "gtklinux/gtktextview.h"
#include "text.h"
#include "sourceformatter.h"

using namespace elena_lang;

//#define MAX_LOADSTRING 100
//
//// Global Variables:
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szSDI[MAX_LOADSTRING];                    // the main window class name
//WCHAR szTextView[MAX_LOADSTRING];               // the main window class name

#define IDE_CHARSET_UTF8                     1 // !! dummy value

// --- Styles ---
StyleInfo defaultStyles[STYLE_MAX + 1] = {
   {Color(0, 0, 0), Color(1, 1, 1), "Monospace", 10, false, false},
   {Color(0, 0, 0), Color(0.84, 0.84, 0.84), "Monospace", 10, false, false},
   {Color(0, 0, 0), Color(0.75, 0.75, 0.75), "Monospace", 10, false, false},
   //{Colour(0, 0, 1), Colour(1, 1, 1), "Monospace", 10, false, false},
   //{Colour(0, 0.4, 0.5), Colour(1, 1, 1), "Monospace", 10, true, false},
   //{Colour(0, 0, 0), Colour(1, 1, 1), "Monospace", 10, true, false},
   //{Colour(1, 0.5, 0.4), Colour(1, 1, 1), "Monospace", 10, false, false},
   //{Colour(0, 0.5, 0.5), Colour(1, 1, 1), "Monospace", 10, false, false},
   //{Colour(0, 0.5, 0), Colour(1, 1, 1), "Monospace", 10, false, false},
   //{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
   //{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
   //{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
   //{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
   //{Colour(0.37, 0.37, 0.37), Colour(0, 1, 1), "Monospace", 10, true, false},
   //{Colour(0.37, 0.37, 0.37), Colour(1, 1, 1), "Monospace", 10, true, false},
};
//
//StyleInfo classicStyles[STYLE_MAX + 1] = {
//   {Colour(1, 1, 0), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0, 0, 0), Colour(0.84, 0.84, 0.84), "Monospace", 10, false, false},
//   {Colour(0.37, 0.37, 0.37), Colour(1, 1, 1), "Monospace", 10, false, false},
//   {Colour(0.7, 0.7, 0.7), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(1, 1, 1), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(1, 1, 0), Colour(0, 0, 0.5), "Monospace", 10, true, false},
//   {Colour(0, 1, 0.5), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0, 1, 1), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(1, 1, 0), Colour(0, 0, 0.5), "Monospace", 10, true, false},
//   {Colour(0, 1, 0.5), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0, 1, 1), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false},
//   {Colour(0.8, 0.8, 0.8), Colour(0, 0, 0.5), "Monospace", 10, false, false}//,
//};

// --- IDEFactory ---

IDEFactory :: IDEFactory(/*HINSTANCE instance, int cmdShow, */IDEModel* ideModel,
   /*IDEController* controller,*/
   GUISettinngs   settings)
{
   _schemes[0] = defaultStyles;
//   _schemes[1] = classicStyles;
//   _settings = settings;
//
//   _instance = instance;
//   _cmdShow = cmdShow;
   _model = ideModel;
//   _controller = controller;

   initializeModel(ideModel);
}

Gtk::Widget* IDEFactory :: createTextControl()
{
   TextViewWindow* view = new TextViewWindow(_model->viewModel(), &_styles/*, &_controller->sourceController*/);
   TextViewFrame* frame = new TextViewFrame(/*_settings.withTabAboverscore, view*/);

   // !! temporal
   frame->addTab(_T("test"), view);

   view->show(); // !! temporal?
   frame->show(); // !! temporal?

//   view->create(_instance, szTextView, owner);
//   frame->createControl(_instance, owner);
//
//   // !! temporal
//   frame->addTabView(_T("test"), nullptr);
//
//   // !! temporal
//   view->showWindow(SW_SHOW);
//   frame->showWindow(SW_SHOW);

   return frame;
}

void IDEFactory :: initializeModel(IDEModel* ideView)
{
   auto viewModel = ideView->viewModel();

   Text* text = new Text(EOLMode::CRLF);
   PathString path("/home/alex/elena-lang/tests60/sandbox/sandbox.l");
   text->load(*path, FileEncoding::UTF8, false);

   viewModel->docView = new DocumentView(text, ELENADocFormatter::getInstance());

   _styles.assign(STYLE_MAX + 1, _schemes[/*model->scheme*/0], viewModel->fontSize + 5, 20, &_fontFactory);
}

SDIWindow* IDEFactory :: createMainWindow()
{
   SDIWindow* sdi = new GTKIDEWindow(/*szTitle, _controller, _model*/);
//   sdi->create(_instance, szSDI, nullptr);
//
   Gtk::Widget* children[1];
   int counter = 0;

   int textIndex = counter++;
   children[textIndex] = createTextControl();

   sdi->populate(counter, children);
   sdi->setLayout(textIndex, -1, -1, -1, -1);

   return sdi;
}
