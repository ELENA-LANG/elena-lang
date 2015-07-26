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

//   enum Mode
//   {
//      mdRoot   = 0,
//      mdRootOp = 1,
//      mdTape   = 2
//   };
//
////   void parseVMCommand(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);
////   void parseNewObject(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);
////   //void parseVariable(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);

   int mapVerb(ident_t literal);
//
////   //void writeTerminal(TapeWriter& writer, const wchar16_t* token, char state, int col, int row);
////
////   int writeVariable(TapeWriter& writer, int index, int level, Mode mode);
////
////   ////int parseOperations(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode);
////   ////int parseList(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, char terminator, int level, int& counter, Mode mode);
////   ////void parseAction(TapeWriter& writer, _ScriptReader& reader);

   void readMessage(_ScriptReader& reader, IdentifierString& message, bool subjectOnly = false);

//   int parseMessage(TapeWriter& writer, _ScriptReader& reader, int counter);
//   int parseReverseList(TapeWriter& writer, _ScriptReader& reader);
////   int parseAction(TapeWriter& writer, _ScriptReader& reader);
////   int parseStruct(TapeWriter& writer, _ScriptReader& rveader, Map<const wchar16_t*, int>& locals);
////   int parseObject(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode);
//   int parseExpression(TapeWriter& writer, _ScriptReader& reader/*, Map<const wchar16_t*, int>& locals, int level, Mode mode*/);
////   int parseStatement(/*TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode*/);

   //bool parseToken(_ScriptReader& reader, TapeWriter& writer, int& level, Map<ident_t, int>& locals);
   
   //void parseSend(_ScriptReader& reader, TapeWriter& writer, int& level, Map<ident_t, int>& locals);

   void writeObject(TapeWriter& writer, char state, ident_t value, Map<ident_t, int>& locals);

   void copyDump(MemoryDump& dump, MemoryDump& line, Stack<int>& arguments);
   void writeLine(MemoryDump& line, TapeWriter& writer, Map<ident_t, int>& locals, int level);
   int parseStatement(_ScriptReader& reader, MemoryDump& line, int& level);

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
