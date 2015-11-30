//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree class implementation
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syntaxtree.h"
#include <stdarg.h>

using namespace _ELENA_;

// --- SyntaxWriter ---

void SyntaxWriter :: insert(int bookmark, LexicalType type, ref_t argument)
{
   size_t position = (bookmark == 0) ? _bookmarks.peek() : *_bookmarks.get(_bookmarks.Count() - bookmark);

   _writer.insertDWord(position, type);
   _writer.insertDWord(position + 4, argument);

   Stack<size_t>::Iterator it = _bookmarks.start();
   while (!it.Eof()) {
      if (*it > position) {
         *it = *it + 8;
      }

      it++;
   }
}

void SyntaxWriter :: newNode(LexicalType type, ref_t argument)
{
   // writer node
   _writer.writeDWord(type);
   _writer.writeDWord(argument);
}

void SyntaxWriter :: closeNode()
{
   _writer.writeDWord(-1);
   _writer.writeDWord(0);
}

// --- SyntaxTree::Node ---

SyntaxTree::Node :: Node(SyntaxTree* tree, size_t position, LexicalType type, ref_t argument)
{
   this->tree = tree;
   this->position = position;

   this->type = type;
   this->argument = argument;
}

// --- SyntaxReader ---

SyntaxTree::Node SyntaxTree :: insertNode(size_t position, LexicalType type, int argument)
{
   SyntaxWriter writer(_dump);

   writer.insertChild(writer.setBookmark(position), type, argument);

   _reader.seek(position);

   return read();
}

SyntaxTree::Node SyntaxTree:: read()
{
   int type = _reader.getDWord();
   ref_t arg = _reader.getDWord();

   if (type == -1) {
      return Node();
   }
   else return Node(this, _reader.Position(), (LexicalType)type, arg);
}

SyntaxTree::Node SyntaxTree:: readRoot()
{
   _reader.seek(0);

   return read();
}

SyntaxTree::Node SyntaxTree:: readFirstNode(size_t position)
{
   _reader.seek(position);

   return read();
}

SyntaxTree::Node SyntaxTree:: readNextNode(size_t position)
{
   _reader.seek(position);

   int level = 1;

   do {
      int type = _reader.getDWord();
      ref_t arg = _reader.getDWord();

      if (type == -1) {
         level--;
      }
      else level++;

   } while (level > 0);

   return read();
}


SyntaxTree::Node SyntaxTree :: readPreviousNode(size_t position)
{
   position = position - 16;

   int level = 0;
   while (position > 7) {
      _reader.seek(position);

      int type = _reader.getDWord();
         _reader.getDWord();

      if (type != -1) {
         if (level == 0)
            break;

         level++;
         if (level == 0) {
            _reader.seek(position);

            return read();
         }
      }
      else level--;

      position -= 8;
   }

   return Node();
}

bool SyntaxTree :: matchPattern(Node node, int mask, int counter, ...)
{
   va_list argptr;
   va_start(argptr, counter);

   Node member = node.firstChild();
   if (member == lxNone)
      return false;

   for (int i = 0; i < counter; i++) {
      // get the next pattern
      NodePattern pattern = va_arg(argptr, NodePattern);

      // find the next tree node
      while (!test(member.type, mask)) {
         member = member.nextNode();
         if (member == lxNone) {
            va_end(argptr);
            return false;
         }
      }

      if (!pattern.match(member)) {
         va_end(argptr);
         return false;
      }
      else member = member.nextNode();
   }

   va_end(argptr);
   return true;
}

SyntaxTree::Node SyntaxTree :: findPattern(Node node, int counter, ...)
{
   va_list argptr;
   va_start(argptr, counter);

   Node member = node;
   for (int i = 0; i < counter; i++) {
      member = member.firstChild();
      if (member == lxNone)
         break;

      // get the next pattern
      NodePattern pattern = va_arg(argptr, NodePattern);

      // find the matched member
      while (member != lxNone && !pattern.match(member)) {
         member = member.nextNode();
      }
   }

   va_end(argptr);

   return member;
}
