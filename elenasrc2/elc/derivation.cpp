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
      case nsMethod:
      case nsConstructor:
      case nsSubCode:
      case nsTemplate:
      case nsField:
      case nsSubject:
      case nsNestedClass:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask));
         if (_hints != lxNone) {
            copyHints(_hints);
            _hints = lxNone;
         }            
         unpackChildren(node);
         _writer.closeNode();
         break;
      case nsImport:
         _writer.newNode(lxImport);
         unpackChildren(node);
         _writer.closeNode();
         break;
      case nsForward:
         _writer.newNode(lxForward);
         unpackChildren(node);
         _writer.closeNode();
         break;
      case nsBaseClass:
         _writer.newNode(lxBaseParent);
         unpackChildren(node);
         _writer.closeNode();
         break;
      case nsMethodParameter:
         _writer.newNode(lxMethodParameter);
         unpackChildren(node);
         _writer.closeNode();
         break;
      case nsSubjectArg:
         _writer.newNode(lxMessage);
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
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask | lxParameter | lxObjectMask));
         copyChildren(node);
         _writer.closeNode();
         break;
      case nsRootExpression:
         node = lxExpression;
      case nsExpression:
      case nsDispatchHandler:
      case nsRetStatement:
         _writer.newBookmark();
         copyExpression(node);
         _writer.removeBookmark();
         break;
      case nsVariable:
         copyVariable(node);
         break;
      case nsAssigning:
         copyAssigning(node);
         break;
      case nsMessageOperation:
         copyMessage(node);
         break;
      case nsL4Operation:
      case nsL7Operation:
         copyMessage(node, true);
         break;
      case nsObject:
         copyObject(node);
         break;
      case nsCodeEnd:
         _writer.newNode(lxEOF);
         copyChildren(node.firstChild());
         _writer.closeNode();
         break;
      case nsHint:
         if (_hints == lxNone) {
            _hints = node;
         }
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
   SyntaxTree::copyNode(_writer, node);
}

void DerivationWriter :: copyExpression(SNode node)
{
   _writer.newNode((LexicalType)(node.type & ~mskAnySymbolMask | lxExprMask));
   SNode current = node.firstChild();
   while (current != lxNone) {
      unpackNode(current);

      current = current.nextNode();
   }
   _writer.closeNode();
}

void DerivationWriter :: copyObject(SNode node)
{
   unpackChildren(node);
}

void DerivationWriter :: copyMessage(SNode node, bool operationMode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch ((Symbol)current.type) {
         case tsIdentifier:
         case tsPrivate:
         case tsReference:
            _writer.newNode(lxMessage);
            unpackNode(current);
            _writer.closeNode();
            break;
         case nsSubjectArg:
            _writer.newNode(lxMessage);
            unpackChildren(current);
            _writer.closeNode();
            break;
         case nsMessageParameter:
         case nsObject:
            _writer.newBookmark();
            _writer.newNode(lxExpression);
            unpackChildren(current);
            _writer.closeNode();
            _writer.removeBookmark();
            break;
         case nsSubCode:
            _writer.newNode(lxExpression);
            unpackNode(current);
            _writer.closeNode();
            break;
         default:
            if (operationMode && current.existChild(lxTerminal)) {
               _writer.newNode(lxOperator);
               copyChildren(current);
               _writer.closeNode();
            }            
            break;
      }
      current = current.nextNode();
   }

   _writer.insert(lxExpression);
   _writer.closeNode();
}

void DerivationWriter :: copyVariable(SNode node)
{
   SNode local = node.firstChild();

   _writer.newNode(lxVariable);

   if (_hints != lxNone) {
      copyHints(_hints);
      _hints = lxNone;
   }

   unpackNode(local);
   _writer.closeNode();

   SNode current = node.findChild((LexicalType)nsAssigning);
   if (current != lxNone) {
      _writer.newNode(lxExpression);
      _writer.appendNode(lxAssign);
      unpackNode(local);

      unpackChildren(current);

      _writer.closeNode();
   }
}

void DerivationWriter :: copyAssigning(SNode node)
{
   _writer.appendNode(lxAssign);

   unpackChildren(node);
}

void DerivationWriter :: copyHints(SNode current)
{
   while (((LexicalType)current.type) == nsHint) {
      _writer.newNode(lxAttribute);
      unpackChildren(current);
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
            SNode root = _buffer.readRoot();

            // hints should be injected into the buffer
            if ((LexicalType)root.type == nsHint) {
               if (_hints == lxNone)
                  _hints = root;

               // skipping hints
               while ((LexicalType)root.type == nsHint)
                  root = root.nextNode();
            }
            if (root.type != lxNone) {
               unpackNode(root);
               _buffer.clear();
            }
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

      tempWriter.newNode((LexicalType)terminal.symbol);

      if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
         // try to use local storage if the quote is not too big
         if (getlength(terminal.value) < 0x100) {
            QuoteTemplate<IdentifierString> quote(terminal.value);
      
            tempWriter.appendNode(lxTerminal, quote);
         }
         else {
            QuoteTemplate<DynamicString<char> > quote(terminal.value);
      
            tempWriter.appendNode(lxTerminal, quote);
         }
      }
      else tempWriter.appendNode(lxTerminal, terminal.value);

      tempWriter.appendNode(lxCol, terminal.col);
      tempWriter.appendNode(lxRow, terminal.row);
      tempWriter.appendNode(lxLength, terminal.length);
      //   _writer->writeDWord(terminal.disp);

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
