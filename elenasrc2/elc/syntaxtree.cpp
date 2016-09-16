//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree class implementation
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syntaxtree.h"
//#include <stdarg.h>

using namespace _ELENA_;

//SyntaxTree::Node _ELENA_::findSubNode(SyntaxTree::Node node, LexicalType type)
//{
//   SyntaxTree::Node child = SyntaxTree::findChild(node, type, lxExpression);
//   if (child == lxExpression)
//   {
//      return SyntaxTree::findChild(child, type);
//   }
//   else return child;
//}
//
//SyntaxTree::Node _ELENA_::findSubNode(SyntaxTree::Node node, LexicalType type1, LexicalType type2)
//{
//   SyntaxTree::Node child = SyntaxTree::findChild(node, type1, type2, lxExpression);
//   if (child == lxExpression)
//   {
//      return SyntaxTree::findChild(child, type1, type2);
//   }
//   else return child;
//}
//
// --- SyntaxWriter ---

void SyntaxWriter :: insert(int bookmark, LexicalType type, ref_t argument)
{
   size_t position = (bookmark == 0) ? _bookmarks.peek() : *_bookmarks.get(_bookmarks.Count() - bookmark);

   _writer.insertDWord(position, type);
   _writer.insertDWord(position + 4, argument);
   _writer.insertDWord(position + 8, 0);

   Stack<size_t>::Iterator it = _bookmarks.start();
   while (!it.Eof()) {
      if (*it > position) {
         *it = *it + 12;
      }

      it++;
   }
}

void SyntaxWriter::insert(int bookmark, LexicalType type, ident_t argument)
{
   size_t position = (bookmark == 0) ? _bookmarks.peek() : *_bookmarks.get(_bookmarks.Count() - bookmark);
   size_t length = getlength(argument);

   _writer.insertDWord(position, length + 1);
   _writer.insert(position, (void*)((const char*)argument), length + 1);
   _writer.insertDWord(position, length + 5);
   _writer.insertDWord(position, 0);
   _writer.insertDWord(position, type);

   Stack<size_t>::Iterator it = _bookmarks.start();
   while (!it.Eof()) {
      if (*it > position) {
         *it = *it + 12;
      }

      it++;
   }
}

void SyntaxWriter :: newNode(LexicalType type, ref_t argument)
{
   // writer node
   _writer.writeDWord(type);
   _writer.writeDWord(argument);
   _writer.writeDWord(0);
}

void SyntaxWriter :: newNode(LexicalType type, ident_t argument)
{
   // writer node
   _writer.writeDWord(type);
   _writer.writeDWord(0);
   _writer.writeDWord(getlength(argument) + 5);
   _writer.writeLiteral(argument);
   _writer.writeDWord(getlength(argument) + 1);
}

void SyntaxWriter :: closeNode()
{
   _writer.writeDWord(-1);
   _writer.writeDWord(0);
   _writer.writeDWord(0);
}

// --- SyntaxTree::Node ---

SyntaxTree::Node :: Node(SyntaxTree* tree, size_t position, LexicalType type, ref_t argument, int length)
{
   this->tree = tree;
   this->position = position;

   this->type = type;
   this->argument = argument;
   this->argLength = length;
}

// --- SyntaxReader ---

SyntaxTree::Node SyntaxTree :: insertNode(size_t position, LexicalType type, int argument)
{
   SyntaxWriter writer(*this);

   writer.insertChild(writer.setBookmark(position), type, argument);

   MemoryReader reader(&_body, position);
   return read(reader);
}

SyntaxTree::Node SyntaxTree::insertNode(size_t position, LexicalType type, ident_t argument)
{
   SyntaxWriter writer(*this);

   writer.insertChild(writer.setBookmark(position), type, argument);

   MemoryReader reader(&_body, position);
   return read(reader);
}

void SyntaxTree :: copyNode(SyntaxTree::Writer& writer, SyntaxTree::Node node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.argLength > 0) {
         writer.newNode(current.type, current.identifier());
      }
      else writer.newNode(current.type, current.argument);

      copyNode(writer, current);

      writer.closeNode();

      current = current.nextNode();
   }
}

//SyntaxTree::Node SyntaxTree :: insertNode(size_t start_position, size_t end_position, LexicalType type, int argument)
//{
//   SyntaxWriter writer(*this);
//
//   writer.insertChild(writer.setBookmark(start_position), writer.setBookmark(end_position), type, argument);
//
//   MemoryReader reader(&_body, start_position);
//   return read(reader);
//}

SyntaxTree::Node SyntaxTree:: read(StreamReader& reader)
{
   int type = reader.getDWord();
   ref_t arg = reader.getDWord();
   int length = reader.getDWord();

   if (type == -1) {
      return Node();
   }
   else return Node(this, reader.Position(), (LexicalType)type, arg, length);
}

SyntaxTree::Node SyntaxTree:: readRoot()
{
   MemoryReader reader(&_body, 0);

   return read(reader);
}

SyntaxTree::Node SyntaxTree:: readFirstNode(size_t position)
{
   MemoryReader reader(&_body, position);

   return read(reader);
}

SyntaxTree::Node SyntaxTree :: readNextNode(size_t position)
{
   MemoryReader reader(&_body, position);

   int level = 1;

   do {
      int type = reader.getDWord();
      ref_t arg = reader.getDWord();
      int length = reader.getDWord();
      // skip the argument body
      if (length > 0)
         reader.seek(reader.Position() + length);

      if (type == -1) {
         level--;
      }
      else level++;

   } while (level > 0);

   return read(reader);
}

size_t SyntaxTree :: seekNodeEnd(size_t position)
{
   MemoryReader reader(&_body, position);

   int level = 1;
   size_t endPosition = 0;

   do {
      endPosition = reader.Position();

      int type = reader.getDWord();
      ref_t arg = reader.getDWord();
      int length = reader.getDWord();

      // skip the argument body
      if (length > 0)
         reader.seek(reader.Position() + length);

      if (type == -1) {
         level--;
      }
      else level++;

   } while (level > 0);

   return endPosition;
}

//SyntaxTree::Node SyntaxTree :: readPreviousNode(size_t position)
//{
//   MemoryReader reader(&_body);
//
//   position -= 16;
//
//   int level = 0;
//   while (position > 7) {
//      reader.seek(position);
//
//      int type = reader.getDWord();
//      reader.getDWord();
//
//      if (type != -1) {
//         if (level == 0)
//            break;
//
//         level++;
//         if (level == 0) {
//            reader.seek(position);
//
//            return read(reader);
//         }
//      }
//      else level--;
//
//      position -= 8;
//   }
//
//   return Node();
//}
//
//SyntaxTree::Node SyntaxTree :: readParentNode(size_t position)
//{
//   MemoryReader reader(&_body);
//   position -= 16;
//
//   reader.seek(position);
//   if (reader.getDWord() != -1) {
//      reader.seek(position);
//
//      return read(reader);
//   }
//
//   int level = 0;
//   while (position > 7) {
//      reader.seek(position);
//
//      int type = reader.getDWord();
//
//      if (type != -1) {
//         if (level == 0) {
//            reader.seek(position);
//
//            return read(reader);
//         }
//
//         level++;
//      }
//      else level--;
//
//      position -= 8;
//   }
//
//   return Node();
//}
//
//bool SyntaxTree :: matchPattern(Node node, int mask, int counter, ...)
//{
//   va_list argptr;
//   va_start(argptr, counter);
//
//   Node member = node.firstChild();
//   if (member == lxNone)
//      return false;
//
//   for (int i = 0; i < counter; i++) {
//      // get the next pattern
//      NodePattern pattern = va_arg(argptr, NodePattern);
//
//      // find the next tree node
//      while (!test(member.type, mask)) {
//         member = member.nextNode();
//         if (member == lxNone) {
//            va_end(argptr);
//            return false;
//         }
//      }
//
//      if (!pattern.match(member)) {
//         va_end(argptr);
//         return false;
//      }
//      else member = member.nextNode();
//   }
//
//   va_end(argptr);
//   return true;
//}
//
//SyntaxTree::Node SyntaxTree :: findPattern(Node node, int counter, ...)
//{
//   va_list argptr;
//   va_start(argptr, counter);
//
//   size_t level = 1;
//   Node nodes[0x10];
//   nodes[0] = node;
//
//   for (int i = 0; i < counter; i++) {
//      // get the next pattern
//      NodePattern pattern = va_arg(argptr, NodePattern);
//
//      size_t newLevel = level;
//      for (int j = 0; j < level; j++) {
//         Node member = nodes[j].firstChild();
//
//         if (member != lxNone) {
//            // find the matched member
//            while (member != lxNone) {
//               if (pattern.match(member)) {
//                  nodes[newLevel] = member;
//                  newLevel++;
//               }
//
//               member = member.nextNode();
//            }
//         }
//      }
//
//      size_t oldLevel = level;
//      level = 0;
//      for (size_t j = oldLevel; j < newLevel; j++) {
//         nodes[level] = nodes[j];
//         level++;
//      }
//
//      if (level == 0) {
//         nodes[0] = Node();
//
//         break;
//      }
//         
//   }
//
//   return nodes[0];
//}
