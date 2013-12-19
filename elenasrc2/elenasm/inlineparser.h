//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef inlineparserH
#define inlineparserH 1

#include "elena.h"
//#include "textsource.h"
#include "session.h"
#include "textsource.h"

namespace _ELENA_
{

// --- TapeWriter ---

class TapeWriter
{
   MemoryDump _tape;

public:
   void writeCallCommand(const wchar16_t* reference);
   void writeCommand(size_t command, const wchar16_t* param);
   void writeCommand(size_t command, size_t param);
   void writeCommand(size_t command)
   {
      writeCommand(command, (size_t)0);
   }
   void writeEndCommand();

//      void copyTape(_Memory* subTape, size_t ptr);
//
//      void parseVMCommand(TokenInfo& token, ScriptReader& reader);
//      void parseCommand(TokenInfo& token, ScriptReader& reader);
//      void parse(ScriptReader& reader);

   size_t Position() { return _tape.Length(); }

   void trim(size_t position)
   {
      _tape.trim(position);
   }

   void* extract()
   {
      return _tape.extract();
   }

   TapeWriter();
   virtual ~TapeWriter() {}
};

// --- ScriptCompiler ---

class ScriptVMCompiler : public _ScriptCompiler
{
   TapeWriter _writer;

   void parseVMCommand(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);
   ////void parseAction(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token/*, Terminal* terminal*/);
   void parseMessage(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token, int command);
   //void parseDynamicArray(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token, Terminal* terminal);
   //void parseVariable(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);

   void parseTerminal(const wchar16_t* token, char state, int row, int column);

public:
   virtual void compile(TextReader* script);

   virtual void* generate();

   //virtual size_t Position()
   //{
   //   return _writer.Position();
   //}

   //virtual void trim(size_t position)
   //{
   //   _writer.trim(position);
   //}

   ScriptVMCompiler()
   {
   }
};

} // _ELENA_

#endif // inlineparserH
