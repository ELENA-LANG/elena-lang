//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                             (C)2021-2025, by Aleksey Rakov
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
      struct TryContextInfo
      {
         bool      catchMode;
         int       index;
         int       retLabel;
         int       endLabel;
         int       altLabel;
         ref_t     ptr;
         BuildNode catchNode;

         TryContextInfo()
            : TryContextInfo(false)
         {

         }
         TryContextInfo(bool catchMode)
            : catchMode(catchMode), index(0), retLabel(0), endLabel(0), altLabel(0), ptr(0)
         {

         }
      };

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
      typedef Stack<TryContextInfo>       TryContexts;

      struct TapeScope
      {
         Scope*      scope;

         int         reserved;
         int         reservedN;

         bool        classMode;
         bool        threadFriendly;

         LoopLabels  loopLabels;
         TryContexts tryContexts;

         TapeScope(Scope* scope, int reserved, int reservedN, bool classMode, bool threadFriendly)
            : scope(scope), reserved(reserved), reservedN(reservedN), classMode(classMode), 
              threadFriendly(threadFriendly), loopLabels({}), tryContexts({})
         {
            
         }
      };

      class BuildTreeTransformerBase
      {
      protected:
         BuildTreeTransformer _btPatterns;

         virtual bool transform(BuildCodeTrieNode matchNode, BuildNode current, BuildPatternArg& args) = 0;

         bool matchBuildKey(BuildPatterns* matched, BuildPatterns* followers, BuildNode current, BuildNode previous);
         bool matchTriePatterns(BuildNode node);

      public:
         void load(StreamReader& reader);

         void proceed(BuildNode node);

         BuildTreeTransformerBase() = default;
      };

      class BuildTreeAnalyzer : public BuildTreeTransformerBase
      {
         bool transform(BuildCodeTrieNode matchNode, BuildNode current, BuildPatternArg& args) override;

      public:
         BuildTreeAnalyzer() = default;
      };

      class BuildTreeOptimizer : public BuildTreeTransformerBase
      {
         bool transform(BuildCodeTrieNode matchNode, BuildNode current, BuildPatternArg& args) override;

      public:
         BuildTreeOptimizer() = default;
      };

      typedef void(*Saver)(CommandTape& tape, BuildNode& node, TapeScope& scope);
      typedef bool(*Transformer)(BuildNode lastNode);

   private:
      BuildTreeAnalyzer    _btAnalyzer;
      BuildTreeOptimizer   _btTransformer;

      ByteCodeTransformer  _bcTransformer;      

      const Saver*         _commands;
      LibraryLoaderBase*   _loader;

      bool                 _threadFriendly;

      pos_t savePath(BuildNode node, Scope& scope, ReferenceMap& paths);

      void openSymbolDebugInfo(Scope& scope, ustr_t symbolName);
      void openClassDebugInfo(Scope& scope, ustr_t className, ref_t flags);
      void saveFieldDebugInfo(Scope& scope, ClassInfo& info);
      void openMethodDebugInfo(Scope& scope, pos_t sourcePath);
      void endDebugInfo(Scope& scope);

      void importTree(CommandTape& tape, BuildNode node, Scope& scope);

      void openTryBlock(CommandTape& tape, TryContextInfo& tryInfo, bool virtualMode);
      void closeTryBlock(CommandTape& tape, TryContextInfo& tryInfo, bool virtualMode,
         TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode);

      void includeTryBlocks(CommandTape& tape, TapeScope& tapeScope);
      void excludeTryBlocks(CommandTape& tape,
         TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode);

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
      void loadBuildTreeXRules(MemoryDump* dump);
      void loadByteCodeRules(MemoryDump* dump);

      void save(BuildTree& tree, SectionScopeBase* moduleScope, int minimalArgList, 
         int ptrSize, bool tapeOptMode);

      ByteCodeWriter(LibraryLoaderBase* loader, bool threadFriendly);
   };
}

#endif
