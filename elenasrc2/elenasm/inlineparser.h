//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef inlineparserH
#define inlineparserH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- InlineScriptParser ---

class InlineScriptParser : public _Parser
{
   MessageMap _verbs;

   int mapVerb(ident_t literal);

   void writeObject(TapeWriter& writer, char state, ident_t value);
   void writeMessage(TapeWriter& writer, ident_t message, int paramCounter, int command);
   void writeSubject(TapeWriter& writer, ident_t message);

   int parseStack(_ScriptReader& reader, TapeWriter& writer, Stack<ScriptBookmark>& stack);

   void parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer);

public:
   virtual bool parseGrammarRule(_ScriptReader& reader)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, TapeWriter& writer);

   InlineScriptParser();
};

// --- InlineTapeParser ---

} // _ELENA_

#endif // inlineparserH
