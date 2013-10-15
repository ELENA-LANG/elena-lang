//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Parser class declaration.
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef parserH
#define parserH 1

#include "parsertable.h"
#include "derivation.h"
#include "source.h"

namespace _ELENA_
{

// --- SyntaxError ---

class SyntaxError : _Exception
{
public:
   const char*    error; 
   const _text_t* token;
   int            column, row;

   SyntaxError(int column, int row, const wchar16_t* token);
   SyntaxError(int column, int row, const wchar16_t* token, const char* error);
};

// --- Parser class ---

class Parser
{
   wchar16_t _buffer[IDENTIFIER_LEN + 1];

   ParserTable _table;

   bool derive(TerminalInfo& terminal, ParserStack& stack, DerivationWriter* writer, bool& traceble);

public:
   void parse(TextReader* reader, DerivationWriter* writer, int tabSize);

   Parser(StreamReader* syntax);
   ~Parser() {}
};

} // _ELENA_

#endif // parserH
