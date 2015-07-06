//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sessionH
#define sessionH 1

#include "scriptengine.h"

namespace _ELENA_
{

////const int cnNameLength = 0x20;
////
////typedef String<wchar16_t, cnNameLength> RuleIdentifier;
////
////// --- EUnrecognizedException ---
////
////struct EUnrecognizedException
////{
////};
////
////// --- EInvalidExpression ---
////
////struct EInvalidExpression
////{
////   RuleIdentifier nonterminal;
////   int            column, row;
////
////   EInvalidExpression(const wchar16_t* nonterminal, int column, int row)
////   {
////      this->nonterminal.copy(nonterminal);
////      this->column = column;
////      this->row = row;
////   }
////};
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
//   void* getBody() 
//   { 
//      write((wchar16_t)0);
//
//      return _log.get(0); 
//   } 
//
//   size_t Length() const { return _log.Length(); }
//
//   void clear()
//   {
//      _log.trim(0);
//   }
//};

typedef _ELENA_TOOL_::TextSourceReader  SourceReader;
typedef String<ident_c, 0x100> TempString;

// --- Session ---

class Session
{
   class ScriptReader : public _ScriptReader
   {
   protected:
      SourceReader reader;

   public:
      virtual ident_t read();

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
      size_t     _position;

      void cache();

   public:
      virtual size_t Position() 
      { 
         _cacheMode = true;

         return _position; 
      }

      virtual void seek(size_t position)
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
   
      virtual ident_t read();

      CachedScriptReader(TextReader* script)
         : ScriptReader(script)
      {
         _cacheMode = false;
         _position = 0;
      }
   };

   _Parser*         _currentParser;

   String<ident_c, 512> _lastError;

   void parseDirectives(MemoryDump& tape, _ScriptReader& reader);
   void parseMetaScript(MemoryDump& tape, CachedScriptReader& reader);
   void parseScript(MemoryDump& tape, _ScriptReader& reader);

   int translate(TextReader* source, bool standalone);

public:
   ident_t getLastError()
   {
      ident_t error = _lastError;

      return !emptystr(error) ? error : NULL;
   }

   int translate(ident_t script, bool standalone);
   int translate(path_t path, int encoding, bool autoDetect, bool standalone);

   Session();
   virtual ~Session();
};

} // _elena_

#endif // sessionh
