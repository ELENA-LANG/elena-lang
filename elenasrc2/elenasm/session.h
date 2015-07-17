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


// --- Session ---

class Session
{
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
