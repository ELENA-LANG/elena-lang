//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef VIEW_H
#define VIEW_H

#include "guieditor.h"

namespace elena_lang
{

   // --- TextViewModel ---
   class TextViewModel : public TextViewModelBase
   {
   public:
      bool          changed;

      bool isAssigned() const override { return docView != nullptr; }

      void resize(Point size) override
      {
         if (docView)
            docView->resize(size);
      }

      TextViewModel(int fontSize);
      virtual ~TextViewModel();
   };

}

#endif