//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sessionH
#define sessionH 1

#include "elena.h"
#include "inlineparser.h"

namespace _ELENA_
{

//const int cnNameLength = 0x20;
//
//typedef String<wchar16_t, cnNameLength> RuleIdentifier;
//
//// --- EUnrecognizedException ---
//
//struct EUnrecognizedException
//{
//};
//
//// --- EInvalidExpression ---
//
//struct EInvalidExpression
//{
//   RuleIdentifier nonterminal;
//   int            column, row;
//
//   EInvalidExpression(const wchar16_t* nonterminal, int column, int row)
//   {
//      this->nonterminal.copy(nonterminal);
//      this->column = column;
//      this->row = row;
//   }
//};
//
//// --- Terminal ---
//
//struct Terminal
//{
//   int       col, row;
//   char      state;
//   wchar16_t value[0x100];
//};
//
//// --- ScriptLog ---
//
//class ScriptLog
//{
//   MemoryDump _log;
//
//public:
//   void write(wchar16_t ch);
//   void write(const wchar16_t* token);
//
//   size_t Position()
//   {
//      return _log.Length();
//   }
//
//   void trim(size_t position)
//   {
//      _log.trim(position);
//   }
//
//   void* getBody() { return _log.get(0); } 
//
//   size_t Length() const { return _log.Length(); }
//
//   void clear()
//   {
//      _log.trim(0);
//   }
//};
//
//// --- Parser ---
//
//class _Parser
//{
//public:
//   virtual void parse(TextReader* script, _ScriptCompiler* compiler) = 0;
//};
//
//typedef Map<const wchar16_t*, _Parser*, true> ParserMap;

typedef _ELENA_TOOL_::TextSourceReader  SourceReader;

// --- Session ---

class Session
{
   class ScriptReader : public _ScriptReader
   {
   protected:
      SourceReader reader;

   public:
      virtual const wchar16_t* read();

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
      size_t     _position;

      void cache();

   public:
      size_t Position() 
      { 
         _cacheMode = true;

         return _position; 
      }

      void seek(size_t position)
      {
         _position = position;
      }
   
//      void reread(TokenInfo& token);

      void clearCache()
      {
         _cacheMode = false;
         if (_position >= _buffer.Length()) {
            _buffer.trim(0);
            _position = 0;
         }
      }
   
      virtual const wchar16_t* read();

      CachedScriptReader(TextReader* script)
         : ScriptReader(script)
      {
         _cacheMode = false;
         _position = 0;
      }
   };

   InlineScriptParser _scriptParser;

//   ParserMap              _parsers;  

   String<wchar16_t, 512> _lastError;

   void parseMetaScript(MemoryDump& tape, CachedScriptReader& reader);
   void parseScript(MemoryDump& tape, _ScriptReader& reader);

   int translate(TextReader* source);

public:
   const wchar16_t* getLastError()
   {
      const wchar16_t* error = _lastError;

      return !emptystr(error) ? error : NULL;
   }

   int translate(const wchar16_t* script);
   int translate(const wchar16_t* path, int encoding, bool autoDetect);
//   void free(void* tape);

   Session();
   virtual ~Session();
};

} // _ELENA_

#endif // sessionH