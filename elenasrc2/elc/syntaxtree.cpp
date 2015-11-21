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
