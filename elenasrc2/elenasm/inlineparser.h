//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef inlineparserH
#define inlineparserH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- InlineScriptParser ---

class VMTapeParser : public _Parser
{
   MessageMap _verbs;

   int mapVerb(ident_t literal);

   void writeSubject(TapeWriter& writer, ident_t message);
   bool writeObject(TapeWriter& writer, int state, ident_t token);
   bool writeArgument(TapeWriter& writer, int state, ident_t token);
   bool writeArray(TapeWriter& writer, int state, ident_t token);
   bool parseMessage(ident_t message, IdentifierString& reference);
   bool writeMessage(TapeWriter& writer, ident_t message, int command);
   bool writeExtension(TapeWriter& writer, ident_t message, int command);
   void parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer);

public:
   virtual bool parseGrammarRule(_ScriptReader&)
   {
      return false;
   }

   virtual bool parseGrammarMode(_ScriptReader&)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   VMTapeParser();
};

} // _ELENA_

#endif // inlineparserH
