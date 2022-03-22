//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SOURCE_H
#define SOURCE_H

#include "textparser.h"

namespace elena_lang
{
   struct SourceInfo
   {
      LineInfo lineInfo;
      char     state;
      ustr_t   symbol;

      SourceInfo()
      {
         state = 0;
      }
   };

   // --- SourceReader ---
   class SourceReader : protected TextParser<char, LINE_LEN>
   {
      void copyToken(char* token, size_t length);

   public:
      SourceInfo read(char* line, size_t length);

      SourceReader(int tabSize, UStrReader* source);
   };

}

#endif
