//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BCWRITER_H
#define BCWRITER_H

#include "buildtree.h"
#include "bytecode.h"
#include "langcommon.h"

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
         int               ptrSize;
      };

      typedef Stack<Pair<int, int>>       LoopLabels;
      typedef CachedList<mssg_t, 0x10>    IndexedMessages;

      struct TapeScope
      {
         Scope*      scope;

         int         reserved;
         int         reservedN;

         bool        classMode;
         bool        threadFriendly;

         LoopLabels  loopLabels;

         TapeScope(Scope* scope, int reserved, int reservedN, bool classMode, bool threadFriendly)
            : scope(scope), reserved(reserved), reservedN(reservedN), classMode(classMode), threadFriendly(threadFriendly), loopLabels({})
         {
            
         }
      };

      class BuildTreeOptimizer
      {
         BuildTreeTransformer _btTransformer;

         bool matchTriePatterns(BuildNode node);

      public:
         void load(StreamReader& reader);

         void proceed(BuildNode node);

         BuildTreeOptimizer();
      };

      typedef void(*Saver)(CommandTape& tape, BuildNode& node, TapeScope& scope);
      typedef bool(*Transformer)(BuildNode lastNode);

   private:
      BuildTreeOptimizer   _buildTreeOptimizer;

      ByteCodeTransformer  _bcTransformer;      

      const Saver*        _commands;
      LibraryLoaderBase*  _loader;

      bool                _threadFriendly;

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
      void saveTernaryOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
         ReferenceMap& paths, bool tapeOptMode);
      void saveNativeBranching(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
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
      void saveStackCondOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode);
      void saveYielding(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode);
      void saveYieldDispatch(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode);
      void saveVariableInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope);
      void saveArgumentsInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope);
      void saveMethodInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope);

      void saveProcedure(BuildNode node, Scope& scope, bool classMode, pos_t sourcePathRef, 
         ReferenceMap& paths, bool tapeOptMode);

      void saveVMT(ClassInfo& info, BuildNode node, Scope& scope, pos_t sourcePathRef, ReferenceMap& paths,
         bool tapeOptMod, IndexedMessages& indexes);
      void saveIndexTable(Scope& scope, IndexedMessages& indexes);

      void saveSymbol(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, 
         int ptrSize, ReferenceMap& paths, bool tapeOptMode);
      void saveClass(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, 
         int ptrSize, ReferenceMap& paths, bool tapeOptMode);

      bool applyRules(CommandTape& tape);

      void optimizeTape(CommandTape& tape);

   public:
      void loadBuildTreeRules(MemoryDump* dump);
      void loadByteCodeRules(MemoryDump* dump);

      void save(BuildTree& tree, SectionScopeBase* moduleScope, int minimalArgList, 
         int ptrSize, bool tapeOptMode);

      ByteCodeWriter(LibraryLoaderBase* loader, bool threadFriendly);
   };
}

#endif
