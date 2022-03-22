//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Syntax Tree class implementation
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syntaxtree.h"

using namespace elena_lang;

// --- SyntaxTree ---

void SyntaxTree :: save(MemoryBase* section)
{
   MemoryWriter writer(section);

   writer.writePos(_body.length());
   writer.write(_body.get(0), _body.length());

   writer.writePos(_strings.length());
   writer.write(_strings.get(0), _strings.length());
}

void SyntaxTree :: load(MemoryBase* section)
{
   _body.clear();
   _strings.clear();

   MemoryReader reader(section);
   pos_t bodyLength = reader.getPos();
   _body.load(reader, bodyLength);

   pos_t stringLength = reader.getPos();
   _strings.load(reader, stringLength);
}

void SyntaxTree :: copyNode(SyntaxTreeWriter& writer, SyntaxNode node, bool includingNode)
{
   if (includingNode) {
      if (node.arg.strArgPosition != INVALID_POS) {
         writer.newNode(node.key, node.identifier());
      }
      else writer.newNode(node.key, node.arg.reference);
   }

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      copyNode(writer, current, true);

      current = current.nextNode();
   }

   if (includingNode)
      writer.closeNode();
}

void SyntaxTree :: saveNode(SyntaxNode node, MemoryBase* section, bool includingNode)
{
   SyntaxTree tree;
   SyntaxTreeWriter writer(tree);

   writer.newNode(SyntaxKey::Root);

   copyNode(writer, node, includingNode);

   writer.closeNode();

   tree.save(section);
}
