//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef VMPARSER_H
#define VMPARSER_H

#include "scriptparser.h"

namespace elena_lang
{
   class VMTapeParser : public ScriptEngineParserBase
   {
   public:
      void parse(ScriptEngineReaderBase& reader, MemoryDump* output) override;
      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      VMTapeParser();
   };

}

#endif
