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

typedef _ELENA_TOOL_::TextSourceReader  SourceReader;
typedef String<ident_c, 0x100> TempString;

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

class ScriptReader : public _ScriptReader
{
protected:
   SourceReader reader;

public:
   virtual ident_t read()
   {
      info = reader.read(token, LINE_LEN);
      if (info.state == _ELENA_TOOL_::dfaQuote) {
         QuoteTemplate<TempString> quote(info.line);

         //!!HOTFIX: what if the literal will be longer than 0x100?
         size_t length = getlength(quote);
         StringHelper::copy(token, quote, length, length);
         token[length] = 0;
      }

      return token;
   }

   virtual void switchDFA(const char** dfa)
   {
      reader.switchDFA(dfa);
   }

   ScriptReader(TextReader* script)
      : reader(4, script)
   {
   }
};

class CachedScriptReader : public ScriptReader
{
protected:
   bool       _cacheMode;
   MemoryDump _buffer;
   size_t     _tokenPosition;
   size_t     _position;

   void cache()
   {
      ScriptReader::read();

      MemoryWriter writer(&_buffer);

      writer.writeDWord(info.column);
      writer.writeDWord(info.row);
      writer.writeChar(info.state);
      writer.writeLiteral(token, getlength(token) + 1);
   }

public:
   virtual size_t Position()
   {
      _cacheMode = true;

      return _tokenPosition;
   }

   virtual void seek(size_t position)
   {
      _tokenPosition = position;
      if (_tokenPosition != -1) {
         // Restore token
         MemoryReader reader(&_buffer, position);

         ident_t s = reader.getLiteral(DEFAULT_STR);
         size_t length = getlength(s);
         StringHelper::copy(token, s, length, length);

         _position = reader.Position();
      }
      else _position = 0;
   }

   //      void reread(TokenInfo& token);

   void clearCache()
   {
      _cacheMode = false;
   }

   virtual ident_t read()
   {
      // read from the outer reader if the cache is empty
      if (_cacheMode && _position >= _buffer.Length()) {
         cache();
      }

      // read from the cache
      if (_position < _buffer.Length()) {
         MemoryReader reader(&_buffer, _position);

         reader.readDWord(info.column);
         reader.readDWord(info.row);
         reader.readChar(info.state);

         _tokenPosition = reader.Position();

         ident_t s = reader.getLiteral(DEFAULT_STR);
         size_t length = getlength(s);
         StringHelper::copy(token, s, length, length);

         _position = reader.Position();

         return token;
      }
      else {
         // token itself should be always cached
         _buffer.clear();
         _tokenPosition = 0;

         ident_t s = ScriptReader::read();

         MemoryWriter writer(&_buffer);

         writer.writeLiteral(s, getlength(s) + 1);
         _position = writer.Position();

         return s;
      }
   }

   CachedScriptReader(TextReader* script)
      : ScriptReader(script)
   {
      _cacheMode = false;
      _position = 0;

      _tokenPosition = -1;
   }
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
