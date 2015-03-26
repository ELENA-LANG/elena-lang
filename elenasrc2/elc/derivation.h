//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Derivation Tree classes
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef derivationH
#define derivationH 1

#include "syntax.h"

namespace _ELENA_
{

// --- DerivationReader class ---
class DerivationReader
{
   StreamReader* _reader;

public:
   // --- TerminalInfo structure ---
   struct TerminalInfo
   {
      Symbol   symbol;

      size_t   disp;          // number of symbols (tab considered as a single char)
      size_t   row;
      size_t   col;           // virtual column
      size_t   length;
      ident_t  value;

      operator ident_t() const { return value; }

      bool operator == (const TerminalInfo& terminal) const
      {
         return (symbol == terminal.symbol && StringHelper::compare(value, terminal.value));
      }

      bool operator != (const TerminalInfo& terminal) const
      {
         return (symbol != terminal.symbol || !StringHelper::compare(value, terminal.value));
      }

      bool operator == (const Symbol& symbol) const
      {
         return (this->symbol == symbol);
      }

      bool operator != (const Symbol& symbol) const
      {
         return (this->symbol != symbol);
      }

      int Row() const { return row; }

      int Col() const { return col; }

      TerminalInfo()
      {
         this->symbol = nsNone;
         this->value = NULL;
      }
   };

   // --- Node structure ---
   struct Node
   {
      DerivationReader* reader;
      size_t            position;
      Symbol            symbol;

      operator Symbol() const { return symbol; }

      bool operator == (const Node& node) const
      {
         return (this->position == node.position);
      }
      bool operator != (const Node& node) const
      {
         return (this->position != node.position);
      }

      TerminalInfo FirstTerminal()
      {
         TerminalInfo terminal = Terminal();
         if (terminal == nsNone) {
            terminal = reader ? firstChild().FirstTerminal() : TerminalInfo();
         }

         return terminal;
      }

      TerminalInfo Terminal()
      {
         return reader ? reader->readTerminal(position) : TerminalInfo();
      }

      Node firstChild() const
      {
         return reader ? reader->readFirstChild(position) : Node();
      }

      Node lastNode() const
      {
         Node last;
         Node node = firstChild();
         while (node != nsNone) {
            last = node;

            node = node.nextNode();
         }
         return last;
      }

      Node nextNode() const
      {
         return reader ? reader->readNextNode(position) : Node();
      }

      Node select(Symbol symbol) const
      {
         return reader ? reader->seekSymbol(position, symbol) : Node();
      }

      Node()
      {
         this->reader = NULL;
         this->position = 0;
         this->symbol = nsNone;
      }
      Node(DerivationReader* reader, size_t position, Symbol symbol)
      {
         this->reader = reader;
         this->position = position;
         this->symbol = symbol;
      }
   };

   Node readRoot();
   Node readFirstChild(size_t position);
   Node readNextNode(size_t position);

   Node seekSymbol(size_t position, Symbol symbol);

   TerminalInfo readTerminal(size_t position);
   TerminalInfo readTerminalInfo(Symbol symbol);

   DerivationReader(StreamReader* reader)
   {
      _reader = reader;
   }
};

// --- DerivationWriter ---

class DerivationWriter
{
   StreamWriter* _writer;

public:
   void writeSymbol(Symbol symbol);
   void writeTerminal(DerivationReader::TerminalInfo terminal);

   DerivationWriter(StreamWriter* writer)
   {
      _writer = writer;
   } 
};

// common type definitions
typedef DerivationReader::Node         DNode;
typedef DerivationReader::TerminalInfo TerminalInfo;

} // _ELENA_

#endif // derivationH
