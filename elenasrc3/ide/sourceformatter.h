//---------------------------------------------------------------------------
//                E L E N A P r o j e c t: ELENA IDE
//                      ELENA Document formatter headers
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SOURCEFORMATTER_H
#define SOURCEFORMATTER_H

#include "guicommon.h"
#include "document.h"

namespace elena_lang
{
   // --- SourceFormatter --
   class SourceFormatter : TextFormatterBase
   {
   public:
      SourceFormatter() = default;

      void start(FormatterInfo& info) override;
      bool next(text_c ch, FormatterInfo& info, pos_t& lastState) override;

      static TextFormatterBase* getInstance()
      {
         static SourceFormatter instance;

         return &instance;
      }
   };

}

#endif
