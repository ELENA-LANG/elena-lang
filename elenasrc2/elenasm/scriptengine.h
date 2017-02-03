//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef scriptEngineH
#define scriptEngineH 1

#include "elena.h"
#include "textsource.h"

namespace _ELENA_
{

typedef _ELENA_TOOL_::TextSourceReader  SourceReader;
typedef String<char, 0x100> TempString;

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

// --- EInvalidOperation ---

class EInvalidOperation
{
   ident_t error;

public:
   ident_t Error() { return error; }

   EInvalidOperation(ident_t error)
   {
      this->error = error;
   }
};

// --- TapeWriter ---

class TapeWriter
{
   MemoryDump* _tape;

public:
   int Bookmark()
   {
      return _tape->Length();
   }

   void writeCallCommand(ident_t reference)
   {
      writeCommand(CALL_TAPE_MESSAGE_ID, reference);
   }

   void writeCommand(pos_t command, ident_t param)
   {
      MemoryWriter writer(_tape);

      writer.writeDWord(command);
      if (!emptystr(param)) {
         writer.writeDWord(getlength(param) + 1);
         writer.writeLiteral(param, getlength(param) + 1);
      }
      else writer.writeDWord(0);
   }

   void writeCommand(pos_t command, pos_t param)
   {
      MemoryWriter writer(_tape);

      // save tape record
      writer.writeDWord(command);
      writer.writeDWord(param);
   }

   void insertCommand(int bookmark, pos_t command, ident_t param)
   {
      if (!emptystr(param)) {
         _tape->insert(bookmark, NULL, getlength(param) + 1);
      }

      _tape->insert(bookmark, NULL, 8);

      MemoryWriter writer(_tape, bookmark);

      writer.writeDWord(command);
      if (!emptystr(param)) {
         writer.writeDWord(getlength(param) + 1);
         writer.writeLiteral(param, getlength(param) + 1);
      }
      else writer.writeDWord(0);
   }

   void insertCommand(int bookmark, pos_t command, pos_t param)
   {
      _tape->insert(bookmark, NULL, 8);

      MemoryWriter writer(_tape, bookmark);

      // save tape record
      writer.writeDWord(command);
      writer.writeDWord(param);
   }

   void writeCommand(pos_t command)
   {
      writeCommand(command, (pos_t)0);
   }

   void writeEndCommand()
   {
      MemoryWriter writer(_tape);

      writer.writeDWord(0);
   }

   TapeWriter(MemoryDump* tape)
   {
      _tape = tape;
   }
   virtual ~TapeWriter() {}
};

struct ScriptBookmark
{
   pos_t offset;
   int   state;
   int   row;
   int   column;

   ScriptBookmark()
   {
      offset = 0;
      state = 0;
      row = column = 0;
   }
   ScriptBookmark(pos_t offset, int state)
   {
      this->state = state;
      this->offset = offset;
      this->row = this->column = 0;
   }
   ScriptBookmark(pos_t offset, int state, int row, int column)
   {
      this->state = state;
      this->offset = offset;
      this->row = row;
      this->column = column;
   }
};

// --- ScriptReader ---

class _ScriptReader
{
public:
   virtual bool Eof() = 0;

   virtual ScriptBookmark read() = 0;

   virtual ident_t lookup(ScriptBookmark& bm) = 0;

   virtual bool compare(ident_t value) = 0;

   virtual void reset() = 0;

   virtual void switchDFA(const char** dfa) = 0;
};

class ScriptReader : public _ScriptReader
{
protected:
   SourceReader reader;
   MemoryDump   buffer;
   char         token[LINE_LEN];
   bool         eof;

public:
   virtual bool Eof()
   {
      return eof;
   }

   virtual bool compare(ident_t value)
   {
      return value.compare(token);
   }

   virtual ScriptBookmark read()
   {
      ScriptBookmark bm;

      LineInfo info = reader.read(token, LINE_LEN);

      bm.offset = buffer.Length();
      bm.state = info.state;
      bm.column = info.column;
      bm.row = info.row;
      if (info.state == _ELENA_TOOL_::dfaEOF)
         eof = true;

      if (info.state == _ELENA_TOOL_::dfaQuote && token != NULL) {
         QuoteTemplate<TempString> quote(info.line);
      
         //!!HOTFIX: what if the literal will be longer than 0x100?
         size_t length = quote.Length();
         Convertor::copy(token, quote.ident(), length, length);
         token[length] = 0;
      }

      MemoryWriter writer(&buffer);
      writer.writeLiteral(token);

      return bm;
   }

   virtual ident_t lookup(ScriptBookmark& bm)
   {
      if (bm.offset < 0)
         return NULL;

      MemoryReader reader(&buffer, bm.offset);

      return reader.getLiteral(DEFAULT_STR);
   }

   virtual void reset()
   {
      eof = false;
      reader.reset();
      buffer.clear();
   }

   virtual void switchDFA(const char** dfa)
   {
      reader.switchDFA(dfa);
   }

   ScriptReader(TextReader* script)
      : reader(4, script)
   {
      eof = false;
   }
};

// --- ScriptLog ---

class ScriptLog
{
   MemoryDump _log;

public:
   void write(char ch)
   {
      MemoryWriter writer(&_log);

      writer.writeChar(ch);
   }
   void write(ident_t token)
   {
      size_t length = getlength(token);
      if (length > 0) {
         MemoryWriter writer(&_log);

         writer.writeLiteral(token, length);
      }
   }

   static void writeQuote(MemoryWriter& writer, ident_t token)
   {
      writer.writeChar('\"');
      size_t start = 0;
      while (true) {
         size_t quote_index = token.find(start, '"', NOTFOUND_POS);
         if (quote_index != NOTFOUND_POS) {
            writer.writeLiteral(token.c_str() + start, quote_index - start);
            writer.writeChar('\"');
            writer.writeChar('\"');
            start = quote_index + 1;
         }
         else {
            writer.writeLiteral(token.c_str() + start, getlength(token) - start);

            break;
         }
      }

      writer.writeChar('\"');
   }

   void writeQuote(ident_t token)
   {
      MemoryWriter writer(&_log);

      writeQuote(writer, token);
   }
   void writeInt(int number)
   {
      MemoryWriter writer(&_log);

      IdentifierString num;
      num.copyInt(number);

      writer.writeLiteral(num);
   }

   void* getBody() 
   { 
      write((char)0);

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
   virtual bool parseGrammarMode(_ScriptReader& reader) = 0;
   virtual void parse(_ScriptReader& reader, MemoryDump* output) = 0;

   virtual ~_Parser() {}
};

} // _elena_

#endif // scriptEngineH
