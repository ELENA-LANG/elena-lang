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
#include <stdarg.h>

using namespace _ELENA_;

// --- SyntaxWriter ---

void SyntaxWriter :: insert(int bookmark, LexicalType type, ref_t argument)
{
   size_t position = (bookmark == 0) ? _bookmarks.peek() : *_bookmarks.get(_bookmarks.Count() - bookmark);

   _bodyWriter.insertDWord(position, type);
   _bodyWriter.insertDWord(position + 4, argument);
   _bodyWriter.insertDWord(position + 8, -1);

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

   _bodyWriter.insertDWord(position, _stringWriter.Position());
   _bodyWriter.insertDWord(position, 0);
   _bodyWriter.insertDWord(position, type);

   _stringWriter.writeLiteral(argument, length + 1);   

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
   _bodyWriter.writeDWord(type);
   _bodyWriter.writeDWord(argument);
   _bodyWriter.writeDWord((pos_t)-1);
}

void SyntaxWriter :: newNode(LexicalType type, ident_t argument)
{
   // writer node
   _bodyWriter.writeDWord(type);
   _bodyWriter.writeDWord(0);
   _bodyWriter.writeDWord(_stringWriter.Position());

   _stringWriter.writeLiteral(argument, getlength(argument) + 1);
}

void SyntaxWriter :: closeNode()
{
   _bodyWriter.writeDWord((pos_t)-1);
   _bodyWriter.writeDWord(0);
   _bodyWriter.writeDWord((pos_t)-1);
}

// --- SyntaxTree::Node ---

SyntaxTree::Node :: Node(SyntaxTree* tree, size_t position, LexicalType type, ref_t argument, int strArgument)
{
   this->tree = tree;
   this->position = position;

   this->type = type;
   this->argument = argument;
   this->strArgument = strArgument;
}

// --- SyntaxReader ---

SyntaxTree::Node SyntaxTree :: insertNode(size_t position, LexicalType type, int argument)
{
   SyntaxWriter writer(*this);

   writer.insertChild(writer.setBookmark(position), type, argument);

   MemoryReader reader(&_body, position);
   return read(reader);
}

SyntaxTree::Node SyntaxTree :: insertStrNode(size_t position, LexicalType type, int strArgument)
{
   int length = getlength((const char*)(_strings.get(strArgument))) + 1;

   // HOTFIX : resever the place for the string before writing it
   _strings.reserve(_strings.Length() + length);

   SyntaxWriter writer(*this);

   writer.insertChild(writer.setBookmark(position), type, (const char*)(_strings.get(strArgument)));

   MemoryReader reader(&_body, position);
   return read(reader);
}

SyntaxTree::Node SyntaxTree :: insertNode(size_t position, LexicalType type, ident_t argument)
{
   SyntaxWriter writer(*this);

   writer.insertChild(writer.setBookmark(position), type, argument);

   MemoryReader reader(&_body, position);
   return read(reader);
}

void SyntaxTree :: saveNode(Node node, _Memory* dump, bool inclusingNode)
{
   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxRoot);

   if (inclusingNode) {
      if (node.strArgument >= 0) {
         writer.newNode(node.type, node.identifier());
      }
      else writer.newNode(node.type, node.argument);
      copyNode(writer, node);
      writer.closeNode();
   }
   else copyNode(writer, node);

   writer.closeNode();

   tree.save(dump);
}

void SyntaxTree :: loadNode(Node node, _Memory* dump)
{
   SyntaxTree tree(dump);

   copyNode(tree.readRoot(), node);
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current != lxIdle) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type1 || current == type2) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type1 || current == type2 || current == type3) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type1 || current == type2 || current == type3 || current == type4) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type1 || current == type2 || current == type3 || current == type4 || current == type5) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type1 || current == type2 || current == type3 || current == type4 || current == type5 || current == type6) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6, LexicalType type7)
{
   SNode current = buffer.readRoot();
   while (current != lxNone) {
      if (current == type1 || current == type2 || current == type3 || current == type4 || current == type5 || current == type6 || current == type7) {
         if (current.strArgument >= 0) {
            writer.newNode(current.type, current.identifier());
         }
         else writer.newNode(current.type, current.argument);

         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         current = lxIdle;
      }
      current = current.nextNode();
   }
}

void SyntaxTree :: copyNode(Writer& writer, LexicalType type, Node owner)
{
   SyntaxTree::Node node = owner.findChild(type);
   if (node != lxNone) {
      writer.appendNode(type, node.argument);
   }
}

void SyntaxTree :: copyNode(SyntaxTree::Writer& writer, SyntaxTree::Node node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.strArgument >= 0) {
         writer.newNode(current.type, current.identifier());
      }
      else writer.newNode(current.type, current.argument);

      copyNode(writer, current);

      writer.closeNode();

      current = current.nextNode();
   }
}

void SyntaxTree :: copyNode(SyntaxTree::Node source, SyntaxTree::Node destination)
{
   SNode current = source.firstChild();
   while (current != lxNone) {
      if (current.strArgument >= 0) {
         if (source.tree == destination.tree) {
            // HOTFIX : literal argument could be corrupted by reallocating the string buffer,
            // so the special routine should be used
            copyNode(current, destination.appendStrNode(current.type, current.strArgument));
         }
         else copyNode(current, destination.appendNode(current.type, current.identifier()));
      }
      else copyNode(current, destination.appendNode(current.type, current.argument));

      current = current.nextNode();
   }
}

void SyntaxTree :: copyNodeSafe(Node source, Node destination, bool inclusingNode)
{
   MemoryDump dump;
   saveNode(source, &dump, inclusingNode);
   loadNode(destination, &dump);
}

SyntaxTree::Node SyntaxTree :: insertNode(size_t start_position, size_t end_position, LexicalType type, int argument)
{
   SyntaxWriter writer(*this);

   writer.insertChild(writer.setBookmark(start_position), writer.setBookmark(end_position), type, argument);

   MemoryReader reader(&_body, start_position);
   return read(reader);
}

void SyntaxTree :: refresh(SyntaxTree::Node& node)
{
   MemoryReader reader(&_body, node.position - 12);

   node.type = (LexicalType)reader.getDWord();
   node.argument = reader.getDWord();
   node.strArgument = reader.getDWord();
}

SyntaxTree::Node SyntaxTree:: read(StreamReader& reader)
{
   int type = reader.getDWord();
   ref_t arg = reader.getDWord();
   int str = reader.getDWord();

   if (type == -1) {
      return Node();
   }
   else return Node(this, reader.Position(), (LexicalType)type, arg, str);
}

SyntaxTree::Node SyntaxTree:: readRoot()
{
   MemoryReader reader(&_body, 0u);

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
      reader.getQWord();

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
      reader.getQWord();

      if (type == -1) {
         level--;
      }
      else level++;

   } while (level > 0);

   return endPosition;
}

SyntaxTree::Node SyntaxTree :: readPreviousNode(size_t position)
{
   MemoryReader reader(&_body);

   position -= 24;

   int level = 0;
   while (position > 11) {
      reader.seek(position);

      int type = reader.getDWord();
      reader.getDWord();

      if (type != -1) {
         if (level == 0)
            break;

         level++;
         if (level == 0) {
            reader.seek(position);

            return read(reader);
         }
      }
      else level--;

      position -= 12;
   }

   return Node();
}

SyntaxTree::Node SyntaxTree :: readParentNode(size_t position)
{
   MemoryReader reader(&_body);
   position -= 24;

   reader.seek(position);
   if (reader.getDWord() != -1) {
      reader.seek(position);

      return read(reader);
   }

   int level = 0;
   while (position > 11) {
      reader.seek(position);

      int type = reader.getDWord();

      if (type != -1) {
         if (level == 0) {
            reader.seek(position);

            return read(reader);
         }

         level++;
      }
      else level--;

      position -= 12;
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

   size_t level = 1;
   Node nodes[0x10];
   nodes[0] = node;

   for (int i = 0; i < counter; i++) {
      // get the next pattern
      NodePattern pattern = va_arg(argptr, NodePattern);

      size_t newLevel = level;
      for (size_t j = 0; j < level; j++) {
         Node member = nodes[j].firstChild();

         if (member != lxNone) {
            // find the matched member
            while (member != lxNone) {
               if (pattern.match(member)) {
                  nodes[newLevel] = member;
                  newLevel++;
               }

               member = member.nextNode();
            }
         }
      }

      size_t oldLevel = level;
      level = 0;
      for (size_t j = oldLevel; j < newLevel; j++) {
         nodes[level] = nodes[j];
         level++;
      }

      if (level == 0) {
         nodes[0] = Node();

         break;
      }
         
   }

   return nodes[0];
}

