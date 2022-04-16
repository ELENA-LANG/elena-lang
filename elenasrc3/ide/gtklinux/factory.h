//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE windows factory
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef FACTORY_H
#define FACTORY_H

#include "idecommon.h"
#include "gtklinux/gtksdi.h"
#include "ideview.h"
#include "idecontroller.h"

namespace elena_lang
{
   // --- IDEFactory ---
   class IDEFactory
   {
   protected:
      FontFactory    _fontFactory;
      ViewStyles     _styles;
      StyleInfo*     _schemes[2];
      GUISettinngs   _settings;

      //HINSTANCE      _instance;
      //int            _cmdShow;

      IDEModel*      _model;
      //IDEController* _controller;

      //void registerClasses();

      Gtk::Widget* createTextControl();

      void initializeModel(IDEModel* ideView);

   public:
      SDIWindow* createMainWindow();

      IDEFactory(/*HINSTANCE instance, int cmdShow, */IDEModel* ideView,
         /*IDEController* ideController,*/
         GUISettinngs   settings);
   };
}

#endif
