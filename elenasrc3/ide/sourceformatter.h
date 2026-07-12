//---------------------------------------------------------------------------
//                E L E N A P r o j e c t: ELENA IDE
//                      ELENA Document formatter headers
//                                             (C)2021-2026, by Aleksey Rakov
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
      static void repeat(text_c ch, FormatterInfo& info);

      SourceFormatter() = default;

      void start(FormatterInfo& info) override;
      bool next(text_c ch, FormatterInfo& info) override;

      static TextFormatterBase* getInstance()
      {
         static SourceFormatter instance;

         return &instance;
      }
   };

}

#endif
