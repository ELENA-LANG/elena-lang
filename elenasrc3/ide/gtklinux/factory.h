//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef FACTORY_H
#define FACTORY_H

#include "idecommon.h"
#include "gtklinux/gtkcommon.h"
//#include "gtklinux/gtksdi.h"
//#include "gtklinux/gtkgraphic.h"
#include "gtklinux/gtktextview.h"
#include "ideview.h"
#include "idecontroller.h"

namespace elena_lang
{
   typedef sigc::signal<void(TextViewModelEvent)> type_textview_changed;

   // --- IDEBroadcaster ---
   class IDEBroadcaster : public BroadcasterBase
   {
   public:
      type_textview_changed textview_changed;

      void sendMessage(EventBase* event) override;

      IDEBroadcaster();
   };

   // --- IDEFactory ---
   class IDEFactory : public GUIFactoryBase, public ViewFactoryBase
   {
   protected:
      IDEBroadcaster _broadcaster;

      FontFactory    _fontFactory;
      ViewStyles     _styles;
      StyleInfo*     _schemes[3];
      GUISettinngs   _settings;

      //HINSTANCE      _instance;
      //int            _cmdShow;

      IDEModel*      _model;
      IDEController* _controller;

      //void registerClasses();

      Gtk::Widget* createTextControl();

      //void initializeModel(IDEModel* ideView);

   public:
      void reloadStyles(TextViewModelBase* viewModel) override;

      void styleControl(GUIControlBase* control) override;

      GUIApp* createApp() override;
      GUIControlBase* createMainWindow(NotifierBase* notifier, ProcessBase* outputProcess,
         ProcessBase* vmConsoleProcess) override;

      IDEFactory(IDEModel* ideView,
         IDEController* ideController,
         GUISettinngs   settings);
   };
}

#endif
