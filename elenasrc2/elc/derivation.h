//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Derivation Tree classes
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef derivationH
#define derivationH 1

#include "syntax.h"
#include "syntaxtree.h"

namespace _ELENA_
{

// --- DerivationWriter ---

class DerivationWriter : public _DerivationWriter
{
   SyntaxWriter _writer;

   void writeNode(Symbol symbol);

public:
   void writeSymbol(Symbol symbol);
   void writeTerminal(TerminalInfo& terminal);

   DerivationWriter(SyntaxTree& target)
      : _writer(target)
   {
   }
};

} // _ELENA_

#endif // derivationH
