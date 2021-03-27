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
#include "source.h"
#include "syntaxtree.h"

namespace _ELENA_
{

// --- ELENA Parser constants ---
constexpr int mskError        = 0x04000;
constexpr int mskTraceble     = 0x01000;

constexpr int nsStart         = 0x00001;
constexpr int tsEof           = 0x03003;

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

// --- TerminalInfo structure ---
struct TerminalInfo
{
   LexicalType symbol;

   pos_t       disp;          // number of symbols (tab considered as a single char)
   pos_t       row;
   pos_t       col;           // virtual column
   pos_t       length;
   ident_t     value;

   operator ident_t() const { return value; }

   bool operator == (TerminalInfo& terminal) const
   {
      return (symbol == terminal.symbol && value.compare(terminal.value));
   }

   bool operator != (TerminalInfo& terminal) const
   {
      return (symbol != terminal.symbol || !value.compare(terminal.value));
   }

   bool operator == (const int& symbol) const
   {
      return (this->symbol == symbol);
   }

   bool operator != (const int& symbol) const
   {
      return (this->symbol != symbol);
   }

   int Row() const { return row; }

   int Col() const { return col; }

   TerminalInfo()
   {
      this->symbol = lxNone;
      this->value = nullptr;
      disp = row = col = length = 0;
   }
};

// --- _DerivationWriter ---

class _DerivationWriter
{
public:
   virtual void newNode(LexicalType symbol) = 0;
   virtual void appendTerminal(TerminalInfo& terminal) = 0;
   virtual void closeNode() = 0;
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
