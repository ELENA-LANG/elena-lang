//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Parser class declaration.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef parserH
#define parserH 1

#include "parsertable.h"
#include "syntax.h"
#include "source.h"

namespace _ELENA_
{

const char _eof_message[] = { '<', 'e', 'n', 'd', ' ', 'o', 'f', ' ', 'f', 'i', 'l', 'e', '>', 0 };

// --- SyntaxError ---

class SyntaxError : public _Exception
{
public:
   const char* error; 
   ident_t     token;
   int         column, row;

   SyntaxError(int column, int row, ident_t token);
   SyntaxError(int column, int row, ident_t token, const char* error);
};

// --- Parser class ---

class Parser
{
   char _buffer[IDENTIFIER_LEN + 1];

   ParserTable _table;

   bool derive(TerminalInfo& terminal, ParserStack& stack, _DerivationWriter& writer, bool& traceble);

public:
   void parse(TextReader* reader, _DerivationWriter& writer, int tabSize);

   Parser(StreamReader* syntax);
   ~Parser() {}
};

} // _ELENA_

#endif // parserH
