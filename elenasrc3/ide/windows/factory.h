//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2023, by Aleksey Rakov
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
   // --- IDEFactory ---
   class IDEFactory : public GUIFactoryBase, public ViewFactoryBase
   {
   protected:
      FontFactory    _fontFactory;
      ViewStyles     _styles;
      StyleInfo*     _schemes[2];
      GUISettinngs   _settings;
      PathSettings   _pathSettings;

      HINSTANCE      _instance;

      IDEModel*      _model;
      IDEController* _controller;

      void registerClasses();

      ControlBase* createTextControl(WindowBase* owner, NotifierBase* notifier);
      ControlBase* createStatusbar(WindowBase* owner);
      ControlBase* createTabBar(WindowBase* owner, NotifierBase* notifier);
      ControlBase* createSplitter(WindowBase* owner, ControlBase* client, bool vertical, NotifierBase* notifier, 
         int notifyCode, NotificationStatus status);
      ControlBase* createCompilerOutput(ControlBase* owner, ProcessBase* outputProcess, NotifierBase* notifier);
      ControlBase* createErrorList(ControlBase* owner, NotifierBase* notifier);
      ControlBase* createProjectView(ControlBase* owner, NotifierBase* notifier);
      ControlBase* createDebugBrowser(ControlBase* owner, NotifierBase* notifier);
      ControlBase* createVmConsoleControl(ControlBase* owner, ProcessBase* outputProcess);
      GUIControlBase* createMenu(ControlBase* owner);
      GUIControlBase* createDebugContextMenu(ControlBase* owner);
      GUIControlBase* createEditorContextMenu(ControlBase* owner);
      GUIControlBase* createToolbar(ControlBase* owner);

      void initializeScheme(int frameTextIndex, int tabBar, int compilerOutput, int errorList, 
         int projectView, int contextBrowser, int menu, int statusBar, int debugContextMenu, 
         int vmConsoleOutput, int toolBarControl, int contextEditor);

   public:
      void reloadStyles(TextViewModelBase* viewModel) override;

      GUIApp* createApp() override;
      GUIControlBase* createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess,
         ProcessBase* vmConsoleProcess) override;

      IDEFactory(HINSTANCE instance, IDEModel* ideView, 
         IDEController* ideController,
         GUISettinngs settings);
   };
}

#endif