//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef inlineparserH
#define inlineparserH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- VMTapeParser ---

class VMTapeParser : public _Parser
{
   TempString _postfix;

   bool parseMessage(ident_t message, IdentifierString& reference);
   bool writeMessage(TapeWriter& writer, ident_t message, int command);

   int writeBuildScriptArgumentList(_ScriptReader& reader, ScriptBookmark terminator, Stack<ScriptBookmark>& callStack, 
                                    TapeWriter& writer);
   void writeBuildScriptStatement(_ScriptReader& reader, ScriptBookmark terminator, Stack<ScriptBookmark>& callStack, 
                                    TapeWriter& writer);

   void parseBuildScriptArgumentList(_ScriptReader& reader, Stack<ScriptBookmark>& callStack);
   void parseBuildScriptStatement(_ScriptReader& reader, ScriptBookmark& bm, Stack<ScriptBookmark>& callStack, int exprBookmark);

   void parseInlineStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer);

   void parseInlineScript(TapeWriter& writer, _ScriptReader& reader);
   bool parseBuildScript(TapeWriter& writer, _ScriptReader& reader);
   void parseInline(ident_t script, TapeWriter& writer);

public:
   virtual bool setPostfix(ident_t postfix_script)
   {
      if (_postfix.Length() == 0) {
         _postfix.copy(postfix_script);

         return true;
      }
      else return false;
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

   VMTapeParser();
};

} // _ELENA_

#endif // inlineparserH
