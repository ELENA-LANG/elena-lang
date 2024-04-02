//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Tree Serializer classes declaration.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "syntaxtree.h"
#include "buildtree.h"

namespace elena_lang
{
   // --- BuildTreeSerializer ---
   class BuildTreeSerializer
   {
   public:
      static void save(BuildNode node, DynamicUStr& target);
      static void load(ustr_t source, BuildNode& target);
   };

   // --- SyntaxTreeReader ---
   class SyntaxTreeSerializer
   {
   public:
      static void save(SyntaxNode node, DynamicUStr& target);
      static void load(ustr_t source, SyntaxNode& target);
   };
}

#endif // SERIALIZER_H