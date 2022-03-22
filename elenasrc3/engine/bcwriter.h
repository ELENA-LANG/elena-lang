//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BCWRITER_H
#define BCWRITER_H

#include "buildtree.h"
#include "bytecode.h"

namespace elena_lang
{
   class ByteCodeWriter
   {
   public:
      struct Scope
      {
         MemoryWriter* vmt;
         MemoryWriter* code;
         SectionScopeBase* moduleScope;
      };

      struct TapeScope
      {
         Scope* scope;

         int    reserved;
         int    reservedN;
      };

      typedef void(*Saver)(CommandTape& tape, BuildNode& node, TapeScope& scope);

   private:
      const Saver*       _commands;
      LibraryLoaderBase* _loader;

      //void saveSendOp(CommandTape& tape, BuildNode node);

      void importTree(CommandTape& tape, BuildNode node, Scope& scope);

      void saveTape(CommandTape& tape, BuildNode node, TapeScope& tapeScope);

      void saveProcedure(BuildNode node, Scope& scope);

      void saveVMT(BuildNode node, Scope& scope);

      void saveSymbol(BuildNode node, SectionScopeBase* moduleScope);
      void saveClass(BuildNode node, SectionScopeBase* moduleScope);

   public:
      void save(BuildTree& tree, SectionScopeBase* moduleScope);

      ByteCodeWriter(LibraryLoaderBase* loader);
   };
}

#endif
