//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2015, by Alexei Rakov
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

   struct Scope
   {
      int level;
      int arg_level;

      Scope()
      {
         level = 0;
         arg_level = 0;
      }
      Scope(int level, int arg_level)
      {
         this->level = level;
         this->arg_level = arg_level;
      }
   };

   int mapVerb(ident_t literal);

   void readMessage(_ScriptReader& reader, IdentifierString& message, bool subjectOnly = false);

   void writeObject(TapeWriter& writer, char state, ident_t value);

   void copyDump(MemoryDump& dump, MemoryDump& line, Stack<int>& arguments);
   void writeLine(MemoryDump& line, TapeWriter& writer);
   int parseStatement(_ScriptReader& reader, MemoryDump& line);

public:
   virtual bool parseGrammarRule(_ScriptReader& reader)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, TapeWriter& writer);

   InlineScriptParser();
};

} // _ELENA_

#endif // inlineparserH
