//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef FACTORY_H
#define FACTORY_H

#include "idecommon.h"
//#include "windows/wincommon.h"
//#include "windows/wincanvas.h"
#include "ideview.h"
#include "idecontroller.h"

namespace elena_lang
{
   // --- IDEFactory ---
   class IDEFactory : public GUIFactoryBase
   {
   protected:
      //FontFactory    _fontFactory;
      //StyleInfo*     _schemes[2];
      //GUISettinngs   _settings;

      //HINSTANCE      _instance;
      //int            _cmdShow;

      //IDEModel*      _model;
      //IDEController* _controller;

      //void registerClasses();

      //ControlBase* createTextControl(WindowBase* owner);

      //void initializeModel(IDEModel* ideView);

   public:
      GUIApp* createApp() override;
      GUIControlBase* createMainWindow() override;

      IDEFactory(/*HINSTANCE instance, int cmdShow, IDEModel* ideView,
         IDEController* ideController,
         GUISettinngs   settings*/);
   };
}

#endif