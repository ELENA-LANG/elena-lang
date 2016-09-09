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

//// --- DerivationReader class ---
//class DerivationReader
//{
//   StreamReader* _reader;
//
//public:
//   // --- Node structure ---
//   struct Node
//   {
//      DerivationReader* reader;
//      size_t            position;
//      Symbol            symbol;
//
//      operator Symbol() const { return symbol; }
//
//      bool operator == (const Node& node) const
//      {
//         return (this->position == node.position);
//      }
//      bool operator != (const Node& node) const
//      {
//         return (this->position != node.position);
//      }
//
//      TerminalInfo FirstTerminal()
//      {
//         TerminalInfo terminal = Terminal();
//         if (terminal == nsNone) {
//            terminal = reader ? firstChild().FirstTerminal() : TerminalInfo();
//         }
//
//         return terminal;
//      }
//
//      TerminalInfo Terminal()
//      {
//         return reader ? reader->readTerminal(position) : TerminalInfo();
//      }
//
//      Node firstChild() const
//      {
//         return reader ? reader->readFirstChild(position) : Node();
//      }
//
//      Node lastNode() const
//      {
//         Node last;
//         Node node = firstChild();
//         while (node != nsNone) {
//            last = node;
//
//            node = node.nextNode();
//         }
//         return last;
//      }
//
//      Node nextNode() const
//      {
//         return reader ? reader->readNextNode(position) : Node();
//      }
//
//      Node select(Symbol symbol) const
//      {
//         return reader ? reader->seekSymbol(position, symbol) : Node();
//      }
//
//      Node()
//      {
//         this->reader = NULL;
//         this->position = 0;
//         this->symbol = nsNone;
//      }
//      Node(DerivationReader* reader, size_t position, Symbol symbol)
//      {
//         this->reader = reader;
//         this->position = position;
//         this->symbol = symbol;
//      }
//   };
//
//   Node readRoot();
//   Node readFirstChild(size_t position);
//   Node readNextNode(size_t position);
//
//   Node seekSymbol(size_t position, Symbol symbol);
//
//   TerminalInfo readTerminal(size_t position);
//   TerminalInfo readTerminalInfo(Symbol symbol);
//
//   DerivationReader(StreamReader* reader)
//   {
//      _reader = reader;
//   }
//};

// --- DerivationWriter ---

class DerivationWriter : public _DerivationWriter
{
   SyntaxWriter _writer;

   SyntaxTree   _buffer;

   int          _level; // tree node level ; starting from level=1 the content is buffered and unpacked on the level=0

   void unpackNode(SNode node);
   void unpackChildren(SNode node);
   void copyChildren(SNode node);

public:
   void writeSymbol(Symbol symbol);
   void writeTerminal(TerminalInfo& terminal);

   DerivationWriter(SyntaxTree& target)
      : _writer(target)
   {
      _level = 0;
   } 
};

//// common type definitions
//typedef DerivationReader::Node         DNode;

} // _ELENA_

#endif // derivationH
