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

   SyntaxTree   _buffer;
   SNode        _hints;

   int          _level; // tree node level ; starting from level=1 the content is buffered and unpacked on the level=0

   void unpackNode(SNode& node, int mode);
   void unpackChildren(SNode node, int mode = 0);
   void copyChildren(SNode node);

   void copyExpression(SNode node, bool explicitOne = true);
   void copyObject(SNode node, int mode);
   void copyMessage(SNode node, bool operationMode = false);
//   void copyVariable(SNode node);
//   void copyAssigning(SNode node);
//   void copySwitching(SNode node);
   void copyHints(SNode node);

public:
   void writeSymbol(Symbol symbol);
   void writeTerminal(TerminalInfo& terminal);

   DerivationWriter(SyntaxTree& target)
      : _writer(target)
   {
      _level = 0;
   } 
};

} // _ELENA_

#endif // derivationH
