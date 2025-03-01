//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "gtklinux/gtkcommon.h"
#include "gtklinux/gtkide.h"
#include "gtklinux/gtktextframe.h"
#include "gtklinux/gtktextview.h"
//#include "text.h"
//#include "sourceformatter.h"

using namespace elena_lang;

//#define MAX_LOADSTRING 100
//
//// Global Variables:
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szSDI[MAX_LOADSTRING];                    // the main window class name
//WCHAR szTextView[MAX_LOADSTRING];               // the main window class name

// --- Styles ---
StyleInfo defaultStyles[STYLE_MAX + 1] = {
   {Color(0,0,0), Color(1, 1, 1), "Monospace", 10, false, false},
   {Color(0,0,0), Color(0.84, 0.84, 0.84), "Monospace", 10, false, false},
   {Color(0,0,0), Color(0.75, 0.75, 0.75), "Monospace", 10, false, false},
   {Color(0.35, 0.35, 0.35), Color(0, 1, 1), "Monospace", 10, true, false},
   {Color(1, 1, 1), Color(1, 0, 0), "Monospace", 10, false, false},
   {Color(1, 1, 1), Color(1, 0, 0), "Monospace", 10, false, false},
   {Color(0, 0, 1), Color(1, 1, 1), "Monospace", 10, false, false},
   {Color(0.25, 0.5, 0.5), Color(1, 1, 1), "Monospace", 10, false, false},
   {Color(0, 0.5, 0), Color(1, 1, 1), "Monospace", 10, false, false},
   {Color(1, 0.5, 0.25), Color(1, 1, 1), "Monospace", 10, false, false},
   {Color(0, 0.5, 0.5), Color(1, 1, 1), "Monospace", 10, false, false},
};

StyleInfo classicStyles[STYLE_MAX + 1] = {
   {Color(1, 1, 0), Color(0, 0, 0.5), "Monospace", 10, false, false},
   {Color(0.84, 0.84, 0.84), Color(0, 0, 68), "Monospace", 10, false, false},
   {Color(0.35, 0.35, 0.35), Color(0.84, 0.84, 0.84), "Monospace", 10, true, false},
   {Color(0.35, 0.35, 0.35), Color(0, 1, 1), "Monospace", 10, true, false},
   {Color(1, 1, 1), Color(1, 0, 0), "Monospace", 10, false, false},
   {Color(1, 1, 1), Color(0, 0, 0.5), "Monospace", 10, false, false},
   {Color(1, 1, 1), Color(0, 0, 0.5), "Monospace", 10, false, false},
   {Color(0.85, 0.85, 0.85), Color(0, 0, 0.5), "Monospace", 10, false, false},
   {Color(0.85, 0.85, 0.85), Color(0, 0, 0.5), "Monospace", 10, false, false},
   {Color(0, 1, 0.5), Color(0, 0, 0.5), "Monospace", 10, false, false},
   {Color(0, 1, 1), Color(0, 0, 0.5), "Monospace", 10, false, false},
};

StyleInfo darkStyles[STYLE_MAX + 1] = {
   {Color(1, 1, 1), Color(0.20, 0.20, 0.20), "Monospace", 10, false, false},
   {Color(0.65, 0.65, 0.65), Color(0.5, 0.5, 0.5), "Monospace", 10, false, false},
   {Color(0.4, 0.4, 0.4), Color(0.65, 0.65, 0.65), "Monospace", 10, true, false},
   {Color(0.95, 0.95, 0.95), Color(0.5, 0.65, 0.65), "Monospace", 10, true, false},
   {Color(1, 1, 1), Color(1, 0, 0), "Monospace", 10, false, false},
   {Color(1, 1, 1), Color(0.5, 0, 0), "Monospace", 10, false, false},
   {Color(0.5, 1, 0.9), Color(0.3, 0.3, 0.3), "Monospace", 10, false, false},
   {Color(0.70, 0.70, 0.70), Color(0.3, 0.3, 0.3), "Monospace", 10, false, false},
   {Color(87, 0.70, 74), Color(0.3, 0.3, 0.3), "Monospace", 10, false, false},
   {Color(0.78, 0.8, 0.70), Color(0.3, 0.3, 0.3), "Monospace", 10, false, false},
   {Color(0.8, 0.55, 0.6), Color(0.3, 0.3, 0.3), "Monospace", 10, false, false},
};

constexpr auto STYLE_SCHEME_COUNT = 3;

// --- IDEBroadcaster ---

IDEBroadcaster :: IDEBroadcaster()
{
}

void IDEBroadcaster :: sendMessage(EventBase* event)
{
   int eventId = event->eventId();
   switch (eventId) {
      case EVENT_TEXTVIEW_MODEL_CHANGED:
         textview_changed.emit(*(TextViewModelEvent*)event);
         break;
      default:
         break;
   }
}

// --- IDEFactory ---

IDEFactory :: IDEFactory(IDEModel* ideModel, IDEController* controller,
   GUISettinngs   settings)
{
   _schemes[0] = defaultStyles;
   _schemes[1] = classicStyles;
   _schemes[2] = darkStyles;
   _settings = settings;

//   _cmdShow = cmdShow;
   _model = ideModel;
   _controller = controller;

   //initializeModel(ideModel);
}

Gtk::Widget* IDEFactory :: createTextControl()
{
   auto viewModel = _model->viewModel();

   // update font size
   for (int j = 0; j < STYLE_MAX; j++) {
      defaultStyles[j].size = viewModel->fontSize;
      classicStyles[j].size = viewModel->fontSize;
      darkStyles[j].size = viewModel->fontSize;
   }

   // initialize view styles
   reloadStyles(viewModel);

   //TextViewWindow* view = new TextViewWindow(/*_model->viewModel(), &_styles*//*, &_controller->sourceController*/);
   TextViewFrame* frame = new TextViewFrame(_model->viewModel(), &_controller->sourceController, &_styles);

   //_broadcaster.textview_changed.connect(sigc::mem_fun(*frame, &TextViewFrame::on_text_model_change));

   return frame;
}

//void IDEFactory :: initializeModel(IDEModel* ideView)
//{
//   auto viewModel = ideView->viewModel();
//
//   Text* text = new Text(EOLMode::CRLF);
//   PathString path("/home/alex/elena-lang/tests60/sandbox/sandbox.l");
//   text->load(*path, FileEncoding::UTF8, false);
//
//   viewModel->docView = new DocumentView(text, ELENADocFormatter::getInstance());
//
//   _styles.assign(STYLE_MAX + 1, _schemes[/*model->scheme*/0], viewModel->fontSize + 5, 20, &_fontFactory);
//}

void IDEFactory :: reloadStyles(TextViewModelBase* viewModel)
{
   _styles.assign(STYLE_MAX + 1, _schemes[viewModel->schemeIndex], viewModel->fontSize + 5, 20, &_fontFactory);
}

void IDEFactory :: styleControl(GUIControlBase* control)
{
}

GUIControlBase* IDEFactory :: createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess,
         ProcessBase* vmConsoleProcess)
{
   Gtk::Widget* children[1];
   int counter = 0;

   int textIndex = counter++;
   children[textIndex] = createTextControl();

   GTKIDEWindow* ideWindow = new GTKIDEWindow(_controller, _model);

   ideWindow->populate(counter, children);
   ideWindow->setLayout(textIndex, -1, -1, -1, -1);

   _broadcaster.textview_changed.connect(sigc::mem_fun(*ideWindow, &GTKIDEWindow::on_text_model_change));

   return new WindowWrapper(ideWindow);
}

GUIApp* IDEFactory :: createApp()
{
   WindowApp* app = new WindowApp(&_broadcaster);

   return app;
}
