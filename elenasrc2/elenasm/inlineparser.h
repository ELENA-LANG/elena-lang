//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef inlineparserH
#define inlineparserH 1

#include "elena.h"
#include "textsource.h"

namespace _ELENA_
{

typedef String<wchar16_t, 0x100> TempString;

// --- EParseError ---

struct EParseError
{
   int column, row;

   EParseError(int column, int row)
   {
      this->column = column;
      this->row = row;
   }
};

// --- ScriptReader ---

class _ScriptReader
{
public:
   LineInfo  info;
   wchar16_t token[LINE_LEN];

   virtual const wchar16_t* read() = 0;

   virtual void switchDFA(const char** dfa) = 0;

   virtual size_t Position() = 0;

   virtual void seek(size_t position) = 0;
};

// --- TapeWriter ---

class TapeWriter
{
   MemoryDump* _tape;

public:
   void writeCallCommand(const wchar16_t* reference);
   void writeCommand(size_t command, const wchar16_t* param);
   void writeCommand(size_t command, size_t param);
   void writeCommand(size_t command)
   {
      writeCommand(command, (size_t)0);
   }
   void writeEndCommand();
//
////      void copyTape(_Memory* subTape, size_t ptr);
////
////      void parseVMCommand(TokenInfo& token, ScriptReader& reader);
////      void parseCommand(TokenInfo& token, ScriptReader& reader);
////      void parse(ScriptReader& reader);
//
//   size_t Position() { return _tape.Length(); }
//
//   void trim(size_t position)
//   {
//      _tape.trim(position);
//   }
//
//   void* extract()
//   {
//      return _tape.extract();
//   }

   TapeWriter(MemoryDump* tape)
   {
      _tape = tape;
   }
   virtual ~TapeWriter() {}
};

// --- InlineScriptParser ---

class InlineScriptParser
{
   MessageMap _verbs;

   enum Mode
   {
      mdRoot   = 0,
      mdRootOp = 1,
      mdTape   = 2
   };

//   void parseVMCommand(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);
//   void parseNewObject(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);
//   //void parseVariable(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);

   int mapVerb(const wchar16_t* literal);

   //void writeTerminal(TapeWriter& writer, const wchar16_t* token, char state, int col, int row);

   int writeVariable(TapeWriter& writer, int index, int level, Mode mode);

   int parseOperations(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode);
   int parseExpression(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode);
   int parseStatement(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode);
   int parseList(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, char terminator, int level, int& counter, Mode mode);
   void parseStruct(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals);
   void parseAction(TapeWriter& writer, _ScriptReader& reader);

public:
   void parseDirectives(MemoryDump& tape, _ScriptReader& reader);
   void parseScript(MemoryDump& tape, _ScriptReader& reader);

   InlineScriptParser();
};

} // _ELENA_

#endif // inlineparserH
