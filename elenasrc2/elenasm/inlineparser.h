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

   //void writeSubject(TapeWriter& writer, ident_t message);
   bool writeObject(TapeWriter& writer, char state, ident_t token);
   bool parseMessage(ident_t message, IdentifierString& reference, int paramCounter);
   bool writeMessage(TapeWriter& writer, ident_t message, int paramCounter, int command);
   bool writeExtension(TapeWriter& writer, ident_t message, int paramCounter, int command);
   bool insertMessage(TapeWriter& writer, int bookmark, ident_t message, int paramCounter, int command);
   bool insertExtension(TapeWriter& writer, int bookmark, ident_t message, int paramCounter, int command);
   bool insertObject(TapeWriter& writer, int bookmark, char state, ident_t token);
   void parseArray(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer);
   int parseExpression(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer, int bookmark);
   int parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer);

public:
   virtual bool parseGrammarRule(_ScriptReader& reader)
   {
      return false;
   }

   virtual bool parseGrammarMode(_ScriptReader& reader)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   InlineScriptParser();
};

} // _ELENA_

#endif // inlineparserH
