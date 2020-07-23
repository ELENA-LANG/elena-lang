//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef treearserH
#define treearserH 1

#include "scriptengine.h"
#include "syntaxtree.h"

namespace _ELENA_
{

// --- TreeScriptParser ---

class TreeScriptParser : public _Parser
{
   Map<ident_t, int> _tokens;
   Map<ident_t, int> _attributes;

   void parseScope(_ScriptReader& reader, ScriptBookmark& bm, SyntaxWriter& writer, LexicalType type);
   void parseStatement(_ScriptReader& reader, ScriptBookmark& bm, SyntaxWriter& writer);

public:
   virtual bool setPostfix(ident_t)
   {
      return false;
   }

   virtual bool parseGrammarRule(_ScriptReader&)
   {
      return false;
   }

   virtual bool parseGrammarMode(_ScriptReader&)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   TreeScriptParser();
};

} // _ELENA_

#endif // treearserH
