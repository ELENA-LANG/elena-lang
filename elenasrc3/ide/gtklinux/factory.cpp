//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "factory.h"
#include "gtklinux/gtkcommon.h"
#include "gtklinux/gtkide.h"
#include "gtklinux/gtkidetextview.h"
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
      case EVENT_TEXTFRAME_SELECTION_CHANGED:
         textframe_changed.emit(*(SelectionEvent*)event);
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
      defaultStyles[j].size = viewModel->fontInfo.size;
      classicStyles[j].size = viewModel->fontInfo.size;
      darkStyles[j].size = viewModel->fontInfo.size;
   }

   // initialize view styles
   reloadStyles(viewModel);

   //TextViewWindow* view = new TextViewWindow(/*_model->viewModel(), &_styles*//*, &_controller->sourceController*/);
   IDETextViewFrame* frame = new IDETextViewFrame(_model->viewModel(), &_controller->sourceController, &_styles, &_broadcaster);

   _broadcaster.textview_changed.connect(sigc::mem_fun(*frame, &IDETextViewFrame::on_text_model_change));

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
}

Gtk::Widget* IDEFactory :: createProjectView()
{
   Gtk::TreeView* projectView = new Gtk::TreeView();

//   _projectView.signal_row_activated().connect(sigc::mem_fun(*this,
//              &GTKIDEWindow::on_projectview_row_activated));

   projectView->set_size_request(200, -1);

   return projectView;
}

GUIControlBase* IDEFactory :: createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess,
         ProcessBase* vmConsoleProcess)
{
   Gtk::Widget* children[2];
   int counter = 0;

   int textIndex = counter++;
   int projectView = counter++;
   children[textIndex] = createTextControl();
   children[projectView] = createProjectView();

   GTKIDEWindow* ideWindow = new GTKIDEWindow(_controller, _model);

   ideWindow->populate(counter, children);
   ideWindow->setLayout(textIndex, -1, -1, projectView, -1);

   _broadcaster.textview_changed.connect(sigc::mem_fun(*ideWindow, &GTKIDEWindow::on_text_model_change));
   _broadcaster.textframe_changed.connect(sigc::mem_fun(*ideWindow, &GTKIDEWindow::on_textframe_change));

   initializeScheme(textIndex, projectView);

   return new WindowWrapper(ideWindow);
}


void IDEFactory :: initializeScheme(int frameTextIndex, /*int tabBar, int compilerOutput, int errorList,*/
   int projectView/*, int contextBrowser, int menu, int statusBar, int debugContextMenu, int vmConsoleControl, 
   int toolBarControl, int contextEditor, int textIndex*/)
{
   _model->ideScheme.textFrameId = frameTextIndex;
//   _model->ideScheme.resultControl = tabBar;
//   _model->ideScheme.compilerOutputControl = compilerOutput;
//   _model->ideScheme.errorListControl = errorList;
   _model->ideScheme.projectView = projectView;
//   _model->ideScheme.debugWatch = contextBrowser;
//   _model->ideScheme.menu = menu;
//   _model->ideScheme.statusBar = statusBar;
//   _model->ideScheme.debugContextMenu = debugContextMenu;
//   _model->ideScheme.vmConsoleControl = vmConsoleControl;
//   _model->ideScheme.toolBarControl = toolBarControl;
//   _model->ideScheme.editorContextMenu = contextEditor;
//   _model->ideScheme.textControlId = textIndex;

//   _model->ideScheme.captions.add(compilerOutput, szCompilerOutput);
//   _model->ideScheme.captions.add(errorList, szErrorList);
//   _model->ideScheme.captions.add(contextBrowser, szWatch);
//   _model->ideScheme.captions.add(vmConsoleControl, szVMOutput);
}

GUIApp* IDEFactory :: createApp()
{
   WindowApp* app = new WindowApp(&_broadcaster);

   return app;
}
