//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef FACTORY_H
#define FACTORY_H

#include "idecommon.h"
#include "windows/wincommon.h"
#include "windows/wincanvas.h"
#include "windows/wintextview.h"
#include "ideview.h"
#include "idecontroller.h"

namespace elena_lang
{
   typedef Pair<ControlBase*, ControlBase*, nullptr, nullptr> ControlPair;

   // --- IDEFactory ---
   class IDEFactory : public GUIFactoryBase, public ViewFactoryBase
   {
   protected:
      static PathSettings  _pathSettings;

      FontFactory          _fontFactory;
      ViewStyles           _styles;
      StyleInfo*           _schemes[3];
      GUISettinngs         _settings;
      HINSTANCE            _instance;

      IDEModel*            _model;
      IDEController*       _controller;

      void registerClasses();

      ControlPair createTextControl(WindowBase* owner, NotifierBase* notifier);
      ControlBase* createStatusbar(WindowBase* owner);
      ControlBase* createTabBar(WindowBase* owner, NotifierBase* notifier);
      ControlBase* createSplitter(WindowBase* owner, ControlBase* client, bool vertical, NotifierBase* notifier);
      ControlBase* createCompilerOutput(ControlBase* owner, ProcessBase* outputProcess, NotifierBase* notifier);
      ControlBase* createErrorList(ControlBase* owner, NotifierBase* notifier);
      ControlBase* createProjectView(ControlBase* owner, NotifierBase* notifier);
      ControlBase* createDebugBrowser(ControlBase* owner, NotifierBase* notifier);
      ControlBase* createVmConsoleControl(ControlBase* owner, ProcessBase* outputProcess);
      GUIControlBase* createMenu(ControlBase* owner);
      GUIControlBase* createDebugContextMenu(ControlBase* owner);
      GUIControlBase* createEditorContextMenu(ControlBase* owner);
      GUIControlBase* createToolbar(ControlBase* owner, bool largeMode);

      void initializeScheme(int frameTextIndex, int tabBar, int compilerOutput, int errorList, 
         int projectView, int contextBrowser, int menu, int statusBar, int debugContextMenu, 
         int vmConsoleOutput, int toolBarControl, int contextEditor, int textIndex);

   public:
      static void initPathSettings(IDEModel* ideModel);

      void reloadStyles(TextViewModelBase* viewModel) override;

      void styleControl(GUIControlBase* control) override;

      GUIApp* createApp() override;
      GUIControlBase* createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess,
         ProcessBase* vmConsoleProcess) override;

      IDEFactory(HINSTANCE instance, IDEModel* ideView, 
         IDEController* ideController,
         GUISettinngs settings);
   };
}

#endif