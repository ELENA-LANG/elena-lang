//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Derivation Tree class implementation
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "derivation.h"

using namespace _ELENA_;

// --- DerivationWriter ---

void DerivationWriter :: unpackNode(SNode node)
{
   Symbol symbol = (Symbol)node.type;
   switch (symbol) {
      case nsClass:
      case nsSymbol:
      case nsStatic:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask));
         unpackChildren(node);
         _writer.closeNode();
         break;
      case tsIdentifier:
      case tsCharacter:
      case tsHexInteger:
      case tsLiteral:
      case tsPrivate:
      case tsReference:
      case tsInteger:
      case tsReal:
      case tsLong:
      case tsWide:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask | lxParameter), node.identifier());
         copyChildren(node);
         _writer.closeNode();
         break;
      default:
         break;
   }
}

void DerivationWriter :: unpackChildren(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      unpackNode(current);

      current = current.nextNode();
   }
}

void DerivationWriter :: copyChildren(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.argLength > 0) {
         _writer.newNode(current.type, current.identifier());
      }
      else _writer.newNode(current.type, current.argument);

      copyChildren(current);

      _writer.closeNode();

      current = current.nextNode();
   }
}

void DerivationWriter :: writeSymbol(Symbol symbol)
{
   if (symbol != nsNone) {
      if (_level > 0) {
         SyntaxWriter tempWriter(_buffer);

         tempWriter.newNode((LexicalType)symbol);
      }
      else _writer.newNode((LexicalType)symbol);

      _level++;
   }
   else {
      _level--;
      if (_level > 0) {
         SyntaxWriter tempWriter(_buffer);

         tempWriter.closeNode();

         if (_level == 1) {
            unpackNode(_buffer.readRoot());
            _buffer.clear();
         }
            
      }
      else _writer.closeNode();
   }
}

void DerivationWriter :: writeTerminal(TerminalInfo& terminal)
{
   // HOT FIX : if there are several constants e.g. #10#13, it should be treated like literal terminal
   if (terminal == tsCharacter && terminal.value.findSubStr(1, '#', terminal.length, -1) != -1) {
      terminal.symbol = tsLiteral;
   }

   if (_level > 1) {
      SyntaxWriter tempWriter(_buffer);

      if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
         // try to use local storage if the quote is not too big
         if (getlength(terminal.value) < 0x100) {
            QuoteTemplate<IdentifierString> quote(terminal.value);
      
            tempWriter.newNode((LexicalType)terminal.symbol, quote);
         }
         else {
            QuoteTemplate<DynamicString<char> > quote(terminal.value);
      
            tempWriter.newNode((LexicalType)terminal.symbol, quote);
         }
      }
      else tempWriter.newNode((LexicalType)terminal.symbol, terminal.value);

      tempWriter.appendNode(lxCol, terminal.col);
      tempWriter.appendNode(lxCol, terminal.col);
      tempWriter.appendNode(lxRow, terminal.row);
      //   _writer->writeDWord(terminal.disp);
      //   _writer->writeDWord(terminal.length);

      tempWriter.closeNode();
   }
}

//// --- DerivationReader ---
//
//DNode DerivationReader :: readRoot()
//{
//   return Node(this, 0, nsNone);
//}
//
//DNode DerivationReader :: readFirstChild(size_t position)
//{
//   _reader->seek(position);
//
//   Symbol current = (Symbol)_reader->getDWord();
//   if (current != nsNone) {
//      position = _reader->Position();
//      current = (Symbol)_reader->getDWord();
//	   if (test(current, mskTerminal)) {
//         readTerminalInfo(current);
//
//		   position = _reader->Position();
//		   if (_reader->readDWord((int&)current)) {
//            return Node(this, position, current);
//         }
//      }
//	   else return Node(this, position, current);
//   }
//   return Node();
//}
//
//DNode DerivationReader :: readNextNode(size_t position)
//{
//   _reader->seek(position);
//
//   int level = 0;
//   Symbol current = nsNone;
//   while (_reader->readDWord((int&)current)) {
//      if (test(current, mskTerminal)) {
//         readTerminalInfo(current);
//         continue;
//      }
//      else if (current==nsNone) {
//         if (level > 1) {
//            level--;
//         }
//		   else break;
//	   }
//      else level++;
//   }
//   position = _reader->Position();
//   if (_reader->readDWord((int&)current)) {
//      return Node(this, position, current);
//   }
//   else return Node();
//}
//
//DNode DerivationReader :: seekSymbol(size_t position, Symbol symbol)
//{
//   _reader->seek(position);
//
//   Symbol current = (Symbol)_reader->getDWord();
//   if (!test(current, mskTerminal)) {
//      int level = 1;
//      while (level > 0) {
//         position = _reader->Position();
//         current = (Symbol)_reader->getDWord();
//         if (test(current, mskTerminal)) {
//            readTerminalInfo(current);
//         }
//         else if (current==nsNone) {
//            level--;
//         }
//         else {
//			   if (level==1 && current==symbol)
//               return Node(this, position, current);
//            level++;
//         }
//      }
//   }
//   return Node();
//}
//
//TerminalInfo DerivationReader :: readTerminalInfo(Symbol symbol)
//{
//   TerminalInfo terminal;
//
//   terminal.symbol = symbol;
//   _reader->readDWord(terminal.disp);
//   _reader->readDWord(terminal.row);
//   _reader->readDWord(terminal.col);
//   _reader->readDWord(terminal.length);
//
//   terminal.value = _reader->getLiteral(DEFAULT_STR);
//
//   return terminal;
//}
//
//TerminalInfo DerivationReader :: readTerminal(size_t position)
//{
//   _reader->seek(position);
//
//   Symbol current = (Symbol)_reader->getDWord();
//   if (current != nsNone) {
//      current = (Symbol)_reader->getDWord();
//      if (test(current, mskTerminal)) {
//         return readTerminalInfo(current);
//      }
//   }
//   return TerminalInfo();
//}
