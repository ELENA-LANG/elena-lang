//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef scriptEngineH
#define scriptEngineH 1

#include "elena.h"
#include "textsource.h"

namespace _ELENA_
{
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

// --- TapeWriter ---

class TapeWriter
{
   MemoryDump* _tape;

public:
   void writeCallCommand(ident_t reference)
   {
      writeCommand(CALL_TAPE_MESSAGE_ID, reference);
   }

   void writeCommand(size_t command, ident_t param)
   {
      MemoryWriter writer(_tape);

      writer.writeDWord(command);
      if (!emptystr(param)) {
         writer.writeDWord(getlength(param) + 1);
         writer.writeLiteral(param, getlength(param) + 1);
      }
      else writer.writeDWord(0);
   }

   void writeCommand(size_t command, size_t param)
   {
      MemoryWriter writer(_tape);

      // save tape record
      writer.writeDWord(command);
      writer.writeDWord(param);
   }

   void writeCommand(size_t command)
   {
      writeCommand(command, (size_t)0);
   }

   void writeEndCommand()
   {
      MemoryWriter writer(_tape);

      writer.writeDWord(0);
   }

   void insert(size_t position, MemoryDump* subTape)
   {
      _tape->insert(position, subTape->get(0), subTape->Length());
   }

   ////      void parseVMCommand(TokenInfo& token, ScriptReader& reader);
   ////      void parseCommand(TokenInfo& token, ScriptReader& reader);
   ////      void parse(ScriptReader& reader);

   size_t Position() { return _tape->Length(); }

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

// --- ScriptReader ---

class _ScriptReader
{
public:
   LineInfo  info;
   ident_c   token[LINE_LEN];

   virtual ident_t read() = 0;

//   virtual void switchDFA(const char** dfa) = 0;

   virtual size_t Position() = 0;

   virtual void seek(size_t position) = 0;
};


// --- ScriptLog ---

class ScriptLog
{
   MemoryDump _log;

public:
   void write(ident_c ch)
   {
      MemoryWriter writer(&_log);

      writer.writeChar(ch);
   }
   void write(ident_t token)
   {
      MemoryWriter writer(&_log);
      
      writer.writeLiteral(token, getlength(token));
      writer.writeChar(' ');
   }

   size_t Position()
   {
      return _log.Length();
   }

   void trim(size_t position)
   {
      _log.trim(position);
   }

   void* getBody() 
   { 
      write((ident_c)0);

      return _log.get(0); 
   } 

   size_t Length() const { return _log.Length(); }

   void clear()
   {
      _log.trim(0);
   }
};

// --- Parser ---

class _Parser
{
public:
   virtual bool parseGrammarRule(_ScriptReader& reader) = 0;  
   virtual void parse(_ScriptReader& reader, TapeWriter& writer) = 0;

   virtual ~_Parser() {}
};

} // _elena_

#endif // scriptEngineH
