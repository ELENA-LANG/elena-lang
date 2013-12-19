//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sessionH
#define sessionH 1

#include "elena.h"

namespace _ELENA_
{

// translate mode constants
#define MASK_MODE        0x000FF
//#define SCRIPT_MODE      0x00000
#define CFGRAMMAR_MODE   0x00001
//#define LALRDSARULE_MODE 0x00002

// translate attribute constants
#define TRACE_MODE       0x20000
#define SYMBOLIC_MODE    0x00100

const int cnNameLength = 0x20;

typedef String<wchar16_t, 0x100> TempString;
typedef String<wchar16_t, cnNameLength> RuleIdentifier;

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

// --- EUnrecognizedException ---

struct EUnrecognizedException
{
};

// --- EInvalidExpression ---

struct EInvalidExpression
{
   RuleIdentifier nonterminal;
   int            column, row;

   EInvalidExpression(const wchar16_t* nonterminal, int column, int row)
   {
      this->nonterminal.copy(nonterminal);
      this->column = column;
      this->row = row;
   }
};

// --- Terminal ---

struct Terminal
{
   int       col, row;
   char      state;
   wchar16_t value[0x100];
};

// --- ScriptCompiler ---

class _ScriptCompiler
{
public:
   //virtual size_t Position() = 0;
   //virtual void trim(size_t position) = 0;

   virtual void compile(TextReader* source) = 0;

   virtual void* generate() = 0;
};

// --- ScriptLog ---

class ScriptLog //: public _ScriptCompiler
{
   MemoryDump _log;

public:
   void write(wchar16_t ch);
   void write(const wchar16_t* token);

//   virtual size_t Position()
//   {
//      return _log.Length();
//   }
//
//   virtual void trim(size_t position)
//   {
//      _log.trim(position);
//   }

   void* getBody() { return _log.get(0); } 

   void clear()
   {
      _log.trim(0);
   }

//   virtual void compile(TextReader* source, Terminal* terminal);
//
//   virtual void* generate();
};

// --- Parser ---

class _Parser
{
public:
   virtual void parse(TextReader* script, _ScriptCompiler* compiler) = 0;
};

typedef Map<const wchar16_t*, _Parser*, true> ParserMap;

// --- Session ---

class Session
{
   ParserMap              _parsers;  

   String<wchar16_t, 512> _lastError;

//   _Parser* createParser(const wchar16_t* name);

   void* translateScript(const wchar16_t* names, TextReader* source);
   void* traceScript(const wchar16_t* names, TextReader* source);

   void* translate(const wchar16_t* name, TextReader* source, int mode);

public:
   const wchar16_t* getLastError()
   {
      const wchar16_t* error = _lastError;

      return !emptystr(error) ? error : NULL;
   }

   void* translate(const wchar16_t* name, const wchar16_t* script, int mode);
   void* translate(const wchar16_t* names, const wchar16_t* path, int encoding, bool autoDetect, int mode);
   void free(void* tape);

   Session();
   virtual ~Session();
};

} // _ELENA_

#endif // sessionH