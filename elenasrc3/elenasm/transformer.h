//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include "scriptparser.h"

namespace elena_lang
{
   // --- ScriptEngineTextParser ---
   class ScriptEngineTextParser : public ScriptEngineParserBase
   {
      int _width;

   public:
      bool parseGrammarRule(ScriptEngineReader& reader) override;

      ScriptEngineTextParser()
      {
         _width = 0x50;
      }
   };

}

#endif
