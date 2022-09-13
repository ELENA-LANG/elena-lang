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
         MemoryWriter*     vmt;
         MemoryWriter*     code;

         SectionScopeBase* moduleScope;

         MemoryWriter*     debug;
         MemoryWriter*     debugStrings;

         int               minimalArgList;
      };

      struct TapeScope
      {
         Scope* scope;

         int    reserved;
         int    reservedN;

         bool   classMode;
      };

      typedef void(*Saver)(CommandTape& tape, BuildNode& node, TapeScope& scope);

   private:
      const Saver*       _commands;
      LibraryLoaderBase* _loader;

      pos_t savePath(BuildNode node, SectionScopeBase* moduleScope, ReferenceMap& paths);

      void openSymbolDebugInfo(Scope& scope, ustr_t symbolName);
      void openClassDebugInfo(Scope& scope, ustr_t className, ref_t flags);
      void openMethodDebugInfo(Scope& scope, pos_t sourcePath);
      void endDebugInfo(Scope& scope);

      void importTree(CommandTape& tape, BuildNode node, Scope& scope);

      void saveTape(CommandTape& tape, BuildNode node, TapeScope& tapeScope, 
         ReferenceMap& paths, bool loopMode = false);
      void saveBranching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, 
         ReferenceMap& paths, bool loopMode);
      void saveLoop(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths);
      void saveCatching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths);
      void saveVariableInfo(CommandTape& tape, BuildNode node);

      void saveProcedure(BuildNode node, Scope& scope, bool classMode, pos_t sourcePathRef, ReferenceMap& paths);

      void saveVMT(BuildNode node, Scope& scope, pos_t sourcePathRef, ReferenceMap& paths);

      void saveSymbol(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, 
         ReferenceMap& paths);
      void saveClass(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, ReferenceMap& paths);

      void optimizeTape(CommandTape& tape);

   public:
      void save(BuildTree& tree, SectionScopeBase* moduleScope, int minimalArgList);

      ByteCodeWriter(LibraryLoaderBase* loader);
   };
}

#endif
