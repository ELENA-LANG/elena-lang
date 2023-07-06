//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                             (C)2021-2023, by Aleksey Rakov
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
      typedef bool(*Transformer)(BuildNode lastNode);

   private:
      ByteCodeTransformer  _bcTransformer;
      BuildTreeTransformer _btTransformer;

      const Saver*        _commands;
      LibraryLoaderBase*  _loader;

      pos_t savePath(BuildNode node, Scope& scope, ReferenceMap& paths);

      void openSymbolDebugInfo(Scope& scope, ustr_t symbolName);
      void openClassDebugInfo(Scope& scope, ustr_t className, ref_t flags);
      void saveFieldDebugInfo(Scope& scope, ClassInfo& info);
      void openMethodDebugInfo(Scope& scope, pos_t sourcePath);
      void endDebugInfo(Scope& scope);

      void importTree(CommandTape& tape, BuildNode node, Scope& scope);

      void saveTape(CommandTape& tape, BuildNode node, TapeScope& tapeScope, 
         ReferenceMap& paths, bool tapeOptMode, bool loopMode = false);
      void saveBranching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, 
         ReferenceMap& paths, bool tapeOptMode, bool loopMode);
      void saveIntBranching(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
         ReferenceMap& paths, bool tapeOptMode, bool loopMode);
      void saveLoop(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveSwitching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveSwitchOption(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveCatching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveFinally(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveAlternate(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveExternOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveShortCircuitOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths,
         bool tapeOptMode);
      void saveVariableInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope);
      void saveParameterInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope);
      void saveMethodInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope);

      void saveProcedure(BuildNode node, Scope& scope, bool classMode, pos_t sourcePathRef, 
         ReferenceMap& paths, bool tapeOptMode);

      void saveVMT(BuildNode node, Scope& scope, pos_t sourcePathRef, ReferenceMap& paths, 
         bool tapeOptMode);

      void saveSymbol(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, 
         ReferenceMap& paths, bool tapeOptMode);
      void saveClass(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, 
         ReferenceMap& paths, bool tapeOptMode);

      bool applyRules(CommandTape& tape);

      void optimizeTape(CommandTape& tape);

      bool matchTriePatterns(BuildNode node);
      void optimizeBuildTree(BuildNode node);

   public:
      void loadBuildTreeRules(MemoryDump* dump);
      void loadByteCodeRules(MemoryDump* dump);

      void save(BuildTree& tree, SectionScopeBase* moduleScope, int minimalArgList, bool tapeOptMode);

      ByteCodeWriter(LibraryLoaderBase* loader);
   };
}

#endif
